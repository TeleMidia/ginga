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
#include "Object.h"
#include "LuaAPI.h"

#include "Composition.h"
#include "Context.h"
#include "Document.h"
#include "StateMachine.h"
#include "Media.h"
#include "Switch.h"

GINGA_NAMESPACE_BEGIN

Object::Object (Document *doc, unused (const string &id))
{
  g_return_if_fail (doc != NULL);

  _L = doc->getLuaState ();
}

Object::~Object ()
{
}

bool
Object::isComposition ()
{
  bool result;

  LuaAPI::Object_call (_L, this, "isComposition", 0, 1);
  result = (bool) lua_toboolean (_L, -1);
  lua_pop (_L, 1);

  return result;
}

Document *
Object::getDocument ()
{
  Document *doc;

  LuaAPI::Object_call (_L, this, "getDocument", 0, 1);
  doc = LuaAPI::Document_check (_L, -1);
  lua_pop (_L, 1);

  return doc;
}

Object::Type
Object::getType ()
{
  Object::Type type;

  LuaAPI::Object_call (_L, this, "getType", 0, 1);
  type = LuaAPI::Object_Type_check (_L, -1);
  lua_pop (_L, 1);

  return type;
}

string
Object::getId ()
{
  const char *id;

  LuaAPI::Object_call (_L, this, "getId", 0, 1);
  id = luaL_checkstring (_L, -1);
  lua_pop (_L, 1);

  return string (id);
}

void
Object::getParents (set<Composition *> *parents)
{
  lua_Integer len;
  lua_Integer i;

  g_return_if_fail (parents != NULL);

  LuaAPI::Object_call (_L, this, "getParents", 0, 1);
  g_assert (lua_type (_L, -1) == LUA_TTABLE);

  len = luaL_len (_L, -1);
  for (i = 1; i <= len; i++)
    {
      Composition *comp;

      lua_rawgeti (_L, -1, i);
      comp = LuaAPI::Composition_check (_L, -1);
      parents->insert (comp);
      lua_pop (_L, 1);
    }
}

void
Object::getStateMachines (set<StateMachine *> *machines)
{
  lua_Integer len;
  lua_Integer i;

  g_return_if_fail (machines != NULL);

  LuaAPI::Object_call (_L, this, "getStateMachines", 0, 1);
  g_assert (lua_type (_L, -1) == LUA_TTABLE);

  len = luaL_len (_L, -1);
  for (i = 1; i <= len; i++)
    {
      StateMachine *sm;

      lua_rawgeti (_L, -1, i);
      sm = LuaAPI::StateMachine_check (_L, -1);
      machines->insert (sm);
      lua_pop (_L, 1);
    }
}

StateMachine *
Object::getStateMachine (StateMachine::Type type, const string &id)
{
  StateMachine *sm = NULL;

  LuaAPI::StateMachine_Type_push (_L, type);
  lua_pushstring (_L, id.c_str ());
  LuaAPI::Object_call (_L, this, "getStateMachine", 2, 1);
  if (!lua_isnil (_L, -1))
    {
      sm = LuaAPI::StateMachine_check (_L, -1);
    }
  lua_pop (_L, 1);

  return sm;
}

StateMachine *
Object::getLambda ()
{
  StateMachine *sm;

  LuaAPI::Object_call (_L, this, "getLambda", 0, 1);
  sm = LuaAPI::StateMachine_check (_L, -1);
  lua_pop (_L, 1);

  return sm;
}

StateMachine *
Object::createStateMachine (StateMachine::Type type, const string &id)
{
  StateMachine *sm = NULL;

  LuaAPI::StateMachine_Type_push (_L, type);
  lua_pushstring (_L, id.c_str ());
  LuaAPI::Object_call (_L, this, "createStateMachine", 2, 1);
  if (!lua_isnil (_L, -1))
    {
      sm = LuaAPI::StateMachine_check (_L, -1);
    }
  lua_pop (_L, 1);

  return sm;
}

lua_Integer
Object::getTime ()
{
  lua_Integer time;

  LuaAPI::Object_call (_L, this, "getTime", 0, 1);
  if (!lua_isnil (_L, -1))
    {
      time = (lua_Integer) luaL_checkinteger (_L, -1);
    }
  lua_pop (_L, 1);

  return time;
}

