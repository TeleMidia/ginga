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

set<NclExecutionObject *> NclExecutionObject::objects;

void
NclExecutionObject::addInstance (NclExecutionObject *object)
{
  objects.insert (object);
}

bool
NclExecutionObject::removeInstance (NclExecutionObject *object)
{
  set<NclExecutionObject *>::iterator i;
  bool removed = false;

  i = objects.find (object);
  if (i != objects.end ())
    {
      objects.erase (i);
      removed = true;
    }

  return removed;
}

bool
NclExecutionObject::hasInstance (NclExecutionObject *object,
                                 bool eraseFromList)
{
  set<NclExecutionObject *>::iterator i;
  bool hasObject;

  hasObject = false;
  i = objects.find (object);
  if (i != objects.end ())
    {
      if (eraseFromList)
        {
          objects.erase (i);
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

  clog << "NclExecutionObject::~NclExecutionObject(" << getId () << ")"
       << endl;

  removeInstance (this);
  unsetParentsAsListeners ();
  deleting = true;

  seListener = NULL;
  dataObject = NULL;
  wholeContent = NULL;
  mainEvent = NULL;

  destroyEvents ();

  i = nodeParentTable.begin ();
  while (i != nodeParentTable.end ())
    {
      parentNode = i->second;
      j = parentTable.find (parentNode);
      if (j != parentTable.end ())
        {
          parentObject = j->second;

          parentObject->removeExecutionObject (this);
        }
      ++i;
    }

  nodeParentTable.clear ();

  parentTable.clear ();
  if (descriptor != NULL)
    {
      delete descriptor;
      descriptor = NULL;
    }
}

void
NclExecutionObject::initializeExecutionObject (
    const string &id, Node *node, NclCascadingDescriptor *descriptor,
    bool handling, INclLinkActionListener *seListener)
{
  typeSet.insert ("NclExecutionObject");

  addInstance (this);
  this->seListener = seListener;
  this->deleting = false;
  this->id = id;
  this->dataObject = node;
  this->wholeContent = NULL;
  this->descriptor = NULL;

  this->isItCompiled = false;

  this->pauseCount = 0;
  this->mainEvent = NULL;
  this->descriptor = descriptor;
  this->isLocked = false;
  this->isHandler = false;
  this->isHandling = handling;

  this->transMan = new NclEventTransitionManager ();
}

void
NclExecutionObject::destroyEvents ()
{
  map<string, NclFormatterEvent *>::iterator i;
  NclFormatterEvent *event;

  i = events.begin ();
  while (i != events.end ())
    {
      event = i->second;
      events.erase (i);
      if (NclFormatterEvent::hasInstance (event, true))
        {
          delete event;
          event = NULL;
        }
      i = events.begin ();
    }
  events.clear ();

  if (transMan != NULL)
    {
      delete transMan;
      transMan = NULL;
    }

  presEvents.clear ();
  selectionEvents.clear ();
  otherEvents.clear ();
}

void
NclExecutionObject::unsetParentsAsListeners ()
{
  removeParentListenersFromEvent (mainEvent);
  removeParentListenersFromEvent (wholeContent);
}

void
NclExecutionObject::removeParentListenersFromEvent (
    NclFormatterEvent *event)
{
  map<Node *, NclCompositeExecutionObject *>::iterator i;
  NclCompositeExecutionObject *parentObject;

  if (NclFormatterEvent::hasInstance (event, false))
    {
      i = parentTable.begin ();
      while (i != parentTable.end ())
        {
          parentObject = (NclCompositeExecutionObject *)(i->second);

          if (NclFormatterEvent::hasInstance (mainEvent, false))
            {
              // register parent as a mainEvent listener
              mainEvent->removeEventListener (parentObject);
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
  if (mainEvent == NULL
      || mainEvent->getCurrentState () == EventUtil::ST_SLEEPING)
    {
      return true;
    }

  return false;
}

bool
NclExecutionObject::isPaused ()
{
  if (mainEvent != NULL
      && mainEvent->getCurrentState () == EventUtil::ST_PAUSED)
    {
      return true;
    }

  return false;
}

bool
NclExecutionObject::instanceOf (const string &s)
{
  return (typeSet.find (s) != typeSet.end ());
}

Node *
NclExecutionObject::getDataObject ()
{
  return dataObject;
}

NclCascadingDescriptor *
NclExecutionObject::getDescriptor ()
{
  return descriptor;
}

string
NclExecutionObject::getId ()
{
  return id;
}

NclCompositeExecutionObject *
NclExecutionObject::getParentObject ()
{
  return getParentObject (dataObject);
}

NclCompositeExecutionObject *
NclExecutionObject::getParentObject (Node *node)
{
  Node *parentNode;
  NclCompositeExecutionObject *parentObj = NULL;

  map<Node *, Node *>::iterator i;
  map<Node *, NclCompositeExecutionObject *>::iterator j;

  i = nodeParentTable.find (node);
  if (i != nodeParentTable.end ())
    {
      parentNode = i->second;
      j = parentTable.find (parentNode);
      if (j != parentTable.end ())
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
  addParentObject (dataObject, parentObject, parentNode);
}

void
NclExecutionObject::addParentObject (Node *node,
                                     NclCompositeExecutionObject *parentObject,
                                     Node *parentNode)
{
  nodeParentTable[node] = parentNode;
  parentTable[parentNode] = parentObject;
}

void
NclExecutionObject::removeParentObject (Node *parentNode,
                                        NclCompositeExecutionObject *parentObject)
{
  map<Node *, NclCompositeExecutionObject *>::iterator i;

  i = parentTable.find (parentNode);
  if (i != parentTable.end () && i->second == parentObject)
    {
      if (wholeContent != NULL)
        {
          wholeContent->removeEventListener (parentObject);
        }
      parentTable.erase (i);
    }
}

void
NclExecutionObject::setDescriptor (
    NclCascadingDescriptor *cascadingDescriptor)
{
  this->descriptor = cascadingDescriptor;
}

void
NclExecutionObject::setDescriptor (GenericDescriptor *descriptor)
{
  NclCascadingDescriptor *cascade;
  cascade = new NclCascadingDescriptor (descriptor);

  if (this->descriptor != NULL)
    {
      delete this->descriptor;
    }

  this->descriptor = cascade;
}

bool
NclExecutionObject::addEvent (NclFormatterEvent *event)
{
  map<string, NclFormatterEvent *>::iterator i;

  i = events.find (event->getId ());
  if (i != events.end ())
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

  events[event->getId ()] = event;
  if (event->instanceOf ("NclPresentationEvent"))
    {
      addPresentationEvent ((NclPresentationEvent *)event);
    }
  else if (event->instanceOf ("NclSelectionEvent"))
    {
      selectionEvents.insert (((NclSelectionEvent *)event));
    }
  else
    {
      otherEvents.push_back (event);
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
      presEvents.insert (presEvents.begin (), event);
      wholeContent = (NclPresentationEvent *)event;
    }
  else
    {
      begin = event->getBegin ();
      posBeg = 0;
      posEnd = (int)(presEvents.size () - 1);
      while (posBeg <= posEnd)
        {
          posMid = (posBeg + posEnd) / 2;
          auxEvent = (NclPresentationEvent *)(presEvents[posMid]);
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

      presEvents.insert ((presEvents.begin () + posBeg), event);
    }

  clog << "NclExecutionObject::addPresentationEvent in '";
  clog << getId () << "'. Call transition ";
  clog << "manager addPresentationEvent for '" << event->getId ();
  clog << "' begin = '" << event->getBegin () << "'; end = '";
  clog << event->getEnd () << "' position = '" << posBeg << "'" << endl;
  transMan->addPresentationEvent (event);
}

bool
NclExecutionObject::containsEvent (NclFormatterEvent *event)
{
  return (events.count (event->getId ()) != 0);
}

NclFormatterEvent *
NclExecutionObject::getEventFromAnchorId (const string &anchorId)
{
  map<string, NclFormatterEvent *>::iterator i;
  NclFormatterEvent *event;

  if (anchorId == "")
    {
      if (wholeContent != NULL)
        {
          return wholeContent;
        }
    }
  else
    {
      if (wholeContent != NULL)
        {
          if (NclFormatterEvent::hasNcmId (wholeContent, anchorId))
            {
              return wholeContent;
            }
        }

      i = events.begin ();
      clog << "NclExecutionObject::getEventFromAnchorId searching '";
      clog << anchorId << "' for '" << id;
      clog << "' with following events = ";
      while (i != events.end ())
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
  if (events.count (id) != 0)
    {
      ev = events[id];
      return ev;
    }
  return NULL;
}

vector<NclFormatterEvent *>
NclExecutionObject::getEvents ()
{
  vector<NclFormatterEvent *> eventsVector;
  for (const auto &i : events)
    {
      eventsVector.push_back (i.second);
    }
  return eventsVector;
}

bool
NclExecutionObject::hasSampleEvents ()
{
  map<string, NclFormatterEvent *>::iterator i;
  NclFormatterEvent *event;
  ContentAnchor *anchor;

  if (events.empty ())
    {
      return false;
    }

  i = events.begin ();
  while (i != events.end ())
    {
      event = i->second;
      if (event->instanceOf ("NclAnchorEvent"))
        {
          anchor = ((NclAnchorEvent *)event)->getAnchor ();
          if (anchor->instanceOf ("SampleIntervalAnchor"))
            {
              return true;
            }
        }
      ++i;
    }

  return false;
}

set<NclAnchorEvent *> *
NclExecutionObject::getSampleEvents ()
{
  set<NclAnchorEvent *> *eventsSet;
  map<string, NclFormatterEvent *>::iterator i;
  NclFormatterEvent *event;
  ContentAnchor *anchor;

  if (events.empty ())
    return NULL;

  eventsSet = new set<NclAnchorEvent *>;
  i = events.begin ();
  while (i != events.end ())
    {
      event = i->second;
      if (event->instanceOf ("NclAnchorEvent"))
        {
          anchor = ((NclAnchorEvent *)event)->getAnchor ();
          if (anchor->instanceOf ("SampleIntervalAnchor"))
            {
              eventsSet->insert ((NclAnchorEvent *)event);
            }
        }
      ++i;
    }

  return eventsSet;
}

NclPresentationEvent *
NclExecutionObject::getWholeContentPresentationEvent ()
{
  return wholeContent;
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
      for (i = presEvents.begin (); i != presEvents.end (); ++i)
        {
          if (*i == (NclPresentationEvent *)event)
            {
              presEvents.erase (i);
              break;
            }
        }
      transMan->removeEventTransition ((NclPresentationEvent *)event);
    }
  else if (event->instanceOf ("NclSelectionEvent"))
    {
      j = selectionEvents.find (((NclSelectionEvent *)event));
      if (j != selectionEvents.end ())
        {
          selectionEvents.erase (j);
        }
    }
  else
    {
      for (k = otherEvents.begin (); k != otherEvents.end (); ++k)
        {
          if (*k == event)
            {
              otherEvents.erase (k);
              break;
            }
        }
    }

  l = events.find (event->getId ());
  if (l != events.end ())
    events.erase (l);

  return true;
}

bool
NclExecutionObject::isCompiled ()
{
  return isItCompiled;
}

void
NclExecutionObject::setCompiled (bool status)
{
  isItCompiled = status;
}

void
NclExecutionObject::removeNode (Node *node)
{
  Node *parentNode;
  map<Node *, Node *>::iterator i;
  map<Node *, NclCompositeExecutionObject *>::iterator j;

  if (node != dataObject)
    {
      i = nodeParentTable.find (node);
      if (i != nodeParentTable.end ())
        {
          parentNode = i->second;
          nodeParentTable.erase (i);

          j = parentTable.find (parentNode);
          if (j != parentTable.end ())
            {
              parentTable.erase (j);
            }
        }
    }
}

vector<Node *>
NclExecutionObject::getNodes ()
{
  vector<Node *> nodes;
  if (!nodeParentTable.empty())
    {
      for (auto &i: nodeParentTable)
        {
          nodes.push_back (i.first);
        }

      if (nodeParentTable.count (dataObject) == 0)
        {
          nodes.push_back (dataObject);
        }
    }

  return nodes;
}

const vector<Anchor *> &
NclExecutionObject::getNCMAnchors ()
{
  g_assert_nonnull (dataObject);

  return dataObject->getAnchors ();
}

PropertyAnchor *
NclExecutionObject::getNCMProperty (const string &propertyName)
{
  PropertyAnchor *property = NULL;

  if (dataObject != NULL)
    {
      property = dataObject->getPropertyAnchor (propertyName);
    }

  return property;
}

NclNodeNesting *
NclExecutionObject::getNodePerspective ()
{
  return getNodePerspective (dataObject);
}

NclNodeNesting *
NclExecutionObject::getNodePerspective (Node *node)
{
  Node *parentNode;
  NclNodeNesting *perspective;
  NclCompositeExecutionObject *parentObject;
  map<Node *, NclCompositeExecutionObject *>::iterator i;

  if (nodeParentTable.count (node) == 0)
    {
      if (dataObject == node)
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
      parentNode = nodeParentTable[node];

      i = parentTable.find (parentNode);
      if (i != parentTable.end ())
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

vector<NclExecutionObject *> *
NclExecutionObject::getObjectPerspective ()
{
  return getObjectPerspective (dataObject);
}

vector<NclExecutionObject *> *
NclExecutionObject::getObjectPerspective (Node *node)
{
  Node *parentNode;
  vector<NclExecutionObject *> *perspective = NULL;
  NclCompositeExecutionObject *parentObject;
  map<Node *, Node *>::iterator i;
  map<Node *, NclCompositeExecutionObject *>::iterator j;

  i = nodeParentTable.find (node);
  if (i == nodeParentTable.end ())
    {
      if (dataObject == node)
        {
          perspective = new vector<NclExecutionObject *>;
        }
      else
        {
          return NULL;
        }
    }
  else
    {
      parentNode = i->second;
      j = parentTable.find (parentNode);
      if (j != parentTable.end ())
        {
          parentObject = j->second;

          perspective = parentObject->getObjectPerspective (parentNode);
        }
      else
        {
          return NULL;
        }
    }
  perspective->push_back (this);
  return perspective;
}

vector<Node *> *
NclExecutionObject::getParentNodes ()
{
  vector<Node *> *parents;
  map<Node *, Node *>::iterator i;

  if (nodeParentTable.empty ())
    {
      return NULL;
    }

  parents = new vector<Node *>;
  for (i = nodeParentTable.begin (); i != nodeParentTable.end (); ++i)
    {
      parents->push_back (i->second);
    }

  return parents;
}

NclFormatterEvent *
NclExecutionObject::getMainEvent ()
{
  return mainEvent;
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

  if (mainEvent != NULL
      && mainEvent->getCurrentState () != EventUtil::ST_SLEEPING)
    {
      return false;
    }

  mainEvent = event;
  if (mainEvent->instanceOf ("NclAnchorEvent"))
    {
      contentAnchor = ((NclAnchorEvent *)mainEvent)->getAnchor ();
      if (contentAnchor != NULL
          && contentAnchor->instanceOf ("LabeledAnchor"))
        {
          i = parentTable.begin ();
          while (i != parentTable.end ())
            {
              clog << "NclExecutionObject::prepare(" << id << ") call ";
              clog << "addEventListener '" << i->second << "' or '";
              clog << i->second;
              clog << "'" << endl;
              // register parent as a mainEvent listener
              mainEvent->addEventListener (
                  (INclEventListener *)(NclCompositeExecutionObject *)
                      i->second);
              ++i;
            }
          return true;
        }
    }

  if (mainEvent->instanceOf ("NclPresentationEvent"))
    {
      startTime
          = ((NclPresentationEvent *)mainEvent)->getBegin () + offsetTime;

      if (startTime > ((NclPresentationEvent *)mainEvent)->getEnd ())
        {
          return false;
        }
    }

  i = parentTable.begin ();
  while (i != parentTable.end ())
    {
      clog << "NclExecutionObject::prepare(" << id << ") 2nd call ";
      clog << "addEventListener '" << i->second << "' or '";
      clog << i->second;
      clog << "'" << endl;
      // register parent as a mainEvent listener
      mainEvent->addEventListener (i->second);
      ++i;
    }

  prepareTransitionEvents (ContentAnchor::CAT_TIME, startTime);

  size = (int) otherEvents.size ();
  for (j = 0; j < size; j++)
    {
      auxEvent = otherEvents[j];
      if (auxEvent->instanceOf ("NclAttributionEvent"))
        {
          attributeEvent = (NclAttributionEvent *)auxEvent;
          attributeAnchor = attributeEvent->getAnchor ();
          value = attributeAnchor->getPropertyValue ();
          if (value != "")
            {
              attributeEvent->setValue (value);
            }
        }
    }

  this->offsetTime = startTime;
  return true;
}

bool
NclExecutionObject::start ()
{
  ContentAnchor *contentAnchor;

  clog << "NclExecutionObject::start(" << id << ")" << endl;
  if (mainEvent == NULL && wholeContent == NULL)
    {
      return false;
    }

  if (mainEvent != NULL
      && mainEvent->getCurrentState () != EventUtil::ST_SLEEPING)
    {
      return true;
    }

  if (mainEvent == NULL)
    {
      prepare (wholeContent, 0.0);
    }

  if (mainEvent != NULL && mainEvent->instanceOf ("NclAnchorEvent"))
    {
      contentAnchor = ((NclAnchorEvent *)mainEvent)->getAnchor ();
      if (contentAnchor != NULL
          && contentAnchor->instanceOf ("LabeledAnchor"))
        {
          transMan->start (offsetTime);
          mainEvent->start ();
          return true;
        }
    }

  transMan->start (offsetTime);
  return true;
}

void
NclExecutionObject::updateTransitionTable (GingaTime value, Player *player,
                                           short int transType)
{
  transMan->updateTransitionTable (value, player, mainEvent, transType);
}

void
NclExecutionObject::resetTransitionEvents (short int transType)
{
  transMan->resetTimeIndexByType (transType);
}

void
NclExecutionObject::prepareTransitionEvents (short int transType,
                                             GingaTime startTime)
{
  transMan->prepare (mainEvent == wholeContent, startTime, transType);
}

NclEventTransition *
NclExecutionObject::getNextTransition ()
{
  if (isSleeping () || !mainEvent->instanceOf ("NclPresentationEvent"))
    {
      return NULL;
    }
  return transMan->getNextTransition (mainEvent);
}

bool
NclExecutionObject::stop ()
{
  ContentAnchor *contentAnchor;
  GingaTime endTime;

  if (isSleeping ())
    {
      clog << "NclExecutionObject::stop for '" << id << "'";
      clog << " returns false because mainEvent is SLEEPING" << endl;
      return false;
    }

  if (mainEvent->instanceOf ("NclPresentationEvent"))
    {
      endTime = ((NclPresentationEvent *)mainEvent)->getEnd ();
      transMan->stop (endTime);
    }
  else if (mainEvent->instanceOf ("NclAnchorEvent"))
    {
      contentAnchor = ((NclAnchorEvent *)mainEvent)->getAnchor ();
      if (contentAnchor != NULL
          && contentAnchor->instanceOf ("LabeledAnchor"))
        {
          /*clog << "NclExecutionObject::stop for '" << id << "'";
          clog << " call mainEvent '" << mainEvent->getId();
          clog << "' stop" << endl;*/
          mainEvent->stop ();
        }
    }

  transMan->resetTimeIndex ();
  pauseCount = 0;
  return true;
}

bool
NclExecutionObject::abort ()
{
  ContentAnchor *contentAnchor;
  GingaTime endTime;

  if (isSleeping ())
    return false;

  if (mainEvent->instanceOf ("NclPresentationEvent"))
    {
      endTime = ((NclPresentationEvent *)mainEvent)->getEnd ();
      transMan->abort (endTime);
    }
  else if (mainEvent->instanceOf ("NclAnchorEvent"))
    {
      contentAnchor = ((NclAnchorEvent *)mainEvent)->getAnchor ();
      if (contentAnchor != NULL
          && contentAnchor->instanceOf ("LabeledAnchor"))
        {
          mainEvent->abort ();
        }
    }

  transMan->resetTimeIndex ();
  pauseCount = 0;
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
  if (pauseCount == 0)
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

  pauseCount++;
  return true;
}

bool
NclExecutionObject::resume ()
{
  NclFormatterEvent *event;
  vector<NclFormatterEvent *>::iterator i;

  // clog << "NclExecutionObject::resume(" << id << ")" << endl;
  if (pauseCount == 0)
    {
      return false;
    }
  else
    {
      pauseCount--;
      if (pauseCount > 0)
        {
          return false;
        }
    }

  vector<NclFormatterEvent *> evs = getEvents ();
  if (pauseCount == 0)
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
NclExecutionObject::setPropertyValue (NclAttributionEvent *event,
                                      const string &value)
{
  string propName;

  NclFormatterRegion *region = NULL;
  LayoutRegion *ncmRegion = NULL;
  bool hasProp = true;
  vector<string> params;
  string left = "", top = "";
  string width = "", height = "";
  string right = "", bottom = "";

  if (descriptor == NULL || descriptor->getFormatterRegion () == NULL)
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

  region = descriptor->getFormatterRegion ();
  ncmRegion = region->getLayoutRegion ();
  if (ncmRegion == NULL)
    {
      clog << "NclExecutionObject::setPropertyValue : The ncmRegion ";
      clog << " is NULL (PROBLEM)!" << endl;
      return false;
    }

  propName = (event->getAnchor ())->getPropertyName ();

  if (propName == "zIndex")
    {
      region->setZIndex (xstrtoint (value, 10));
      return true;
    }

  params = xstrsplit (xstrchomp (value), ',');

  if (propName == "size")
    {
      if (params.size () == 2)
        {
          width = xstrchomp (params[0]);
          height = xstrchomp (params[1]);
        }
      else
        {
          hasProp = false;
        }
    }
  else if (propName == "location")
    {
      if (params.size () == 2)
        {
          left = params[0];
          top = params[1];
        }
      else
        {
          hasProp = false;
        }
    }
  else if (propName == "bounds")
    {
      if (params.size () == 4)
        {
          left = params[0];
          top = params[1];
          width = params[2];
          height = params[3];
        }
      else
        {
          hasProp = false;
        }
    }
  else if (propName == "left")
    {
      left = params[0];
    }
  else if (propName == "top")
    {
      top = params[0];
    }
  else if (propName == "width")
    {
      width = params[0];
    }
  else if (propName == "height")
    {
      height = params[0];
    }
  else if (propName == "bottom")
    {
      bottom = params[0];
    }
  else if (propName == "right")
    {
      right = params[0];
    }
  else
    {
      hasProp = false;
    }

  if (hasProp)
    {
      // set the values
      region->updateRegionBounds ();
      return true;
    }

  return false;
}

string
NclExecutionObject::getPropertyValue (const string &param)
{
  NclFormatterRegion *region = NULL;
  LayoutRegion *ncmRegion = NULL;
  string value = "";

  if (descriptor == NULL || descriptor->getFormatterRegion () == NULL)
    {
      clog << "NclExecutionObject::setPropertyValue : setPropertyValue could ";
      clog << "not be performed. Descriptor or formatterRegion is NULL";
      clog << endl;
      return value;
    }

  region = descriptor->getFormatterRegion ();
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
  if (mainEvent == NULL
      || mainEvent->getCurrentState () != EventUtil::ST_SLEEPING)
    {
      clog << "NclExecutionObject::unprepare(" << id << ") unlocked";
      clog << " ret FALSE" << endl;
      return false;
    }

  removeParentListenersFromEvent (mainEvent);
  mainEvent = NULL;
  return true;
}

void
NclExecutionObject::setHandling (bool isHandling)
{
  this->isHandling = isHandling;
}

void
NclExecutionObject::setHandler (bool isHandler)
{
  this->isHandler = isHandler;
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

   if ((!isHandling && !isHandler) || sleeping /*|| paused*/)
    {
      clog << "NclExecutionObject::selectionEvent Can't receive event on '";
      clog << getId () << "': isHandling = '" << isHandling << "' ";
      clog << "isHandler = '" << isHandler << "' ";
      clog << "isSleeping() = '" << sleeping << "' ";
      clog << "isPaused() = '" << paused << "' ";
      clog << endl;
      return false;
    }

  if (selectionEvents.empty ())
    {
      clog << "NclExecutionObject::selectionEvent Can't receive event on '";
      clog << getId () << "': selection events is empty";
      clog << endl;
      return false;
    }

  selectedEvents = new set<NclSelectionEvent *>;
  i = selectionEvents.begin ();
  while (i != selectionEvents.end ())
    {
      selectionEvent = (NclSelectionEvent *)(*i);
      selCode = selectionEvent->getSelectionCode ();

      clog << "NclExecutionObject::selectionEvent(" << id << ") event '";
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

              clog << "NclExecutionObject::selectionEvent(" << id << ") ";
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
                  clog << id << ")";
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
                  clog << "NclExecutionObject::selectionEvent(" << id
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

      if (seListener != NULL)
        {
          clog << "NclExecutionObject::selectionEvent(" << id << ")";
          clog << " calling scheduler to execute fake action";
          clog << endl;

          fakeAct = new NclLinkSimpleAction (selectionEvent, ACT_START);

          seListener->scheduleAction (fakeAct);
        }

      ++i;
    }

  delete selectedEvents;
  selectedEvents = NULL;

  return selected;
}

GINGA_FORMATTER_END
