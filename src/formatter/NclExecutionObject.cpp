/* Copyright (C) 2006-2017 PUC-Rio/Laboratorio TeleMidia

This file is part of Ginga (Ginga-NCL).

Ginga is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

Ginga is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
License for more details.

You should have received a copy of the GNU General Public License
along with Ginga.  If not, see <http://www.gnu.org/licenses/>.  */

#include "ginga.h"

#include "NclExecutionObject.h"
#include "NclCompositeExecutionObject.h"
#include "NclApplicationExecutionObject.h"

#include "NclSwitchEvent.h"

#include "mb/Display.h"
using namespace ::ginga::mb;

GINGA_PRAGMA_DIAG_IGNORE (-Wfloat-conversion)
GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_FORMATTER_BEGIN

set<NclExecutionObject *> NclExecutionObject::_objects;

void
NclExecutionObject::addInstance (NclExecutionObject *object)
{
  _objects.insert (object);
}

bool
NclExecutionObject::removeInstance (NclExecutionObject *object)
{
  bool removed = false;

  auto i = _objects.find (object);
  if (i != _objects.end ())
    {
      _objects.erase (i);
      removed = true;
    }

  return removed;
}

bool
NclExecutionObject::hasInstance (NclExecutionObject *object,
                                 bool eraseFromList)
{
  bool hasObject = false;
  auto i = _objects.find (object);
  if (i != _objects.end ())
    {
      if (eraseFromList)
        {
          _objects.erase (i);
        }
      hasObject = true;
    }

  return hasObject;
}

NclExecutionObject::NclExecutionObject (const string &id, Node *node,
                                        bool handling,
                                        INclLinkActionListener *seListener)
{
  initializeExecutionObject (id, node, NULL, handling, seListener);
}

NclExecutionObject::NclExecutionObject (const string &id, Node *node,
                                        GenericDescriptor *descriptor,
                                        bool handling,
                                        INclLinkActionListener *seListener)
{
  initializeExecutionObject (id, node,
                             new NclCascadingDescriptor (descriptor),
                             handling, seListener);
}

NclExecutionObject::NclExecutionObject (const string &id, Node *node,
                                        NclCascadingDescriptor *descriptor,
                                        bool handling,
                                        INclLinkActionListener *seListener)
{
  initializeExecutionObject (id, node, descriptor, handling, seListener);
}

NclExecutionObject::~NclExecutionObject ()
{
  map<Node *, Node *>::iterator i;
  map<Node *, NclCompositeExecutionObject *>::iterator j;

  Node *parentNode;
  NclCompositeExecutionObject *parentObject;

  removeInstance (this);
  unsetParentsAsListeners ();
  _isDeleting = true;

  _seListener = NULL;
  _dataObject = NULL;
  _wholeContent = NULL;
  _mainEvent = NULL;

  destroyEvents ();

  i = _nodeParentTable.begin ();
  while (i != _nodeParentTable.end ())
    {
      parentNode = i->second;
      j = _parentTable.find (parentNode);
      if (j != _parentTable.end ())
        {
          parentObject = j->second;

          parentObject->removeExecutionObject (this);
        }
      ++i;
    }

  _nodeParentTable.clear ();

  _parentTable.clear ();
  if (_descriptor != NULL)
    {
      delete _descriptor;
      _descriptor = NULL;
    }
}

void
NclExecutionObject::initializeExecutionObject (
    const string &id, Node *node, NclCascadingDescriptor *descriptor,
    bool handling, INclLinkActionListener *seListener)
{
  _typeSet.insert ("NclExecutionObject");

  addInstance (this);
  this->_seListener = seListener;
  this->_isDeleting = false;
  this->_id = id;
  this->_dataObject = node;
  this->_wholeContent = NULL;
  this->_descriptor = NULL;

  this->_isCompiled = false;

  this->_pauseCount = 0;
  this->_mainEvent = NULL;
  this->_descriptor = descriptor;
  this->_isLocked = false;
  this->_isHandler = false;
  this->_isHandling = handling;

  this->_transMan = new NclEventTransitionManager ();
}

