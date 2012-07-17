/* Event.h -- The semantics of NCL 3.0 events.
   Copyright (C) 2012 PUC-Rio/Laboratorio TeleMidia

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2 of the License, or (at your option)
any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc., 51
Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

#ifndef LUAPLAYER_EVENT_H
#define LUAPLAYER_EVENT_H

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include "nclua.h"

#include "player/LuaPlayer.h"

#define ASSERT_NOT_REACHED assert (!"reached")

#define integralof(x)  (((char *)(x)) - ((char *) 0))
#define nelementsof(x) (sizeof (x) / sizeof (x[0]))
#define pointerof(x)   ((void *)((char *) 0 + (x)))
#define streq(x,y)     (strcmp (x, y) == 0)


/* Constants.  */

#define EV_FIELD_CLASS           "class"

/* NCL */
#define EV_NCL_CLASS             "ncl"
#define EV_NCL_FIELD_ACTION      "action"
#define EV_NCL_FIELD_LABEL       "label"
#define EV_NCL_FIELD_NAME        "name"
#define EV_NCL_FIELD_TYPE        "type"
#define EV_NCL_FIELD_VALUE       "value"
#define EV_NCL_TYPE_ATTRIBUTION  "attribution"
#define EV_NCL_TYPE_PRESENTATION "presentation"
#define EV_NCL_TYPE_SELECTION    "selection"
#define EV_NCL_ACTION_ABORT      "abort"
#define EV_NCL_ACTION_PAUSE      "pause"
#define EV_NCL_ACTION_RESUME     "resume"
#define EV_NCL_ACTION_START      "start"
#define EV_NCL_ACTION_STOP       "stop"

/* Key */
#define EV_KEY_CLASS             "key"
#define EV_KEY_FIELD_KEY         "key"
#define EV_KEY_FIELD_TYPE        "type"
#define EV_KEY_TYPE_PRESS        "press"
#define EV_KEY_TYPE_RELEASE      "release"

/* TCP */
#define EV_TCP_CLASS             "tcp"
#define EV_TCP_FIELD_CONNECTION  "connection"
#define EV_TCP_FIELD_ERROR       "error"
#define EV_TCP_FIELD_HOST        "host"
#define EV_TCP_FIELD_PORT        "port"
#define EV_TCP_FIELD_TIMEOUT     "timeout"
#define EV_TCP_FIELD_TYPE        "type"
#define EV_TCP_FIELD_VALUE       "value"
#define EV_TCP_TYPE_CONNECT      "connect"
#define EV_TCP_TYPE_DATA         "data"
#define EV_TCP_TYPE_DISCONNECT   "disconnect"

/* User */
#define EV_USER_CLASS            "user"


/* Error messages.  */

