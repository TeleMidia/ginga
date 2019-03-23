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
#include "StateMachine.h"

#define STATE_MACHINE_ATTRIBUTION_STRING  "attribution"
#define STATE_MACHINE_PRESENTATION_STRING "presentation"
#define STATE_MACHINE_SELECTION_STRING    "selection"

#define STATE_MACHINE_OCCURRING_STRING    "occurring"
#define STATE_MACHINE_PAUSED_STRING       "paused"
#define STATE_MACHINE_SLEEPING_STRING     "sleeping"

#define STATE_MACHINE_ABORT_STRING        "abort"
#define STATE_MACHINE_PAUSE_STRING        "pause"
#define STATE_MACHINE_RESUME_STRING       "resume"
#define STATE_MACHINE_START_STRING        "start"
#define STATE_MACHINE_STOP_STRING         "stop"

void
LuaAPI::StateMachine_attachWrapper (lua_State *L, StateMachine *evt, Object *obj,
                                    StateMachine::Type type,
                                    const string &id)
{
  static const struct luaL_Reg *const funcs[] =
    {
     _funcs,
     NULL,
    };

  static const Chunk *const chunks[] =
    {
     &LuaAPI::_initMt,
     &LuaAPI::_StateMachine_initMt,
     &LuaAPI::_traceMt,
     NULL,
    };

  StateMachine **wrapper;

  g_return_if_fail (L != NULL);
  g_return_if_fail (evt != NULL);
  g_return_if_fail (obj != NULL);

  // Load and initialize metatable, if not loaded yet.
  LuaAPI::_loadLuaWrapperMt (L, LuaAPI::_STATE_MACHINE, funcs, chunks);

  // Create wrapper for state machine.
  wrapper = (StateMachine **) lua_newuserdata (L, sizeof (StateMachine **));
  g_assert_nonnull (wrapper);
  *wrapper = evt;
  luaL_setmetatable (L, LuaAPI::_STATE_MACHINE);

  // Set LUA_REGISTRY[evt]=wrapper.
  LuaAPI::_attachLuaWrapper (L, evt);

  // Call evt:_attachData().
  LuaAPI::Object_push (L, obj);
  LuaAPI::StateMachine_Type_push (L, type);
  lua_pushstring (L, id.c_str ());
  LuaAPI::_callLuaWrapper (L, evt, "_attachData", 3, 0);
}

void
LuaAPI::StateMachine_detachWrapper (lua_State *L, StateMachine *evt)
{
  g_return_if_fail (L != NULL);
  g_return_if_fail (evt != NULL);

  // Call evt:_detachData().
  LuaAPI::_callLuaWrapper (L, evt, "_detachData", 0, 0);

  // Set LUA_REGISTRY[evt] = nil.
  LuaAPI::_detachLuaWrapper (L, evt);
}

StateMachine *
LuaAPI::StateMachine_check (lua_State *L, int i)
{
  g_return_val_if_fail (L != NULL, NULL);
  return *((StateMachine **) luaL_checkudata (L, i, LuaAPI::_STATE_MACHINE));
}

StateMachine::Type
LuaAPI::StateMachine_Type_check (lua_State *L, int i)
{
  static const char *types[] = {STATE_MACHINE_ATTRIBUTION_STRING,
                                STATE_MACHINE_PRESENTATION_STRING,
                                STATE_MACHINE_SELECTION_STRING,
                                NULL};

  g_return_val_if_fail (L != NULL, StateMachine::PRESENTATION);

  switch (luaL_checkoption (L, i, NULL, types))
    {
    case 0:
      return StateMachine::ATTRIBUTION;
    case 1:
      return StateMachine::PRESENTATION;
    case 2:
      return StateMachine::SELECTION;
    default:
      g_assert_not_reached ();
    }
}