void
NclExecutionObject::destroyEvents ()
{
  map<string, NclFormatterEvent *>::iterator i;
  NclFormatterEvent *event;

  i = _events.begin ();
  while (i != _events.end ())
    {
      event = i->second;
      _events.erase (i);
      if (NclFormatterEvent::hasInstance (event, true))
        {
          delete event;
          event = NULL;
        }
      i = _events.begin ();
    }
  _events.clear ();

  if (_transMan != NULL)
    {
      delete _transMan;
      _transMan = NULL;
    }

  _presEvents.clear ();
  _selectionEvents.clear ();
  _otherEvents.clear ();
}

void
NclExecutionObject::unsetParentsAsListeners ()
{
  removeParentListenersFromEvent (_mainEvent);
  removeParentListenersFromEvent (_wholeContent);
}

void
NclExecutionObject::removeParentListenersFromEvent (
    NclFormatterEvent *event)
{
  map<Node *, NclCompositeExecutionObject *>::iterator i;
  NclCompositeExecutionObject *parentObject;

  if (NclFormatterEvent::hasInstance (event, false))
    {
      i = _parentTable.begin ();
      while (i != _parentTable.end ())
        {
          parentObject = (NclCompositeExecutionObject *)(i->second);

          if (NclFormatterEvent::hasInstance (_mainEvent, false))
            {
              // register parent as a mainEvent listener
              _mainEvent->removeEventListener (parentObject);
            }
          else
            {
              break;
            }
          ++i;
        }
    }
}

bool
NclExecutionObject::isSleeping ()
{
  if (_mainEvent == NULL
      || _mainEvent->getCurrentState () == EventUtil::ST_SLEEPING)
    {
      return true;
    }

  return false;
}

bool
NclExecutionObject::isPaused ()
{
  if (_mainEvent != NULL
      && _mainEvent->getCurrentState () == EventUtil::ST_PAUSED)
    {
      return true;
    }

  return false;
}

bool
NclExecutionObject::instanceOf (const string &s)
{
  return (_typeSet.find (s) != _typeSet.end ());
}

Node *
NclExecutionObject::getDataObject ()
{
  return _dataObject;
}

NclCascadingDescriptor *
NclExecutionObject::getDescriptor ()
{
  return _descriptor;
}

string
NclExecutionObject::getId ()
{
  return _id;
}

NclCompositeExecutionObject *
NclExecutionObject::getParentObject ()
{
  return getParentObject (_dataObject);
}

NclCompositeExecutionObject *
NclExecutionObject::getParentObject (Node *node)
{
  Node *parentNode;
  NclCompositeExecutionObject *parentObj = NULL;

  map<Node *, Node *>::iterator i;
  map<Node *, NclCompositeExecutionObject *>::iterator j;

  i = _nodeParentTable.find (node);
  if (i != _nodeParentTable.end ())
    {
      parentNode = i->second;
      j = _parentTable.find (parentNode);
      if (j != _parentTable.end ())
        {
          parentObj = j->second;
        }
    }

  return parentObj;
}

void
NclExecutionObject::addParentObject (NclCompositeExecutionObject *parentObject,
                                     Node *parentNode)
{
  addParentObject (_dataObject, parentObject, parentNode);
}

void
NclExecutionObject::addParentObject (Node *node,
                                     NclCompositeExecutionObject *parentObject,
                                     Node *parentNode)
{
  _nodeParentTable[node] = parentNode;
  _parentTable[parentNode] = parentObject;
}

void
NclExecutionObject::removeParentObject (Node *parentNode,
                                        NclCompositeExecutionObject *parentObject)
{
  map<Node *, NclCompositeExecutionObject *>::iterator i;

  i = _parentTable.find (parentNode);
  if (i != _parentTable.end () && i->second == parentObject)
    {
      if (_wholeContent != NULL)
        {
          _wholeContent->removeEventListener (parentObject);
        }
      _parentTable.erase (i);
    }
}

void
NclExecutionObject::setDescriptor (
    NclCascadingDescriptor *cascadingDescriptor)
{
  this->_descriptor = cascadingDescriptor;
}

