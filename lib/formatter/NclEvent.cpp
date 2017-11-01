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
#include "ExecutionObject.h"

GINGA_FORMATTER_BEGIN

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

string
NclEvent::getParameter (const string &name)
{
  return (_params.count (name) != 0) ? _params[name] : "";
}

void
NclEvent::setParameter (const string &name, const string &value)
{
  _params[name] = value;
}

bool
NclEvent::transition (EventStateTransition trans)
{
  EventState curr = _state;
  EventState next;
  switch (trans)
    {
    case EventStateTransition::START:
      if (curr == EventState::OCCURRING)
        return false;
      next = EventState::OCCURRING;
      break;
    case EventStateTransition::PAUSE:
      if (curr != EventState::OCCURRING)
        return false;
      next = EventState::PAUSED;
      break;
    case EventStateTransition::RESUME:
      if (curr != EventState::PAUSED)
        return false;
      next = EventState::OCCURRING;
      break;
    case EventStateTransition::STOP: // fall through
    case EventStateTransition::ABORT:
      if (curr == EventState::SLEEPING)
        return false;
      next = EventState::SLEEPING;
      break;
    default:
      g_assert_not_reached ();
    }
  if (!_object->exec (this, curr, next, trans))
    return false;
  _state = next;
  for (INclEventListener *lst: _listeners)
    lst->eventStateChanged (this, trans);
  return true;
}

GINGA_FORMATTER_END
