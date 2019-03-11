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

#include "LuaAPI.h"
#include "Event.h"

// Event type.
#define EVENT_ATTRIBUTION_STRING   "attribution"
#define EVENT_PRESENTATION_STRING  "presentation"
#define EVENT_SELECTION_STRING     "selection"

// Event state.
#define EVENT_OCCURRING_STRING     "occurring"
#define EVENT_PAUSED_STRING        "paused"
#define EVENT_SLEEPING_STRING      "sleeping"

const char *LuaAPI::_EVENT = "Ginga.Event";

const struct luaL_Reg LuaAPI::_Event_funcs[] =
  {
   {"__getUnderlyingObject", LuaAPI::_l_Event_getUnderlyingObject},
   {"getType",               LuaAPI::_l_Event_getType},
   {"getObject",             LuaAPI::_l_Event_getObject},
   {"getId",                 LuaAPI::_l_Event_getId},
   {"getQualifiedId",        LuaAPI::_l_Event_getQualifiedId},
   {"getState",              LuaAPI::_l_Event_getState},
   {"setState",              LuaAPI::_l_Event_setState},
   {"getBeginTime",          LuaAPI::_l_Event_getBeginTime},
   {"setBeginTime",          LuaAPI::_l_Event_setBeginTime},
   {"getEndTime",            LuaAPI::_l_Event_getEndTime},
   {"setEndTime",            LuaAPI::_l_Event_setEndTime},
   {"getLabel",              LuaAPI::_l_Event_getLabel},
   {"setLabel",              LuaAPI::_l_Event_setLabel},
   {NULL, NULL}
  };

void
LuaAPI::Event_attachWrapper (lua_State *L, Event *evt)
{
  static const struct luaL_Reg *const funcs[] =
    {
     _funcs,
     _Event_funcs,
     NULL,
    };

  static const Chunk *const chunks[] =
    {
     &LuaAPI::_initMt,
     &LuaAPI::_Event_initMt,
     &LuaAPI::_traceMt,
     NULL,
    };

  Event **wrapper;

  g_return_if_fail (L != NULL);
  g_return_if_fail (evt != NULL);

  // Load and initialize metatable, if not loaded yet.
  LuaAPI::_loadLuaWrapperMt (L, LuaAPI::_EVENT, funcs, chunks);

  // Create wrapper for event.
  wrapper = (Event **) lua_newuserdata (L, sizeof (Event **));
  g_assert_nonnull (wrapper);
  *wrapper = evt;
  luaL_setmetatable (L, LuaAPI::_EVENT);

  // Set LUA_REGISTRY[evt]=wrapper.
  LuaAPI::_attachLuaWrapper (L, evt);

  // Call evt:_attachData().
  LuaAPI::_callLuaWrapper (L, evt, "_attachData", 0, 0);
}

void
LuaAPI::Event_detachWrapper (lua_State *L, Event *evt)
{
  g_return_if_fail (L != NULL);
  g_return_if_fail (evt != NULL);

  // Call evt:_detachData().
  LuaAPI::_callLuaWrapper (L, evt, "_detachData", 0, 0);

  // Set LUA_REGISTRY[evt] = nil.
  LuaAPI::_detachLuaWrapper (L, evt);
}

Event *
LuaAPI::Event_check (lua_State *L, int i)
{
  g_return_val_if_fail (L != NULL, NULL);
  return *((Event **) luaL_checkudata (L, i, LuaAPI::_EVENT));
}

Event::Type
LuaAPI::Event_Type_check (lua_State *L, int i)
{
  static const char *types[] = {EVENT_ATTRIBUTION_STRING,
                                EVENT_PRESENTATION_STRING,
                                EVENT_SELECTION_STRING,
                                NULL};

  g_return_val_if_fail (L != NULL, Event::PRESENTATION);

  switch (luaL_checkoption (L, i, NULL, types))
    {
    case 0:
      return Event::ATTRIBUTION;
    case 1:
      return Event::PRESENTATION;
    case 2:
      return Event::SELECTION;
    default:
      g_assert_not_reached ();
    }
}

Event::State
LuaAPI::Event_State_check (lua_State *L, int i)
{
  static const char *states[] = {EVENT_OCCURRING_STRING,
                                 EVENT_PAUSED_STRING,
                                 EVENT_SLEEPING_STRING,
                                 NULL};

  g_return_val_if_fail (L != NULL, Event::SLEEPING);

  switch (luaL_checkoption (L, i, NULL, states))
    {
    case 0:
      return Event::OCCURRING;
    case 1:
      return Event::PAUSED;
    case 2:
      return Event::SLEEPING;
    default:
      g_assert_not_reached ();
    }
}

void
LuaAPI::Event_push (lua_State *L, Event *evt)
{
  g_return_if_fail (L != NULL);
  g_return_if_fail (evt != NULL);

  LuaAPI::_pushLuaWrapper (L, evt);
}

