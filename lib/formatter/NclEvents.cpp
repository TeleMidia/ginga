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

NclEvent::NclEvent (GingaInternal *ginga, EventType type,
                    ExecutionObject *object, Anchor *anchor)
{
  g_assert_nonnull (ginga);
  _ginga = ginga;
  _scheduler = ginga->getScheduler ();
  g_assert_nonnull (_scheduler);

  _type = type;
  g_assert_nonnull (object);
  _object = object;
  g_assert_nonnull (anchor);
  _anchor = anchor;

  _state = EventState::SLEEPING;
  _previousState = EventState::SLEEPING;
}

NclEvent::~NclEvent ()
{
}

EventType
NclEvent::getType ()
{
  return _type;
}

ExecutionObject *
NclEvent::getObject ()
{
  return _object;
}

Anchor *
NclEvent::getAnchor ()
{
  return _anchor;
}

EventState
NclEvent::getState ()
{
  return _state;
}

void
NclEvent::addListener (INclEventListener *listener)
{
  _listeners.insert (listener);
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

bool
NclEvent::changeState (EventState newState,
                       EventStateTransition transition)
{
  _previousState = _state;
  _state = newState;

  set<INclEventListener *> clone (_listeners);

  for (INclEventListener *listener: clone)
    listener->eventStateChanged (this, transition);

  return true;
}



// PresentationEvent.

PresentationEvent::PresentationEvent (GingaInternal *ginga,
                                      ExecutionObject *exeObj,
                                      Area *anchor)
  : NclEvent (ginga, EventType::PRESENTATION, exeObj, anchor)
{
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

GingaTime
PresentationEvent::getDuration ()
{
  if (!GINGA_TIME_IS_VALID (this->_end))
    return GINGA_TIME_NONE;
  return this->_end - this->_begin;
}


// SelectionEvent

SelectionEvent::SelectionEvent (GingaInternal *ginga,
                                ExecutionObject *exeObj,
                                Area *anchor, const string &key)
  : NclEvent (ginga, EventType::SELECTION, exeObj, anchor)
{
  _key = key;
}

SelectionEvent::~SelectionEvent ()
{
}

string
SelectionEvent::getKey ()
{
  return _key;
}

bool
SelectionEvent::start ()
{
  if (NclEvent::start ())
    return NclEvent::stop ();
  else
    return false;
}


// AttributionEvent

AttributionEvent::AttributionEvent (GingaInternal *ginga,
                                    ExecutionObject *exeObj,
                                    Property *anchor)
  : NclEvent (ginga, EventType::ATTRIBUTION, exeObj, anchor)
{
}

AttributionEvent::~AttributionEvent ()
{
}


// SwitchEvent.

SwitchEvent::SwitchEvent (GingaInternal *ginga,
                          ExecutionObject *exeObjSwitch,
                          Anchor *interface,
                          EventType type, const string &key)
  : NclEvent (ginga, type, exeObjSwitch, interface)
{
  this->_key = key;
  _mappedEvent = nullptr;
}

SwitchEvent::~SwitchEvent ()
{
}

void
SwitchEvent::setMappedEvent (NclEvent *evt)
{
  _mappedEvent = evt;
}

void
SwitchEvent::eventStateChanged (unused (NclEvent *evt),
                                EventStateTransition trans)

{
  changeState (EventUtil::getNextState (trans), trans);
}

GINGA_FORMATTER_END
