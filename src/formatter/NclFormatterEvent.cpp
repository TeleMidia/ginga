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
#include "NclFormatterEvent.h"
#include "NclPresentationEvent.h"

#include "NclCompositeExecutionObject.h"

GINGA_FORMATTER_BEGIN

set<NclFormatterEvent *> NclFormatterEvent::instances;
bool NclFormatterEvent::init = false;
pthread_mutex_t NclFormatterEvent::iMutex;

NclFormatterEvent::NclFormatterEvent (string id, void *execObject)
{
  this->id = id;
  currentState = EventUtil::ST_SLEEPING;
  occurrences = 0;
  executionObject = execObject;
  deleting = false;
  eventType = -1;

  if (!init)
    {
      init = true;
      Thread::mutexInit (&iMutex, false);
    }

  typeSet.insert ("NclFormatterEvent");
  Thread::mutexInit (&mutex, false);

  addInstance (this);
}

NclFormatterEvent::~NclFormatterEvent ()
{
  deleting = true;

  removeInstance (this);

  destroyListeners ();
  Thread::mutexDestroy (&mutex);
}

bool
NclFormatterEvent::hasInstance (NclFormatterEvent *event, bool remove)
{
  set<NclFormatterEvent *>::iterator i;
  bool inst = false;

  if (!init)
    {
      return false;
    }

  Thread::mutexLock (&iMutex);
  i = instances.find (event);
  if (i != instances.end ())
    {
      if (remove)
        {
          instances.erase (i);
        }
      inst = true;
    }
  Thread::mutexUnlock (&iMutex);

  return inst;
}

void
NclFormatterEvent::addInstance (NclFormatterEvent *event)
{
  Thread::mutexLock (&iMutex);
  instances.insert (event);
  Thread::mutexUnlock (&iMutex);
}

bool
NclFormatterEvent::removeInstance (NclFormatterEvent *event)
{
  set<NclFormatterEvent *>::iterator i;
  bool inst = false;

  Thread::mutexLock (&iMutex);
  i = instances.find (event);
  if (i != instances.end ())
    {
      instances.erase (i);
      inst = true;
    }
  Thread::mutexUnlock (&iMutex);

  return inst;
}

bool
NclFormatterEvent::instanceOf (string s)
{
  if (typeSet.empty ())
    {
      return false;
    }
  else
    {
      return (typeSet.find (s) != typeSet.end ());
    }
}

bool
NclFormatterEvent::hasNcmId (NclFormatterEvent *event, string anchorId)
{
  Anchor *anchor;
  string anchorName = " ";

  if (event->instanceOf ("NclAnchorEvent"))
    {
      anchor = ((NclAnchorEvent *)event)->getAnchor ();
      if (anchor != NULL)
        {
          if (anchor->instanceOf ("IntervalAnchor"))
            {
              anchorName = anchor->getId ();
            }
          else if (anchor->instanceOf ("LabeledAnchor"))
            {
              anchorName = ((LabeledAnchor *)anchor)->getLabel ();
            }
          else if (anchor->instanceOf ("LambdaAnchor"))
            {
              anchorName = "";
            }

          if (anchorName == anchorId
              && !event->instanceOf ("NclSelectionEvent"))
            {

              return true;
            }
        }
    }
  else if (event->instanceOf ("NclAttributionEvent"))
    {
      anchor = ((NclAttributionEvent *)event)->getAnchor ();
      if (anchor != NULL)
        {
          anchorName = ((PropertyAnchor *)anchor)->getPropertyName ();
          if (anchorName == anchorId)
            {
              return true;
            }
        }
    }

  return false;
}

void
NclFormatterEvent::setEventType (short eventType)
{
  this->eventType = eventType;
}

short
NclFormatterEvent::getEventType ()
{
  return eventType;
}

