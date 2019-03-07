/* Copyright (C) 2006-2018 PUC-Rio/Laboratorio TeleMidia

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
along with Ginga.  If not, see <https://www.gnu.org/licenses/>.  */

#include "aux-ginga.h"
#include "Event.h"
#include "LuaAPI.h"

#include "Document.h"
#include "Object.h"

GINGA_NAMESPACE_BEGIN

// Public.

Event::Event (Event::Type type, Object *object, const string &id)
{
  g_return_if_fail (object != NULL);

  _type = type;
  _object = object;
  _L = object->getDocument ()->getLuaState ();
  _id = id;
  _state = Event::SLEEPING;
  _begin = 0;
  _end = GINGA_TIME_NONE;
  _label = "";                  // empty

  LuaAPI::Event_attachWrapper (_L, this);
}

Event::~Event ()
{
  LuaAPI::Event_detachWrapper (_L, this);
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

string
Event::getQualifiedId ()
{
  string obj;

  g_assert_nonnull (_object);

  obj = _object->getId ();
  switch (_type)
    {
    case Event::PRESENTATION:
      if (_id == "@lambda")
        return obj + _id;
      else
        return obj + "@" + _id;
    case Event::ATTRIBUTION:
      return obj + "." + _id;
    case Event::SELECTION:
      return obj + "<" + _id + ">";
    default:
      g_assert_not_reached ();
    }
}

Event::State
Event::getState ()
{
  return _state;
}

void
Event::setState (Event::State state)
{
  _state = state;
}

Time
Event::getBeginTime ()
{
  return _begin;
}

void
Event::setBeginTime (Time time)
{
  _begin = time;
}

Time
Event::getEndTime ()
{
  return _end;
}

void
Event::setEndTime (Time time)
{
  _end = time;
}

string
Event::getLabel ()
{
  return _label;
}

void
Event::setLabel (const string &label)
{
  _label = label;
}

// TODO --------------------------------------------------------------------

bool
Event::isLambda ()
{
  return _type == Event::PRESENTATION && _id == "@lambda";
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

/**
 * @brief Transitions event.
 * @param trans The desired transition.
 * @return \c true if successful, or \c false otherwise.
 */
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

  // Initiate transition.
  if (unlikely (!_object->beforeTransition (this, trans)))
    return false;

  // Update event state.
  _state = next;

  // Finish transition.
  if (unlikely (!_object->afterTransition (this, trans)))
    {
      _state = curr;
      return false;
    }

  return true;
}

GINGA_NAMESPACE_END
