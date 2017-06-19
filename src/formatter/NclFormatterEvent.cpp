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

#include "ExecutionObjectContext.h"
#include "ncl/ContentNode.h"

GINGA_FORMATTER_BEGIN

set<NclFormatterEvent *> NclFormatterEvent::instances;
bool NclFormatterEvent::init = false;

NclFormatterEvent::NclFormatterEvent (const string &id,
                                      ExecutionObject *execObject)
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
    }

  typeSet.insert ("NclFormatterEvent");
  addInstance (this);
}

NclFormatterEvent::~NclFormatterEvent ()
{
  deleting = true;

  removeInstance (this);

  destroyListeners ();
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

  i = instances.find (event);
  if (i != instances.end ())
    {
      if (remove)
        {
          instances.erase (i);
        }
      inst = true;
    }
  return inst;
}

void
NclFormatterEvent::addInstance (NclFormatterEvent *event)
{
  instances.insert (event);
}

bool
NclFormatterEvent::removeInstance (NclFormatterEvent *event)
{
  set<NclFormatterEvent *>::iterator i;
  bool inst = false;

  i = instances.find (event);
  if (i != instances.end ())
    {
      instances.erase (i);
      inst = true;
    }

  return inst;
}

bool
NclFormatterEvent::instanceOf (const string &s)
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
NclFormatterEvent::hasNcmId (NclFormatterEvent *event, const string &anchorId)
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
          anchorName = ((PropertyAnchor *)anchor)->getName ();
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
  this->executionObject = NULL;
  listeners.clear ();
}

void
NclFormatterEvent::setId (const string &id)
{
  this->id = id;
}

void
NclFormatterEvent::addEventListener (INclEventListener *listener)
{
  this->listeners.insert (listener);
}

bool
NclFormatterEvent::containsEventListener (INclEventListener *listener)
{
  if (listeners.count (listener) != 0)
    return true;
  return false;
}

void
NclFormatterEvent::removeEventListener (INclEventListener *listener)
{
  set<INclEventListener *>::iterator i;

  i = listeners.find (listener);
  if (i != listeners.end ())
    {
      listeners.erase (i);
    }
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
      return EventUtil::ST_SLEEPING;

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
      return changeState (EventUtil::ST_SLEEPING, EventUtil::TR_ABORTS);

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

  if (transition == EventUtil::TR_STOPS)
    {
      occurrences++;
    }

  previousState = currentState;
  currentState = newState;

  if (deleting)
    {
      return false;
    }

  set<INclEventListener *> *clone = new set<INclEventListener *> (listeners);

  i = clone->begin ();
  while (i != clone->end ())
    {
      if (deleting)
        {
          break;
        }

      if (*i != NULL)
        {
          ((INclEventListener *)(*i))
              ->eventStateChanged (this, transition, previousState);
        }
      ++i;
    }

  clone->clear ();
  delete clone;
  clone = NULL;

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
        default:
          return -1;
        }
      break;

    default:
      break;
    }

  return -1;
}

ExecutionObject *
NclFormatterEvent::getExecutionObject ()
{
  return executionObject;
}

void
NclFormatterEvent::setExecutionObject (ExecutionObject *object)
{
  executionObject = object;
}

string
NclFormatterEvent::getId ()
{
  return id;
}

int
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

// NclAnchorEvent
NclAnchorEvent::NclAnchorEvent (const string &id,
                                ExecutionObject *executionObject,
                                ContentAnchor *anchor)
    : NclFormatterEvent (id, executionObject)
{
  this->_anchor = anchor;
  typeSet.insert ("NclAnchorEvent");
}

NclAnchorEvent::~NclAnchorEvent () { removeInstance (this); }

ContentAnchor *
NclAnchorEvent::getAnchor ()
{
  return _anchor;
}

// NclPresentationEvent
NclPresentationEvent::NclPresentationEvent (const string &id,
                                            ExecutionObject *exeObj,
                                            ContentAnchor *anchor)
    : NclAnchorEvent (id, exeObj, anchor)
{
  typeSet.insert ("NclPresentationEvent");

  numPresentations = 1;
  repetitionInterval = 0;

  if (anchor->instanceOf ("IntervalAnchor"))
    {
      begin = ((IntervalAnchor *)anchor)->getBegin ();
      end = ((IntervalAnchor *)anchor)->getEnd ();
    }
  else
    {
      begin = 0;
      end = GINGA_TIME_NONE;
    }
}

NclPresentationEvent::~NclPresentationEvent () { removeInstance (this); }

bool
NclPresentationEvent::stop ()
{
  if (currentState == EventUtil::ST_OCCURRING && numPresentations > 1)
    {
      numPresentations--;
    }

  return NclFormatterEvent::stop ();
}

GingaTime
NclPresentationEvent::getDuration ()
{
  if (!GINGA_TIME_IS_VALID (this->end))
    return GINGA_TIME_NONE;
  return this->end - this->begin;
}

GingaTime
NclPresentationEvent::getRepetitionInterval ()
{
  return repetitionInterval;
}

int
NclPresentationEvent::getRepetitions ()
{
  return (numPresentations - 1);
}

void
NclPresentationEvent::setEnd (GingaTime end)
{
  this->end = end;
}

void
NclPresentationEvent::setRepetitionSettings (int repetitions,
                                             GingaTime repetitionInterval)
{
  if (repetitions >= 0)
    {
      this->numPresentations = repetitions + 1;
    }
  else
    {
      this->numPresentations = 1;
    }

  this->repetitionInterval = repetitionInterval;
}

