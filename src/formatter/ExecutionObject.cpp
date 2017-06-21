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

#include "ExecutionObject.h"
#include "ExecutionObjectContext.h"
#include "ExecutionObjectApplication.h"

#include "FormatterEvents.h"

#include "mb/Display.h"
using namespace ::ginga::mb;

GINGA_PRAGMA_DIAG_IGNORE (-Wfloat-conversion)
GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_FORMATTER_BEGIN

set<ExecutionObject *> ExecutionObject::_objects;

void
ExecutionObject::addInstance (ExecutionObject *object)
{
  _objects.insert (object);
}

bool
ExecutionObject::removeInstance (ExecutionObject *object)
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
ExecutionObject::hasInstance (ExecutionObject *obj,
                              bool eraseFromList)
{
  auto i = _objects.find (obj);
  bool hasObj = (i != _objects.end());

  if (hasObj && eraseFromList)
    {
      _objects.erase (obj);
    }

  return hasObj;
}

ExecutionObject::ExecutionObject (const string &id,
                                  Node *node,
                                  NclCascadingDescriptor *descriptor,
                                  bool handling,
                                  INclLinkActionListener *seListener)
{
  _typeSet.insert ("ExecutionObject");

  addInstance (this);
  this->_seListener = seListener;
  this->_isDeleting = false;
  this->_id = id;
  this->_dataObject = node;
  this->_wholeContent = nullptr;
  this->_descriptor = nullptr;

  this->_isCompiled = false;

  this->_pauseCount = 0;
  this->_mainEvent = nullptr;
  this->_descriptor = descriptor;
  this->_isLocked = false;
  this->_isHandler = false;
  this->_isHandling = handling;

  this->_transMan = new NclEventTransitionManager ();
}

ExecutionObject::~ExecutionObject ()
{
  map<Node *, ExecutionObjectContext *>::iterator j;

  Node *parentNode;
  ExecutionObjectContext *parentObject;

  removeInstance (this);
  unsetParentsAsListeners ();
  _isDeleting = true;

  _seListener = nullptr;
  _dataObject = nullptr;
  _wholeContent = nullptr;
  _mainEvent = nullptr;

  destroyEvents ();

  for (auto i : _nodeParentTable)
    {
      parentNode = i.second;
      j = _parentTable.find (parentNode);
      if (j != _parentTable.end ())
        {
          parentObject = j->second;

          parentObject->removeExecutionObject (this);
        }
    }

  _nodeParentTable.clear ();

  _parentTable.clear ();
  if (_descriptor != nullptr)
    {
      delete _descriptor;
      _descriptor = nullptr;
    }
}

void
ExecutionObject::destroyEvents ()
{
  for (auto i : _events)
    {
      NclFormatterEvent *event = i.second;
      if (NclFormatterEvent::hasInstance (event, true))
        {
          delete event;
        }
    }
  _events.clear ();

  if (_transMan != nullptr)
    {
      delete _transMan;
      _transMan = nullptr;
    }

  _presEvents.clear ();
  _selectionEvents.clear ();
  _otherEvents.clear ();
}

void
ExecutionObject::unsetParentsAsListeners ()
{
  removeParentListenersFromEvent (_mainEvent);
  removeParentListenersFromEvent (_wholeContent);
}

