/* nclua-event.cpp -- The NCLua Event API.
   Copyright (C) 2006-2012 PUC-Rio/Laboratorio TeleMidia

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

#include <limits.h>
#include <pthread.h>
#include "nclua-internal.h"

#include "player/LuaPlayer.h"
using namespace ::br::pucrio::telemidia::ginga::core::player;

/* Key of the Event table in registry.  This table contains the internal
   structures of the NCLua Event module.  Unless said otherwise, we shall
   use the expression "Event table" to refer to this table.  */
#define NCLUAEVENT_REGISTRY_KEY "NCLua.Event"

/* Event table contents.  */
enum
{
  NCLUAEVENT_INDEX_EMPTY_TABLE = INT_MIN, /* empty table */
  NCLUAEVENT_INDEX_LISTENERS_QUEUE,       /* listeners queue */
  NCLUAEVENT_INDEX_TIMERS_TABLE,          /* active timers table */

  /* The following is a special value indicating the
     number of index values defined in this enumeration.  */

  NCLUAEVENT_INDEX_LAST,
};

/* Gets the index of symbol SYM in Event table.  */
#define indexof(sym) NCLUA_CONCAT (NCLUAEVENT_INDEX_, sym)
NCLUA_COMPILE_TIME_ASSERT(indexof (LAST) < 0);

/* Pushes onto stack the object at index INDEX in Event table.  */
#define _pushenv(L, index)                                              \
  NCLUA_STMT_BEGIN                                                      \
  {                                                                     \
    lua_getfield (L, LUA_REGISTRYINDEX, NCLUAEVENT_REGISTRY_KEY);       \
    lua_rawgeti (L, -1, index);                                         \
    lua_replace (L, -2);                                                \
  }                                                                     \
  NCLUA_STMT_END

/* Pushes onto stack the specified object.  */
#define push_empty_table(L)     _pushenv (L, indexof (EMPTY_TABLE))
#define push_listeners_queue(L) _pushenv (L, indexof (LISTENERS_QUEUE))
#define push_timers_table(L)    _pushenv (L, indexof (TIMERS_TABLE))

/* Function prototypes: */
static int l_post (lua_State* L);
static int l_post_edit_event (lua_State *L);
static int l_post_key_event (lua_State *L);
static int l_post_ncl_event (lua_State *L);
static int l_post_pointer_event (lua_State *L);
static int l_post_si_event (lua_State *L);
static int l_post_sms_event (lua_State *L);
static int l_post_tcp_event (lua_State *L);
static int l_post_user_event (lua_State *L);
static int l_register (lua_State *L);
static int l_register_edit_event (lua_State *L);
static int l_register_key_event (lua_State *L);
static int l_register_ncl_event (lua_State *L);
static int l_register_pointer_event (lua_State *L);
static int l_register_si_event (lua_State *L);
static int l_register_sms_event (lua_State *L);
static int l_register_tcp_event (lua_State *L);
static int l_register_user_event (lua_State *L);
static int l_unregister (lua_State *L);
static int l_uptime (lua_State *L);
static int l_timer (lua_State *L);
static int l_cancel (lua_State *L);
static void *timer_thread (void *data);

/* Table mapping action name to its integer code.  */
static const struct _acttab
{
  const char *name;             /* action name */
  int code;                     /* action code */
}
action_table[] =
{
  /* KEEP THIS SORTED ALPHABETICALLY */
  { "abort",  Player::PL_NOTIFY_ABORT  },
  { "pause",  Player::PL_NOTIFY_PAUSE  },
  { "resume", Player::PL_NOTIFY_RESUME },
  { "start",  Player::PL_NOTIFY_START  },
  { "stop",   Player::PL_NOTIFY_STOP   },
};

static int
_compare__acttab_name (const void *p1, const void *p2)
{
  return strcmp (((struct _acttab *) p1)->name,
                 ((struct _acttab *) p2)->name);
}

/* Returns the positive integer code associated with action name ACTION,
   or -1 if there is no such action.  */

static int
action_to_code (const char *action)
{
  struct _acttab key;
  struct _acttab *result;

  key.name = action;
  result = (struct _acttab *)
    bsearch (&key, action_table, nelementsof (action_table),
             sizeof (struct _acttab), _compare__acttab_name);

  return (likely (result != NULL)) ? result->code : -1;
}

/* Table mapping event class to its handler.  */
static const struct _classtab
{
  const char *name;             /* class name */
  lua_CFunction post_fn;        /* post handler */
  lua_CFunction register_fn;    /* register handler */
}
class_table[] =
{
  /* KEEP THIS SORTED ALPHABETICALLY */
  { "edit",    l_post_edit_event,    l_register_edit_event    },
  { "key",     l_post_key_event,     l_register_key_event     },
  { "ncl",     l_post_ncl_event,     l_register_ncl_event     },
  { "pointer", l_post_pointer_event, l_register_pointer_event },
  { "si",      l_post_si_event,      l_register_si_event      },
  { "sms",     l_post_sms_event,     l_register_sms_event     },
  { "tcp",     l_post_tcp_event,     l_register_tcp_event     },
  { "user",    l_post_user_event,    l_register_user_event    },
};

static int _compare__classtab_name (const void *p1, const void *p2)
{
  return strcmp (((struct _classtab *) p1)->name,
                 ((struct _classtab *) p2)->name);
}