void
NclExecutionObject::setDescriptor (GenericDescriptor *descriptor)
{
  NclCascadingDescriptor *cascade;
  cascade = new NclCascadingDescriptor (descriptor);

  if (this->_descriptor != NULL)
    {
      delete this->_descriptor;
    }

  this->_descriptor = cascade;
}

bool
NclExecutionObject::addEvent (NclFormatterEvent *event)
{
  map<string, NclFormatterEvent *>::iterator i;

  i = _events.find (event->getId ());
  if (i != _events.end ())
    {
      clog << "NclExecutionObject::addEvent Warning! Trying to add ";
      clog << "the same event '" << event->getId () << "' twice";
      clog << " current event address '" << i->second << "' ";
      clog << " addEvent address '" << event << "'";
      clog << endl;

      return false;
    }

  clog << "NclExecutionObject::addEvent '" << event->getId () << "' in '";
  clog << getId () << "'" << endl;

  _events[event->getId ()] = event;
  if (event->instanceOf ("NclPresentationEvent"))
    {
      addPresentationEvent ((NclPresentationEvent *)event);
    }
  else if (event->instanceOf ("NclSelectionEvent"))
    {
      _selectionEvents.insert (((NclSelectionEvent *)event));
    }
  else
    {
      _otherEvents.push_back (event);
    }

  return true;
}

void
NclExecutionObject::addPresentationEvent (NclPresentationEvent *event)
{
  NclPresentationEvent *auxEvent;
  GingaTime begin, auxBegin;
  int posBeg = -1;
  int posEnd, posMid;

  if ((event->getAnchor ())->instanceOf ("LambdaAnchor"))
    {
      _presEvents.insert (_presEvents.begin (), event);
      _wholeContent = (NclPresentationEvent *)event;
    }
  else
    {
      begin = event->getBegin ();
      posBeg = 0;
      posEnd = (int)(_presEvents.size () - 1);
      while (posBeg <= posEnd)
        {
          posMid = (posBeg + posEnd) / 2;
          auxEvent = (NclPresentationEvent *)(_presEvents[posMid]);
          auxBegin = auxEvent->getBegin ();
          if (begin < auxBegin)
            {
              posEnd = posMid - 1;
            }
          else if (begin > auxBegin)
            {
              posBeg = posMid + 1;
            }
          else
            {
              posBeg = posMid + 1;
              break;
            }
        }

      _presEvents.insert ((_presEvents.begin () + posBeg), event);
    }

  clog << "NclExecutionObject::addPresentationEvent in '";
  clog << getId () << "'. Call transition ";
  clog << "manager addPresentationEvent for '" << event->getId ();
  clog << "' begin = '" << event->getBegin () << "'; end = '";
  clog << event->getEnd () << "' position = '" << posBeg << "'" << endl;
  _transMan->addPresentationEvent (event);
}

bool
NclExecutionObject::containsEvent (NclFormatterEvent *event)
{
  return (_events.count (event->getId ()) != 0);
}

NclFormatterEvent *
NclExecutionObject::getEventFromAnchorId (const string &anchorId)
{
  map<string, NclFormatterEvent *>::iterator i;
  NclFormatterEvent *event;

  if (anchorId == "")
    {
      if (_wholeContent != NULL)
        {
          return _wholeContent;
        }
    }
  else
    {
      if (_wholeContent != NULL)
        {
          if (NclFormatterEvent::hasNcmId (_wholeContent, anchorId))
            {
              return _wholeContent;
            }
        }

      i = _events.begin ();
      clog << "NclExecutionObject::getEventFromAnchorId searching '";
      clog << anchorId << "' for '" << _id;
      clog << "' with following events = ";
      while (i != _events.end ())
        {
          event = i->second;
          if (event != NULL)
            {
              if (NclFormatterEvent::hasNcmId (event, anchorId))
                {
                  return event;
                }
            }
          ++i;
        }
      clog << endl;
    }

  return NULL;
}