void
ExecutionObject::removeParentListenersFromEvent (
    NclFormatterEvent *event)
{
  map<Node *, ExecutionObjectContext *>::iterator i;
  ExecutionObjectContext *parentObject;

  if (NclFormatterEvent::hasInstance (event, false))
    {
      i = _parentTable.begin ();
      while (i != _parentTable.end ())
        {
          parentObject = (ExecutionObjectContext *)(i->second);

          if (NclFormatterEvent::hasInstance (_mainEvent, false))
            {
              // register parent as a mainEvent listener
              _mainEvent->removeListener (parentObject);
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
ExecutionObject::isSleeping ()
{
  if (_mainEvent == nullptr
      || _mainEvent->getCurrentState () == EventState::SLEEPING)
    {
      return true;
    }

  return false;
}

bool
ExecutionObject::isPaused ()
{
  if (_mainEvent != nullptr
      && _mainEvent->getCurrentState () == EventState::PAUSED)
    {
      return true;
    }

  return false;
}

bool
ExecutionObject::instanceOf (const string &s)
{
  return (_typeSet.find (s) != _typeSet.end ());
}

Node *
ExecutionObject::getDataObject ()
{
  return _dataObject;
}

NclCascadingDescriptor *
ExecutionObject::getDescriptor ()
{
  return _descriptor;
}

string
ExecutionObject::getId ()
{
  return _id;
}

ExecutionObjectContext *
ExecutionObject::getParentObject ()
{
  return getParentObject (_dataObject);
}

ExecutionObjectContext *
ExecutionObject::getParentObject (Node *node)
{
  ExecutionObjectContext *parentObj = nullptr;

  auto i = _nodeParentTable.find (node);
  if (i != _nodeParentTable.end ())
    {
      Node *parentNode = i->second;
      auto j = _parentTable.find (parentNode);
      if (j != _parentTable.end ())
        {
          parentObj = j->second;
        }
    }

  return parentObj;
}

void
ExecutionObject::addParentObject (ExecutionObjectContext *parentObject,
                                  Node *parentNode)
{
  addParentObject (_dataObject, parentObject, parentNode);
}

void
ExecutionObject::addParentObject (Node *node,
                                  ExecutionObjectContext *parentObject,
                                  Node *parentNode)
{
  _nodeParentTable[node] = parentNode;
  _parentTable[parentNode] = parentObject;
}

void
ExecutionObject::removeParentObject (Node *parentNode,
                                     ExecutionObjectContext *parentObject)
{
  map<Node *, ExecutionObjectContext *>::iterator i;

  i = _parentTable.find (parentNode);
  if (i != _parentTable.end () && i->second == parentObject)
    {
      if (_wholeContent != nullptr)
        {
          _wholeContent->removeListener (parentObject);
        }
      _parentTable.erase (i);
    }
}

void
ExecutionObject::setDescriptor (
    NclCascadingDescriptor *cascadingDescriptor)
{
  this->_descriptor = cascadingDescriptor;
}

void
ExecutionObject::setDescriptor (GenericDescriptor *descriptor)
{
  NclCascadingDescriptor *cascade;
  cascade = new NclCascadingDescriptor (descriptor);

  if (this->_descriptor != nullptr)
    {
      delete this->_descriptor;
    }

  this->_descriptor = cascade;
}

bool
ExecutionObject::addEvent (NclFormatterEvent *event)
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
ExecutionObject::addPresentationEvent (NclPresentationEvent *event)
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
ExecutionObject::containsEvent (NclFormatterEvent *event)
{
  return (_events.count (event->getId ()) != 0);
}

NclFormatterEvent *
ExecutionObject::getEventFromAnchorId (const string &anchorId)
{
  map<string, NclFormatterEvent *>::iterator i;
  NclFormatterEvent *event;

  if (anchorId == "")
    {
      if (_wholeContent != nullptr)
        {
          return _wholeContent;
        }
    }
  else
    {
      if (_wholeContent != nullptr)
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
          if (event != nullptr)
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

  return nullptr;
}

NclFormatterEvent *
ExecutionObject::getEvent (const string &id)
{
  NclFormatterEvent *ev;
  if (_events.count (id) != 0)
    {
      ev = _events[id];
      return ev;
    }
  return nullptr;
}

vector<NclFormatterEvent *>
ExecutionObject::getEvents ()
{
  vector<NclFormatterEvent *> eventsVector;
  for (const auto &i : _events)
    {
      eventsVector.push_back (i.second);
    }
  return eventsVector;
}

NclPresentationEvent *
ExecutionObject::getWholeContentPresentationEvent ()
{
  return _wholeContent;
}

bool
ExecutionObject::removeEvent (NclFormatterEvent *event)
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
ExecutionObject::isCompiled ()
{
  return _isCompiled;
}

void
ExecutionObject::setCompiled (bool status)
{
  _isCompiled = status;
}

vector<Node *>
ExecutionObject::getNodes ()
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
ExecutionObject::getNCMProperty (const string &propertyName)
{
  PropertyAnchor *property = nullptr;

  if (_dataObject != nullptr)
    {
      property = _dataObject->getPropertyAnchor (propertyName);
    }

  return property;
}

NclNodeNesting *
ExecutionObject::getNodePerspective ()
{
  return getNodePerspective (_dataObject);
}

NclNodeNesting *
ExecutionObject::getNodePerspective (Node *node)
{
  Node *parentNode;
  NclNodeNesting *perspective;
  ExecutionObjectContext *parentObject;
  map<Node *, ExecutionObjectContext *>::iterator i;

  if (_nodeParentTable.count (node) == 0)
    {
      if (_dataObject == node)
        {
          perspective = new NclNodeNesting ();
        }
      else
        {
          return nullptr;
        }
    }
  else
    {
      parentNode = _nodeParentTable[node];

      i = _parentTable.find (parentNode);
      if (i != _parentTable.end ())
        {
          parentObject = (ExecutionObjectContext *)(i->second);

          perspective = parentObject->getNodePerspective (parentNode);
        }
      else
        {
          return nullptr;
        }
    }
  perspective->insertAnchorNode (node);
  return perspective;
}

NclFormatterEvent *
ExecutionObject::getMainEvent ()
{
  return _mainEvent;
}

bool
ExecutionObject::prepare (NclFormatterEvent *event, GingaTime offsetTime)
{
  int size;
  map<Node *, ExecutionObjectContext *>::iterator i;
  GingaTime startTime = 0;
  ContentAnchor *contentAnchor;
  NclFormatterEvent *auxEvent;
  NclAttributionEvent *attributeEvent;
  PropertyAnchor *attributeAnchor;
  int j;
  string value;

  // clog << "NclExecutionObject::prepare(" << id << ")" << endl;
  if (event == nullptr || !containsEvent (event)
      || event->getCurrentState () != EventState::SLEEPING)
    {
      // clog << "NclExecutionObject::prepare(" << id << ") ret FALSE" <<
      // endl;
      return false;
    }

  if (_mainEvent != nullptr
      && _mainEvent->getCurrentState () != EventState::SLEEPING)
    {
      return false;
    }

  _mainEvent = event;
  if (_mainEvent->instanceOf ("NclAnchorEvent"))
    {
      contentAnchor = ((NclAnchorEvent *)_mainEvent)->getAnchor ();
      if (contentAnchor != nullptr
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
              _mainEvent->addListener (
                    (INclEventListener *)(ExecutionObjectContext *)
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
      _mainEvent->addListener (i->second);
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
ExecutionObject::start ()
{
  ContentAnchor *contentAnchor;

  clog << "NclExecutionObject::start(" << _id << ")" << endl;
  if (_mainEvent == nullptr && _wholeContent == nullptr)
    {
      return false;
    }

  if (_mainEvent != nullptr
      && _mainEvent->getCurrentState () != EventState::SLEEPING)
    {
      return true;
    }

  if (_mainEvent == nullptr)
    {
      prepare (_wholeContent, 0.0);
    }

  if (_mainEvent != nullptr && _mainEvent->instanceOf ("NclAnchorEvent"))
    {
      contentAnchor = ((NclAnchorEvent *)_mainEvent)->getAnchor ();
      if (contentAnchor != nullptr
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
ExecutionObject::updateTransitionTable (GingaTime value, Player *player)
{
  _transMan->updateTransitionTable (value, player, _mainEvent);
}

void
ExecutionObject::prepareTransitionEvents (GingaTime startTime)
{
  _transMan->prepare (_mainEvent == _wholeContent, startTime);
}

NclEventTransition *
ExecutionObject::getNextTransition ()
{
  if (isSleeping () || !_mainEvent->instanceOf ("NclPresentationEvent"))
    {
      return nullptr;
    }
  return _transMan->getNextTransition (_mainEvent);
}

bool
ExecutionObject::stop ()
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
      if (contentAnchor != nullptr
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
ExecutionObject::abort ()
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
      if (contentAnchor != nullptr
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
ExecutionObject::pause ()
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
          if (event->getCurrentState () == EventState::OCCURRING)
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
ExecutionObject::resume ()
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
          if (event->getCurrentState () == EventState::PAUSED)
            {
              event->resume ();
            }
          ++i;
        }
    }

  return true;
}

bool
ExecutionObject::unprepare ()
{
  if (_mainEvent == nullptr
      || _mainEvent->getCurrentState () != EventState::SLEEPING)
    {
      clog << "NclExecutionObject::unprepare(" << _id << ") unlocked";
      clog << " ret FALSE" << endl;
      return false;
    }

  removeParentListenersFromEvent (_mainEvent);
  _mainEvent = nullptr;
  return true;
}

void
ExecutionObject::setHandling (bool isHandling)
{
  this->_isHandling = isHandling;
}

void
ExecutionObject::setHandler (bool isHandler)
{
  this->_isHandler = isHandler;
}

bool
ExecutionObject::selectionEvent (SDL_Keycode key, GingaTime currentTime)
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

      if (keyString == selCode)
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
              if (expectedEvent != nullptr)
                {
                  clog << "NclExecutionObject::selectionEvent(";
                  clog << _id << ")";
                  clog << " analyzing event '";
                  clog << expectedEvent->getId ();

                  if (expectedEvent->getCurrentState ()
                      == EventState::OCCURRING)
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

      if (_seListener != nullptr)
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
  selectedEvents = nullptr;

  return selected;
}

GINGA_FORMATTER_END