void
LuaAPI::Event_Type_push (lua_State *L, Event::Type type)
{
  g_return_if_fail (L != NULL);

  switch (type)
    {
    case Event::ATTRIBUTION:
      lua_pushliteral (L, EVENT_ATTRIBUTION_STRING);
      break;
    case Event::PRESENTATION:
      lua_pushliteral (L, EVENT_PRESENTATION_STRING);
      break;
    case Event::SELECTION:
      lua_pushliteral (L, EVENT_SELECTION_STRING);
      break;
    default:
      g_assert_not_reached ();
    }
}

void
LuaAPI::Event_State_push (lua_State *L, Event::State state)
{
  g_return_if_fail (L != NULL);

  switch (state)
    {
    case Event::OCCURRING:
      lua_pushliteral (L, EVENT_OCCURRING_STRING);
      break;
    case Event::PAUSED:
      lua_pushliteral (L, EVENT_PAUSED_STRING);
      break;
    case Event::SLEEPING:
      lua_pushliteral (L, EVENT_SLEEPING_STRING);
      break;
    default:
      g_assert_not_reached ();
    }
}

void
LuaAPI::Event_call (lua_State *L, Event *evt, const char *name,
                    int nargs, int nresults)
{
  g_return_if_fail (L != NULL);
  g_return_if_fail (evt != NULL);
  g_return_if_fail (name != NULL);
  g_return_if_fail (nargs >= 0);
  g_return_if_fail (nresults >= 0);

  LuaAPI::_callLuaWrapper (L, evt, name, nargs, nresults);
}

int
LuaAPI::_l_Event_getUnderlyingObject (lua_State *L)
{
  lua_pushlightuserdata (L, LuaAPI::Event_check (L, 1));
  return 1;
}

int
LuaAPI::_l_Event_getType (lua_State *L)
{
  Event *evt;

  evt = LuaAPI::Event_check (L, 1);
  Event_Type_push (L, evt->getType ());

  return 1;
}

int
LuaAPI::_l_Event_getObject (lua_State *L)
{
  Event *evt;

  evt = LuaAPI::Event_check (L, 1);
  LuaAPI::_pushLuaWrapper (L, evt->getObject ());

  return 1;
}

int
LuaAPI::_l_Event_getId (lua_State *L)
{
  Event *evt;

  evt = LuaAPI::Event_check (L, 1);
  lua_pushstring (L, evt->getId ().c_str ());

  return 1;
}

int
LuaAPI::_l_Event_getQualifiedId (lua_State *L)
{
  Event *evt;

  evt = LuaAPI::Event_check (L, 1);
  lua_pushstring (L, evt->getQualifiedId ().c_str ());

  return 1;
}

int
LuaAPI::_l_Event_getState (lua_State *L)
{
  Event *evt;

  evt = LuaAPI::Event_check (L, 1);
  Event_State_push (L, evt->getState ());

  return 1;
}

int
LuaAPI::_l_Event_setState (lua_State *L)
{
  Event *evt;
  Event::State state;

  evt = LuaAPI::Event_check (L, 1);
  state = LuaAPI::Event_State_check (L, 2);
  evt->setState (state);

  return 0;
}

int
LuaAPI::_l_Event_getBeginTime (lua_State *L)
{
  Event *evt;
  Time time;

  evt = LuaAPI::Event_check (L, 1);
  time = evt->getBeginTime ();
  if (time == GINGA_TIME_NONE)
    lua_pushnil (L);
  else
    lua_pushinteger (L, (lua_Integer) time);

  return 1;
}

int
LuaAPI::_l_Event_setBeginTime (lua_State *L)
{
  Event *evt;
  Time time;

  evt = LuaAPI::Event_check (L, 1);
  time = (Time) luaL_optinteger (L, 2, GINGA_TIME_NONE);
  evt->setBeginTime (time);

  return 0;
}

int
LuaAPI::_l_Event_getEndTime (lua_State *L)
{
  Event *evt;
  Time time;

  evt = LuaAPI::Event_check (L, 1);
  time = evt->getEndTime ();
  if (time == GINGA_TIME_NONE)
    lua_pushnil (L);
  else
    lua_pushinteger (L, (lua_Integer) time);

  return 1;
}

int
LuaAPI::_l_Event_setEndTime (lua_State *L)
{
  Event *evt;
  Time time;

  evt = LuaAPI::Event_check (L, 1);
  time = (Time) luaL_optinteger (L, 2, GINGA_TIME_NONE);
  evt->setEndTime (time);

  return 0;
}

int
LuaAPI::_l_Event_getLabel (lua_State *L)
{
  Event *evt;
  const char *label;

  evt = LuaAPI::Event_check (L, 1);
  label = evt->getLabel ().c_str ();
  if (g_str_equal (label, ""))
    lua_pushnil (L);
  else
    lua_pushstring (L, label);

  return 1;
}

int
LuaAPI::_l_Event_setLabel (lua_State *L)
{
  Event *evt;
  const char *label;

  evt = LuaAPI::Event_check (L, 1);
  label = luaL_optstring (L, 2, "");
  evt->setLabel (label);

  return 0;
}