NclFormatterEvent *
NclExecutionObject::getEvent (const string &id)
{
  NclFormatterEvent *ev;
  if (_events.count (id) != 0)
    {
      ev = _events[id];
      return ev;
    }
  return NULL;
}

vector<NclFormatterEvent *>
NclExecutionObject::getEvents ()
{
  vector<NclFormatterEvent *> eventsVector;
  for (const auto &i : _events)
    {
      eventsVector.push_back (i.second);
    }
  return eventsVector;
}

NclPresentationEvent *
NclExecutionObject::getWholeContentPresentationEvent ()
{
  return _wholeContent;
}

bool
NclExecutionObject::removeEvent (NclFormatterEvent *event)
{
  vector<NclPresentationEvent *>::iterator i;
  set<NclSelectionEvent *>::iterator j;
  vector<NclFormatterEvent *>::iterator k;
  map<string, NclFormatterEvent *>::iterator l;

  if (!containsEvent (event))
    {
      return false;
    }

  clog << "NclExecutionObject::removeEvent '" << event->getId () << "'";
  clog << "from '" << getId () << "'" << endl;

  if (event->instanceOf ("NclPresentationEvent"))
    {
      for (i = _presEvents.begin (); i != _presEvents.end (); ++i)
        {
          if (*i == (NclPresentationEvent *)event)
            {
              _presEvents.erase (i);
              break;
            }
        }
      _transMan->removeEventTransition ((NclPresentationEvent *)event);
    }
  else if (event->instanceOf ("NclSelectionEvent"))
    {
      j = _selectionEvents.find (((NclSelectionEvent *)event));
      if (j != _selectionEvents.end ())
        {
          _selectionEvents.erase (j);
        }
    }
  else
    {
      for (k = _otherEvents.begin (); k != _otherEvents.end (); ++k)
        {
          if (*k == event)
            {
              _otherEvents.erase (k);
              break;
            }
        }
    }

  l = _events.find (event->getId ());
  if (l != _events.end ())
    _events.erase (l);

  return true;
}

bool
NclExecutionObject::isCompiled ()
{
  return _isCompiled;
}

void
NclExecutionObject::setCompiled (bool status)
{
  _isCompiled = status;
}

vector<Node *>
NclExecutionObject::getNodes ()
{
  vector<Node *> nodes;
  if (!_nodeParentTable.empty())
    {
      for (auto &i: _nodeParentTable)
        {
          nodes.push_back (i.first);
        }

      if (_nodeParentTable.count (_dataObject) == 0)
        {
          nodes.push_back (_dataObject);
        }
    }

  return nodes;
}

PropertyAnchor *
NclExecutionObject::getNCMProperty (const string &propertyName)
{
  PropertyAnchor *property = NULL;

  if (_dataObject != NULL)
    {
      property = _dataObject->getPropertyAnchor (propertyName);
    }

  return property;
}

NclNodeNesting *
NclExecutionObject::getNodePerspective ()
{
  return getNodePerspective (_dataObject);
}

NclNodeNesting *
NclExecutionObject::getNodePerspective (Node *node)
{
  Node *parentNode;
  NclNodeNesting *perspective;
  NclCompositeExecutionObject *parentObject;
  map<Node *, NclCompositeExecutionObject *>::iterator i;

  if (_nodeParentTable.count (node) == 0)
    {
      if (_dataObject == node)
        {
          perspective = new NclNodeNesting ();
        }
      else
        {
          return NULL;
        }
    }
  else
    {
      parentNode = _nodeParentTable[node];

      i = _parentTable.find (parentNode);
      if (i != _parentTable.end ())
        {
          parentObject = (NclCompositeExecutionObject *)(i->second);

          perspective = parentObject->getNodePerspective (parentNode);
        }
      else
        {
          return NULL;
        }
    }
  perspective->insertAnchorNode (node);
  return perspective;
}

NclFormatterEvent *
NclExecutionObject::getMainEvent ()
{
  return _mainEvent;
}