GingaTime
NclPresentationEvent::getBegin ()
{
  return begin;
}

GingaTime
NclPresentationEvent::getEnd ()
{
  return end;
}

void
NclPresentationEvent::incrementOccurrences ()
{
  occurrences++;
}

// NclSelectionEvent
NclSelectionEvent::NclSelectionEvent (const string &id,
                                      ExecutionObject *exeObj,
                                      ContentAnchor *anchor)
    : NclAnchorEvent (id, exeObj, anchor)
{
  selectionCode.assign("NO_CODE");

  typeSet.insert ("NclSelectionEvent");
}

NclSelectionEvent::~NclSelectionEvent () { removeInstance (this); }

const string
NclSelectionEvent::getSelectionCode ()
{
  return selectionCode;
}

bool
NclSelectionEvent::start ()
{
  if (NclAnchorEvent::start ())
    return NclAnchorEvent::stop ();
  else
    return false;
}

void
NclSelectionEvent::setSelectionCode (const string &codeStr)
{
   selectionCode = codeStr;
}

// NclAttributionEvent
NclAttributionEvent::NclAttributionEvent (const string &id,
                                          ExecutionObject *exeObj,
                                          PropertyAnchor *anchor,
                                          Settings *settings)
    : NclFormatterEvent (id, exeObj)
{
  Entity *entity;
  NodeEntity *dataObject;

  typeSet.insert ("NclAttributionEvent");

  this->anchor = anchor;
  this->valueMaintainer = NULL;
  this->settingNode = false;
  this->settings = settings;

  dataObject = (NodeEntity *)(exeObj->getDataObject ());

  if (dataObject->instanceOf ("ContentNode")
      && ((ContentNode *)dataObject)->isSettingNode ())
    {
      settingNode = true;
    }

  if (dataObject->instanceOf ("ReferNode"))
    {
      if (((ReferNode *)dataObject)->getInstanceType () == "instSame")
        {
          entity = ((ReferNode *)dataObject)->getDataEntity ();
          if (entity->instanceOf ("ContentNode")
              && ((ContentNode *)entity)->isSettingNode ())
            {
              settingNode = true;
            }
        }
    }
}

NclAttributionEvent::~NclAttributionEvent ()
{
  removeInstance (this);

  assessments.clear ();
}

PropertyAnchor *
NclAttributionEvent::getAnchor ()
{
  return anchor;
}

string
NclAttributionEvent::getCurrentValue ()
{
  string propName;
  string maintainerValue = "";

  if (unlikely (anchor == NULL))
    {
      ERROR ("trying to set a null property anchor of object '%s'",
             id.c_str ());
    }

  if (settingNode)
    {
      propName = anchor->getName ();
      if (propName != "")
        {
          maintainerValue = settings->get (propName);
        }
    }
  else
    {
      if (valueMaintainer != NULL)
        {
          maintainerValue = valueMaintainer->getProperty (this);
        }

      if (maintainerValue == "")
        {
          maintainerValue = anchor->getValue ();
        }
    }

  return maintainerValue;
}

bool
NclAttributionEvent::setValue (const string &newValue)
{
  if (anchor->getValue () != newValue)
    {
      anchor->setValue (newValue);
      return true;
    }
  return false;
}

void
NclAttributionEvent::setValueMaintainer (
    INclAttributeValueMaintainer *valueMaintainer)
{
  this->valueMaintainer = valueMaintainer;
}

INclAttributeValueMaintainer *
NclAttributionEvent::getValueMaintainer ()
{
  return this->valueMaintainer;
}

void
NclAttributionEvent::setImplicitRefAssessmentEvent (
    const string &roleId, NclFormatterEvent *event)
{
  assessments[roleId] = event;
}

NclFormatterEvent *
NclAttributionEvent::getImplicitRefAssessmentEvent (const string &roleId)
{
  if (assessments.count (roleId) == 0)
    {
      return NULL;
    }

  return assessments[roleId];
}

// NclSwitchEvent
NclSwitchEvent::NclSwitchEvent (const string &id,
                                ExecutionObject *executionObjectSwitch,
                                InterfacePoint *interfacePoint,
                                int eventType, const string &key)
    : NclFormatterEvent (id, executionObjectSwitch)
{
  this->interfacePoint = interfacePoint;
  this->eventType = (short) eventType;
  this->key = key;
  this->mappedEvent = NULL;

  typeSet.insert ("NclSwitchEvent");
}

NclSwitchEvent::~NclSwitchEvent ()
{
  if (NclFormatterEvent::hasInstance (mappedEvent, false))
    {
      mappedEvent->removeEventListener (this);
      mappedEvent = NULL;
    }
}

InterfacePoint *
NclSwitchEvent::getInterfacePoint ()
{
  return interfacePoint;
}

short
NclSwitchEvent::getEventType ()
{
  return eventType;
}

string
NclSwitchEvent::getKey ()
{
  return key;
}

void
NclSwitchEvent::setMappedEvent (NclFormatterEvent *event)
{
  if (mappedEvent != NULL)
    {
      mappedEvent->removeEventListener (this);
    }

  mappedEvent = event;
  if (mappedEvent != NULL)
    {
      mappedEvent->addEventListener (this);
    }
}

NclFormatterEvent *
NclSwitchEvent::getMappedEvent ()
{
  return mappedEvent;
}

void
NclSwitchEvent::eventStateChanged (arg_unused (NclFormatterEvent *someEvent),
                                   short transition,
                                   arg_unused (short previousState))
{
  changeState (getNewState (transition), transition);
}

GINGA_FORMATTER_END
