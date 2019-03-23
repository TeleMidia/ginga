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
#include "Player.h"

#define PLAYER_PLAYING_STRING "playing"
#define PLAYER_PAUSED_STRING  "paused"
#define PLAYER_STOPPED_STRING "stopped"

void
LuaAPI::Player_attachWrapper (lua_State *L, Player *player, Media *media)
{
  static const struct luaL_Reg _Player_funcs[] =
    {
     {"getState",     LuaAPI::l_Player_getState},
     {"getURI",       LuaAPI::l_Player_getURI},
     {"setURI",       LuaAPI::l_Player_setURI},
     {"getRect",      LuaAPI::l_Player_getRect},
     {"setRect",      LuaAPI::l_Player_setRect},
     {"getEOS",       LuaAPI::l_Player_getEOS},
     {"setEOS",       LuaAPI::l_Player_setEOS},
     {"start",        LuaAPI::l_Player_start},
     {"pause",        LuaAPI::l_Player_pause},
     {"stop",         LuaAPI::l_Player_stop},
     {NULL, NULL},
    };

  static const struct luaL_Reg *const funcs[] =
    {
     _funcs,
     _Player_funcs,
     NULL,
    };

  static const Chunk *const chunks[] =
    {
     &LuaAPI::_initMt,
     &LuaAPI::_Player_initMt,
     &LuaAPI::_traceMt,
     NULL,
    };

  Player **wrapper;

  g_return_if_fail (L != NULL);
  g_return_if_fail (player != NULL);
  g_return_if_fail (media != NULL);

  // Load and initialize metatable, if not loaded yet.
  LuaAPI::_loadLuaWrapperMt (L, LuaAPI::_PLAYER, funcs, chunks);

  // Create wrapper for player.
  wrapper = (Player **) lua_newuserdata (L, sizeof (Player **));
  g_assert_nonnull (wrapper);
  *wrapper = player;
  luaL_setmetatable (L, LuaAPI::_PLAYER);

  // Set LUA_REGISTRY[evt]=wrapper.
  LuaAPI::_attachLuaWrapper (L, player);

  // Call evt:_attachData().
  LuaAPI::Media_push (L, media);
  LuaAPI::_callLuaWrapper (L, player, "_attachData", 1, 0);
}

void
LuaAPI::Player_detachWrapper (lua_State *L, Player *player)
{
  g_return_if_fail (L != NULL);
  g_return_if_fail (player != NULL);

  // Call evt:_detachData().
  LuaAPI::_callLuaWrapper (L, player, "_detachData", 0, 0);

  // Set LUA_REGISTRY[evt] = nil.
  LuaAPI::_detachLuaWrapper (L, player);
}

Player *
LuaAPI::Player_check (lua_State *L, int i)
{
  g_return_val_if_fail (L != NULL, NULL);
  return *((Player **) luaL_checkudata (L, i, LuaAPI::_PLAYER));
}

Player::State
LuaAPI::Player_State_check (lua_State *L, int i)
{
  static const char *states[] = {PLAYER_PLAYING_STRING,
                                 PLAYER_PAUSED_STRING,
                                 PLAYER_STOPPED_STRING,
                                 NULL};

  g_return_val_if_fail (L != NULL, Player::STOPPED);

  switch (luaL_checkoption (L, i, NULL, states))
    {
    case 0:
      return Player::PLAYING;
    case 1:
      return Player::PAUSED;
    case 2:
      return Player::STOPPED;
    default:
      g_assert_not_reached ();
    }
}

void
LuaAPI::Player_push (lua_State *L, Player *player)
{
  g_return_if_fail (L != NULL);
  g_return_if_fail (player != NULL);

  LuaAPI::_pushLuaWrapper (L, player);
}

void
LuaAPI::Player_State_push (lua_State *L, Player::State state)
{
  g_return_if_fail (L != NULL);

  switch (state)
    {
    case Player::PLAYING:
      lua_pushliteral (L, PLAYER_PLAYING_STRING);
      break;
    case Player::PAUSED:
      lua_pushliteral (L, PLAYER_PAUSED_STRING);
      break;
    case Player::STOPPED:
      lua_pushliteral (L, PLAYER_STOPPED_STRING);
      break;
    default:
      g_assert_not_reached ();
    }
}

void
LuaAPI::Player_call (lua_State *L, Player *player, const char *name,
                     int nargs, int nresults)
{
  g_return_if_fail (L != NULL);
  g_return_if_fail (player != NULL);
  g_return_if_fail (name != NULL);
  g_return_if_fail (nargs >= 0);
  g_return_if_fail (nresults >= 0);

  LuaAPI::_callLuaWrapper (L, player, name, nargs, nresults);
}

int
LuaAPI::l_Player_getState (lua_State *L)
{
  Player *player;

  player = LuaAPI::Player_check (L, 1);
  LuaAPI::Player_State_push (L, player->getState ());

  return 1;
}

int
LuaAPI::l_Player_getURI (lua_State *L)
{
  Player *player;

  player = LuaAPI::Player_check (L, 1);
  lua_pushstring (L, player->getURI ().c_str ());

  return 1;
}

int
LuaAPI::l_Player_setURI (lua_State *L)
{
  Player *player;
  const char *uri;

  player = LuaAPI::Player_check (L, 1);
  uri = luaL_optstring (L, 2, "");
  player->setURI (string (uri));

  return 0;
}

int
LuaAPI::l_Player_getRect (lua_State *L)
{
  Player *player;
  int x, y, width, height;

  player = LuaAPI::Player_check (L, 1);
  player->getRect (&x, &y, &width, &height);
  lua_pushinteger (L, x);
  lua_pushinteger (L, y);
  lua_pushinteger (L, width);
  lua_pushinteger (L, height);

  return 4;
}

int
LuaAPI::l_Player_setRect (lua_State *L)
{
  Player *player;
  lua_Integer x, y, width, height;

  player = LuaAPI::Player_check (L, 1);
  x = luaL_checkinteger (L, 2);
  y = luaL_checkinteger (L, 3);
  width = luaL_checkinteger (L, 4);
  height = luaL_checkinteger (L, 5);
  player->setRect ((int) CLAMP (x, G_MININT, G_MAXINT),
                   (int) CLAMP (y, G_MININT, G_MAXINT),
                   (int) CLAMP (width, G_MININT, G_MAXINT),
                   (int) CLAMP (height, G_MININT, G_MAXINT));
  return 0;
}

int
LuaAPI::l_Player_getEOS (lua_State *L)
{
  Player *player;

  player = LuaAPI::Player_check (L, 1);
  lua_pushboolean (L, player->getEOS ());

  return 1;
}

int
LuaAPI::l_Player_setEOS (lua_State *L)
{
  Player *player;

  player = LuaAPI::Player_check (L, 1);
  luaL_checkany (L, 2);
  player->setEOS (lua_toboolean (L, 2));

  return 0;
}

int
LuaAPI::l_Player_start (lua_State *L)
{
  Player *player;

  player = LuaAPI::Player_check (L, 1);
  player->start ();

  return 0;
}

int
LuaAPI::l_Player_pause (lua_State *L)
{
  Player *player;

  player = LuaAPI::Player_check (L, 1);
  player->pause ();

  return 0;
}

int
LuaAPI::l_Player_stop (lua_State *L)
{
  Player *player;

  player = LuaAPI::Player_check (L, 1);
  player->stop ();

  return 0;
}