bool
NclExecutionObject::prepare (NclFormatterEvent *event, GingaTime offsetTime)
{
  int size;
  map<Node *, NclCompositeExecutionObject *>::iterator i;
  GingaTime startTime = 0;
  ContentAnchor *contentAnchor;
  NclFormatterEvent *auxEvent;
  NclAttributionEvent *attributeEvent;
  PropertyAnchor *attributeAnchor;
  int j;
  string value;

  // clog << "NclExecutionObject::prepare(" << id << ")" << endl;
  if (event == NULL || !containsEvent (event)
      || event->getCurrentState () != EventUtil::ST_SLEEPING)
    {
      // clog << "NclExecutionObject::prepare(" << id << ") ret FALSE" <<
      // endl;
      return false;
    }

  if (_mainEvent != NULL
      && _mainEvent->getCurrentState () != EventUtil::ST_SLEEPING)
    {
      return false;
    }

  _mainEvent = event;
  if (_mainEvent->instanceOf ("NclAnchorEvent"))
    {
      contentAnchor = ((NclAnchorEvent *)_mainEvent)->getAnchor ();
      if (contentAnchor != NULL
          && contentAnchor->instanceOf ("LabeledAnchor"))
        {
          i = _parentTable.begin ();
          while (i != _parentTable.end ())
            {
              clog << "NclExecutionObject::prepare(" << _id << ") call ";
              clog << "addEventListener '" << i->second << "' or '";
              clog << i->second;
              clog << "'" << endl;
              // register parent as a mainEvent listener
              _mainEvent->addEventListener (
                  (INclEventListener *)(NclCompositeExecutionObject *)
                      i->second);
              ++i;
            }
          return true;
        }
    }

  if (_mainEvent->instanceOf ("NclPresentationEvent"))
    {
      startTime
          = ((NclPresentationEvent *)_mainEvent)->getBegin () + offsetTime;

      if (startTime > ((NclPresentationEvent *)_mainEvent)->getEnd ())
        {
          return false;
        }
    }

  i = _parentTable.begin ();
  while (i != _parentTable.end ())
    {
      clog << "NclExecutionObject::prepare(" << _id << ") 2nd call ";
      clog << "addEventListener '" << i->second << "' or '";
      clog << i->second;
      clog << "'" << endl;
      // register parent as a mainEvent listener
      _mainEvent->addEventListener (i->second);
      ++i;
    }

  prepareTransitionEvents (startTime);

  size = (int) _otherEvents.size ();
  for (j = 0; j < size; j++)
    {
      auxEvent = _otherEvents[j];
      if (auxEvent->instanceOf ("NclAttributionEvent"))
        {
          attributeEvent = (NclAttributionEvent *)auxEvent;
          attributeAnchor = attributeEvent->getAnchor ();
          value = attributeAnchor->getValue ();
          if (value != "")
            {
              attributeEvent->setValue (value);
            }
        }
    }

  this->_offsetTime = startTime;
  return true;
}

bool
NclExecutionObject::start ()
{
  ContentAnchor *contentAnchor;

  clog << "NclExecutionObject::start(" << _id << ")" << endl;
  if (_mainEvent == NULL && _wholeContent == NULL)
    {
      return false;
    }

  if (_mainEvent != NULL
      && _mainEvent->getCurrentState () != EventUtil::ST_SLEEPING)
    {
      return true;
    }

  if (_mainEvent == NULL)
    {
      prepare (_wholeContent, 0.0);
    }

  if (_mainEvent != NULL && _mainEvent->instanceOf ("NclAnchorEvent"))
    {
      contentAnchor = ((NclAnchorEvent *)_mainEvent)->getAnchor ();
      if (contentAnchor != NULL
          && contentAnchor->instanceOf ("LabeledAnchor"))
        {
          _transMan->start (_offsetTime);
          _mainEvent->start ();
          return true;
        }
    }

  _transMan->start (_offsetTime);
  return true;
}

void
NclExecutionObject::updateTransitionTable (GingaTime value, Player *player)
{
  _transMan->updateTransitionTable (value, player, _mainEvent);
}

void
NclExecutionObject::prepareTransitionEvents (GingaTime startTime)
{
  _transMan->prepare (_mainEvent == _wholeContent, startTime);
}