StateMachine::State
LuaAPI::StateMachine_State_check (lua_State *L, int i)
{
  static const char *states[] = {STATE_MACHINE_OCCURRING_STRING,
                                 STATE_MACHINE_PAUSED_STRING,
                                 STATE_MACHINE_SLEEPING_STRING,
                                 NULL};

  g_return_val_if_fail (L != NULL, StateMachine::SLEEPING);

  switch (luaL_checkoption (L, i, NULL, states))
    {
    case 0:
      return StateMachine::OCCURRING;
    case 1:
      return StateMachine::PAUSED;
    case 2:
      return StateMachine::SLEEPING;
    default:
      g_assert_not_reached ();
    }
}

StateMachine::Transition
LuaAPI::StateMachine_Transition_check (lua_State *L, int i)
{
  static const char *transitions[] = {STATE_MACHINE_ABORT_STRING,
                                      STATE_MACHINE_PAUSE_STRING,
                                      STATE_MACHINE_RESUME_STRING,
                                      STATE_MACHINE_START_STRING,
                                      STATE_MACHINE_STOP_STRING,
                                      NULL};

  g_return_val_if_fail (L != NULL, StateMachine::ABORT);

  switch (luaL_checkoption (L, i, NULL, transitions))
    {
    case 0:
      return StateMachine::ABORT;
    case 1:
      return StateMachine::PAUSE;
    case 2:
      return StateMachine::RESUME;
    case 3:
      return StateMachine::START;
    case 4:
      return StateMachine::STOP;
    default:
      g_assert_not_reached ();
    }
}

void
LuaAPI::StateMachine_push (lua_State *L, StateMachine *evt)
{
  g_return_if_fail (L != NULL);
  g_return_if_fail (evt != NULL);

  LuaAPI::_pushLuaWrapper (L, evt);
}

void
LuaAPI::StateMachine_Type_push (lua_State *L, StateMachine::Type type)
{
  g_return_if_fail (L != NULL);

  switch (type)
    {
    case StateMachine::ATTRIBUTION:
      lua_pushliteral (L, STATE_MACHINE_ATTRIBUTION_STRING);
      break;
    case StateMachine::PRESENTATION:
      lua_pushliteral (L, STATE_MACHINE_PRESENTATION_STRING);
      break;
    case StateMachine::SELECTION:
      lua_pushliteral (L, STATE_MACHINE_SELECTION_STRING);
      break;
    default:
      g_assert_not_reached ();
    }
}

void
LuaAPI::StateMachine_State_push (lua_State *L, StateMachine::State state)
{
  g_return_if_fail (L != NULL);

  switch (state)
    {
    case StateMachine::OCCURRING:
      lua_pushliteral (L, STATE_MACHINE_OCCURRING_STRING);
      break;
    case StateMachine::PAUSED:
      lua_pushliteral (L, STATE_MACHINE_PAUSED_STRING);
      break;
    case StateMachine::SLEEPING:
      lua_pushliteral (L, STATE_MACHINE_SLEEPING_STRING);
      break;
    default:
      g_assert_not_reached ();
    }
}

void
LuaAPI::StateMachine_Transition_push (lua_State *L,
                                      StateMachine::Transition trans)
{
  g_return_if_fail (L != NULL);

  switch (trans)
    {
    case StateMachine::ABORT:
      lua_pushliteral (L, STATE_MACHINE_ABORT_STRING);
      break;
    case StateMachine::PAUSE:
      lua_pushliteral (L, STATE_MACHINE_PAUSE_STRING);
      break;
    case StateMachine::RESUME:
      lua_pushliteral (L, STATE_MACHINE_RESUME_STRING);
      break;
    case StateMachine::START:
      lua_pushliteral (L, STATE_MACHINE_START_STRING);
      break;
    case StateMachine::STOP:
      lua_pushliteral (L, STATE_MACHINE_STOP_STRING);
      break;
    default:
      g_assert_not_reached ();
    }
}

void
LuaAPI::StateMachine_call (lua_State *L, StateMachine *evt,
                           const char *name, int nargs, int nresults)
{
  g_return_if_fail (L != NULL);
  g_return_if_fail (evt != NULL);
  g_return_if_fail (name != NULL);
  g_return_if_fail (nargs >= 0);
  g_return_if_fail (nresults >= 0);

  LuaAPI::_callLuaWrapper (L, evt, name, nargs, nresults);
}