void
NclFormatterEvent::destroyListeners ()
{
  Thread::mutexLock (&mutex);
  this->executionObject = NULL;

  // TODO, avoid to leave a link bind with an inconsistent event
  coreListeners.clear ();
  linksListeners.clear ();
  objectsListeners.clear ();

  Thread::mutexUnlock (&mutex);
}

void
NclFormatterEvent::setId (string id)
{
  this->id = id;
}

void
NclFormatterEvent::addEventListener (INclEventListener *listener)
{
  short pType = listener->getPriorityType ();

  Thread::mutexLock (&mutex);

  switch (pType)
    {
    case INclEventListener::PT_CORE:
      coreListeners.insert (listener);
      break;

    case INclEventListener::PT_LINK:
      linksListeners.insert (listener);
      break;

    case INclEventListener::PT_OBJECT:
      objectsListeners.insert (listener);
      break;

    default:
      g_assert_not_reached ();
    }

  Thread::mutexUnlock (&mutex);
}

bool
NclFormatterEvent::containsEventListener (INclEventListener *listener)
{
  Thread::mutexLock (&mutex);
  if (coreListeners.count (listener) != 0
      || linksListeners.count (listener) != 0
      || objectsListeners.count (listener) != 0)
    {

      Thread::mutexUnlock (&mutex);
      return true;
    }
  Thread::mutexUnlock (&mutex);
  return false;
}

void
NclFormatterEvent::removeEventListener (INclEventListener *listener)
{
  set<INclEventListener *>::iterator i;

  Thread::mutexLock (&mutex);
  i = coreListeners.find (listener);
  if (i != coreListeners.end ())
    {
      coreListeners.erase (i);
    }

  i = linksListeners.find (listener);
  if (i != linksListeners.end ())
    {
      linksListeners.erase (i);
    }

  i = objectsListeners.find (listener);
  if (i != objectsListeners.end ())
    {
      objectsListeners.erase (i);
    }

  Thread::mutexUnlock (&mutex);
}

short
NclFormatterEvent::getNewState (short transition)
{
  switch (transition)
    {
    case EventUtil::TR_STOPS:
      return EventUtil::ST_SLEEPING;

    case EventUtil::TR_STARTS:
    case EventUtil::TR_RESUMES:
      return EventUtil::ST_OCCURRING;

    case EventUtil::TR_PAUSES:
      return EventUtil::ST_PAUSED;

    case EventUtil::TR_ABORTS:
      return ST_ABORTED;

    default:
      return -1;
    }
}

short
NclFormatterEvent::getTransition (short newState)
{
  return getTransistion (currentState, newState);
}

bool
NclFormatterEvent::abort ()
{
  switch (currentState)
    {
    case EventUtil::ST_OCCURRING:
    case EventUtil::ST_PAUSED:
      return changeState (ST_ABORTED, EventUtil::TR_ABORTS);

    default:
      return false;
    }
}

bool
NclFormatterEvent::start ()
{
  switch (currentState)
    {
    case EventUtil::ST_SLEEPING:
      return changeState (EventUtil::ST_OCCURRING, EventUtil::TR_STARTS);
    default:
      return false;
    }
}

bool
NclFormatterEvent::stop ()
{
  switch (currentState)
    {
    case EventUtil::ST_OCCURRING:
    case EventUtil::ST_PAUSED:
      return changeState (EventUtil::ST_SLEEPING, EventUtil::TR_STOPS);
    default:
      return false;
    }
}

bool
NclFormatterEvent::pause ()
{
  switch (currentState)
    {
    case EventUtil::ST_OCCURRING:
      return changeState (EventUtil::ST_PAUSED, EventUtil::TR_PAUSES);

    default:
      return false;
    }
}

bool
NclFormatterEvent::resume ()
{
  switch (currentState)
    {
    case EventUtil::ST_PAUSED:
      return changeState (EventUtil::ST_OCCURRING, EventUtil::TR_RESUMES);

    default:
      return false;
    }
}

void
NclFormatterEvent::setCurrentState (short newState)
{
  previousState = currentState;
  currentState = newState;
}