NclEventTransition *
NclExecutionObject::getNextTransition ()
{
  if (isSleeping () || !_mainEvent->instanceOf ("NclPresentationEvent"))
    {
      return NULL;
    }
  return _transMan->getNextTransition (_mainEvent);
}

bool
NclExecutionObject::stop ()
{
  ContentAnchor *contentAnchor;
  GingaTime endTime;

  if (isSleeping ())
    {
      clog << "NclExecutionObject::stop for '" << _id << "'";
      clog << " returns false because mainEvent is SLEEPING" << endl;
      return false;
    }

  if (_mainEvent->instanceOf ("NclPresentationEvent"))
    {
      endTime = ((NclPresentationEvent *)_mainEvent)->getEnd ();
      _transMan->stop (endTime);
    }
  else if (_mainEvent->instanceOf ("NclAnchorEvent"))
    {
      contentAnchor = ((NclAnchorEvent *)_mainEvent)->getAnchor ();
      if (contentAnchor != NULL
          && contentAnchor->instanceOf ("LabeledAnchor"))
        {
          /*clog << "NclExecutionObject::stop for '" << id << "'";
          clog << " call mainEvent '" << mainEvent->getId();
          clog << "' stop" << endl;*/
          _mainEvent->stop ();
        }
    }

  _transMan->resetTimeIndex ();
  _pauseCount = 0;
  return true;
}

bool
NclExecutionObject::abort ()
{
  ContentAnchor *contentAnchor;
  GingaTime endTime;

  if (isSleeping ())
    return false;

  if (_mainEvent->instanceOf ("NclPresentationEvent"))
    {
      endTime = ((NclPresentationEvent *)_mainEvent)->getEnd ();
      _transMan->abort (endTime);
    }
  else if (_mainEvent->instanceOf ("NclAnchorEvent"))
    {
      contentAnchor = ((NclAnchorEvent *)_mainEvent)->getAnchor ();
      if (contentAnchor != NULL
          && contentAnchor->instanceOf ("LabeledAnchor"))
        {
          _mainEvent->abort ();
        }
    }

  _transMan->resetTimeIndex ();
  _pauseCount = 0;
  return true;
}

bool
NclExecutionObject::pause ()
{
  NclFormatterEvent *event;
  vector<NclFormatterEvent *>::iterator i;

  // clog << "NclExecutionObject::pause(" << id << ")" << endl;
  if (isSleeping ())
    {
      return false;
    }

  vector<NclFormatterEvent *> evs = getEvents ();
  if (_pauseCount == 0)
    {
      i = evs.begin ();
      while (i != evs.end ())
        {
          event = *i;
          if (event->getCurrentState () == EventUtil::ST_OCCURRING)
            {
              event->pause ();
            }
          ++i;
        }
    }

  _pauseCount++;
  return true;
}

bool
NclExecutionObject::resume ()
{
  NclFormatterEvent *event;
  vector<NclFormatterEvent *>::iterator i;

  // clog << "NclExecutionObject::resume(" << id << ")" << endl;
  if (_pauseCount == 0)
    {
      return false;
    }
  else
    {
      _pauseCount--;
      if (_pauseCount > 0)
        {
          return false;
        }
    }

  vector<NclFormatterEvent *> evs = getEvents ();
  if (_pauseCount == 0)
    {
      i = evs.begin ();
      while (i != evs.end ())
        {
          event = *i;
          if (event->getCurrentState () == EventUtil::ST_PAUSED)
            {
              event->resume ();
            }
          ++i;
        }
    }

  return true;
}

