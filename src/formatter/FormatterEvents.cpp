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
#include "FormatterEvents.h"

#include "ExecutionObjectContext.h"
#include "ncl/ContentNode.h"

GINGA_FORMATTER_BEGIN

set<NclFormatterEvent *> NclFormatterEvent::_instances;
bool NclFormatterEvent::_init = false;

NclFormatterEvent::NclFormatterEvent (const string &id,
                                      ExecutionObject *exeObj)
{
  this->id = id;
  _currentState = EventState::SLEEPING;
  _occurrences = 0;
  _exeObj = exeObj;
  _deleting = false;
  _eventType = EventType::UNKNOWN;

  if (!_init)
    {
      _init = true;
    }

  _typeSet.insert ("NclFormatterEvent");
  addInstance (this);
}

NclFormatterEvent::~NclFormatterEvent ()
{
  _deleting = true;

  removeInstance (this);

  destroyListeners ();
}

bool
NclFormatterEvent::hasInstance (NclFormatterEvent *evt, bool remove)
{
  bool inst = false;

  if (!_init)
    {
      return false;
    }

  auto i = _instances.find (evt);
  if (i != _instances.end ())
    {
      if (remove)
        {
          _instances.erase (i);
        }
      inst = true;
    }
  return inst;
}

void
NclFormatterEvent::addInstance (NclFormatterEvent *evt)
{
  _instances.insert (evt);
}

bool
NclFormatterEvent::removeInstance (NclFormatterEvent *evt)
{
  bool inst = false;

  auto i = _instances.find (evt);
  if (i != _instances.end ())
    {
      _instances.erase (i);
      inst = true;
    }

  return inst;
}

bool
NclFormatterEvent::instanceOf (const string &s)
{
  if (_typeSet.empty ())
    {
      return false;
    }
  else
    {
      return (_typeSet.find (s) != _typeSet.end ());
    }
}

