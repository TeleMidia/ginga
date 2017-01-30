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

GINGA_FORMATTER_BEGIN

bool NclExecutionObject::initMutex = false;
set<NclExecutionObject *> NclExecutionObject::objects;
pthread_mutex_t NclExecutionObject::_objMutex;

void
NclExecutionObject::addInstance (NclExecutionObject *object)
{
  Thread::mutexLock (&_objMutex);
  objects.insert (object);
  Thread::mutexUnlock (&_objMutex);
}

bool
NclExecutionObject::removeInstance (NclExecutionObject *object)
{
  set<NclExecutionObject *>::iterator i;
  bool removed = false;

  Thread::mutexLock (&_objMutex);
  i = objects.find (object);
  if (i != objects.end ())
    {
      objects.erase (i);
      removed = true;
    }
  Thread::mutexUnlock (&_objMutex);

  return removed;
}

bool
NclExecutionObject::hasInstance (NclExecutionObject *object,
                                 bool eraseFromList)
{
  set<NclExecutionObject *>::iterator i;
  bool hasObject;

  if (!initMutex)
    {
      return false;
    }

  Thread::mutexLock (&_objMutex);
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
  Thread::mutexUnlock (&_objMutex);

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
  map<Node *, void *>::iterator j;

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

  lockParentTable ();
  i = nodeParentTable.begin ();
  while (i != nodeParentTable.end ())
    {
      parentNode = i->second;
      j = parentTable.find (parentNode);
      if (j != parentTable.end ())
        {
          parentObject = (NclCompositeExecutionObject *)(j->second);

          parentObject->removeExecutionObject (this);
        }
      ++i;
    }

  nodeParentTable.clear ();

  parentTable.clear ();
  unlockParentTable ();

  if (descriptor != NULL)
    {
      delete descriptor;
      descriptor = NULL;
    }
  unlock ();
  Thread::mutexDestroy (&mutex);
  Thread::mutexDestroy (&mutexEvent);
  Thread::mutexDestroy (&mutexParentTable);
}

void
NclExecutionObject::initializeExecutionObject (
    const string &id, Node *node, NclCascadingDescriptor *descriptor,
    bool handling, INclLinkActionListener *seListener)
{
  typeSet.insert ("NclExecutionObject");

  if (!initMutex)
    {
      initMutex = true;
      Thread::mutexInit (&NclExecutionObject::_objMutex, false);
    }

  addInstance (this);

  this->seListener = seListener;
  this->deleting = false;
  this->id = id;
  this->dataObject = node;
  this->wholeContent = NULL;
  this->startTime = infinity ();
  this->descriptor = NULL;

  this->isItCompiled = false;

  this->pauseCount = 0;
  this->mainEvent = NULL;
  this->descriptor = descriptor;
  this->isLocked = false;
  this->isHandler = false;
  this->isHandling = handling;
  this->mirrorSrc = NULL;

  this->transMan = new NclEventTransitionManager ();

  Thread::mutexInit (&mutex, false);
  Thread::mutexInit (&mutexEvent, false);
  Thread::mutexInit (&mutexParentTable, false);
}