typedef enum
{
  POST_HANDLER,
  REGISTER_HANDLER,
} class_handler_type_t;

/* Returns the handler function of type TYPE associated with class NAME,
   or NULL if there is no such class.  */

static lua_CFunction
class_to_handler (const char *name, class_handler_type_t type)
{
  struct _classtab key;
  struct _classtab *result;

  key.name = name;
  result = (struct _classtab *)
    bsearch (&key, class_table, nelementsof (class_table),
             sizeof (struct _classtab), _compare__classtab_name);

  if (unlikely (result == NULL))
    return NULL;

  switch (type)
    {
    case POST_HANDLER:     return result->post_fn;
    case REGISTER_HANDLER: return result->register_fn;
    default:               assert (!"shouldn't get here");
    }
}


/**************************************************************************
 *                                                                        *
 *                          The NCLua Event API                           *
 *                        Part I -- From Lua to C                         *
 *                                                                        *
 **************************************************************************/


/* event.post ([dst:string], evt:event) -> sent:boolean, errmsg:string
 *
 * This function posts event EVT to the NCL player (DST == 'out'), or to the
 * NCLua player itself (DST == 'in').  Returns true if successful.
 * Otherwise, returns false plus error message.
 */

static int l_post (lua_State* L)
{
  const char *dst;              /* destination */
  const char *cls;              /* event class */
  lua_CFunction handler;        /* class handler */

  if (lua_type (L, 1) == LUA_TSTRING)
    {
      dst = lua_tostring (L, 1);
    }
  else
    {
      dst = "out";
      lua_pushstring (L, dst);
      if (lua_type (L, 1) == LUA_TNIL)
        lua_remove(L, 1);
      lua_insert (L, 1);
    }

  luaL_checktype (L, 2, LUA_TTABLE);

  /* This must come after checktype(); otherwise, we might return false
     in cases where trowing an exception would be more appropriate.
     For example, if dst != "out" or "in" and #2 is not a table.  */

  if (unlikely (!streq (dst, "out") && !streq (dst, "in")))
    goto error_bad_argument;    /* invalid destination */

  if (unlikely (lua_gettop (L) > 2))
    {
      nclua_lwarn_extra_args (L);
      lua_settop (L, 2);
    }

  /* Post event to itself.  */
  if (streq (dst, "in"))
    {
      /* TODO: Move this to the LuaPlayer.  */
      LuaPlayer *player = GETPLAYER (L);
      GingaScreenID id = player->getScreenId ();
      ILocalScreenManager *m = player->getScreenManager ();
      IInputEvent* evt;
      int ref;

      ref = luaL_ref (L, LUA_REGISTRYINDEX);
      evt = m->createApplicationEvent (id, ref, (void *) L);
      player->im->postInputEvent (evt);

      lua_pushboolean (L, TRUE);
      return 1;
    }

  /* Post event to NCL.  */
  if (unlikely (!luax_getstringfield (L, -1, "class", &cls)))
    goto error_bad_argument;    /* invalid class */

  handler = class_to_handler (cls, POST_HANDLER);
  if (unlikely (handler == NULL))
    goto error_bad_argument;    /* unknown class */

  return handler (L);

 error_bad_argument:
  lua_pushboolean (L, FALSE);
  nclua_pushstatus (L, NCLUA_STATUS_BAD_ARGUMENT);
  return 2;
}

#ifndef NWARNINGS
static nclua_bool_t
_warn_extra_fields_in_event (lua_State *L, int index, ...)
{
  nclua_bool_t status = false;
  int saved_top = lua_gettop (L);
  int t = luax_absindex (L, index);

  va_list ap;
  const char *k;

  va_start (ap, index);
  while ((k = va_arg (ap, char *)) != NULL)
    luax_unsetfield (L, t, k);
  va_end (ap);

  lua_pushnil (L);
  if (unlikely (lua_next (L, t)) != 0)
    status = true;

  lua_settop (L, saved_top);
  return status;
}

/* Prints warning message if event table at INDEX is not empty.
   Before performing the check, removes from table all fields indexed by
   strings in the NULL-terminated varargs (...) argument.  */