bool
NclExecutionObject::setProperty (NclAttributionEvent *event,
                                 const string &value)
{
  string propName;

  NclFormatterRegion *region = NULL;
  LayoutRegion *ncmRegion = NULL;
  vector<string> params;

  if (_descriptor == NULL || _descriptor->getFormatterRegion () == NULL)
    {
      clog << "NclExecutionObject::setPropertyValue : setPropertyValue could ";
      clog << "not be performed. Descriptor or formatterRegion is NULL";
      clog << endl;

      return false;
    }

  if (!containsEvent (event) || value == "")
    {
      clog << "NclExecutionObject::setPropertyValue(" << getId () << ")";
      clog << " returns false: event '";
      clog << event->getId () << "' not found!" << endl;
      return false;
    }

  region = _descriptor->getFormatterRegion ();
  ncmRegion = region->getLayoutRegion ();
  if (ncmRegion == NULL)
    {
      clog << "NclExecutionObject::setPropertyValue : The ncmRegion ";
      clog << " is NULL (PROBLEM)!" << endl;
      return false;
    }

  propName = (event->getAnchor ())->getName ();

  if (propName == "zIndex")
    {
      region->setZIndex (xstrtoint (value, 10));
      return true;
    }
  return false;
}

string
NclExecutionObject::getProperty (const string &param)
{
  NclFormatterRegion *region = NULL;
  LayoutRegion *ncmRegion = NULL;
  string value = "";

  if (_descriptor == NULL || _descriptor->getFormatterRegion () == NULL)
    {
      clog << "NclExecutionObject::setPropertyValue : setPropertyValue could ";
      clog << "not be performed. Descriptor or formatterRegion is NULL";
      clog << endl;
      return value;
    }

  region = _descriptor->getFormatterRegion ();
  ncmRegion = region->getLayoutRegion ();

  double current = 0.0;
  if (param == "left")
    {
      current = ncmRegion->getLeft ();
      xstrassign (value, "%d", (int) current);
    }
  else if (param == "top")
    {
      current = ncmRegion->getTop ();
      xstrassign (value, "%d", (int) current);
    }
  else if (param == "width")
    {
      current = ncmRegion->getWidth ();
      xstrassign (value, "%d", (int) current);
    }
  else if (param == "height")
    {
      current = ncmRegion->getHeight ();
      xstrassign (value, "%d", (int) current);
    }
  else if (param == "bottom")
    {
      current = ncmRegion->getBottom ();
      xstrassign (value, "%d", (int) current);
    }
  else if (param == "right")
    {
      current = ncmRegion->getRight ();
      xstrassign (value, "%d", (int) current);
    }
  else if (param == "transparency")
    {
      current = region->getTransparency ();
      xstrassign (value, "%d", (int) current);
    }
  else if (param == "bounds")
    {
      xstrassign (value, "%d,%d,%d,%d",
                  ncmRegion->getLeft (),
                  ncmRegion->getTop (),
                  ncmRegion->getWidth (),
                  ncmRegion->getHeight ());
    }
  else if (param == "location")
    {
      xstrassign (value, "%d,%d",
                  ncmRegion->getLeft (),
                  ncmRegion->getTop ());
    }
  else if (param == "size")
    {
      xstrassign (value, "%d,%d",
                  ncmRegion->getWidth (),
                  ncmRegion->getHeight ());
    }

  return value;
}

bool
NclExecutionObject::unprepare ()
{
  if (_mainEvent == NULL
      || _mainEvent->getCurrentState () != EventUtil::ST_SLEEPING)
    {
      clog << "NclExecutionObject::unprepare(" << _id << ") unlocked";
      clog << " ret FALSE" << endl;
      return false;
    }

  removeParentListenersFromEvent (_mainEvent);
  _mainEvent = NULL;
  return true;
}

void
NclExecutionObject::setHandling (bool isHandling)
{
  this->_isHandling = isHandling;
}

void
NclExecutionObject::setHandler (bool isHandler)
{
  this->_isHandler = isHandler;
}

