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

#include "aux-ginga.h"
#include "NclEvents.h"

#include "ExecutionObjectContext.h"
#include "Scheduler.h"

GINGA_FORMATTER_BEGIN


// NclEvent.

NclEvent::NclEvent (GingaInternal *ginga, const string &id,
                    ExecutionObject *exeObj)
{
  g_assert_nonnull (ginga);
  _ginga = ginga;

  _scheduler = ginga->getScheduler ();
  g_assert_nonnull (_scheduler);

  _id = id;
  _state = EventState::SLEEPING;
  _occurrences = 0;
  _exeObj = exeObj;

  TRACE ("%s", _id.c_str ());
}

NclEvent::~NclEvent ()
{
  TRACE ("%s", _id.c_str ());
}

void
NclEvent::addListener (INclEventListener *listener)
{
  this->_listeners.insert (listener);
}

EventStateTransition
NclEvent::getTransition (EventState newState)
{
  EventStateTransition trans;
  g_assert (EventUtil::getTransition (_state, newState, &trans));
  return trans;
}

bool
NclEvent::abort ()
{
  if (_state == EventState::OCCURRING || _state == EventState::PAUSED)
    return changeState (EventState::SLEEPING, EventStateTransition::ABORT);
  else
    return false;
}

bool
NclEvent::start ()
{
  if (_state == EventState::SLEEPING)
    return changeState (EventState::OCCURRING, EventStateTransition::START);
  else
    return false;
}

bool
NclEvent::stop ()
{
  if (_state == EventState::OCCURRING || _state == EventState::PAUSED)
    return changeState (EventState::SLEEPING, EventStateTransition::STOP);
  else
    return false;
}

bool
NclEvent::pause ()
{
  if (_state == EventState::OCCURRING)
    return changeState (EventState::PAUSED, EventStateTransition::PAUSE);
  else
    return false;
}

bool
NclEvent::resume ()
{
  if (_state == EventState::PAUSED)
    return changeState (EventState::OCCURRING, EventStateTransition::RESUME);
  else
    return false;
}

void
NclEvent::setState (EventState newState)
{
  _previousState = _state;
  _state = newState;
}

bool
NclEvent::changeState (EventState newState,
                       EventStateTransition transition)
{
  if (transition == EventStateTransition::STOP)
    {
      _occurrences++;
    }

  _previousState = _state;
  _state = newState;

  set<INclEventListener *> clone (_listeners);

  for (INclEventListener *listener: clone)
    {
      listener->eventStateChanged (this, transition, _previousState);
    }

  return true;
}


// AnchorEvent.

AnchorEvent::AnchorEvent (GingaInternal *ginga, const string &id,
                          ExecutionObject *executionObject,
                          Area *anchor)
  : NclEvent (ginga, id, executionObject)
{
  this->_anchor = anchor;
}


// PresentationEvent.

PresentationEvent::PresentationEvent (GingaInternal *ginga,
                                      const string &id,
                                      ExecutionObject *exeObj,
                                      Area *anchor)
  : AnchorEvent (ginga, id, exeObj, anchor)
{
  _numPresentations = 1;
  _type = EventType::PRESENTATION;

  auto intervalAnchor = cast (Area *, anchor);
  if (intervalAnchor)
    {
      _begin = intervalAnchor->getBegin ();
      _end = intervalAnchor->getEnd ();
    }
  else
    {
      _begin = 0;
      _end = GINGA_TIME_NONE;
    }
}

bool
PresentationEvent::stop ()
{
  if (_state == EventState::OCCURRING && _numPresentations > 1)
    {
      _numPresentations--;
    }

  return NclEvent::stop ();
}

GingaTime
PresentationEvent::getDuration ()
{
  if (!GINGA_TIME_IS_VALID (this->_end))
    return GINGA_TIME_NONE;
  return this->_end - this->_begin;
}


// SelectionEvent

SelectionEvent::SelectionEvent (GingaInternal *ginga,
                                const string &id,
                                ExecutionObject *exeObj,
                                Area *anchor)
  : AnchorEvent (ginga, id, exeObj, anchor)
{
  _type = EventType::SELECTION;
  _selCode.assign ("NO_CODE");
}

bool
SelectionEvent::start ()
{
  if (AnchorEvent::start ())
    return AnchorEvent::stop ();
  else
    return false;
}


// AttributionEvent

AttributionEvent::AttributionEvent (GingaInternal *ginga,
                                    const string &id,
                                    ExecutionObject *exeObj,
                                    Property *anchor)
  : NclEvent (ginga, id, exeObj)
{
  _type = EventType::ATTRIBUTION;
  this->_anchor = anchor;
  this->_player = nullptr;

  Node *node = exeObj->getNode ();
  g_assert_nonnull (node);
}

AttributionEvent::~AttributionEvent ()
{
  _assessments.clear ();
}

string
AttributionEvent::getCurrentValue ()
{
  string propName;
  string value = "";

  if (unlikely (_anchor == nullptr))
    {
      ERROR ("Trying to set a null property anchor of object '%s'.",
             _id.c_str ());
    }

  if (_player)
    {
      value = _player->getProperty (this->getAnchor ()->getName ());
    }

  if (value == "")
    {
      value = _anchor->getValue ();
    }

  return value;
}

bool
AttributionEvent::setValue (const string &newValue)
{
  if (_anchor->getValue () != newValue)
    {
      _anchor->setValue (newValue);
      return true;
    }
  return false;
}

void
AttributionEvent::setImplicitRefAssessmentEvent (
    const string &roleId, NclEvent *event)
{
  _assessments[roleId] = event;
}

NclEvent *
AttributionEvent::getImplicitRefAssessmentEvent (const string &id)
{
  return (_assessments.count (id) > 0) ? _assessments[id] : nullptr;
}

string
AttributionEvent::solveImplicitRefAssessment (const string &val)
{
  AttributionEvent *evt;

  if (val.substr (0, 1) != "$")
    return val;

  evt = cast (AttributionEvent *,
    this->getImplicitRefAssessmentEvent (val.substr (1, val.length ())));
  return (evt != nullptr) ? evt->getCurrentValue () : "";
}


// SwitchEvent.

SwitchEvent::SwitchEvent (GingaInternal *ginga,
                          const string &id,
                          ExecutionObject *exeObjSwitch,
                          Anchor *interface,
                          EventType type, const string &key)
  : NclEvent (ginga, id, exeObjSwitch)
{
  this->_interface = interface;
  this->_type = type;
  this->_key = key;
  _mappedEvent = nullptr;
}

SwitchEvent::~SwitchEvent ()
{
  //_mappedEvent->removeListener (this);
}

void
SwitchEvent::setMappedEvent (NclEvent *evt)
{
  _mappedEvent = evt;
}

void
SwitchEvent::eventStateChanged (
    unused (NclEvent *evt),
    EventStateTransition trans,
    unused (EventState prevState))
{
  changeState (EventUtil::getNextState (trans), trans);
}

GINGA_FORMATTER_END
