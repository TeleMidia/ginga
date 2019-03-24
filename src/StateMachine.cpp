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
#include "StateMachine.h"
#include "LuaAPI.h"

#include "Document.h"
#include "Object.h"

GINGA_NAMESPACE_BEGIN

// Public.

StateMachine::StateMachine (Object *object, StateMachine::Type type,
                            const string &id)
{
  g_return_if_fail (object != NULL);

  _L = object->getDocument ()->getLuaState ();
  LuaAPI::StateMachine_attachWrapper (_L, this, object, type, id);
}

StateMachine::~StateMachine ()
{
  LuaAPI::StateMachine_detachWrapper (_L, this);
}

Object *
StateMachine::getObject ()
{
  Object *obj;

  LuaAPI::StateMachine_call (_L, this, "getObject", 0, 1);
  obj = LuaAPI::Object_check (_L, -1);
  lua_pop (_L, 1);

  return obj;
}

StateMachine::Type
StateMachine::getType ()
{
  StateMachine::Type type;

  LuaAPI::StateMachine_call (_L, this, "getType", 0, 1);
  type = LuaAPI::StateMachine_Type_check (_L, -1);
  lua_pop (_L, 1);

  return type;
}

string
StateMachine::getId ()
{
  const char *id;

  LuaAPI::StateMachine_call (_L, this, "getId", 0, 1);
  id = luaL_checkstring (_L, -1);
  lua_pop (_L, 1);

  return string (id);
}

string
StateMachine::getQualifiedId ()
{
  const char *qualId;

  LuaAPI::StateMachine_call (_L, this, "getQualifiedId", 0, 1);
  qualId = luaL_checkstring (_L, -1);
  lua_pop (_L, 1);

  return string (qualId);
}

StateMachine::State
StateMachine::getState ()
{
  StateMachine::State state;

  LuaAPI::StateMachine_call (_L, this, "getState", 0, 1);
  state = LuaAPI::StateMachine_State_check (_L, -1);
  lua_pop (_L, 1);

  return state;
}

void
StateMachine::setState (StateMachine::State state)
{
  LuaAPI::StateMachine_State_push (_L, state);
  LuaAPI::StateMachine_call (_L, this, "setState", 1, 0);
}

lua_Integer
StateMachine::getBeginTime ()
{
  lua_Integer time = -1;        // none

  LuaAPI::StateMachine_call (_L, this, "getBeginTime", 0, 1);
  if (!lua_isnil (_L, -1))
    {
      time = luaL_checkinteger (_L, -1);
    }
  lua_pop (_L, 1);

  return time;
}

void
StateMachine::setBeginTime (lua_Integer time)
{
  if (time >= 0)
    {
      lua_pushinteger (_L, time);
    }
  else
    {
      lua_pushnil (_L);
    }
  LuaAPI::StateMachine_call (_L, this, "setBeginTime", 1, 0);
}

lua_Integer
StateMachine::getEndTime ()
{
  lua_Integer time = -1;        // none

  LuaAPI::StateMachine_call (_L, this, "getEndTime", 0, 1);
  if (!lua_isnil (_L, -1))
    {
      time = luaL_checkinteger (_L, -1);
    }
  lua_pop (_L, 1);

  return time;
}

void
StateMachine::setEndTime (lua_Integer time)
{
  if (time >= 0)
    {
      lua_pushinteger (_L, time);
    }
  else
    {
      lua_pushnil (_L);
    }
  LuaAPI::StateMachine_call (_L, this, "setEndTime", 1, 0);
}

string
StateMachine::getLabel ()
{
  const char *label = NULL;

  LuaAPI::StateMachine_call (_L, this, "getLabel", 0, 1);
  if (!lua_isnil (_L, -1))
    {
      label = luaL_checkstring (_L, -1);
    }
  lua_pop (_L, 1);

  return string (label != NULL ? label : "");
}

void
StateMachine::setLabel (const string &label)
{
  if (label == "")
    {
      lua_pushnil (_L);
    }
  else
    {
      lua_pushstring (_L, label.c_str ());
    }

  LuaAPI::StateMachine_call (_L, this, "setLabel", 1, 0);
}

bool
StateMachine::transition (StateMachine::Transition trans,
                   const map<string,string> &params)
{
  bool status;

  LuaAPI::StateMachine_Transition_push (_L, trans);
  lua_newtable (_L);
  for (auto &it: params)
    {
      lua_pushstring (_L, it.first.c_str ());
      lua_pushstring (_L, it.second.c_str ());
      lua_rawset (_L, -3);
    }
  LuaAPI::StateMachine_call (_L, this, "transition", 2, 1);
  status = lua_toboolean (_L, 1);
  lua_pop (_L, 1);

  return status;
}

GINGA_NAMESPACE_END
