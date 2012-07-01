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

#if HAVE_CONFIG_H
# include "config.h"
#endif

extern "C"{
#include <lua.h>
#include <lauxlib.h>
}
#include <pthread.h>

#include "nclua.h"
#include "nclua-private.h"
#include "nclua-luax-private.h"

/* BEGIN_DEPRECATED */
#include <string.h>
#include "player/LuaPlayer.h"
using namespace ::br::pucrio::telemidia::ginga::core::player;
/* END_DEPRECATED */

/* Timer thread data.  */
typedef struct
{
  lua_State *L;                 /* Lua state */
  pthread_t tid;                /* thread id */
  pthread_mutex_t mutex;        /* associated mutex */
  unsigned int delay;           /* delay in milliseconds */
} nclua_event_timer_t;

/* Mutex.  */
#define mutex_init(m)                                           \
  NCLUA_STMT_BEGIN                                              \
  {                                                             \
    pthread_mutexattr_t attr;                                   \
    pthread_mutexattr_init (&attr);                             \
    pthread_mutexattr_settype (&attr, PTHREAD_MUTEX_RECURSIVE); \
    pthread_mutex_init (m, &attr);                              \
    pthread_mutexattr_destroy (&attr);                          \
  }                                                             \
  NCLUA_STMT_END
#define mutex_fini(m)   pthread_mutex_destroy (m)
#define mutex_lock(m)   assert (pthread_mutex_lock (m) == 0)
#define mutex_unlock(m) while (pthread_mutex_unlock (m) == 0)

/* Common warnings:  */
#define _nclua_warning_extra_arguments(L)                       \
  _nclua_warning (L, 1, "ignoring extra arguments to '%s'",     \
                  ncluax_get_function_name (L, 0))

/* Function prototypes: */
static int l_get_handler_list (lua_State *L);
static int l_post (lua_State* L);
static int l_register (lua_State *L);
static int l_unregister (lua_State *L);
static int l_uptime (lua_State *L);
static int l_timer (lua_State *L);
static int l_cancel (lua_State *L);
static void *timer_thread (void *data);

#define DEFINE_LUA_FUNCTION(name) \
  { NCLUA_STRINGIFY (name), NCLUA_CONCAT (l_, name) }

/* Lua functions registered by the NCLua Event module.  */
static const struct luaL_Reg _nclua_event_functions[] =
{
  DEFINE_LUA_FUNCTION (get_handler_list),
  DEFINE_LUA_FUNCTION (post),
  DEFINE_LUA_FUNCTION (register),
  DEFINE_LUA_FUNCTION (timer),
  DEFINE_LUA_FUNCTION (unregister),
  DEFINE_LUA_FUNCTION (uptime),
  { NULL, NULL }
};


/* Exported private functions.  */

/* Opens the Event module.  Returns NCLUA_STATUS_SUCCESS or
   NCLUA_STATUS_NO_MEMORY if memory could not be allocated.  */

nclua_status_t
_nclua_event_open (lua_State *L)
{
  lua_newtable (L);
  _nclua_set_registry_data (L, _NCLUA_REGISTRY_INPUT_QUEUE);

  lua_newtable (L);
  _nclua_set_registry_data (L, _NCLUA_REGISTRY_OUTPUT_QUEUE);

  lua_newtable (L);
  _nclua_set_registry_data (L, _NCLUA_REGISTRY_HANDLER_LIST);

  lua_newtable (L);
  _nclua_set_registry_data (L, _NCLUA_REGISTRY_TIMER_TABLE);

  luaL_register (L, "event", _nclua_event_functions);
  lua_pop (L, 1);

  return NCLUA_STATUS_SUCCESS;
}

/* Closes the Event module.  */

void
_nclua_event_close (lua_State *L)
{
  /* Traverse timers table canceling active timers.  */
  _nclua_get_registry_data (L, _NCLUA_REGISTRY_TIMER_TABLE);
  if (unlikely (!lua_isnil (L, -1)))
    {
      int t = ncluax_abs (L, -1);
      lua_pushnil (L);
      while (lua_next (L, t) != 0)
        {
          nclua_event_timer_t *timer;
          timer = (nclua_event_timer_t *) lua_touserdata (L, -2);
          pthread_cancel (timer->tid);
          lua_pop (L, 1);
        }
    }
  lua_pop (L, 1);

  /* Cleanup registry data.  */
  _nclua_unset_registry_data (L, _NCLUA_REGISTRY_INPUT_QUEUE);
  _nclua_unset_registry_data (L, _NCLUA_REGISTRY_OUTPUT_QUEUE);
  _nclua_unset_registry_data (L, _NCLUA_REGISTRY_HANDLER_LIST);
  _nclua_unset_registry_data (L, _NCLUA_REGISTRY_TIMER_TABLE);
}

