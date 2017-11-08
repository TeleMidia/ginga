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
#include "Event.h"
#include "Object.h"

GINGA_NAMESPACE_BEGIN


// Public.

Event::Event (Event::Type type, Object *object, const string &id)
{
  _type = type;
  g_assert_nonnull (object);
  _object = object;
  _id = id;
  _state = Event::SLEEPING;
  _begin = 0;
  _end = GINGA_TIME_NONE;
}

Event::~Event ()
{
}

Event::Type
Event::getType ()
{
  return _type;
}

Object *
Event::getObject ()
{
  return _object;
}

string
Event::getId ()
{
  return _id;
}

Event::State
Event::getState ()
{
  return _state;
}

bool
Event::isLambda ()
{
  return _type == Event::PRESENTATION && _id == "@lambda";
}

void
Event::getInterval (GingaTime *begin, GingaTime *end)
{
  tryset (begin, _begin);
  tryset (end, _end);
}

void
Event::setInterval (GingaTime begin, GingaTime end)
{
  _begin = begin;
  _end = end;
}

bool
Event::getParameter (const string &name, string *value)
{
  MAP_GET_IMPL (_parameters, name, value);
}

bool
Event::setParameter (const string &name, const string &value)
{
  MAP_SET_IMPL (_parameters, name, value);
}

bool
Event::transition (Event::Transition trans)
{
  Event::State curr = _state;
  Event::State next;
  switch (trans)
    {
    case Event::START:
      if (curr == Event::OCCURRING)
        return false;
      next = Event::OCCURRING;
      break;
    case Event::PAUSE:
      if (curr != Event::OCCURRING)
        return false;
      next = Event::PAUSED;
      break;
    case Event::RESUME:
      if (curr != Event::PAUSED)
        return false;
      next = Event::OCCURRING;
      break;
    case Event::STOP: // fall through
    case Event::ABORT:
      if (curr == Event::SLEEPING)
        return false;
      next = Event::SLEEPING;
      break;
    default:
      g_assert_not_reached ();
    }
  if (!unlikely (_object->startTransition (this, trans)))
    return false;
  _state = next;
  _object->endTransition (this, trans);
  return true;
}

void
Event::reset ()
{
  _state = Event::SLEEPING;
}


// Public: Static.

string
Event::getEventTypeAsString (Event::Type type)
{
  switch (type)
    {
    case Event::PRESENTATION:
      return "presentation";
    case Event::ATTRIBUTION:
      return "attribution";
    case Event::SELECTION:
      return "selection";
    default:
      g_assert_not_reached ();
    }
}

string
Event::getEventStateAsString (Event::State state)
{
  switch (state)
    {
    case Event::SLEEPING:
      return "sleeping";
    case Event::OCCURRING:
      return "occurring";
    case Event::PAUSED:
      return "paused";
    default:
      g_assert_not_reached ();
    }
}

string
Event::getEventTransitionAsString (Event::Transition tr)
{
  switch (tr)
    {
    case Event::START:
      return "start";
    case Event::PAUSE:
      return "pause";
    case Event::RESUME:
      return "resume";
    case Event::STOP:
      return "stop";
    case Event::ABORT:
      return "abort";
    default:
      g_assert_not_reached ();
    }
}

GINGA_NAMESPACE_END