bool
NclFormatterEvent::hasNcmId (NclFormatterEvent *evt, const string &anchorId)
{
  Anchor *anchor;
  string anchorName = " ";

  NclAnchorEvent *anchorEvt = dynamic_cast<NclAnchorEvent *> (evt);
  if (anchorEvt)
    {
      anchor = anchorEvt->getAnchor ();
      if (anchor != nullptr)
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
              && !evt->instanceOf ("NclSelectionEvent"))
            {
              return true;
            }
        }
    }
  else if (evt->instanceOf ("NclAttributionEvent"))
    {
      anchor = ((NclAttributionEvent *)evt)->getAnchor ();
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
NclFormatterEvent::setType (EventType evtType)
{
  this->_eventType = evtType;
}

EventType
NclFormatterEvent::getType ()
{
  return _eventType;
}

void
NclFormatterEvent::destroyListeners ()
{
  this->_exeObj = NULL;
  _listeners.clear ();
}

void
NclFormatterEvent::addListener (INclEventListener *listener)
{
  this->_listeners.insert (listener);
}

void
NclFormatterEvent::removeListener (INclEventListener *listener)
{
  set<INclEventListener *>::iterator i;

  i = _listeners.find (listener);
  if (i != _listeners.end ())
    {
      _listeners.erase (i);
    }
}

EventState
NclFormatterEvent::getNewState (EventStateTransition transition)
{
  switch (transition)
    {
    case EventStateTransition::STOPS:
      return EventState::SLEEPING;

    case EventStateTransition::STARTS:
    case EventStateTransition::RESUMES:
      return EventState::OCCURRING;

    case EventStateTransition::PAUSES:
      return EventState::PAUSED;

    case EventStateTransition::ABORTS:
      return EventState::SLEEPING;

    default:
      return EventState::UNKNOWN;
    }
}

EventStateTransition
NclFormatterEvent::getTransition (EventState newState)
{
  return EventUtil::getTransition (_currentState, newState);
}

bool
NclFormatterEvent::abort ()
{
  switch (_currentState)
    {
    case EventState::OCCURRING:
    case EventState::PAUSED:
      return changeState (EventState::SLEEPING, EventStateTransition::ABORTS);

    default:
      return false;
    }
}

bool
NclFormatterEvent::start ()
{
  switch (_currentState)
    {
    case EventState::SLEEPING:
      return changeState (EventState::OCCURRING, EventStateTransition::STARTS);
    default:
      return false;
    }
}

bool
NclFormatterEvent::stop ()
{
  switch (_currentState)
    {
    case EventState::OCCURRING:
    case EventState::PAUSED:
      return changeState (EventState::SLEEPING, EventStateTransition::STOPS);
    default:
      return false;
    }
}

bool
NclFormatterEvent::pause ()
{
  switch (_currentState)
    {
    case EventState::OCCURRING:
      return changeState (EventState::PAUSED, EventStateTransition::PAUSES);

    default:
      return false;
    }
}

bool
NclFormatterEvent::resume ()
{
  switch (_currentState)
    {
    case EventState::PAUSED:
      return changeState (EventState::OCCURRING, EventStateTransition::RESUMES);

    default:
      return false;
    }
}

void
NclFormatterEvent::setCurrentState (EventState newState)
{
  _previousState = _currentState;
  _currentState = newState;
}

bool
NclFormatterEvent::changeState (EventState newState,
                                EventStateTransition transition)
{
  set<INclEventListener *>::iterator i;

  if (transition == EventStateTransition::STOPS)
    {
      _occurrences++;
    }

  _previousState = _currentState;
  _currentState = newState;

  if (_deleting)
    {
      return false;
    }

  set<INclEventListener *> *clone = new set<INclEventListener *> (_listeners);

  i = clone->begin ();
  while (i != clone->end ())
    {
      if (_deleting)
        {
          break;
        }

      if (*i != NULL)
        {
          ((INclEventListener *)(*i))
              ->eventStateChanged (this, transition, _previousState);
        }
      ++i;
    }

  clone->clear ();
  delete clone;
  clone = NULL;

  return true;
}

EventState
NclFormatterEvent::getCurrentState ()
{
  return _currentState;
}

EventState
NclFormatterEvent::getPreviousState ()
{
  return _previousState;
}

ExecutionObject *
NclFormatterEvent::getExecutionObject ()
{
  return _exeObj;
}

void
NclFormatterEvent::setExecutionObject (ExecutionObject *object)
{
  _exeObj = object;
}

string
NclFormatterEvent::getId ()
{
  return id;
}

int
NclFormatterEvent::getOccurrences ()
{
  return _occurrences;
}

// NclAnchorEvent
NclAnchorEvent::NclAnchorEvent (const string &id,
                                ExecutionObject *executionObject,
                                ContentAnchor *anchor)
    : NclFormatterEvent (id, executionObject)
{
  this->_anchor = anchor;
  _typeSet.insert ("NclAnchorEvent");
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
  _typeSet.insert ("NclPresentationEvent");

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
  if (_currentState == EventState::OCCURRING && numPresentations > 1)
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
  _occurrences++;
}

// NclSelectionEvent
NclSelectionEvent::NclSelectionEvent (const string &id,
                                      ExecutionObject *exeObj,
                                      ContentAnchor *anchor)
    : NclAnchorEvent (id, exeObj, anchor)
{
  selectionCode.assign("NO_CODE");

  _typeSet.insert ("NclSelectionEvent");
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

  _typeSet.insert ("NclAttributionEvent");

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
                                EventType eventType, const string &key)
    : NclFormatterEvent (id, executionObjectSwitch)
{
  this->interfacePoint = interfacePoint;
  this->_eventType = eventType;
  this->key = key;
  this->mappedEvent = NULL;

  _typeSet.insert ("NclSwitchEvent");
}

NclSwitchEvent::~NclSwitchEvent ()
{
  if (NclFormatterEvent::hasInstance (mappedEvent, false))
    {
      mappedEvent->removeListener (this);
      mappedEvent = NULL;
    }
}

InterfacePoint *
NclSwitchEvent::getInterfacePoint ()
{
  return interfacePoint;
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
      mappedEvent->removeListener (this);
    }

  mappedEvent = event;
  if (mappedEvent != NULL)
    {
      mappedEvent->addListener (this);
    }
}

NclFormatterEvent *
NclSwitchEvent::getMappedEvent ()
{
  return mappedEvent;
}

void
NclSwitchEvent::eventStateChanged (
    arg_unused (NclFormatterEvent *someEvent),
    EventStateTransition transition,
    arg_unused (EventState _previousState))
{
  changeState (getNewState (transition), transition);
}

GINGA_FORMATTER_END
