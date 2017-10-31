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
#include "NclEvent.h"

GINGA_FORMATTER_BEGIN

NclEvent::NclEvent (GingaInternal *ginga, EventType type,
                    ExecutionObject *object, Anchor *anchor,
                    const string &key)
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
  _key = key;
  _state = EventState::SLEEPING;
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

const vector<INclEventListener *> *
NclEvent::getListeners ()
{
  return &_listeners;
}

void
NclEvent::addListener (INclEventListener *listener)
{
  _listeners.push_back (listener);
}

bool
NclEvent::getInterval (GingaTime *begin, GingaTime *end)
{
  Area *area;
  if (_type != EventType::PRESENTATION)
    return false;
  area = cast (Area *, _anchor);
  g_assert_nonnull (area);
  tryset (begin, area->getBegin ());
  tryset (end, area->getEnd ());
  return true;
}

bool
NclEvent::getKey (string *key)
{
  if (_type != EventType::SELECTION)
    return false;
  tryset (key, _key);
  return true;
}

bool
NclEvent::abort ()
{
  if (_state == EventState::SLEEPING)
    return false;
  _state = EventState::SLEEPING;
  this->notifyListeners (EventStateTransition::ABORT);
  return true;
}

bool
NclEvent::start ()
{
  if (_state == EventState::OCCURRING)
    return false;
  _state = EventState::OCCURRING;
  this->notifyListeners (EventStateTransition::START);
  return true;
}

bool
NclEvent::stop ()
{
  if (_state == EventState::SLEEPING)
    return false;
  _state = EventState::SLEEPING;
  this->notifyListeners (EventStateTransition::STOP);
  return true;
}

bool
NclEvent::pause ()
{
  if (_state != EventState::OCCURRING)
    return false;
  _state = EventState::PAUSED;
  this->notifyListeners (EventStateTransition::PAUSE);
  return true;
}

bool
NclEvent::resume ()
{
  if (_state != EventState::PAUSED)
    return false;
  _state = EventState::OCCURRING;
  this->notifyListeners (EventStateTransition::RESUME);
  return true;
}

void
NclEvent:: notifyListeners (EventStateTransition transition)
{
  for (INclEventListener *listener: _listeners)
    listener->eventStateChanged (this, transition);
}

GINGA_FORMATTER_END