# define warn_extra_fields_in_event(L, index, ...)                      \
  NCLUA_STMT_BEGIN                                                      \
  {                                                                     \
    if (_warn_extra_fields_in_event (L, index, ## __VA_ARGS__, NULL))   \
      nclua_lwarn (L, 1, "ignoring extra fields in event");             \
  }                                                                     \
  NCLUA_STMT_END
#else
# define warn_extra_fields_in_event(L, index, ...)
#endif /* !NWARNINGS */

/* Posts NCL edit event at index 2.  Returns true if successful.
   Otherwise, returns false plus error message.  */

static int
l_post_edit_event (lua_State *L)
{
  lua_pushboolean (L, FALSE);
  nclua_pushstatus (L, NCLUA_STATUS_NOT_IMPLEMENTED);
  return 2;
}

/* Posts key event.  */

static int
l_post_key_event (lua_State * L)
{
  lua_pushboolean (L, FALSE);
  nclua_pushstatus (L, NCLUA_STATUS_NOT_IMPLEMENTED);
  return 2;
}

/* Posts NCL (presentation, attribution, or selection) event.  */

static int
l_post_ncl_event (lua_State *L)
{
  LuaPlayer *player;

  const char *type;
  const char *action;
  int action_code;

  if (unlikely (!luax_getstringfield (L, 2, "type", &type)))
    goto error_bad_argument;    /* invalid type */

  if (unlikely (!luax_getstringfield (L, 2, "action", &action)))
    goto error_bad_argument;    /* invalid action */

  action_code = action_to_code (action);
  if (unlikely (action_code < 0))
    goto error_bad_argument;    /* unknown action */

  if (streq (type, "presentation"))
    {
      const char *label;

      lua_getfield (L, 2, "label");
      label = (!lua_isnil (L, -1)) ? lua_tostring (L, -1) : "";
      if (unlikely (label == NULL))
        goto error_bad_argument; /* invalid label */
      lua_pop (L, 1);

      warn_extra_fields_in_event (L, 2, "class", "type", "action", "label");

      /* Execute presentation event.  */

      /* TODO: Move this to the LuaPlayer.  */
      player = GETPLAYER (L);
      if (action_code == Player::PL_NOTIFY_STOP && *label == '\0')
        {
          player->im->removeApplicationInputEventListener (player);
        }
      player->notifyPlayerListeners (action_code, label);
    }
  else if (streq (type, "attribution"))
    {
      const char *name;
      const char *value;

      if (unlikely (!luax_getstringfield (L, 2, "name", &name)))
        goto error_bad_argument; /* invalid name */

      if (unlikely (!luax_getstringfield (L, 2, "value", &value)))
        goto error_bad_argument; /* invalid value */

      warn_extra_fields_in_event (L, 2, "class", "type", "action",
                                  "name", "value");

      /* Execute attribution event.  */

      /* TODO: Move this to the LuaPlayer.  */
      player = GETPLAYER (L);
      player->unprotectedSetPropertyValue (name, value);
      player->notifyPlayerListeners (action_code, name,
                                     Player::TYPE_ATTRIBUTION, value);
    }
  else if (streq (type, "selection"))
    {
      goto error_not_implemented;
    }
  else
    {
      goto error_bad_argument;  /* unknown type */
    }

  /* Success.  */
  lua_pushboolean (L, TRUE);
  return 1;

  /* Failure. */
 error:
  lua_pushboolean (L, FALSE);
  lua_insert (L, -2);
  return 2;

 error_bad_argument:
  nclua_pushstatus (L, NCLUA_STATUS_BAD_ARGUMENT);
  goto error;

 error_not_implemented:
  nclua_pushstatus (L, NCLUA_STATUS_NOT_IMPLEMENTED);
  goto error;
}

/* Posts pointer (cursor) event.  */

static int
l_post_pointer_event (lua_State * L)
{
  lua_pushboolean (L, FALSE);
  nclua_pushstatus (L, NCLUA_STATUS_NOT_IMPLEMENTED);
  return 2;
}

/* Posts SI event.  */

static int
l_post_si_event (lua_State * L)
{
  lua_pushboolean (L, FALSE);
  nclua_pushstatus (L, NCLUA_STATUS_NOT_IMPLEMENTED);
  return 2;
}

/* Posts SMS event.  */

static int
l_post_sms_event (lua_State * L)
{
  lua_pushboolean (L, FALSE);
  nclua_pushstatus (L, NCLUA_STATUS_NOT_IMPLEMENTED);
  return 2;
}

/* Posts TCP event. */

static int
l_post_tcp_event (lua_State * L)
{
  lua_pushboolean (L, FALSE);
  nclua_pushstatus (L, NCLUA_STATUS_NOT_IMPLEMENTED);
  return 2;
}

/* Posts user event.  */

static int
l_post_user_event (lua_State * L)
{
  lua_pushboolean (L, FALSE);
  nclua_pushstatus (L, NCLUA_STATUS_NOT_IMPLEMENTED);
  return 2;
}

/* event.uptime () -> ms:number
 *
 * Returns the number of milliseconds elapsed since last start.
 */

static int
l_uptime (lua_State *L)
{
#ifndef NWARNINGS
  if (unlikely (lua_gettop (L) > 0))
    nclua_lwarn_extra_args (L);
#endif

  lua_pushnumber (L, GETPLAYER (L)->getMediaTime() * 1000);
  return 1;
}

/* event.get_listeners_queue () -> queue:table
 *
 * Returns the listeners queue.
 * This function is an extension to the ABNT standard.
 */

static int
l_get_listeners_queue (lua_State *L)
{
#ifndef NWARNINGS
  if (unlikely (lua_gettop (L) > 0))
    nclua_lwarn_extra_args (L);
#endif

  push_listeners_queue (L);
  return 1;
}

/* event.unregister (f:function) -> n:number; or
 * event.unregister (f1:function, f2:function, ...) -> n:number; or
 * event.unregister () -> n:number
 *
 * Remove all entries indexed by function F from listeners queue.
 *
 * In the second form, remove all given functions from listeners queue.
 * In the third form, remove all functions from listeners queue.
 * The second and third forms are extensions to the ABNT standard.
 *
 * Returns the number of entries removed from queue.
 * This return value is an extension to the ABNT standard.
 */

static int
l_unregister (lua_State *L)
{
  int nremoved;                 /* number of entries removed */
  int top;
  int n;
  int i;

  /* 3rd form: Remove all entries from queue.  */
  if (lua_isnoneornil (L, 1) && lua_gettop (L) <= 1)
    {
      push_listeners_queue (L);
      n = lua_objlen (L, -1);

      for (i = 1; i <= n; i++)
        {
          lua_pushnil (L);
          lua_rawseti (L, -2, i);
        }

      lua_pushinteger (L, n);
      return 1;
    }

  /* 1st and 2nd forms: Process each argument.  */
  nremoved = 0;
  while ((top = lua_gettop (L)) > 0)
    {
      int saved_n;
      int q;

      luaL_checktype (L, 1, LUA_TFUNCTION);

      push_listeners_queue (L);
      q = luax_absindex (L, -1);
      saved_n = n = lua_objlen (L, q);

      i = 1;
      while (i <= n)
        {
          lua_rawgeti (L, q, i);
          lua_rawgeti (L, -1, 1);

          if (lua_equal (L, -1, 1))
            {
              /* Remove function from listeners queue
                 shifting down other elements to close space.  */

              luax_tableremove (L, q, i);
              n--;
              continue;
            }

          i++;
        }

      nremoved += saved_n - n;
      lua_settop (L, top);
      lua_remove (L, 1);
    }

  lua_pushinteger (L, nremoved);
  return 1;
}

/* event.register ([pos:number], f:function, [class:string], ...)
 *     -> status:boolean, [errmsg:string]; or
 *
 * event.register ([pos:number], f:function, [filter:table])
 *     -> status:boolean, [errmsg:string]
 *
 * Appends the function F to the listeners queue.
 * If POS is given, then F is registered in position POS of listeners queue.
 * If CLASS is given, then F called only for events of the given class.
 *
 * In the first form, any additional parameters are treated as class
 * dependent filters.  In the second form, the table FILTER is used instead.
 * The second form is an extension to the ABNT standard.
 *
 * Returns true if successful, otherwise return false plus error message.
 * This return value is an extension to the ABNT standard.
 */

static int
l_register (lua_State *L)
{
  int pos;
  int n;

  push_listeners_queue (L);
  n = lua_objlen (L, -1);
  lua_pop (L, 1);

  if (lua_type (L, 1) == LUA_TNUMBER)
    {
      pos = (int) lua_tointeger (L, 1);
      pos = min (max (pos, 1), n + 1);
    }
  else
    {
      pos = n + 1;
      lua_pushinteger (L, pos);
      if (lua_type (L, 1) == LUA_TNIL)
        lua_remove (L, 1);
      lua_insert (L, 1);
    }

  luaL_checktype (L, 2, LUA_TFUNCTION);

  switch (lua_type (L, 3))
    {
    case LUA_TNIL:
    case LUA_TNONE:
      push_empty_table (L);
      lua_insert (L, 3);

      /* fall-through */

    case LUA_TTABLE:
      if (unlikely (lua_gettop (L) > 3))
        {
          nclua_lwarn_extra_args (L);
          lua_settop (L, 3);
        }
      break;

    default:
      {
        /* Collects extra arguments and pushes the resulting filter table
           onto stack (or false plus error message, in case of error).  */
        const char *cls;
        lua_CFunction handler;

        cls = luaL_checkstring (L, 3);
        handler = class_to_handler (cls, REGISTER_HANDLER);
        if (unlikely (handler == NULL))
          goto error_bad_argument;

        assert (handler (L) > 0);
        if (unlikely (lua_type (L, -1) != LUA_TTABLE))
          return 2;

        /* Replace the 3rd argument by the
           resulting table and update stack pointer.  */
        lua_insert (L, 3);
        lua_settop (L, 3);
      }
    }

  push_listeners_queue (L);

  /* Creates {f, {filter}}.  */
  lua_createtable (L, 2, 0);
  lua_pushvalue (L, 2);
  lua_rawseti (L, -2, 1);
  lua_pushvalue (L, 3);
  lua_rawseti (L, -2, 2);

  /* Insert {f, {filter}} into listeners queue,
     shifting up any other elements, if necessary.  */

  luax_tableinsert (L, -2, pos);

  /* Success.  */
  lua_pushboolean (L, TRUE);
  return 1;

  /* Failure.  */
 error:
  lua_pushboolean (L, FALSE);
  lua_insert (L, -2);
  return 2;

 error_bad_argument:
  nclua_pushstatus (L, NCLUA_STATUS_BAD_ARGUMENT);
  goto error;
}

/* The following l_register_*_event() functions are used by l_register() to
   create a filter table from the given varargs parameters.  There is a
   function for each event class.  If the call is successful, the function
   pushes the resulting filter table onto stack.  Otherwise, it pushes false
   plus error message.  */

/* event.register (pos:number, f:function, 'edit', ...)
       -> filter:table or false, [errmsg:string]

   Creates a filter for NCL edit events with the given parameters.  */

static int
l_register_edit_event (lua_State *L)
{
  lua_pushboolean (L, FALSE);
  nclua_pushstatus (L, NCLUA_STATUS_NOT_IMPLEMENTED);
  return 2;
}

/* event.register (pos:number, f:function, 'key',
                   [type:string], [key:string])
       -> filter:table or false, [errmsg:string]

   Creates a filter for key events with the given parameters.

   The TYPE parameter is a string denoting the awaited status of the key:
   'press' if key was pressed, or 'release' if key was released.  If TYPE is
   nil, matches both.

   The KEY parameter is a string denoting the pressed (or released) key, or
   nil (matches any key).  The event.keys table stores the available
   keys.  */

static int
l_register_key_event (lua_State *L)
{
  const char *type = NULL;
  const char *key = NULL;

  if (unlikely (lua_gettop (L) > 5))
    {
      nclua_lwarn_extra_args (L);
      lua_settop (L, 5);
    }

  /* Get type.  */
  if (!lua_isnoneornil (L, 4))
    {
      type = lua_tostring (L, 4);
      if (unlikely (type == NULL))
        goto error_bad_argument; /* invalid type */

      if (unlikely (!streq (type, "press") && !streq (type, "release")))
        goto error_bad_argument; /* unknown type */
    }

  /* Get key.  */
  if (!lua_isnoneornil (L, 5))
    {
      key = lua_tostring (L, 5);
      if (unlikely (key == NULL))
        goto error_bad_argument; /* invalid key */

      /* TODO: Check if key is in event.keys.  */
    }

  /* Create the resulting table.  */
  lua_createtable (L, 0, 3);
  lua_pushvalue (L, 3);
  lua_setfield (L, -2, "class");

  if (type != NULL)
    {
      lua_pushstring (L, type);
      lua_setfield (L, -2, "type");
    }

  if (key != NULL)
    {
      lua_pushstring (L, key);
      lua_setfield (L, -2, "key");
    }

  /* Success.  */
  return 1;

  /* Failure.  */
 error_bad_argument:
  lua_pushboolean (L, FALSE);
  nclua_pushstatus (L, NCLUA_STATUS_BAD_ARGUMENT);
  return 2;
}

/* event.register (pos:number, f:function, 'ncl', [type:string],
                   [label or name:string], [action:string])
       -> filter:table or false, [errmsg:string]

   Creates a filter for NCL events with the given parameters.

   The TYPE parameter is a string denoting the type of the awaited NCL
   event: 'presentation', 'selection', or 'attribution'.  If TYPE is nil,
   matches any of the previous.

   If TYPE == 'presentation' or TYPE == 'selection', then the 4th parameter
   is treated as the label of a child anchor of the NCLua node.

   If TYPE == 'attribution', then the 4th parameter is treated as the name
   of child property of the NCLua node.

   The ACTION para meter is a string denoting the awaited action: 'abort',
   'pause', 'resume', 'start', or 'stop'.  If ACTION is not given, matches
   any of the previous.  */

static int
l_register_ncl_event (lua_State *L)
{
  const char *type = NULL;
  const char *specname = NULL;  /* "label" or "name" */
  const char *specvalue = NULL; /* label (or name) */
  const char *action = NULL;

  if (unlikely (lua_gettop (L) > 6))
    {
      nclua_lwarn_extra_args (L);
      lua_settop (L, 6);
    }

  /* Get type.  */
  if (!lua_isnoneornil (L, 4))
    {
      type = lua_tostring (L, 4);
      if (unlikely (type == NULL))
        goto error_bad_argument; /* invalid type */

      if (streq (type, "presentation") || streq (type, "selection"))
        {
          if (!lua_isnoneornil (L, 5))
            {
              specname = "label";
              specvalue = lua_tostring (L, 5);
              if (unlikely (specvalue == NULL))
                goto error_bad_argument; /* invalid label */
            }
        }
      else if (streq (type, "attribution"))
        {
          /* The ABNT standard wrongly assumes that the 5th parameter
             is always a label.  If type == 'attribution', however,
             we must treat the 5th parameter as the name of a property.  */

          specname = "name";
          specvalue = lua_tostring (L, 5);
          if (unlikely (specvalue == NULL))
            goto error_bad_argument; /* invalid name */
        }
      else
        {
          goto error_bad_argument; /* unknown type */
        }
    }

  if (unlikely (!lua_isnoneornil (L, 5) && type == NULL))
    goto error_bad_argument;    /* label requires type */

  /* Get action.  */
  if (!lua_isnoneornil (L, 6))
    {
      action = lua_tostring (L, 6);
      if (unlikely (action == NULL))
        goto error_bad_argument; /* invalid action */

      if (unlikely (action_to_code (action) < 0))
        goto error_bad_argument; /* unknown action */
    }

  /* Create the resulting table.  */
  lua_createtable (L, 0, 4);
  lua_pushvalue (L, 3);
  lua_setfield (L, -2, "class");

  if (type != NULL)
    {
      lua_pushstring (L, type);
      lua_setfield (L, -2, "type");
    }

  if (specvalue != NULL)
    {
      lua_pushstring (L, specvalue);
      assert (streq (specname, "label") || streq (specname, "name"));
      lua_setfield (L, -2, specname);
    }

  if (action != NULL)
    {
      lua_pushstring (L, action);
      lua_setfield (L, -2, "action");
    }

  /* Success.  */
  return 1;

  /* Failure.  */
 error_bad_argument:
  lua_pushboolean (L, FALSE);
  nclua_pushstatus (L, NCLUA_STATUS_BAD_ARGUMENT);
  return 2;
}

/* event.register (pos:number, f:function, 'pointer', [type:string])
       -> filter:table or false, [errmsg:string]

   Creates a filter for pointer events with the given parameters.

   The TYPE parameter is a string denoting the type of the awaited pointer
   event: 'press', if pointer was pressed, 'release' if pointer was
   released, or 'move if pointer moved.  If TYPE is nil, matches any of the
   previous.  */

static int
l_register_pointer_event (lua_State *L)
{
  lua_pushboolean (L, FALSE);
  nclua_pushstatus (L, NCLUA_STATUS_NOT_IMPLEMENTED);
  return 2;
}

/* event.register (pos:number, f:function, 'si', ...)
       -> filter:table or false, [errmsg:string]

   Creates a filter for SI events with the given parameters.  */

static int
l_register_si_event (lua_State *L)
{
  lua_pushboolean (L, FALSE);
  nclua_pushstatus (L, NCLUA_STATUS_NOT_IMPLEMENTED);
  return 2;
}

/* event.register (pos:number, f:function, 'sms', ...)
       -> filter:table or false, [errmsg:string]

   Creates a filter for SMS events with the given parameters.  */

static int
l_register_sms_event (lua_State *L)
{
  lua_pushboolean (L, FALSE);
  nclua_pushstatus (L, NCLUA_STATUS_NOT_IMPLEMENTED);
  return 2;
}

/* event.register (pos:number, f:function, 'tcp', [connection:any])
       -> filter:table or false, [errmsg:string]

   Creates a filter for TCP events with the given parameters.

   The CONNECTION parameter is the connection identifier, returned by the
   'connect' event.  If CONNECTION is nil, matches any connection.  */

static int
l_register_tcp_event (lua_State *L)
{
  lua_pushboolean (L, FALSE);
  nclua_pushstatus (L, NCLUA_STATUS_NOT_IMPLEMENTED);
  return 2;
}

/* event.register (pos:number, f:function, 'user')
       -> filter:table or false:boolean, [errmsg:string]

   Creates a filter for user events with the given parameters.  */

static int
l_register_user_event (lua_State *L)
{

  if (unlikely (lua_gettop (L) > 3))
    {
      nclua_lwarn_extra_args (L);
      lua_settop (L, 3);
    }

  lua_createtable (L, 0, 1);
  lua_pushvalue (L, 3);
  lua_setfield (L, -2, "class");

  return 1;
}

/* event.timer (ms:number, f:function) -> cancel:function
 *
 * Creates a timer that expires after a timeout of MS milliseconds and then
 * call the callback function F.
 *
 * Returns a 'cancel' function that can be used to cancel the timer.
 */

typedef struct _nclua_event_timer_s
{
  lua_State *L;
  pthread_t tid;
  int ms;
} nclua_event_timer_t;

static int
l_timer (lua_State *L)
{
  nclua_event_timer_t *timer;

  timer = (nclua_event_timer_t *) malloc (sizeof (*timer));
  assert (timer != NULL);

  timer->L = L;
  timer->ms = luaL_checkinteger (L, 1);

  luaL_checktype (L, 2, LUA_TFUNCTION);

  if (unlikely (lua_gettop (L) > 2))
    {
      nclua_lwarn_extra_args (L);
      lua_settop (L, 2);
    }

  /* Store callback in timers table, indexed by the timer structure.  */
  push_timers_table (L);
  lua_pushlightuserdata (L, (void *) timer);
  lua_pushvalue (L, 2);
  lua_rawset (L, -3);

  /* Start timer thread.  */
  pthread_create (&(timer->tid), 0, timer_thread, (void *) timer);
  pthread_detach (timer->tid);

  /* Create and return the cancel closure,
     with the timer structure as up-value.  */
  lua_pushlightuserdata (L, (void *) timer);
  lua_pushcclosure (L, l_cancel, 1);
  return 1;
}

/* Cancel the thread of the (up-value) timer.  */

static int
l_cancel (lua_State *L)
{
  nclua_event_timer_t *timer;

  if (unlikely (lua_gettop (L) > 0))
    {
      nclua_lwarn_extra_args (L);
      lua_settop (L, 0);
    }

  lua_pushvalue (L, lua_upvalueindex (1));
  timer = (nclua_event_timer_t *) lua_touserdata (L, -1);
  if (unlikely (timer == NULL))
    return 0;

  /* FIXME: If l_cancel() is called after cleanup_timer_thread(), the timer
     structure is no longer valid.  The test below is not enough to prevent
     memory errors, since malloc() might return some old, already freed,
     address to a new timer.  */

  push_timers_table (L);
  lua_pushlightuserdata (L, (void *) timer);
  lua_rawget (L, -2);
  if (!lua_isnil (L, -1))
    pthread_cancel (timer->tid);
  return 0;
}

/* Cleanup timer thread data.  */

static void
cleanup_timer_thread (void *data)
{
  nclua_event_timer_t *timer = (nclua_event_timer_t *) data;
  lua_State *L = timer->L;

  push_timers_table (L);
  lua_pushlightuserdata (L, (void *) timer);
  lua_pushnil (L);
  lua_rawset (L, -3);
  free (timer);
}

/* Timer thread.  */

static void *
timer_thread (void *data)
{
  nclua_event_timer_t *timer = (nclua_event_timer_t *) data;
  lua_State *L = timer->L;
  LuaPlayer *player = GETPLAYER (L);

  pthread_cleanup_push (cleanup_timer_thread, data);
  SystemCompat::uSleep (timer->ms * 1000);

  /* Define a cancellation point.  */
  pthread_testcancel ();

  if (unlikely (!player->isRunning ()))
    goto tail;

  /* FIXME: This is **DANGEROUS**! We're calling
     the Lua callback function in a different thread.  */

  push_timers_table (L);
  lua_pushlightuserdata (L, (void *) timer);
  lua_rawget (L, -2);

  player->lock ();
  lua_call (L, 0, 0);
  player->unlock ();

 tail:
  pthread_cleanup_pop (1);
  return NULL;
}


/**************************************************************************
 *                                                                        *
 *                          The NCLua Event API                           *
 *                        Part II -- From C to Lua                        *
 *                                                                        *
 **************************************************************************/


/* Returns true if event at index EVENT matches filter at index FILTER.
   Otherwise, returns false.  */

static nclua_bool_t
match (lua_State *L, int event, int filter)
{
  nclua_bool_t status = TRUE;
  int top = lua_gettop (L);

  event = luax_absindex (L, event);
  filter = luax_absindex (L, filter);

  lua_pushnil (L);
  while (lua_next (L, filter) != 0)
    {
      lua_pushvalue (L, -2);
      lua_gettable (L, event);

      if (lua_isnil (L, -1) || !lua_equal (L, -1, -2))
        {
          status = FALSE;
          break;                /* mismatch */
        }

      lua_pop (L, 2);
    }

  lua_settop (L, top);
  return status;
}

/* For each function in listeners queue, if its filter matches event at
   INDEX, call it passing the event at INDEX as argument.  If the function
   returns true, stop processing (event handled).  Otherwise, continue until
   all functions have been called.

   Returns the number of handlers called.  */

static int
notify (lua_State *L, int index)
{
  int result;                   /* number of handlers called */
  int event;                    /* index of event */
  int queue;                    /* index of queue */
  int n;                        /* queue size */
  int i;

  event = luax_absindex (L, index);

  push_listeners_queue (L);
  queue = luax_absindex (L, -1);
  n = lua_objlen (L, queue);

  result = 0;
  for (i = 1; i <= n; i++)
    {
      lua_rawgeti (L, queue, i);
      lua_rawgeti (L, -1, 2);
      if (match (L, event, -1))
        {
          lua_rawgeti (L, -2, 1);
          lua_pushvalue (L, event);
          lua_call (L, 1, 1);
          result++;
          if (lua_toboolean (L, -1))
            break;
        }
    }

  return result;
}

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */


/* TODO: REMOVE THIS!  */

enum
{
    REFTCPIN=6,
    REFTCPOUT,
};

LUALIB_API int ext_postRef (lua_State* L, int ref)
{
    // [ ... ]
    lua_rawgeti(L, LUA_REGISTRYINDEX, ref);  // [ ... | evt ]
    luaL_unref(L, LUA_REGISTRYINDEX, ref);
    notify(L, -1);                         // [ ... ]
    return 0;
}

LUALIB_API int ext_postHash (lua_State* L, map<string,string>evt)
{
    // [ ... ]
    map<string,string>::iterator it;
	lua_newtable(L);                            // [ ... | evt ]
    for (it=evt.begin(); it!=evt.end(); it++) {
		lua_pushstring(L, it->second.c_str());  // [ ... | evt | value ]
		lua_setfield(L, -2, it->first.c_str()); // [ ... | evt ]
    }
    // [ ... | evt ]
    notify(L, -1);  // [ ...]
    return 0;
}

LUALIB_API int ext_postHashRec (lua_State* L, map<string, struct Field> evt,
		bool dispatch)
{
	string str;
	map<string, struct Field>::iterator it;

	// [ ... ]
	lua_newtable(L);    // [ ... | evt ]

	//clog << "LuaEvent::ext_postHashRec TABLE = {" << endl;

	for (it=evt.begin(); it!=evt.end(); ++it) {
    	//clog << "	LuaEvent::ext_postHashRec: " << it->first.c_str();

    	 if ( it->second.table.empty() == false){ //table inside table
    		//clog << " (table) = " << endl;

    		ext_postHashRec(L, it->second.table, false);// [ ... | evt | evt ]
			lua_setfield(L, -2, it->first.c_str()); // [ ... | evt ]
    	}
    	 else if (it->second.str.empty() == false) { // field is a string
			//clog << " = " << it->second.str.c_str() << endl;

			lua_pushstring(L, it->second.str.c_str());  // [ ... | evt | value ]
    		lua_setfield(L, -2, it->first.c_str()); // [ ... | evt ]
    	}
    }
    //clog << "LuaEvent::ext_postHashRec }" << endl;
    // [ ... | evt ]
    if (dispatch == true){
    	//clog << "LuaEvent::ext_postHashRec going to dispatch!" << endl;
      notify(L, -1);  // [ ... ]
	}
    return 0;
}


/*********************************************************************
 * event.timer
 * Cria uma thread para dormir pelo tempo especificado.
 * A thread recebe uma estrutura com o tempo, Player e estado Lua.
 * Essa estrutura eh associada aa callback a ser executada na expiracao do
 * timer, assim a thread pode ter acesso a ela apos expirar.
 *********************************************************************/
#if 0
struct t_timer {
	LuaPlayer* player;
    lua_State* L;
	int time;
};

static void* sleep_thread (void* data)
{
	struct t_timer* t = (struct t_timer *)data;

	if (!t->player->isRunning()) {
		return NULL;
	}

	SystemCompat::uSleep(t->time*1000);

	if (!t->player->isRunning()) {
		return NULL;
	}
	t->player->lock();
	lua_pushlightuserdata(t->L, t);         // [ ... | t* ]
	lua_gettable(t->L, LUA_REGISTRYINDEX);  // [ ... | func ]
	if (!lua_isnil(t->L, -1)) {
		if (lua_pcall(t->L, 0, 0, 0) != 0) {// [ ... ]
			clog << "LUAEVENT sleep_thread ERROR:: ";
			clog << lua_tostring(t->L, -1) << endl;
		}

	} else {
		lua_pop(t->L, 1);                   // [ ... ]
	}
	t->player->unlock();

	delete t;
	return NULL;
}

static int l_cancel (lua_State* L)
{
	// [ ]
	lua_pushvalue(L, lua_upvalueindex(2));  // [ t* ]
	lua_pushvalue(L, -1);                   // [ t* | t* ]
	lua_gettable(L, LUA_REGISTRYINDEX);     // [ t* | funcR ]
	lua_pushvalue(L, lua_upvalueindex(1));  // [ t* | funcR | funcU ]

	if (lua_equal(L, -1, -2)) {
		lua_pop(L, 2);                      // [ t* ]
		lua_pushnil(L);                     // [ t* | nil ]
		lua_settable(L, LUA_REGISTRYINDEX); // [ ]
	} else
		lua_pop(L, 3);                      // [ ]

	return 0;
}

static int l_timer (lua_State* L)
{
	// [ msec, func ]
	struct t_timer* t = new struct t_timer;
	t->player = GETPLAYER(L);
    t->L      = L;
	t->time   = luaL_checkinteger(L, 1);

	lua_pushlightuserdata(L, t);            // [ msec | func | t* ]
	lua_pushvalue(L, -1);                   // [ msec | func | t* | t* ]
	lua_pushvalue(L, -3);                   // [ msec | func | t* | t* | func ]

	// REGISTRY[t] = func
	//luaL_checktype(L, 2, LUA_TFUNCTION);
	if(!lua_isfunction(L, 2)){

		fprintf(stderr,"Lua conformity error: event module\n");
		fprintf(stderr,"Call to 'timer' does not follow Ginga-NCL standard: bad argument #2 (function expected)\n");
		fprintf(stderr,"Chamada a 'timer' nao segue padrao Ginga-NCL: argumento #2 (function esperado)\n");

		return 0;
	}
	lua_settable(L, LUA_REGISTRYINDEX);     // [ msec | func | t* ]

	// returns `cancel` function
	lua_pushcclosure(L, l_cancel, 2);       // [ msec | fcancel ]

	// sleep msec
	pthread_t id;
	pthread_create(&id, 0, sleep_thread, (void*)t);
	pthread_detach(id);

	return 1;
}
#endif

/*********************************************************************
 * Funcoes exportadas pelo modulo.
 ********************************************************************/

static const struct luaL_Reg funcs[] =
{
  { "post",        l_post },
  { "timer",       l_timer },
  { "uptime",      l_uptime },
  { "register",    l_register },
  { "unregister",  l_unregister },
  { "get_listeners_queue", l_get_listeners_queue },
  { NULL, NULL }
};

/*********************************************************************
 * Funcao que carrega o modulo.
 * - cria as variaveis locais ao modulo
 ********************************************************************/

LUALIB_API int luaclose_event (lua_State* L)
{
  /* TODO: Traverse timers table, canceling timers.  */
	pthread_cancel(GETPLAYER(L)->tcp_thread_id);
	return 1;
}

LUALIB_API int luaopen_event (lua_State* L)
{
  /* Create Event table and store it in registry.  */
  lua_newtable (L);
  lua_pushvalue (L, -1);
  lua_setfield (L, LUA_REGISTRYINDEX, NCLUAEVENT_REGISTRY_KEY);

  /* Replace the current Lua environment by the Event table.  */
  lua_replace (L, LUA_ENVIRONINDEX);

  lua_newtable (L);
  lua_rawseti (L, LUA_ENVIRONINDEX, indexof (EMPTY_TABLE));

  lua_newtable (L);
  lua_rawseti (L, LUA_ENVIRONINDEX, indexof (LISTENERS_QUEUE));

  lua_newtable (L);
  lua_rawseti (L, LUA_ENVIRONINDEX, indexof (TIMERS_TABLE));

  // trigger tcp thread
  lua_getglobal(L, "require");                  // [ require ]
  lua_pushstring(L, "tcp_event");               // [ require | "tcp_event" ]

    if (lua_pcall(L, 1, 1, 0) != 0) {             // [ {f_in,f_out} ]
		clog << "LUAEVENT luaopen_event ERROR:: ";
		clog << lua_tostring(L, -1) << endl;
    }

    lua_rawgeti(L, -1, 1);                        // [ {f_in,f_out} | f_in ]
	lua_rawseti(L, LUA_ENVIRONINDEX, -REFTCPIN);  // [ {f_in,f_out} ]
    lua_rawgeti(L, -1, 2);                        // [ {f_in,f_out} | f_out ]
	lua_rawseti(L, LUA_ENVIRONINDEX, -REFTCPOUT); // [ {f_in,f_out} ]
    lua_pop(L, 1);                                // [ ]

	luaL_register(L, "event", funcs);             // [ event ]

	return 1;
}