void
NclExecutionObject::destroyEvents ()
{
  map<string, NclFormatterEvent *>::iterator i;
  NclFormatterEvent *event;

  lockEvents ();
  i = events.begin ();
  while (i != events.end ())
    {
      event = i->second;
      events.erase (i);
      unlockEvents ();
      if (NclFormatterEvent::hasInstance (event, true))
        {
          delete event;
          event = NULL;
        }
      lockEvents ();
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

  unlockEvents ();
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
  map<Node *, void *>::iterator i;
  NclCompositeExecutionObject *parentObject;

  if (NclFormatterEvent::hasInstance (event, false))
    {
      lockParentTable ();
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
      unlockParentTable ();
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

int
NclExecutionObject::compareToUsingId (NclExecutionObject *object)
{
  return id.compare (object->getId ());
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

NclExecutionObject *
NclExecutionObject::getMirrorSrc ()
{
  return mirrorSrc;
}

void
NclExecutionObject::setMirrorSrc (NclExecutionObject *mirrorSrc)
{
  this->mirrorSrc = mirrorSrc;
}

void *
NclExecutionObject::getParentObject ()
{
  return getParentObject (dataObject);
}

void *
NclExecutionObject::getParentObject (Node *node)
{
  Node *parentNode;
  void *parentObj = NULL;

  map<Node *, Node *>::iterator i;
  map<Node *, void *>::iterator j;

  lockParentTable ();
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
  unlockParentTable ();

  return parentObj;
}

void
NclExecutionObject::addParentObject (void *parentObject, Node *parentNode)
{
  addParentObject (dataObject, parentObject, parentNode);
}

void
NclExecutionObject::addParentObject (Node *node, void *parentObject,
                                     Node *parentNode)
{
  lockParentTable ();
  nodeParentTable[node] = parentNode;
  parentTable[parentNode] = parentObject;
  unlockParentTable ();
}

void
NclExecutionObject::removeParentObject (Node *parentNode,
                                        void *parentObject)
{
  map<Node *, void *>::iterator i;

  lockParentTable ();
  i = parentTable.find (parentNode);
  if (i != parentTable.end () && i->second == parentObject)
    {
      if (wholeContent != NULL)
        {
          wholeContent->removeEventListener (
              (NclCompositeExecutionObject *)parentObject);
        }
      parentTable.erase (i);
    }
  unlockParentTable ();
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

string
NclExecutionObject::toString ()
{
  return id;
}

bool
NclExecutionObject::addEvent (NclFormatterEvent *event)
{
  map<string, NclFormatterEvent *>::iterator i;

  lockEvents ();
  i = events.find (event->getId ());
  if (i != events.end ())
    {
      clog << "NclExecutionObject::addEvent Warning! Trying to add ";
      clog << "the same event '" << event->getId () << "' twice";
      clog << " current event address '" << i->second << "' ";
      clog << " addEvent address '" << event << "'";
      clog << endl;

      unlockEvents ();
      return false;
    }

  clog << "NclExecutionObject::addEvent '" << event->getId () << "' in '";
  clog << getId () << "'" << endl;

  events[event->getId ()] = event;
  unlockEvents ();
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
  double begin, auxBegin;
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

      // undefined events are not inserted into transition table
      if (NclPresentationEvent::isUndefinedInstant (begin))
        {
          clog << "NclExecutionObject::addPresentationEvent event '";
          clog << event->getId () << "' has an undefined begin instant '";
          clog << begin << "'" << endl;
          return;
        }

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

int
NclExecutionObject::compareTo (NclExecutionObject *object)
{
  int ret;

  ret = compareToUsingStartTime (object);
  if (ret == 0)
    return compareToUsingId (object);
  else
    return ret;
}

int
NclExecutionObject::compareToUsingStartTime (NclExecutionObject *object)
{
  double thisTime, otherTime;

  thisTime = startTime;
  otherTime = (object->getExpectedStartTime ());

  if (thisTime < otherTime)
    {
      return -1;
    }
  else if (thisTime > otherTime)
    {
      return 1;
    }

  return 0;
}

bool
NclExecutionObject::containsEvent (NclFormatterEvent *event)
{
  bool contains;

  lockEvents ();
  contains = (events.count (event->getId ()) != 0);
  unlockEvents ();

  return contains;
}

NclFormatterEvent *
NclExecutionObject::getEventFromAnchorId (const string &anchorId)
{
  map<string, NclFormatterEvent *>::iterator i;
  NclFormatterEvent *event;

  lockEvents ();
  if (anchorId == "")
    {
      if (wholeContent != NULL)
        {
          unlockEvents ();
          return wholeContent;
        }
    }
  else
    {
      if (wholeContent != NULL)
        {
          if (NclFormatterEvent::hasNcmId (wholeContent, anchorId))
            {
              unlockEvents ();
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
                  unlockEvents ();
                  return event;
                }
            }
          ++i;
        }
      clog << endl;
    }

  unlockEvents ();
  return NULL;
}

NclFormatterEvent *
NclExecutionObject::getEvent (const string &id)
{
  NclFormatterEvent *ev;
  lockEvents ();
  if (events.count (id) != 0)
    {
      ev = events[id];
      unlockEvents ();
      return ev;
    }

  unlockEvents ();
  return NULL;
}

vector<NclFormatterEvent *> *
NclExecutionObject::getEvents ()
{
  vector<NclFormatterEvent *> *eventsVector = NULL;
  map<string, NclFormatterEvent *>::iterator i;

  lockEvents ();
  if (events.empty ())
    {
      unlockEvents ();
      return NULL;
    }

  eventsVector = new vector<NclFormatterEvent *>;
  for (i = events.begin (); i != events.end (); ++i)
    {
      eventsVector->push_back (i->second);
    }
  unlockEvents ();

  return eventsVector;
}

bool
NclExecutionObject::hasSampleEvents ()
{
  map<string, NclFormatterEvent *>::iterator i;
  NclFormatterEvent *event;
  ContentAnchor *anchor;

  lockEvents ();
  if (events.empty ())
    {
      unlockEvents ();
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
              unlockEvents ();
              return true;
            }
        }
      ++i;
    }
  unlockEvents ();

  return false;
}

set<NclAnchorEvent *> *
NclExecutionObject::getSampleEvents ()
{
  set<NclAnchorEvent *> *eventsSet;
  map<string, NclFormatterEvent *>::iterator i;
  NclFormatterEvent *event;
  ContentAnchor *anchor;

  lockEvents ();
  if (events.empty ())
    {
      unlockEvents ();
      return NULL;
    }

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
  unlockEvents ();

  return eventsSet;
}

double
NclExecutionObject::getExpectedStartTime ()
{
  return startTime;
}

NclPresentationEvent *
NclExecutionObject::getWholeContentPresentationEvent ()
{
  return wholeContent;
}

void
NclExecutionObject::setStartTime (double t)
{
  startTime = t;
}

void
NclExecutionObject::updateEventDurations ()
{
  vector<NclPresentationEvent *>::iterator i;

  i = presEvents.begin ();
  while (i != presEvents.end ())
    {
      updateEventDuration (*i);
      ++i;
    }
}

void
NclExecutionObject::updateEventDuration (NclPresentationEvent *event)
{
  double duration;

  if (!containsEvent ((NclFormatterEvent *)event))
    {
      return;
    }

  duration = NaN ();

  if (descriptor != NULL)
    {
      if (descriptor->instanceOf ("NclCascadingDescriptor"))
        {
          if (!isNaN (descriptor->getExplicitDuration ())
              && event == wholeContent)
            {
              duration = descriptor->getExplicitDuration ();
            }
          else if (event->getDuration () > 0)
            {
              duration = event->getDuration ();
            }
          else
            {
              duration = 0;
            }
        }
    }
  else
    {
      if (event->getDuration () > 0)
        {
          duration = event->getDuration ();
        }
      else
        {
          duration = 0;
        }
    }

  if (duration < 0)
    {
      event->setDuration (NaN ());
    }
  else
    {
      event->setDuration (duration);
    }
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

  lockEvents ();
  l = events.find (event->getId ());
  if (l != events.end ())
    {
      events.erase (l);
    }
  unlockEvents ();
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
  map<Node *, void *>::iterator j;

  if (node != dataObject)
    {
      lockParentTable ();
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
      unlockParentTable ();
    }
}

vector<Node *> *
NclExecutionObject::getNodes ()
{
  vector<Node *> *nodes;
  map<Node *, Node *>::iterator i;

  lockParentTable ();
  if (nodeParentTable.empty ())
    {
      unlockParentTable ();
      return NULL;
    }

  nodes = new vector<Node *>;
  for (i = nodeParentTable.begin (); i != nodeParentTable.end (); ++i)
    {
      nodes->push_back (i->first);
    }

  if (nodeParentTable.count (dataObject) == 0)
    {
      nodes->push_back (dataObject);
    }
  unlockParentTable ();

  return nodes;
}

vector<Anchor *> *
NclExecutionObject::getNCMAnchors ()
{
  vector<Anchor *> *anchors = NULL;

  if (dataObject != NULL)
    {
      anchors = dataObject->getAnchors ();
    }

  return anchors;
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
  map<Node *, void *>::iterator i;

  lockParentTable ();
  if (nodeParentTable.count (node) == 0)
    {
      if (dataObject == node)
        {
          perspective = new NclNodeNesting ();
        }
      else
        {
          unlockParentTable ();
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
          unlockParentTable ();
          return NULL;
        }
    }
  perspective->insertAnchorNode (node);
  unlockParentTable ();
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
  map<Node *, void *>::iterator j;

  lockParentTable ();
  i = nodeParentTable.find (node);
  if (i == nodeParentTable.end ())
    {
      if (dataObject == node)
        {
          perspective = new vector<NclExecutionObject *>;
        }
      else
        {
          unlockParentTable ();
          return NULL;
        }
    }
  else
    {
      parentNode = i->second;
      j = parentTable.find (parentNode);
      if (j != parentTable.end ())
        {
          parentObject = (NclCompositeExecutionObject *)(j->second);

          perspective = parentObject->getObjectPerspective (parentNode);
        }
      else
        {
          unlockParentTable ();
          return NULL;
        }
    }
  unlockParentTable ();

  perspective->push_back (this);
  return perspective;
}

vector<Node *> *
NclExecutionObject::getParentNodes ()
{
  vector<Node *> *parents;
  map<Node *, Node *>::iterator i;

  lockParentTable ();
  if (nodeParentTable.empty ())
    {
      unlockParentTable ();
      return NULL;
    }

  parents = new vector<Node *>;
  for (i = nodeParentTable.begin (); i != nodeParentTable.end (); ++i)
    {
      parents->push_back (i->second);
    }

  unlockParentTable ();
  return parents;
}

NclFormatterEvent *
NclExecutionObject::getMainEvent ()
{
  return mainEvent;
}

bool
NclExecutionObject::prepare (NclFormatterEvent *event, double offsetTime)
{
  int size;
  map<Node *, void *>::iterator i;
  double startTime = 0;
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

  if (!lock ())
    {
      return false;
    }

  // clog << "NclExecutionObject::prepare(" << id << ") locked" << endl;
  mainEvent = event;
  if (mainEvent->instanceOf ("NclAnchorEvent"))
    {
      contentAnchor = ((NclAnchorEvent *)mainEvent)->getAnchor ();
      if (contentAnchor != NULL
          && contentAnchor->instanceOf ("LabeledAnchor"))
        {
          lockParentTable ();
          i = parentTable.begin ();
          while (i != parentTable.end ())
            {
              clog << "NclExecutionObject::prepare(" << id << ") call ";
              clog << "addEventListener '" << i->second << "' or '";
              clog << (INclEventListener *)(NclCompositeExecutionObject *)
                          i->second;
              clog << "'" << endl;
              // register parent as a mainEvent listener
              mainEvent->addEventListener (
                  (INclEventListener *)(NclCompositeExecutionObject *)
                      i->second);
              ++i;
            }
          unlockParentTable ();
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

  lockParentTable ();
  i = parentTable.begin ();
  while (i != parentTable.end ())
    {
      clog << "NclExecutionObject::prepare(" << id << ") 2nd call ";
      clog << "addEventListener '" << i->second << "' or '";
      clog << (INclEventListener *)(NclCompositeExecutionObject *)i->second;
      clog << "'" << endl;
      // register parent as a mainEvent listener
      mainEvent->addEventListener (
          (INclEventListener *)(NclCompositeExecutionObject *)i->second);
      ++i;
    }
  unlockParentTable ();

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
NclExecutionObject::timeBaseNaturalEnd (int64_t timeValue,
                                        short int transType)
{
  transMan->timeBaseNaturalEnd (timeValue, mainEvent, transType);
}

void
NclExecutionObject::updateTransitionTable (double value, IPlayer *player,
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
                                             double startTime)
{
  transMan->prepare (mainEvent == wholeContent, startTime, transType);
}

set<double> *
NclExecutionObject::getTransitionsValues (short int transType)
{
  return transMan->getTransitionsValues (transType);
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
  double endTime;

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
  double endTime;

  // clog << "NclExecutionObject::abort(" << id << ")" << endl;
  if (isSleeping ())
    {
      return false;
    }

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
  vector<NclFormatterEvent *> *evs;
  vector<NclFormatterEvent *>::iterator i;

  // clog << "NclExecutionObject::pause(" << id << ")" << endl;
  if (isSleeping ())
    {
      return false;
    }

  evs = getEvents ();
  if (evs != NULL)
    {
      if (pauseCount == 0)
        {
          i = evs->begin ();
          while (i != evs->end ())
            {
              event = *i;
              if (event->getCurrentState () == EventUtil::ST_OCCURRING)
                {
                  event->pause ();
                }
              ++i;
            }
        }
      delete evs;
      evs = NULL;
    }

  pauseCount++;
  return true;
}

bool
NclExecutionObject::resume ()
{
  NclFormatterEvent *event;
  vector<NclFormatterEvent *> *evs;
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

  evs = getEvents ();
  if (evs != NULL)
    {
      if (pauseCount == 0)
        {
          i = evs->begin ();
          while (i != evs->end ())
            {
              event = *i;
              if (event->getCurrentState () == EventUtil::ST_PAUSED)
                {
                  event->resume ();
                }
              ++i;
            }
        }
      delete evs;
      evs = NULL;
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
  vector<string> *params;
  string left = "", top = "";
  string width = "", height = "";
  string right = "", bottom = "";

  if (descriptor == NULL || descriptor->getFormatterRegion () == NULL)
    {
      clog << "NclExecutionObject::setPropertyValue : setProperty could ";
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

  lock (); // lock the object

  if (propName == "zIndex")
    {
      region->setZIndex (xstrto_int (value));
      unlock ();
      return true;
    }

  params = split (xstrchomp (value), ",");

  if (propName == "size")
    {
      if (params->size () == 2)
        {
          width = (*params)[0];
          height = (*params)[1];
        }
      else
        {
          hasProp = false;
        }
    }
  else if (propName == "location")
    {
      if (params->size () == 2)
        {
          left = (*params)[0];
          top = (*params)[1];
        }
      else
        {
          hasProp = false;
        }
    }
  else if (propName == "bounds")
    {
      if (params->size () == 4)
        {
          left = (*params)[0];
          top = (*params)[1];
          width = (*params)[2];
          height = (*params)[3];
        }
      else
        {
          hasProp = false;
        }
    }
  else if (propName == "left")
    {
      left = (*params)[0];
    }
  else if (propName == "top")
    {
      top = (*params)[0];
    }
  else if (propName == "width")
    {
      width = (*params)[0];
    }
  else if (propName == "height")
    {
      height = (*params)[0];
    }
  else if (propName == "bottom")
    {
      bottom = (*params)[0];
    }
  else if (propName == "right")
    {
      right = (*params)[0];
    }
  else
    {
      hasProp = false;
    }

  if (hasProp)
    {
      if (left != "")
        {
          if (isPercentualValue (left))
            {
              ncmRegion->setTargetLeft (getPercentualValue (left), true);
            }
          else
            {
              ncmRegion->setTargetLeft (xstrtod (left), false);
            }
        }

      if (top != "")
        {
          if (isPercentualValue (top))
            {
              ncmRegion->setTargetTop (getPercentualValue (top), true);
            }
          else
            {
              ncmRegion->setTargetTop (xstrtod (top), false);
            }
        }

      if (width != "")
        {
          if (isPercentualValue (width))
            {
              ncmRegion->setTargetWidth (getPercentualValue (width), true);
            }
          else
            {
              ncmRegion->setTargetWidth (xstrtod (width), false);
            }
        }

      if (height != "")
        {
          if (isPercentualValue (height))
            {
              ncmRegion->setTargetHeight (getPercentualValue (height),
                                          true);
            }
          else
            {
              ncmRegion->setTargetHeight (xstrtod (height), false);
            }
        }

      if (bottom != "")
        {
          if (isPercentualValue (bottom))
            {
              ncmRegion->setTargetBottom (getPercentualValue (bottom),
                                          true);
            }
          else
            {
              ncmRegion->setTargetBottom (xstrtod (bottom), false);
            }
        }

      if (right != "")
        {
          if (isPercentualValue (right))
            {
              ncmRegion->setTargetRight (getPercentualValue (right), true);
            }
          else
            {
              ncmRegion->setTargetRight (xstrtod (right), false);
            }
        }

      // set the values
      ncmRegion->validateTarget ();

      region->updateRegionBounds ();
      unlock ();
      delete params;
      params = NULL;
      return true;
    }
  unlock ();

  delete params;
  params = NULL;

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
      clog << "NclExecutionObject::setPropertyValue : setProperty could ";
      clog << "not be performed. Descriptor or formatterRegion is NULL";
      clog << endl;
      return value;
    }

  region = descriptor->getFormatterRegion ();
  ncmRegion = region->getLayoutRegion ();

  double current = 0.0;
  if (param == "left")
    {
      current = ncmRegion->getLeftInPixels ();
      xstrassign (value, "%d", (int) current);
    }
  else if (param == "top")
    {
      current = ncmRegion->getTopInPixels ();
      xstrassign (value, "%d", (int) current);
    }
  else if (param == "width")
    {
      current = ncmRegion->getWidthInPixels ();
      xstrassign (value, "%d", (int) current);
    }
  else if (param == "height")
    {
      current = ncmRegion->getHeightInPixels ();
      xstrassign (value, "%d", (int) current);
    }
  else if (param == "bottom")
    {
      current = ncmRegion->getBottomInPixels ();
      xstrassign (value, "%d", (int) current);
    }
  else if (param == "right")
    {
      current = ncmRegion->getRightInPixels ();
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
                  ncmRegion->getLeftInPixels (),
                  ncmRegion->getTopInPixels (),
                  ncmRegion->getWidthInPixels (),
                  ncmRegion->getHeightInPixels ());
    }
  else if (param == "location")
    {
      xstrassign (value, "%d,%d",
                  ncmRegion->getLeftInPixels (),
                  ncmRegion->getTopInPixels ());
    }
  else if (param == "size")
    {
      xstrassign (value, "%d,%d",
                  ncmRegion->getWidthInPixels (),
                  ncmRegion->getHeightInPixels ());
    }

  return value;
}

bool
NclExecutionObject::unprepare ()
{
  if (mainEvent == NULL
      || mainEvent->getCurrentState () != EventUtil::ST_SLEEPING)
    {
      unlock ();
      clog << "NclExecutionObject::unprepare(" << id << ") unlocked";
      clog << " ret FALSE" << endl;
      return false;
    }

  removeParentListenersFromEvent (mainEvent);

  mainEvent = NULL;
  unlock ();
  /*clog << "NclExecutionObject::unprepare(" << id << ") unlocked";
  clog << endl;*/
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

bool
NclExecutionObject::selectionEvent (int keyCode, double currentTime)
{
  int selCode;
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
  double intervalBegin;
  double intervalEnd;

  if ((!isHandling && !isHandler) || sleeping || paused)
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
      clog << "' (looking for key code '" << keyCode << "'" << endl;

      if (selCode == keyCode)
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

          fakeAct = new NclLinkSimpleAction (selectionEvent,
                                             SimpleAction::ACT_START);

          seListener->scheduleAction (NULL, fakeAct);
        }

      ++i;
    }

  delete selectedEvents;
  selectedEvents = NULL;

  return selected;
}

set<int> *
NclExecutionObject::getInputEvents ()
{
  set<NclSelectionEvent *>::iterator i;
  set<int> *evs;
  NclSelectionEvent *ev;
  int keyCode;

  evs = new set<int>;
  i = selectionEvents.begin ();
  while (i != selectionEvents.end ())
    {
      ev = (*i);
      keyCode = ev->getSelectionCode ();
      evs->insert (keyCode);
      if (keyCode == CodeMap::KEY_RED)
        {
          evs->insert (CodeMap::KEY_F1);
        }
      else if (keyCode == CodeMap::KEY_GREEN)
        {
          evs->insert (CodeMap::KEY_F2);
        }
      else if (keyCode == CodeMap::KEY_YELLOW)
        {
          evs->insert (CodeMap::KEY_F3);
        }
      else if (keyCode == CodeMap::KEY_BLUE)
        {
          evs->insert (CodeMap::KEY_F4);
        }
      else if (keyCode == CodeMap::KEY_MENU)
        {
          evs->insert (CodeMap::KEY_F5);
        }
      else if (keyCode == CodeMap::KEY_INFO)
        {
          evs->insert (CodeMap::KEY_F6);
        }
      else if (keyCode == CodeMap::KEY_EPG)
        {
          evs->insert (CodeMap::KEY_F7);
        }
      else if (keyCode == CodeMap::KEY_VOLUME_UP)
        {
          evs->insert (CodeMap::KEY_PLUS_SIGN);
        }
      else if (keyCode == CodeMap::KEY_VOLUME_DOWN)
        {
          evs->insert (CodeMap::KEY_MINUS_SIGN);
        }
      else if (keyCode == CodeMap::KEY_CHANNEL_UP)
        {
          evs->insert (CodeMap::KEY_PAGE_UP);
        }
      else if (keyCode == CodeMap::KEY_CHANNEL_DOWN)
        {
          evs->insert (CodeMap::KEY_PAGE_DOWN);
        }
      else if (keyCode == CodeMap::KEY_BACK)
        {
          evs->insert (CodeMap::KEY_BACKSPACE);
        }
      else if (keyCode == CodeMap::KEY_EXIT)
        {
          evs->insert (CodeMap::KEY_ESCAPE);
        }
      ++i;
    }

  return evs;
}

bool
NclExecutionObject::lock ()
{
  if (isLocked)
    {
      return false;
    }
  isLocked = true;
  Thread::mutexLock (&mutex);
  return true;
}

bool
NclExecutionObject::unlock ()
{
  if (!isLocked)
    {
      return false;
    }

  Thread::mutexUnlock (&mutex);
  isLocked = false;
  return true;
}

void
NclExecutionObject::lockEvents ()
{
  Thread::mutexLock (&mutexEvent);
}

void
NclExecutionObject::unlockEvents ()
{
  Thread::mutexUnlock (&mutexEvent);
}

void
NclExecutionObject::lockParentTable ()
{
  Thread::mutexLock (&mutexParentTable);
}

void
NclExecutionObject::unlockParentTable ()
{
  Thread::mutexUnlock (&mutexParentTable);
}

GINGA_FORMATTER_END