//dragon head
bool
NclExecutionObject::selectionEvent (SDL_Keycode key, GingaTime currentTime)
{
  string selCode;
  string keyString;
  NclSelectionEvent *selectionEvent;
  IntervalAnchor *intervalAnchor;
  NclFormatterEvent *expectedEvent;
  Anchor *expectedAnchor;
  string anchorId = "";
  set<NclSelectionEvent *> *selectedEvents;
  set<NclSelectionEvent *>::iterator i;
  bool sleeping = isSleeping ();
  bool paused = isPaused ();
  bool selected = false;
  GingaTime intervalBegin;
  GingaTime intervalEnd;

  keyString = "UNKNOWN";
  ginga_key_table_index (key, &keyString);

   if ((!_isHandling && !_isHandler) || sleeping /*|| paused*/)
    {
      clog << "NclExecutionObject::selectionEvent Can't receive event on '";
      clog << getId () << "': isHandling = '" << _isHandling << "' ";
      clog << "isHandler = '" << _isHandler << "' ";
      clog << "isSleeping() = '" << sleeping << "' ";
      clog << "isPaused() = '" << paused << "' ";
      clog << endl;
      return false;
    }

  if (_selectionEvents.empty ())
    {
      clog << "NclExecutionObject::selectionEvent Can't receive event on '";
      clog << getId () << "': selection events is empty";
      clog << endl;
      return false;
    }

  selectedEvents = new set<NclSelectionEvent *>;
  i = _selectionEvents.begin ();
  while (i != _selectionEvents.end ())
    {
      selectionEvent = (NclSelectionEvent *)(*i);
      selCode = selectionEvent->getSelectionCode ();

      clog << "NclExecutionObject::selectionEvent(" << _id << ") event '";
      clog << selectionEvent->getId () << "' has selCode = '" << selCode;
      clog << "' (looking for key code '" << keyString << "'" << endl;

     if ( !keyString.compare(selCode) )
        {
          if (selectionEvent->getAnchor ()->instanceOf ("LambdaAnchor"))
            {
              selectedEvents->insert (selectionEvent);
            }
          else if (selectionEvent->getAnchor ()->instanceOf (
                       "IntervalAnchor"))
            {
              intervalAnchor
                  = (IntervalAnchor *)(selectionEvent->getAnchor ());

              intervalBegin = intervalAnchor->getBegin ();
              intervalEnd = intervalAnchor->getEnd ();

              clog << "NclExecutionObject::selectionEvent(" << _id << ") ";
              clog << "interval anchor '" << intervalAnchor->getId ();
              clog << "' begins at '" << intervalBegin << "', ends at ";
              clog << intervalEnd << "' (current time is '";
              clog << currentTime << "'";
              clog << endl;

              if (intervalBegin <= currentTime
                  && intervalEnd >= currentTime)
                {
                  selectedEvents->insert (selectionEvent);
                }
            }
          else
            {
              expectedAnchor = selectionEvent->getAnchor ();
              if (expectedAnchor->instanceOf ("LabeledAnchor"))
                {
                  anchorId = ((LabeledAnchor *)expectedAnchor)->getLabel ();
                }
              else
                {
                  anchorId = expectedAnchor->getId ();
                }

              expectedEvent = getEventFromAnchorId (anchorId);
              if (expectedEvent != NULL)
                {
                  clog << "NclExecutionObject::selectionEvent(";
                  clog << _id << ")";
                  clog << " analyzing event '";
                  clog << expectedEvent->getId ();

                  if (expectedEvent->getCurrentState ()
                      == EventUtil::ST_OCCURRING)
                    {
                      selectedEvents->insert (selectionEvent);
                    }
                  else
                    {
                      clog << "' not occurring";
                    }

                  clog << "'" << endl;
                }
              else
                {
                  clog << "NclExecutionObject::selectionEvent(" << _id
                       << ")";
                  clog << " can't find event for anchorid = '";
                  clog << anchorId << "'" << endl;
                }
            }
        }
      ++i;
    }

  NclLinkSimpleAction *fakeAct;

  i = selectedEvents->begin ();
  while (i != selectedEvents->end ())
    {
      selected = true;
      selectionEvent = (*i);

      if (_seListener != NULL)
        {
          clog << "NclExecutionObject::selectionEvent(" << _id << ")";
          clog << " calling scheduler to execute fake action";
          clog << endl;

          fakeAct = new NclLinkSimpleAction (selectionEvent, ACT_START);

          _seListener->scheduleAction (fakeAct);
        }

      ++i;
    }

  delete selectedEvents;
  selectedEvents = NULL;

  return selected;
}

GINGA_FORMATTER_END
