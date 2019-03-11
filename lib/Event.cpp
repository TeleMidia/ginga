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

Event::Event (Object *object, Event::Type type, const string &id)
{
  g_return_if_fail (object != NULL);

  _L = object->getDocument ()->getLuaState ();
  LuaAPI::Event_attachWrapper (_L, this, object, type, id);
}

Event::~Event ()
{
  LuaAPI::Event_detachWrapper (_L, this);
}

Object *
Event::getObject ()
{
  Object *obj;

  LuaAPI::Event_call (_L, this, "getObject", 0, 1);
  obj = LuaAPI::Object_check (_L, -1);
  lua_pop (_L, 1);

  return obj;
}

Event::Type
Event::getType ()
{
  Event::Type type;

  LuaAPI::Event_call (_L, this, "getType", 0, 1);
  type = LuaAPI::Event_Type_check (_L, -1);
  lua_pop (_L, 1);

  return type;
}

string
Event::getId ()
{
  const char *id;

  LuaAPI::Event_call (_L, this, "getId", 0, 1);
  id = luaL_checkstring (_L, -1);
  lua_pop (_L, 1);

  return string (id);
}

string
Event::getQualifiedId ()
{
  const char *qualId;

  LuaAPI::Event_call (_L, this, "getQualifiedId", 0, 1);
  qualId = luaL_checkstring (_L, -1);
  lua_pop (_L, 1);

  return string (qualId);
}

Event::State
Event::getState ()
{
  Event::State state;

  LuaAPI::Event_call (_L, this, "getState", 0, 1);
  state = LuaAPI::Event_State_check (_L, -1);
  lua_pop (_L, 1);

  return state;
}

void
Event::setState (Event::State state)
{
  LuaAPI::Event_State_push (_L, state);
  LuaAPI::Event_call (_L, this, "setState", 1, 0);
}

Time
Event::getBeginTime ()
{
  Time time = GINGA_TIME_NONE;

  LuaAPI::Event_call (_L, this, "getBeginTime", 0, 1);
  if (!lua_isnil (_L, -1))
    {
      time = luaL_checkinteger (_L, -1);
    }
  lua_pop (_L, 1);

  return time;
}

void
Event::setBeginTime (Time time)
{
  lua_pushinteger (_L, (lua_Integer) time);
  LuaAPI::Event_call (_L, this, "setBeginTime", 1, 0);
}

Time
Event::getEndTime ()
{
  Time time = GINGA_TIME_NONE;

  LuaAPI::Event_call (_L, this, "getEndTime", 0, 1);
  if (!lua_isnil (_L, -1))
    {
      time = luaL_checkinteger (_L, -1);
    }
  lua_pop (_L, 1);

  return time;
}

void
Event::setEndTime (Time time)
{
  lua_pushinteger (_L, (lua_Integer) time);
  LuaAPI::Event_call (_L, this, "setEndTime", 1, 0);
}

string
Event::getLabel ()
{
  const char *label = NULL;

  LuaAPI::Event_call (_L, this, "getLabel", 0, 1);
  if (!lua_isnil (_L, -1))
    {
      label = luaL_checkstring (_L, -1);
    }
  lua_pop (_L, 1);

  return string (label != NULL ? label : "");
}

void
Event::setLabel (const string &label)
{
  if (label == "")
    lua_pushnil (_L);
  else
    lua_pushstring (_L, label.c_str ());

  LuaAPI::Event_call (_L, this, "setLabel", 1, 0);
}

// TODO --------------------------------------------------------------------

bool
Event::isLambda ()
{
  return this->getType () == Event::PRESENTATION
    && this->getId () == "@lambda";
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
  Event::State curr = this->getState ();
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
  if (unlikely (!this->getObject ()->beforeTransition (this, trans)))
    return false;

  // Update event state.
  this->setState (next);

  // Finish transition.
  if (unlikely (!this->getObject ()->afterTransition (this, trans)))
    {
      this->setState (curr);
      return false;
    }

  return true;
}

GINGA_NAMESPACE_END
