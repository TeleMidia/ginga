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
#include "FormatterEvent.h"
#include "ExecutionObject.h"

GINGA_FORMATTER_BEGIN

FormatterEvent::FormatterEvent (GingaInternal *ginga, EventType type,
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

FormatterEvent::~FormatterEvent ()
{
}

EventType
FormatterEvent::getType ()
{
  return _type;
}

ExecutionObject *
FormatterEvent::getObject ()
{
  return _object;
}

Anchor *
FormatterEvent::getAnchor ()
{
  return _anchor;
}

EventState
FormatterEvent::getState ()
{
  return _state;
}

const vector<IFormatterEventListener *> *
FormatterEvent::getListeners ()
{
  return &_listeners;
}

void
FormatterEvent::addListener (IFormatterEventListener *listener)
{
  _listeners.push_back (listener);
}

string
FormatterEvent::getParameter (const string &name)
{
  return (_params.count (name) != 0) ? _params[name] : "";
}

void
FormatterEvent::setParameter (const string &name, const string &value)
{
  _params[name] = value;
}

bool
FormatterEvent::transition (EventStateTransition trans)
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
  for (IFormatterEventListener *lst: _listeners)
    lst->eventStateChanged (this, trans);
  return true;
}

GINGA_FORMATTER_END