/* Returns true if event at index EVENT matches filter at index FILTER.
   Otherwise, returns false.  */

static inline nclua_bool_t
match (lua_State *L, int event, int filter)
{
  nclua_bool_t status = TRUE;
  int top = lua_gettop (L);

  event = ncluax_abs (L, event);
  filter = ncluax_abs (L, filter);

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

/* For each function in handler list, if its filter matches event at top of
   stack, call it passing the event as argument.  If the function returns
   true, stop processing (event handled).  Otherwise, continue until all
   functions have been called.

   Returns the number of handlers called.  */

int
_nclua_notify (lua_State *L)
{
  int saved_top;
  int result;                   /* number of handlers called */
  int event;                    /* index of event */
  int queue;                    /* index of queue */
  int size;                     /* queue size */
  int i;

  saved_top = lua_gettop (L);
  event = ncluax_abs (L, -1);

  _nclua_get_registry_data (L, _NCLUA_REGISTRY_HANDLER_LIST);
  queue = ncluax_abs (L, -1);
  size = lua_objlen (L, queue);

  result = 0;
  for (i = 1; i <= size; i++)
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

  lua_settop (L, saved_top);
  return result;
}


/* The NCLua Event API (part i -- from Lua to C).  */

/* event.get_handler_list () -> list:table

   Returns the handler list.
   This function is an extension to the ABNT standard. */

static int
l_get_handler_list (lua_State *L)
{
  if (unlikely (lua_gettop (L) > 0))
    _nclua_warning_extra_arguments (L);

  _nclua_get_registry_data (L, _NCLUA_REGISTRY_HANDLER_LIST);
  return 1;
}

/* event.unregister (f:function) -> n:number; or
   event.unregister (f1:function, f2:function, ...) -> n:number; or
   event.unregister () -> n:number

   Remove all entries indexed by function F from handler list.

   In the second form, remove all given functions from handler list.
   In the third form, remove all functions from handler list.
   The second and third forms are extensions to the ABNT standard.

   Returns the number of entries removed from queue.
   This return value is an extension to the ABNT standard.  */

static int
l_unregister (lua_State *L)
{
  int nremoved;                 /* number of entries removed */
  int top;                      /* saved stack top */
  int queue;                    /* queue index */
  int size;                     /* queue size */
  int i;

  /* 3rd form: Remove all entries from queue.  */
  if (lua_isnoneornil (L, 1) && lua_gettop (L) <= 1)
    {
      _nclua_get_registry_data (L, _NCLUA_REGISTRY_HANDLER_LIST);
      size = lua_objlen (L, -1);

      for (i = 1; i <= size; i++)
        {
          lua_pushnil (L);
          lua_rawseti (L, -2, i);
        }

      lua_pushinteger (L, size);
      return 1;
    }

  /* 1st and 2nd forms: Process each argument.  */
  nremoved = 0;
  while ((top = lua_gettop (L)) > 0)
    {
      int saved_size;

      luaL_checktype (L, 1, LUA_TFUNCTION);

      _nclua_get_registry_data (L, _NCLUA_REGISTRY_HANDLER_LIST);
      queue = ncluax_abs (L, -1);
      saved_size = size = lua_objlen (L, queue);

      i = 1;
      while (i <= size)
        {
          lua_rawgeti (L, queue, i);
          lua_rawgeti (L, -1, 1);

          if (lua_equal (L, -1, 1))
            {
              /* Remove function from handler list
                 shifting down other elements to close space.  */

              ncluax_tableremove (L, queue, i);
              size--;
              continue;
            }

          i++;
        }

      nremoved += saved_size - size;
      lua_settop (L, top);
      lua_remove (L, 1);
    }

  lua_pushinteger (L, nremoved);
  return 1;
}

/* event.timer (ms:number, f:function) -> cancel:function

   Creates a timer that expires after a timeout of MS milliseconds and then
   call the callback function F.

   Returns a 'cancel' function that can be used to cancel the timer.  */

static int
l_timer (lua_State *L)
{
  nclua_event_timer_t *timer;

  if (unlikely (lua_gettop (L) > 2))
    {
      _nclua_warning_extra_arguments (L);
      lua_settop (L, 2);
    }

  timer = (nclua_event_timer_t *) malloc (sizeof (*timer));
  if (unlikely (timer == NULL))
    {
      /* FIXME: Find a way to throw LUA_ERRMEM.  */
      luaL_error (L, "out of memory");
    }

  if (unlikely (!lua_isnumber (L, 1)))
    {
      free (timer);
      luaL_typerror (L, 1, lua_typename (L, LUA_TNUMBER));
    }

  if (unlikely (!lua_isfunction (L, 2)))
    {
      free (timer);
      luaL_typerror (L, 2, lua_typename (L, LUA_TFUNCTION));
    }

  timer->L = L;
  timer->delay = lua_tointeger (L, 1);
  mutex_init (&timer->mutex);

  /* Store callback in timers table, indexed by the timer structure.  */
  _nclua_get_registry_data (L, _NCLUA_REGISTRY_TIMER_TABLE);
  lua_pushlightuserdata (L, (void *) timer);
  lua_pushvalue (L, 2);
  lua_rawset (L, -3);

  /* Create and return the cancel closure,
     with the timer structure as up-value.  */
  lua_pushlightuserdata (L, (void *) timer);
  lua_pushcclosure (L, l_cancel, 1);

  /* Start timer thread.  */
  pthread_create (&timer->tid, 0, timer_thread, (void *) timer);
  assert (pthread_detach (timer->tid) == 0);

  return 1;
}

/* Cancel the thread of the (up-value) timer.  */

static int
l_cancel (lua_State *L)
{
  nclua_event_timer_t *timer;

  if (unlikely (lua_gettop (L) > 0))
    {
      _nclua_warning_extra_arguments (L);
      lua_settop (L, 0);
    }

  lua_pushvalue (L, lua_upvalueindex (1));
  timer = (nclua_event_timer_t *) lua_touserdata (L, -1);
  if (unlikely (timer == NULL))
    return 0;

  mutex_lock (&timer->mutex);

  /* FIXME: If l_cancel() is called after cleanup_timer_thread(), the timer
     structure is no longer valid.  The test below is not enough to prevent
     memory errors, since malloc() might return some old, already freed,
     address to a new timer.  */

  _nclua_get_registry_data (L, _NCLUA_REGISTRY_TIMER_TABLE);
  lua_pushlightuserdata (L, (void *) timer);
  lua_rawget (L, -2);
  if (!lua_isnil (L, -1))
    pthread_cancel (timer->tid);

  mutex_unlock (&timer->mutex);

  return 0;
}

/* Release data attached to timer thread.  */

static void
cleanup_timer_thread (void *data)
{
  nclua_event_timer_t *timer;
  lua_State *L;

  timer = (nclua_event_timer_t *) data;
  L = timer->L;

  mutex_lock (&timer->mutex);

  _nclua_get_registry_data (L, _NCLUA_REGISTRY_TIMER_TABLE);
  lua_pushlightuserdata (L, (void *) timer);
  lua_pushnil (L);
  lua_rawset (L, -3);

  mutex_fini (&timer->mutex);
  free (timer);
}

/* Timer thread.  */

static void *
timer_thread (void *data)
{
  nclua_event_timer_t *timer;
  lua_State *L;

  timer = (nclua_event_timer_t *) data;
  L = timer->L;

  pthread_cleanup_push (cleanup_timer_thread, data);
  SystemCompat::uSleep (timer->delay * 1000);

  /* Define a cancellation point.  */
  pthread_testcancel ();

  /* FIXME: This is **DANGEROUS**! We're calling
     the Lua callback function in a different thread.  */
  mutex_lock (&timer->mutex);

  _nclua_get_registry_data (L, _NCLUA_REGISTRY_TIMER_TABLE);
  lua_pushlightuserdata (L, (void *) timer);
  lua_rawget (L, -2);
  lua_call (L, 0, 0);

  mutex_unlock (&timer->mutex);

  pthread_cleanup_pop (1);
  return NULL;
}


/* The NCLua Event API (part ii -- from C to Lua).  */

/* Enqueues the event at top of stack into input queue.
   This function pops the event from stack.  */

void
nclua_send (nclua_t *nc, lua_State *L)
{
  int n;

  assert (nclua_get_lua_state (nc) == L);
  assert (lua_istable (L, -1));

  _nclua_get_registry_data (L, _NCLUA_REGISTRY_INPUT_QUEUE);
  n = lua_objlen (L, -1);
  lua_pushvalue (L, -2);
  ncluax_rawinsert (L, -2, n + 1);
  lua_pop (L, 3);
}

/* Dequeues one event from the output queue and pushes it onto stack.
   If the output queue is empty, pushes nil onto stack.  */

void
nclua_receive (nclua_t *nc, lua_State *L)
{
  int n;

  assert (nclua_get_lua_state (nc) == L);

  _nclua_get_registry_data (L, _NCLUA_REGISTRY_OUTPUT_QUEUE);
  n = lua_objlen (L, -1);
  if (n == 0)
    {
      lua_pop (L, 1);
      lua_pushnil (L);
      return;
    }

  ncluax_rawremove (L, -1, 1);
  lua_remove (L, -2);
}

/* DEPRECATED */

#include "nclua-event-deprecated.cpp"