bool
NclFormatterEvent::changeState (short newState, short transition)
{
  set<INclEventListener *>::iterator i;
  set<INclEventListener *> *coreClone, *linkClone, *objectClone;

  Thread::mutexLock (&mutex);

  if (transition == EventUtil::TR_STOPS)
    {
      occurrences++;
    }

  previousState = currentState;
  currentState = newState;

  if (deleting)
    {
      Thread::mutexUnlock (&mutex);
      return false;
    }

  coreClone = new set<INclEventListener *> (coreListeners);
  linkClone = new set<INclEventListener *> (linksListeners);
  objectClone = new set<INclEventListener *> (objectsListeners);

  Thread::mutexUnlock (&mutex);

  i = coreClone->begin ();
  while (i != coreClone->end ())
    {
      if (deleting)
        {
          break;
        }

      if (*i != NULL)
        {
          ((INclEventListener *)(*i))
              ->eventStateChanged ((void *)this, transition, previousState);
        }
      ++i;
    }

  i = linkClone->begin ();
  while (i != linkClone->end ())
    {
      if (deleting)
        {
          break;
        }

      if (*i != NULL)
        {
          ((INclEventListener *)(*i))
              ->eventStateChanged ((void *)this, transition, previousState);
        }
      ++i;
    }

  i = objectClone->begin ();
  while (i != objectClone->end ())
    {
      if (deleting)
        {
          break;
        }

      if (*i != NULL)
        {
          ((INclEventListener *)(*i))
              ->eventStateChanged ((void *)this, transition, previousState);
        }
      ++i;
    }

  coreClone->clear ();
  delete coreClone;
  coreClone = NULL;

  linkClone->clear ();
  delete linkClone;
  linkClone = NULL;

  objectClone->clear ();
  delete objectClone;
  objectClone = NULL;

  if (currentState == ST_ABORTED)
    {
      currentState = EventUtil::ST_SLEEPING;
    }

  return true;
}

short
NclFormatterEvent::getCurrentState ()
{
  return currentState;
}

short
NclFormatterEvent::getPreviousState ()
{
  return previousState;
}

short
NclFormatterEvent::getTransistion (short previousState, short newState)
{

  switch (previousState)
    {
    case EventUtil::ST_SLEEPING:
      switch (newState)
        {
        case EventUtil::ST_OCCURRING:
          return EventUtil::TR_STARTS;
        default:
          return -1;
        }
      break;

    case EventUtil::ST_OCCURRING:
      switch (newState)
        {
        case EventUtil::ST_SLEEPING:
          return EventUtil::TR_STOPS;
        case EventUtil::ST_PAUSED:
          return EventUtil::TR_PAUSES;
        case ST_ABORTED:
          return EventUtil::TR_ABORTS;
        default:
          return -1;
        }
      break;

    case EventUtil::ST_PAUSED:
      switch (newState)
        {
        case EventUtil::ST_OCCURRING:
          return EventUtil::TR_RESUMES;
        case EventUtil::ST_SLEEPING:
          return EventUtil::TR_STOPS;
        case ST_ABORTED:
          return EventUtil::TR_ABORTS;
        default:
          return -1;
        }
      break;

    default:
      break;
    }

  return -1;
}

void *
NclFormatterEvent::getExecutionObject ()
{
  return executionObject;
}

void
NclFormatterEvent::setExecutionObject (void *object)
{
  executionObject = object;
}

string
NclFormatterEvent::getId ()
{
  return id;
}

long
NclFormatterEvent::getOccurrences ()
{
  return occurrences;
}

string
NclFormatterEvent::getStateName (short state)
{
  switch (state)
    {
    case EventUtil::ST_OCCURRING:
      return "occurring";

    case EventUtil::ST_PAUSED:
      return "paused";

    case EventUtil::ST_SLEEPING:
      return "sleeping";

    default:
      return "";
    }
}

GINGA_FORMATTER_END