#define _ev_error(L, format, ...) \
  (lua_pushboolean (L, 0), lua_pushfstring (L, format, ## __VA_ARGS__), 2)

#define ev_error_bad(L, field) \
  _ev_error (L, "bad event %s", field)

#define ev_error_invalid(L, field, value) \
  _ev_error (L, "invalid event %s '%s'", field, value)


/* Auxiliary functions.  */

typedef struct
{
  const char *key;              /* map key */
  void *value;                  /* map value */
} ev_map_t;

static int
ev_map_compare (const void *p1, const void *p2)
{
  return strcmp (((ev_map_t *) p1)->key, ((ev_map_t *) p2)->key);
}

static const ev_map_t *
ev_map_get (const ev_map_t map[], size_t size, const char *key)
{
  ev_map_t e = {key, NULL};
  return (const ev_map_t *)
    bsearch (&e, map, size, sizeof (ev_map_t), ev_map_compare);
}

#define _EV_GETXFIELD_BODY(lua_isx, lua_tox)            \
  {                                                     \
    int status = 0;                                     \
    lua_getfield (L, index, key);                       \
    if (lua_isx (L, -1))                                \
      {                                                 \
        status = 1;                                     \
        *value = lua_tox (L, -1);                       \
      }                                                 \
    lua_pop (L, 1);                                     \
    return status;                                      \
  }

/* Gets the integer value associated with key KEY in table at INDEX.
   If successful, stores value into *VALUE and returns true.
   Otherwise, returns false.  */

static int
ev_getintegerfield (lua_State *L, int index, const char *key, int *value)
{
  _EV_GETXFIELD_BODY (lua_isnumber, lua_tointeger);
}

/* Gets the number value associated with key KEY in table at INDEX.
   If successful, stores the value into *VALUE and returns true.
   Otherwise, returns false.  */

static int
ev_getnumberfield (lua_State *L, int index, const char *key, double *value)
{
  _EV_GETXFIELD_BODY (lua_isnumber, lua_tonumber);
}

/* Gets the string value associated with key KEY in table at INDEX.
   If successful, stores value into *VALUE and returns true.
   Otherwise, returns false.  */

static int
ev_getstringfield (lua_State *L, int index,
                   const char *key, const char **value)
{
  _EV_GETXFIELD_BODY (lua_isstring, lua_tostring)
}

/* Gets the user data value associated with key KEY in table at INDEX.
   If successful, stores value into *VALUE and returns true.
   Otherwise, returns false.  */

static int
ev_getuserdatafield (lua_State *L, int index, const char *key, void **value)
{
  _EV_GETXFIELD_BODY (lua_isuserdata, lua_touserdata);
}


/* NCL class.  */

/* Type name to code.  */
static const ev_map_t ev_ncl_type_map[] =
{
  /* KEEP THIS SORTED ALPHABETICALLY */
  { EV_NCL_TYPE_ATTRIBUTION,  (void *) Player::TYPE_ATTRIBUTION  },
  { EV_NCL_TYPE_PRESENTATION, (void *) Player::TYPE_PRESENTATION },
  { EV_NCL_TYPE_SELECTION,    (void *) Player::TYPE_SELECTION    },
};

/* Action name to code.  */
static const ev_map_t ev_ncl_action_map[] =
{
  /* KEEP THIS SORTED ALPHABETICALLY */
  { EV_NCL_ACTION_ABORT,  (void *) Player::PL_NOTIFY_ABORT  },
  { EV_NCL_ACTION_PAUSE,  (void *) Player::PL_NOTIFY_PAUSE  },
  { EV_NCL_ACTION_RESUME, (void *) Player::PL_NOTIFY_RESUME },
  { EV_NCL_ACTION_START,  (void *) Player::PL_NOTIFY_START  },
  { EV_NCL_ACTION_STOP,   (void *) Player::PL_NOTIFY_STOP   },
};

#define _ev_ncl_get_type(type) \
  ev_map_get (ev_ncl_type_map, nelementsof (ev_ncl_type_map), type)

#define _ev_ncl_get_action(action) \
  ev_map_get (ev_ncl_action_map, nelementsof (ev_ncl_action_map), action)

/* Gets the type value associated with type name TYPE.  */
#define ev_ncl_get_type_value(type) \
  ((ptrdiff_t)(_ev_ncl_get_type (type))->value)

/* Gets the action value associated with type name ACTION.  */
#define ev_ncl_get_action_value(action) \
  ((ptrdiff_t)(_ev_ncl_get_action (action))->value)

/* Gets the type name associated with type value TYPE.  */

static const char *
ev_ncl_get_type_name (int type)
{
  switch (type)
    {
    case Player::TYPE_ATTRIBUTION:  return EV_NCL_TYPE_ATTRIBUTION;
    case Player::TYPE_PRESENTATION: return EV_NCL_TYPE_PRESENTATION;
    case Player::TYPE_SELECTION:    return EV_NCL_TYPE_SELECTION;
    }
  ASSERT_NOT_REACHED;
  return NULL;
}

/* Gets the action name associated with action value ACTION.  */

static const char *
ev_ncl_get_action_name (int action)
{
  switch (action)
    {
    case Player::PL_NOTIFY_ABORT:  return EV_NCL_ACTION_ABORT;
    case Player::PL_NOTIFY_PAUSE:  return EV_NCL_ACTION_PAUSE;
    case Player::PL_NOTIFY_RESUME: return EV_NCL_ACTION_RESUME;
    case Player::PL_NOTIFY_START:  return EV_NCL_ACTION_START;
    case Player::PL_NOTIFY_STOP:   return EV_NCL_ACTION_STOP;
    }
  ASSERT_NOT_REACHED;
  return NULL;
}

#define ev_ncl_class_is_valid(c)  (streq (c, EV_NCL_CLASS))
#define ev_ncl_type_is_valid(t)   (_ev_ncl_get_type (t) != NULL)
#define ev_ncl_action_is_valid(a) (_ev_ncl_get_action (action) != NULL)

/* ev_check_ncl_event (event:table)
   -> status:boolean, [error_message:string]

   Checks if event EVENT is a valid NCL event.  Returns true if successful,
   otherwise returns false plus error message.  */

static int
ev_check_ncl_event (lua_State *L)
{
  const char *cls;
  const char *type;
  const char *action;

  luaL_checktype (L, 1, LUA_TTABLE);

  if (!ev_getstringfield (L, 1, EV_FIELD_CLASS, &cls))
    return ev_error_bad (L, EV_FIELD_CLASS);

  if (!streq (cls, EV_NCL_CLASS))
    return ev_error_invalid (L, EV_FIELD_CLASS, cls);

  if (!ev_getstringfield (L, 1, EV_NCL_FIELD_TYPE, &type))
    return ev_error_bad (L, EV_NCL_FIELD_TYPE);

  if (!ev_ncl_type_is_valid (type))
    return ev_error_invalid (L, EV_NCL_FIELD_TYPE, type);

  if (!ev_getstringfield (L, 1, EV_NCL_FIELD_ACTION, &action))
    return ev_error_bad (L, EV_NCL_FIELD_ACTION);

  if (!ev_ncl_action_is_valid (action))
    return ev_error_invalid (L, EV_NCL_FIELD_ACTION, action);

  if (streq (type, EV_NCL_TYPE_ATTRIBUTION))
    {
      const char *name;
      const char *value;

      if (!ev_getstringfield (L, 1, EV_NCL_FIELD_NAME, &name))
        return ev_error_bad (L, EV_NCL_FIELD_NAME);

      if (!ev_getstringfield (L, 1, EV_NCL_FIELD_VALUE, &value))
        return ev_error_bad (L, EV_NCL_FIELD_VALUE);
    }
  else if (streq (type, EV_NCL_TYPE_PRESENTATION)
           || streq (type, EV_NCL_TYPE_SELECTION))
    {
      const char *label;

      if (!ev_getstringfield (L, 1, EV_NCL_FIELD_LABEL, &label))
        return ev_error_bad (L, EV_NCL_FIELD_LABEL);
    }
  else
    {
      ASSERT_NOT_REACHED;
    }

  lua_pushboolean (L, 1);
  return 1;
}

/* ev_get_ncl_filter (class:string, [type, name, action:string])
   -> filter:table or status:boolean, [error_message:string]

   Creates a filter table for NCL events with the given parameters.
   Returns the resulting filter table if successful,
   otherwise returns false plus error message.  */

static int
ev_get_ncl_filter (lua_State *L)
{
  const char *cls;
  const char *type;
  const char *name;
  const char *action;

  cls = luaL_checkstring (L, 1);
  if (!streq (cls, EV_NCL_CLASS))
    return ev_error_invalid (L, EV_NCL_CLASS, cls);

  type = luaL_optstring (L, 2, NULL);
  if (type != NULL && !ev_ncl_type_is_valid (type))
    return ev_error_invalid (L, EV_NCL_FIELD_TYPE, type);

  name = luaL_optstring (L, 3, NULL);

  action = luaL_optstring (L, 4, NULL);
  if (action != NULL && !ev_ncl_action_is_valid (action))
    return ev_error_invalid (L, EV_NCL_FIELD_ACTION, action);

  /* Create filter table.  */

  lua_createtable (L, 0, 4);
  lua_pushstring (L, EV_NCL_CLASS);
  lua_setfield (L, -2, EV_FIELD_CLASS);

  if (type != NULL)
    {
      lua_pushstring (L, type);
      lua_setfield (L, -2, EV_NCL_FIELD_TYPE);

      if (name != NULL)
        {
          lua_pushstring (L, name);
          if (streq (type, EV_NCL_TYPE_ATTRIBUTION))
            {
              lua_setfield (L, -2, EV_NCL_FIELD_NAME);
            }
          else if (streq (type, EV_NCL_TYPE_PRESENTATION)
                   || streq (type, EV_NCL_TYPE_SELECTION))
            {
              lua_setfield (L, -2, EV_NCL_FIELD_LABEL);
            }
          else
            {
              ASSERT_NOT_REACHED;
            }
        }
    }

  if (action != NULL)
    {
      lua_pushstring (L, action);
      lua_setfield (L, -2, EV_NCL_FIELD_ACTION);
    }

  return 1;
}

/* ev_receive_ncl_event (event:table)

   Receives the NCL event EVENT.
   This function assumes that event is a valid NCL event.  */

static int
ev_receive_ncl_event (lua_State *L)
{
  const char *type;
  const char *action;
  const char *name;
  const char *value;

  nclua_t *nc;
  LuaPlayer *player;
  int type_value;
  int action_value;

  int n = ev_check_ncl_event (L);
  if (n > 1)
    {
      lua_pop (L, n);
    }
  assert (lua_toboolean (L, -1));
  lua_pop (L, 1);

  /* Receive event.  */

  nc = nclua_get_nclua_state (L);
  player = (LuaPlayer *) nclua_get_user_data (nc, NULL);

  assert (ev_getstringfield (L, 1, EV_NCL_FIELD_TYPE, &type));
  assert (ev_getstringfield (L, 1, EV_NCL_FIELD_ACTION, &action));

  type_value = ev_ncl_get_type_value (type);
  action_value = ev_ncl_get_action_value (action);

  switch (type_value)
    {
    case Player::TYPE_ATTRIBUTION:
      assert (ev_getstringfield (L, 1, EV_NCL_FIELD_NAME, &name));
      assert (ev_getstringfield (L, 1, EV_NCL_FIELD_VALUE, &value));
      player->notifyPlayerListeners (action_value, string (name),
                                     type_value, string (value));
      break;

    case Player::TYPE_PRESENTATION:
      assert (ev_getstringfield (L, 1, EV_NCL_FIELD_LABEL, &name));

      if (streq (name, "") && action_value == Player::PL_NOTIFY_STOP)
        {

          /* FIXME (The "Stop" Mess - Part I): If the NCLua script posted a
             "stop" event, we have to destroy the NCLua state immediately.
             Otherwise, notifyPlayerListeners() will cause the event to be
             sent back to the NCLua engine.  */

          LuaPlayer::nc_update_remove (nc);
          player->doStop ();
        }

      player->notifyPlayerListeners (action_value, string (name));
      break;

    case Player::TYPE_SELECTION:
      break;                    /* TODO: not implemented */
    }

  return 0;
}

/* Sends NCL event with the given parameters to NCLua state NC.  */

#define ev_send_ncl_presentation_event(nc, action, label) \
  ev_send_ncl_event (nc, Player::TYPE_PRESENTATION, action, label, NULL)

#define ev_send_ncl_attribution_event(nc, action, name, value)  \
  ev_send_ncl_event (nc, Player::TYPE_ATTRIBUTION, action, name, value)

static void
ev_send_ncl_event (nclua_t *nc, int type, int action,
                   const char *name, const char *value)
{
  lua_State *L;

  L = nclua_get_lua_state (nc);

  lua_createtable (L, 0, 5);

  lua_pushstring (L, EV_NCL_CLASS);
  lua_setfield (L, -2, EV_FIELD_CLASS);

  lua_pushstring (L, ev_ncl_get_type_name (type));
  lua_setfield (L, -2, EV_NCL_FIELD_TYPE);

  lua_pushstring (L, ev_ncl_get_action_name (action));
  lua_setfield (L, -2, EV_NCL_FIELD_ACTION);

  switch (type)
    {
    case Player::TYPE_ATTRIBUTION:
      assert (name != NULL);
      lua_pushstring (L, name);
      lua_setfield (L, -2, EV_NCL_FIELD_NAME);

      assert (value != NULL);
      lua_pushstring (L, value);
      lua_setfield (L, -2, EV_NCL_FIELD_VALUE);
      break;

    case Player::TYPE_PRESENTATION:
    case Player::TYPE_SELECTION:
      assert (name != NULL);
      lua_pushstring (L, name);
      lua_setfield (L, -2, EV_NCL_FIELD_LABEL);
      break;
    default:
      ASSERT_NOT_REACHED;
    }

  nclua_send (nc, L);
}


/* Key class.  */

/* Returns true if type TYPE is valid.  */
#define ev_key_type_is_valid(type) \
  (streq (type, EV_KEY_TYPE_PRESS) || streq (type, EV_KEY_TYPE_RELEASE))

/* ev_check_key_event (event:table)
   -> status:boolean, [error_message:string]

   Checks if event EVENT is a valid key event.  Returns true if successful,
   otherwise returns false plus error message.  */

static int
ev_check_key_event (lua_State *L)
{
  const char *cls;
  const char *type;
  const char *key;

  luaL_checktype (L, 1, LUA_TTABLE);

  if (!ev_getstringfield (L, 1, EV_FIELD_CLASS, &cls))
    return ev_error_bad (L, EV_FIELD_CLASS);

  if (!streq (cls, EV_KEY_CLASS))
    return ev_error_invalid (L, EV_FIELD_CLASS, cls);

  if (!ev_getstringfield (L, 1, EV_KEY_FIELD_TYPE, &type))
    return ev_error_bad (L, EV_KEY_FIELD_TYPE);

  if (!ev_key_type_is_valid (type))
    return ev_error_invalid (L, EV_KEY_FIELD_TYPE, type);

  if (!ev_getstringfield (L, 1, EV_KEY_FIELD_KEY, &key))
    return ev_error_bad (L, EV_KEY_FIELD_KEY);

  /* TODO: Check if key is valid.  */

  key = lua_tostring (L, -1);
  lua_pop (L, 1);

  lua_pushboolean (L, 1);
  return 1;
}

/* ev_get_key_filter (class:string, [type, key:string])
   -> filter:table or status:boolean, [error_message:string]

   Creates a filter table for key events with the given parameters.
   Returns the resulting filter table if successful,
   otherwise returns false plus error message.  */

static int
ev_get_key_filter (lua_State *L)
{
  const char *cls;
  const char *type;
  const char *key;

  cls = luaL_checkstring (L, 1);
  if (!streq (cls, EV_KEY_CLASS))
    return ev_error_invalid (L, EV_KEY_CLASS, cls);

  type = luaL_optstring (L, 2, NULL);
  if (type != NULL && !ev_key_type_is_valid (type))
    return ev_error_invalid (L, EV_KEY_FIELD_TYPE, type);

  key = luaL_optstring (L, 3, NULL);

  /* Create push filter table.  */

  lua_createtable (L, 0, 3);
  lua_pushstring (L, EV_KEY_CLASS);
  lua_setfield (L, -2, EV_FIELD_CLASS);

  if (type != NULL)
    {
      lua_pushstring (L, type);
      lua_setfield (L, -2, EV_KEY_FIELD_TYPE);
    }

  if (key != NULL)
    {
      lua_pushstring (L, key);
      lua_setfield (L, -2, EV_KEY_FIELD_KEY);
    }

  return 1;
}

/* Sends key event with the given parameters to the NCLua state NC.  */

static void
ev_send_key_event (nclua_t *nc, const char *key, int press)
{
  lua_State *L;
  const char *type;

  L = nclua_get_lua_state (nc);

  lua_createtable (L, 0, 3);

  lua_pushstring (L, EV_KEY_CLASS);
  lua_setfield (L, -2, EV_FIELD_CLASS);

  lua_pushstring (L, key);
  lua_setfield (L, -2, EV_KEY_FIELD_KEY);

  type = (press) ? EV_KEY_TYPE_PRESS : EV_KEY_TYPE_RELEASE;
  lua_pushstring (L, type);
  lua_setfield (L, -2, EV_KEY_FIELD_TYPE);

  nclua_send (nc, L);
}


/* TCP class.  */

/* Registry key of the TCP input function.  */
static int ev_tcp_in_key;

/* Registry key of the TCP output function.  */
static int ev_tcp_out_key;

/* Opens the TCP module.  */

static void
ev_tcp_open (lua_State *L)
{
  lua_getglobal (L, "require");
  lua_pushstring (L, "tcp_event");
  lua_call (L, 1, 1);

  lua_pushlightuserdata (L, (void *) &ev_tcp_in_key);
  lua_rawgeti (L, -2, 1);
  lua_rawset (L, LUA_REGISTRYINDEX);

  lua_pushlightuserdata (L, (void *) &ev_tcp_out_key);
  lua_rawgeti (L, -2, 2);
  lua_rawset (L, LUA_REGISTRYINDEX);

  lua_pop (L, 1);
}

/* Closes the TCP module.  */

static void
ev_tcp_close (lua_State *L)
{
  lua_pushlightuserdata (L, (void *) &ev_tcp_in_key);
  lua_pushnil (L);
  lua_rawset (L, LUA_REGISTRYINDEX);

  lua_pushlightuserdata (L, (void *) &ev_tcp_out_key);
  lua_pushnil (L);
  lua_rawset (L, LUA_REGISTRYINDEX);
}

/* Pushes the TCP input function onto stack.  */

static void
ev_tcp_push_input_function (lua_State *L)
{
  lua_pushlightuserdata (L, (void *) &ev_tcp_in_key);
  lua_rawget (L, LUA_REGISTRYINDEX);
}

/* Pushes the TCP output function onto stack.  */

static void
ev_tcp_push_output_function (lua_State *L)
{
  lua_pushlightuserdata (L, (void *) &ev_tcp_out_key);
  lua_rawget (L, LUA_REGISTRYINDEX);
}

/* Pushes pending TCP events onto input queue.
   This function calls event.post directly.  */

static void
ev_tcp_cycle (lua_State *L)
{
  ev_tcp_push_input_function (L);
  lua_call (L, 0, 0);
}

/* Returns true if type TYPE is valid.  */
#define ev_tcp_type_is_valid(type)              \
  (streq (type, EV_TCP_TYPE_CONNECT)            \
   || streq (type, EV_TCP_TYPE_DATA)            \
   || streq (type, EV_TCP_TYPE_DISCONNECT))

/* ev_check_tcp_event (event:table)
   -> status:boolean, [error_message:string]

   Checks if event EVENT is a valid TCP event.  Returns true if successful,
   otherwise returns false plus error message.  */

static int
ev_check_tcp_event (lua_State *L)
{
  const char *cls;
  const char *type;

  luaL_checktype (L, 1, LUA_TTABLE);

  if (!ev_getstringfield (L, 1, EV_FIELD_CLASS, &cls))
    return ev_error_bad (L, EV_FIELD_CLASS);

  if (!streq (cls, EV_TCP_CLASS))
    return ev_error_invalid (L, EV_FIELD_CLASS, cls);

  if (!ev_getstringfield (L, 1, EV_TCP_FIELD_TYPE, &type))
    return ev_error_bad (L, EV_TCP_FIELD_TYPE);

  /* connect */

  if (streq (type, EV_TCP_TYPE_CONNECT))
    {
      const char *host;
      int port;

      if (!ev_getstringfield (L, 1, EV_TCP_FIELD_HOST, &host))
        return ev_error_bad (L, EV_TCP_FIELD_HOST);

      if (!ev_getintegerfield (L, 1, EV_TCP_FIELD_PORT, &port))
        return ev_error_bad (L, EV_TCP_FIELD_PORT);

      lua_getfield (L, 1, EV_TCP_FIELD_TIMEOUT);
      if (!lua_isnil (L, -1) && !lua_isnumber (L, -1))
        return ev_error_bad (L, EV_TCP_FIELD_TIMEOUT);
      lua_pop (L, 1);

      lua_getfield (L, 1, EV_TCP_FIELD_CONNECTION);
      if (!lua_isnil (L, -1) && !lua_isuserdata (L, -1))
        return ev_error_bad (L, EV_TCP_FIELD_CONNECTION);
      lua_pop (L, 1);

      lua_getfield (L, 1, EV_TCP_FIELD_ERROR);
      if (!lua_isnil (L, -1) && !lua_isstring (L, -1))
        return ev_error_bad (L, EV_TCP_FIELD_ERROR);
      lua_pop (L, 1);
    }

  /* data */

  else if (streq (type, EV_TCP_TYPE_DATA))
    {
      void *connection;
      const char *value;

      if (!ev_getuserdatafield (L, 1, EV_TCP_FIELD_CONNECTION, &connection))
        return ev_error_bad (L, EV_TCP_FIELD_CONNECTION);

      if (!ev_getstringfield (L, 1, EV_TCP_FIELD_VALUE, &value))
        return ev_error_bad (L, EV_TCP_FIELD_VALUE);

      lua_getfield (L, 1, EV_TCP_FIELD_TIMEOUT);
      if (!lua_isnil (L, -1) && !lua_isnumber (L, -1))
        return ev_error_bad (L, EV_TCP_FIELD_TIMEOUT);
      lua_pop (L, 1);

      lua_getfield (L, 1, EV_TCP_FIELD_ERROR);
      if (!lua_isnil (L, -1) && !lua_isstring (L, -1))
        return ev_error_bad (L, EV_TCP_FIELD_ERROR);
      lua_pop (L, 1);
    }

  /* disconnect */

  else if (streq (type, EV_TCP_TYPE_DISCONNECT))
    {
      void *connection;

      if (!ev_getuserdatafield (L, 1, EV_TCP_FIELD_CONNECTION, &connection))
        return ev_error_bad (L, EV_TCP_FIELD_CONNECTION);
    }

  /* oops... */

  else
    {
      return ev_error_invalid (L, EV_TCP_FIELD_TYPE, type);
    }

  lua_pushboolean (L, 1);
  return 1;
}

/* ev_get_ncl_filter (class:string, [connection:userdata])
   -> filter:table or status:boolean, [error_message:string]

   Creates a filter table for TCP events with the given parameters.
   Returns the resulting filter table if successful,
   otherwise returns false plus error message.  */

static int
ev_get_tcp_filter (lua_State *L)
{
  const char *cls;
  void *connection = NULL;

  cls = luaL_checkstring (L, 1);
  if (!streq (cls, EV_TCP_CLASS))
    return ev_error_invalid (L, EV_NCL_CLASS, cls);

  if (!lua_isnoneornil (L, 2))
    {
      if (!lua_isuserdata (L, 2))
        return ev_error_bad (L, EV_TCP_FIELD_CONNECTION);
      connection = lua_touserdata (L, 2);
    }

  /* Create filter table.  */

  lua_createtable (L, 0, 4);
  lua_pushstring (L, EV_TCP_CLASS);
  lua_setfield (L, -2, EV_FIELD_CLASS);

  if (connection != NULL)
    {
      lua_pushlightuserdata (L, connection);
      lua_setfield (L, -2, EV_TCP_FIELD_CONNECTION);
    }

  return 1;
}

/* ev_receive_tcp_event (event:table)

   Receives the TCP event EVENT.
   This function assumes that event is a valid TCP event.  */

static int
ev_receive_tcp_event (lua_State *L)
{
  ev_tcp_push_output_function (L);
  lua_pushvalue (L, 1);
  lua_call (L, 1, 0);
  return 0;
}


/* User class.  */

/* ev_check_user_event (event:table)
   -> status:boolean, [error_message:string]

   Checks if event EVENT is a valid user event.  Returns true if successful,
   otherwise returns false plus error message.  */

static int
ev_check_user_event (lua_State *L)
{
  const char *cls;

  luaL_checktype (L, 1, LUA_TTABLE);

  if (!ev_getstringfield (L, 1, EV_FIELD_CLASS, &cls))
    return ev_error_bad (L, EV_FIELD_CLASS);

  if (!streq (cls, EV_USER_CLASS))
    return ev_error_invalid (L, EV_FIELD_CLASS, cls);

  lua_pushboolean (L, 1);
  return 1;
}

/* ev_get_user_filter (class:string)
   -> filter:table or status:boolean, [error_message:string]

   Creates a filter table for user events with the given parameters.
   Returns the resulting filter table if successful,
   otherwise returns false plus error message.  */

static int
ev_get_user_filter (lua_State *L)
{
  const char *cls;

  cls = luaL_checkstring (L, 1);
  if (!streq (cls, EV_USER_CLASS))
    return ev_error_invalid (L, EV_USER_CLASS, cls);

  /* Create filter table.  */

  lua_createtable (L, 0, 1);
  lua_pushstring (L, EV_USER_CLASS);
  lua_setfield (L, -2, EV_FIELD_CLASS);

  return 1;
}


/* Generic event handling.  */

typedef struct
{
  const char *cls;               /* event class */
  lua_CFunction check_func;      /* pointer to check function */
  lua_CFunction get_filter_func; /* pointer to the get_filter function */
  lua_CFunction receive_func;    /* pointer to receive function */
} ev_class_map_t;

static const ev_class_map_t ev_class_map[] =
{
  /* KEEP THIS SORTED ALPHABETICALLY */
  { EV_KEY_CLASS,  ev_check_key_event,  ev_get_key_filter,  NULL },
  { EV_NCL_CLASS,  ev_check_ncl_event,  ev_get_ncl_filter,  ev_receive_ncl_event },
  { EV_TCP_CLASS,  ev_check_tcp_event,  ev_get_tcp_filter,  ev_receive_tcp_event },
  { EV_USER_CLASS, ev_check_user_event, ev_get_user_filter, NULL },
};

static int
ev_class_map_compare (const void *p1, const void *p2)
{
  return strcmp (((ev_class_map_t *) p1)->cls,
                 ((ev_class_map_t *) p2)->cls);
}

static const
ev_class_map_t *ev_class_map_get (const char *cls)
{
  ev_class_map_t key = {cls, NULL, NULL, NULL};
  return (const ev_class_map_t *)
    bsearch (&key, ev_class_map, nelementsof (ev_class_map),
             sizeof (ev_class_map_t), ev_class_map_compare);
}

static int
ev_call_handler (lua_State *L, ptrdiff_t offset)
{
  const ev_class_map_t *entry;
  lua_CFunction handler;
  const char *cls;

  luaL_checktype (L, 1, LUA_TTABLE);

  lua_getfield (L, -1, EV_FIELD_CLASS);
  if (!lua_isstring (L, -1))
    goto done;                  /* unknown class */

  cls = lua_tostring (L, -1);
  lua_pop (L, 1);

  entry = ev_class_map_get (cls);
  if (entry == NULL)
    goto done;                  /* unknown class */

  handler = *(lua_CFunction *)((char *) entry + offset);
  if (handler == NULL)
    goto done;                  /* empty handler */

  return handler (L);

 done:
  lua_pushboolean (L, 1);
  return 1;
}

/* Generic handlers.  */

static int
ev_check_event (lua_State *L)
{
  return ev_call_handler (L, offsetof (ev_class_map_t, check_func));
}

static int
ev_get_filter (lua_State *L)
{
  const ev_class_map_t *entry;
  const char *cls;

  cls = luaL_checkstring (L, 1);
  entry = ev_class_map_get (cls);
  if (entry == NULL)
    return ev_error_invalid (L, EV_FIELD_CLASS, cls);

  return entry->get_filter_func (L);
}

static int
ev_receive_event (lua_State *L)
{
  return ev_call_handler (L, offsetof (ev_class_map_t, receive_func));
}


/* NCLua Event wrappers.  */

static int ev_post_wrapper (lua_State *L);
static int ev_register_wrapper (lua_State *L);

static const ev_map_t ev_wrapper_map[] =
{
  { "post",     pointerof ((ptrdiff_t) ev_post_wrapper)     },
  { "register", pointerof ((ptrdiff_t) ev_register_wrapper) },
};

static const ev_map_t *
ev_wrapper_map_get (const char *key)
{
  return ev_map_get (ev_wrapper_map, nelementsof (ev_wrapper_map), key);
}

/* Replaces the functions listed in ev_wrapper_map by the corresponding
   wrapper functions.  */

static void
ev_install_wrappers (lua_State *L)
{
  size_t i;

  lua_getglobal (L, NCLUA_EVENT_LIBNAME);

  for (i = 0; i < nelementsof (ev_wrapper_map); i++)
    {
      const char *name = ev_wrapper_map[i].key;
      lua_CFunction wrapper = (lua_CFunction)
        integralof (ev_wrapper_map[i].value);

      /* Save the original function into Lua registry.  */

      lua_getfield (L, -1, name);
      assert (lua_isfunction (L, -1));
      lua_pushlightuserdata (L, (void *) &ev_wrapper_map[i]);
      lua_insert (L, -2);
      lua_rawset (L, LUA_REGISTRYINDEX);

      /* Replace original function by wrapper.  */

      lua_pushcfunction (L, wrapper);
      lua_setfield (L, -2, name);
    }

  lua_pop (L, 1);
}

/* Replaces the wrappers listed in ev_wrapper_map by the corresponding
   original functions.  */

static void
ev_uninstall_wrappers (lua_State *L)
{
  size_t i;

  lua_getglobal (L, NCLUA_EVENT_LIBNAME);

  for (i = 0; i < nelementsof (ev_wrapper_map); i++)
    {
      const char *name = ev_wrapper_map[i].key;

      /* Replace wrapper by the original function.  */

      lua_pushlightuserdata (L, (void *) &ev_wrapper_map[i]);
      lua_pushvalue (L, -1);
      lua_rawget (L, LUA_REGISTRYINDEX);
      lua_setfield (L, -3, name);

      /* Cleanup registry.  */

      lua_pushnil (L);
      lua_rawset (L, LUA_REGISTRYINDEX);
    }

  lua_pop (L, 1);
}

/* event.post ([destination:string], event:table)
   -> status:boolean, [error_message:string]

   Similar to the original event.post, but checks if event is valid before
   posting it.  */

static int
ev_post_wrapper (lua_State *L)
{
  int event;                 /* event parameter */

  void *key;
  int saved_top;
  int n;

  event = lua_istable (L, 1) ? 1 : 2;
  luaL_checktype (L, event, LUA_TTABLE);

  /* Check if event is valid.  */

  saved_top = lua_gettop (L);
  lua_pushcfunction (L, ev_check_event);
  lua_pushvalue (L, event);
  lua_call (L, 1, LUA_MULTRET);

  n = lua_gettop (L) - saved_top;
  assert (n == 1 || n == 2);

  if (n == 2)
    {
      assert (lua_isstring (L, -1));
      assert (!lua_toboolean (L, -2));
      return 2;
    }

  if (!lua_toboolean (L, -1))
    return 1;

  lua_pop (L, 1);

  /* Call the original event.post.  */

  assert ((key = (void *) ev_wrapper_map_get ("post")) != NULL);
  lua_pushlightuserdata (L, key);
  lua_rawget (L, LUA_REGISTRYINDEX);
  lua_insert (L, 1);
  lua_call (L, lua_gettop (L) - 1, 0);

  lua_pushboolean (L, 1);
  return 1;
}

/* event.register ([position:number], function:function,
                   [class:string], ...)
   -> status:boolean, [error_message:string]

   Similar to the original event.register, but uses the class and extra
   parameters to create the event filter table.  */

static int
ev_register_wrapper (lua_State *L)
{
  int cls;                      /* class parameter */
  void *key;

  cls = lua_isfunction (L, 1) ? 2 : 3;
  if (lua_isstring (L, cls))
    {
      int n;

      /* Creates filter table from the given parameters.  */

      lua_pushcfunction (L, ev_get_filter);
      lua_insert (L, cls);

      n = lua_gettop (L) - cls;
      lua_call (L, n, LUA_MULTRET);

      n = lua_gettop (L) - cls + 1;
      assert (n == 1 || n == 2);

      if (n == 2)
        {
          assert (lua_isstring (L, -1));
          assert (!lua_toboolean (L, -2));
          return 2;
        }

      if (!lua_toboolean (L, -1))
        return 1;

      assert (lua_istable (L, -1));
    }

  /* Call the original event.register.  */

  assert ((key = (void *) ev_wrapper_map_get ("register")) != NULL);
  lua_pushlightuserdata (L, key);
  lua_rawget (L, LUA_REGISTRYINDEX);
  lua_insert (L, 1);
  lua_call (L, lua_gettop (L) - 1, LUA_MULTRET);

  lua_pushboolean (L, 1);
  return 1;
}

#endif /* LUAPLAYER_EVENT_H */