void
Object::setTime (lua_Integer time)
{
  if (time == -1)
    {
      lua_pushnil (_L);
    }
  else
    {
      lua_pushinteger (_L, time);
    }

  LuaAPI::Object_call (_L, this, "setTime", 1, 0);
}

bool
Object::getProperty (const string &name, GValue *value)
{
  bool status = false;

  lua_pushstring (_L, name.c_str ());
  LuaAPI::Object_call (_L, this, "getProperty", 1, 1);
  if (!lua_isnil (_L, -1))
    {
      status = LuaAPI::GValue_to (_L, -1, value);
    }
  lua_pop (_L, 1);

  return status;
}

bool
Object::getPropertyBool (const string &name, bool *value)
{
  GValue val = G_VALUE_INIT;

  g_return_val_if_fail (value != NULL, false);

  if (!this->getProperty (name, &val))
    {
      return false;
    }

  if (!G_VALUE_HOLDS (&val, G_TYPE_BOOLEAN))
    {
      g_value_unset (&val);
      return false;
    }

  *value = g_value_get_boolean (&val);
  g_value_unset (&val);

  return true;
}

bool
Object::getPropertyInteger (const string &name, lua_Integer *value)
{
  GValue val = G_VALUE_INIT;

  g_return_val_if_fail (value != NULL, false);

  if (!this->getProperty (name, &val))
    {
      return false;
    }

  if (!G_VALUE_HOLDS (&val, G_TYPE_INT64))
    {
      g_value_unset (&val);
      return false;
    }

  *value = (lua_Integer) g_value_get_int64 (&val);
  g_value_unset (&val);

  return true;
}

bool
Object::getPropertyNumber (const string &name, lua_Number *value)
{
  GValue val = G_VALUE_INIT;

  g_return_val_if_fail (value != NULL, false);

  if (!this->getProperty (name, &val))
    {
      return false;
    }

  if (!G_VALUE_HOLDS (&val, G_TYPE_DOUBLE))
    {
      g_value_unset (&val);
      return false;
    }

  *value = (lua_Number) g_value_get_double (&val);
  g_value_unset (&val);

  return true;
}

bool
Object::getPropertyString (const string &name, string *value)
{
  GValue val = G_VALUE_INIT;
  const char *str;

  g_return_val_if_fail (value != NULL, false);

  if (!this->getProperty (name, &val))
    {
      return false;
    }

  if (!G_VALUE_HOLDS (&val, G_TYPE_STRING))
    {
      g_value_unset (&val);
      return false;
    }

  str = g_value_get_string (&val);
  if (str == NULL)
    {
      g_value_unset (&val);
      return false;
    }

  *value = string (str);
  g_value_unset (&val);

  return true;
}

void
Object::setProperty (const string &name, const GValue *value)
{
  lua_pushstring (_L, name.c_str ());
  g_assert (LuaAPI::GValue_push (_L, value));
  LuaAPI::Object_call (_L, this, "setProperty", 2, 0);
}

void
Object::setPropertyBool (const string &name, bool value)
{
  GValue val = G_VALUE_INIT;

  g_value_init (&val, G_TYPE_BOOLEAN);
  g_value_set_boolean (&val, value);
  this->setProperty (name, &val);
  g_value_unset (&val);
}

void
Object::setPropertyInteger (const string &name, lua_Integer value)
{
  GValue val = G_VALUE_INIT;

  g_value_init (&val, G_TYPE_INT64);
  g_value_set_int64 (&val, value);
  this->setProperty (name, &val);
  g_value_unset (&val);
}

void
Object::setPropertyNumber (const string &name, lua_Number value)
{
  GValue val = G_VALUE_INIT;

  g_value_init (&val, G_TYPE_DOUBLE);
  g_value_set_double (&val, value);
  this->setProperty (name, &val);
  g_value_unset (&val);
}

void
Object::setPropertyString (const string &name, const string &value)
{
  GValue val = G_VALUE_INIT;

  g_value_init (&val, G_TYPE_STRING);
  g_value_set_string (&val, value.c_str ());
  this->setProperty (name, &val);
  g_value_unset (&val);
}

void
Object::unsetProperty (const string &name)
{
  lua_pushstring (_L, name.c_str ());
  lua_pushnil (_L);
  LuaAPI::Object_call (_L, this, "setProperty", 2, 0);
}

GINGA_NAMESPACE_END
