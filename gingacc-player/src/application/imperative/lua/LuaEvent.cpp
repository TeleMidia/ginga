/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware
Ginga (Ginga-NCL).

Direitos Autorais Reservados (c) 1989-2007 PUC-Rio/Laboratorio TeleMidia

Este programa eh software livre; voce pode redistribui-lo e/ou modificah-lo sob
os termos da Licenca Publica Geral GNU versao 2 conforme publicada pela Free
Software Foundation.

Este programa eh distribuido na expectativa de que seja util, porem, SEM
NENHUMA GARANTIA; nem mesmo a garantia implicita de COMERCIABILIDADE OU
ADEQUACAO A UMA FINALIDADE ESPECIFICA. Consulte a Licenca Publica Geral do
GNU versao 2 para mais detalhes.

Voce deve ter recebido uma copia da Licenca Publica Geral do GNU versao 2 junto
com este programa; se nao, escreva para a Free Software Foundation, Inc., no
endereco 59 Temple Street, Suite 330, Boston, MA 02111-1307 USA.

Para maiores informacoes:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
******************************************************************************
This file is part of the declarative environment of middleware Ginga (Ginga-NCL)

Copyright: 1989-2007 PUC-RIO/LABORATORIO TELEMIDIA, All Rights Reserved.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License version 2 as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License version 2 for more
details.

You should have received a copy of the GNU General Public License version 2
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

For further information contact:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
*******************************************************************************/

extern "C" {
#include <stdarg.h>
#include <assert.h>             /* for assert */
#include <stdio.h>              /* for vfprintf, fflush, fputc */
#include <stdlib.h>             /* for bsearch */
#include <string.h>             /* for strcmp */
}

#include "player/LuaPlayer.h"
using namespace ::br::pucrio::telemidia::ginga::core::player;

/* ---------------------------------------------------------------------- */

/* Uncomment the following line to disable assertions.  */
/* #define NDEBUG 1 */

/* Uncomment the following line to disable warning messages.  */
/* #define NWARNING 1 */

#define nclua_array_size(x) (sizeof (x) / sizeof (x[0]))
#define nclua_streq(x,y) (*(x) == *(y) && strcmp ((x), (y)) == 0)

/* Error reporting:  */

/* Prints a printf-like message to standard error.  */

static inline void
nclua_print_error (const char *format, ...)
{
  va_list args;

  fflush (stdout);
  va_start (args, format);
  vfprintf (stderr, format, args);
  va_end (args);

  putc ('\n', stderr);
  fflush (stderr);
}

/* Warning messages.  */
#ifndef NWARNING
# define nclua_warn(fmt, ...)                                           \
  nclua_print_error ("NCLUA warning:%s:%d:%s: "fmt"\n",                 \
                     __FILE__, __LINE__, __func__, ## __VA_ARGS__)

# define nclua_warn_ignored_args(L)                                     \
  nclua_warn("ignoring extra arguments to '%s'", nclua_currlfunc (L))
#else
# define nclua_warn(fmt,...)        /* empty */
# define nclua_warn_ignored_args(L) /* empty */
#endif

/* Auxiliary Lua stuff: */

/* Pushes onto the stack the value of t[K], where t is the table at INDEX,
   and set t[K] to nil.  */

#define nclua_fetchfield(L, index, k)                                   \
  do {                                                                  \
    lua_getfield ((L), (index), (k));                                   \
    lua_pushnil ((L));                                                  \
    lua_setfield ((L), ((index) > 0) ? (index) : (index) - 2, (k));     \
  } while (0)

/* Returns the name of the current Lua function.  */

static inline const char *
nclua_currlfunc (lua_State *L)
{
  lua_Debug ar;

  lua_getstack (L, 0, &ar);
  lua_getinfo (L, "n", &ar);
  return ar.name;
}

/* Dumps Lua stack.  */

static inline void
nclua_dump_stack (lua_State *L)
{
  int i;
  int top;

  fflush (stderr);
  fputs ("NCLUA stack dump: <", stdout);

  top = lua_gettop (L);
  for (i = 0; i <= top; i++)
    {
      int t = lua_type (L, i);
      switch (t)
        {
        case LUA_TSTRING:
          printf ("'%s'", lua_tostring (L, i));
          break;
        case LUA_TBOOLEAN:
          fputs (lua_toboolean (L, i) ? "true" : "false", stdout);
          break;
        case LUA_TNUMBER:
          printf ("%g", lua_tonumber (L, i));
          break;
        default:
          printf ("%s (%p)", lua_typename (L, t), lua_topointer (L, i));
          break;
        }
      fputs ((i < top) ? ", " : ">", stdout);
    }

  putc ('\n', stdout);
  fflush (stdout);
}

/* ---------------------------------------------------------------------- */

/* Key of the event table in registry.  */
#define LUAEVENT_TABLE_KEY "LuaPlayer.Event"

/* Index of the hooks queue in registry.  */
#define LUAEVENT_HOOKSQ_INDEX -1

/* status_t is used to indicate errors.  */
typedef enum _status_s
{
  STATUS_SUCCESS = 0,       /* no error has occurred */
  STATUS_INVALID_FIELD,     /* invalid event field */
  STATUS_MISSING_FIELD,     /* required event filed is missing */
  STATUS_UNKNOWN_ACTION,    /* unknown event action */
  STATUS_UNKNOWN_CLASS,     /* unknown event class */
  STATUS_UNKNOWN_TYPE,      /* unknown event type */
  STATUS_UNSUPPORTED_CLASS, /* unsupported event class */
  STATUS_UNSUPPORTED_TYPE,  /* unsupported event type */

  /* The following is a special value indicating the
     number of status values defined in this enumeration.  */

  STATUS_LAST_STATUS
} status_t;

/* Function prototypes: */
static const char *status_to_string (status_t status);
static void push_errmsg (lua_State *L, status_t status, const char *name);
static int action_to_code (const char *action);
static const char *code_to_action (int code);
static lua_CFunction class_to_handler (const char *name);
static int l_post (lua_State* L);
static int l_post_edit_event (lua_State *L);
static int l_post_key_event (lua_State *L);
static int l_post_ncl_event (lua_State *L);
static int l_post_pointer_event (lua_State *L);
static int l_post_si_event (lua_State *L);
static int l_post_sms_event (lua_State *L);
static int l_post_tcp_event (lua_State *L);
static int l_post_user_event (lua_State *L);
static int l_uptime (lua_State *L);
static int l_register (lua_State *L);

/* Provides a human-readable description of a nclua_status_t.
   Returns a string representation of status STATUS.  */

static const char *
status_to_string (status_t status)
{
  switch (status)
    {
    case STATUS_SUCCESS:
      return "no error has occurred";

    case STATUS_INVALID_FIELD:
      return "invalid value for event field";

    case STATUS_MISSING_FIELD:
      return "missing required event field";

    case STATUS_UNKNOWN_ACTION:
      return "unknown event action";

    case STATUS_UNKNOWN_CLASS:
      return "unknown event class";

    case STATUS_UNKNOWN_TYPE:
      return "unknown event type";

    case STATUS_UNSUPPORTED_CLASS:
      return "unsupported event class";

    case STATUS_UNSUPPORTED_TYPE:
      return "unsupported event type";

    default:
    case STATUS_LAST_STATUS:
      return "<unknown error status>";
    }
  assert (0);
}

/* Pushes onto stack the error message associated with STATUS.
   If NAME is non-NULL then append " 'NAME'" to the pushed message.  */

static inline void
push_errmsg (lua_State *L, status_t status, const char *name)
{
  lua_pushstring (L, status_to_string (status));
  if (name != NULL)
    {
      lua_pushfstring (L, " '%s'", name);
      lua_concat (L, 2);
    }
}

/* Table mapping each action name to its integer code.  */
static const struct _acttab
{
  const char *name;             /* action name */
  int code;                     /* action code */
}
action_table[] =
{
  /* KEEP THIS SORTED ALPHABETICALLY */
  { "abort",  Player::PL_NOTIFY_ABORT },
  { "pause",  Player::PL_NOTIFY_PAUSE },
  { "resume", Player::PL_NOTIFY_RESUME },
  { "start",  Player::PL_NOTIFY_START },
  { "stop",   Player::PL_NOTIFY_STOP },
};

static int
_compare__acttab_name (const void *p1, const void *p2)
{
  return strcmp (((struct _acttab *) p1)->name,
                 ((struct _acttab *) p2)->name);
}

static int
_compare__acttab_code (const void *p1, const void *p2)
{
  int c1 = ((struct _acttab *) p1)->code;
  int c2 = ((struct _acttab *) p2)->code;
  return (c1 < c2) ? -1 : (c1 == c2) ? 0 : 1;
}

/* Returns the positive integer code associated with action name ACTION
   or -1 if there is no such action.  */

static int
action_to_code (const char *action)
{
  struct _acttab key;
  struct _acttab *result;

  key.name = action;
  result = (struct _acttab *)
    bsearch (&key, action_table, nclua_array_size (action_table),
             sizeof (struct _acttab), _compare__acttab_name);

  return (result != NULL) ? result->code : -1;
}

/* Returns the name associated with action integer code CODE
   or NULL if there is no such code.  */

static const char *
code_to_action (int code)
{
  struct _acttab key;
  struct _acttab *result;

  key.code = code;
  result = (struct _acttab *)
    bsearch (&key, action_table, nclua_array_size (action_table),
             sizeof (struct _acttab), _compare__acttab_code);

  return (result != NULL) ? result->name : NULL;
}

/* Table mapping each event class to its handler.  */
static const struct _classtab
{
  const char *name;             /* class name */
  lua_CFunction handler;        /* handler function */
}
class_table[] =
{
  /* KEEP THIS SORTED ALPHABETICALLY */
  { "edit",    l_post_edit_event },
  { "key",     l_post_key_event },
  { "ncl",     l_post_ncl_event },
  { "pointer", l_post_pointer_event },
  { "si",      l_post_si_event },
  { "sms",     l_post_sms_event },
  { "tcp",     l_post_tcp_event },
  { "user",    l_post_user_event },
};

static int _compare__classtab_name (const void *p1, const void *p2)
{
  return strcmp (((struct _classtab *) p1)->name,
                 ((struct _classtab *) p2)->name);
}

/* Returns the handler function associated with class NAME
   or NULL if there is no such class.  */

static lua_CFunction class_to_handler (const char *name)
{
  struct _classtab key;
  struct _classtab *result;

  key.name = name;
  result = (struct _classtab *)
    bsearch (&key, class_table, nclua_array_size (class_table),
             sizeof (struct _classtab), _compare__classtab_name);

  return (result != NULL) ? result->handler : NULL;
}

/* Event API: */

/* event.post ([dst:string], evt:event) -> sent:boolean, errmsg:string

   This function posts event EVT to the NCL player (DST == 'out'), or to the
   NCLua player itself (DST == 'in').  Returns true if successful.
   Otherwise, returns false plus error message. */

static int l_post (lua_State* L)
{
  const char *dst;           /* destination */
  const char *cls;           /* event class */
  lua_CFunction handler;     /* class handler */

  int nargs = lua_gettop (L);
  if (nargs == 1)
    {
      lua_pushstring (L, "out");
      lua_insert (L, 1);
    }

  dst = luaL_checkstring (L, 1);
  luaL_checktype (L, 2, LUA_TTABLE);

  if (nargs > 2)
    {
      nclua_warn_ignored_args (L);
      lua_settop (L, 2);
    }

  /* Post event to itself.  */
  if (nclua_streq (dst, "in"))
    {
      /* FIXME: Find a sane way to do this.  */

      LuaPlayer *player = GETPLAYER (L);
      GingaScreenID id = player->getScreenId ();
      ILocalScreenManager *m = player->getScreenManager ();
      IInputEvent* evt;
      int ref;

      ref = luaL_ref (L, LUA_REGISTRYINDEX);
      evt = m->createApplicationEvent (id, ref, (void *) L);
      player->im->postInputEvent (evt);

      lua_pushboolean (L, 1);
      return 1;
    }

  /* Post event to NCL.  */
  lua_getfield (L, -1, "class");
  cls = lua_tostring (L, -1);
  if (cls == NULL)
    {
      push_errmsg (L, STATUS_MISSING_FIELD, "class");
      goto fail;
    }

  handler = class_to_handler (cls);
  if (handler == NULL)
    {
      push_errmsg (L, STATUS_UNKNOWN_CLASS, cls);
      goto fail;
    }

  lua_pop (L, 1);
  return handler (L);

 fail:
  lua_pushboolean (L, 0);
  lua_insert (L, -2);
  return 2;
}

/* Prints warning message if event table at INDEX is not empty.
   This is used by the l_post_*_event functions below.  */

#ifndef NWARNING
# define warn_extra_fields_in_event(L, index)                           \
  do {                                                                  \
    int saved_top = lua_gettop ((L));                                   \
    nclua_fetchfield ((L), (index), "class");                           \
    lua_pop ((L), 1);                                                   \
    lua_pushnil ((L));                                                  \
    if (lua_next ((L), ((index) > 0) ? (index) : (index) - 1) != 0)     \
      nclua_warn ("ignoring extra fields in event table");              \
    lua_settop ((L), saved_top);                                        \
  } while (0)
#else
# define warn_extra_fields_in_event(L, index) /* empty */
#endif

/* Posts NCL editing event.  */

static int
l_post_edit_event (lua_State *L)
{
  lua_pushboolean (L, 0);
  push_errmsg (L, STATUS_UNSUPPORTED_CLASS, "edit");
  return 2;
}

/* Posts key event.  */

static int
l_post_key_event (lua_State * L)
{
  lua_pushboolean (L, 0);
  push_errmsg (L, STATUS_UNSUPPORTED_CLASS, "key");
  return 2;
}

/* Posts NCL (presentation, attribution, or selection) events.  */

static int
l_post_ncl_event (lua_State *L)
{
  LuaPlayer *player;
  const char *type;
  const char *action;
  int type_code;
  int action_code;

  nclua_fetchfield (L, -1, "type");
  type = lua_tostring (L, -1);
  if (type == NULL)
    {
      push_errmsg (L, STATUS_MISSING_FIELD, "type");
      goto fail;
    }
  lua_pop (L, 1);

  /* Get action.  */
  nclua_fetchfield (L, -1, "action");
  action = lua_tostring (L, -1);
  if (action == NULL)
    {
      push_errmsg (L, STATUS_INVALID_FIELD, "action");
      goto fail;
    }
  action_code = action_to_code (action);
  if (action_code < 0)
    {
      push_errmsg (L, STATUS_UNKNOWN_ACTION, "action");
      goto fail;
    }
  lua_pop (L, 1);

  /* Get type.  */
  if (nclua_streq (type, "presentation"))
    {
      const char *label;

      type_code = Player::TYPE_PRESENTATION;

      /* Get label.  */
      nclua_fetchfield (L, -1, "label");
      label = (!lua_isnil (L, -1)) ? lua_tostring (L, -1) : "";
      if (label == NULL)
        {
          push_errmsg (L, STATUS_INVALID_FIELD, "label");
          goto fail;
        }
      lua_pop (L, 1);

      warn_extra_fields_in_event (L, -1);

      /* Execute presentation event.  */
      player = GETPLAYER (L);
      if (action_code == Player::PL_NOTIFY_STOP && *label == '\0')
        {
          player->im->removeApplicationInputEventListener (player);
        }
      player->notifyPlayerListeners (action_code, label);
    }
  else if (nclua_streq (type, "attribution"))
    {
      const char *name;
      const char *value;

      type_code = Player::TYPE_ATTRIBUTION;

      /* Get name.  */
      nclua_fetchfield (L, -1, "name");
      name = lua_tostring (L, -1);
      if (name == NULL)
        {
          push_errmsg (L, STATUS_INVALID_FIELD, "name");
          goto fail;
        }
      lua_pop (L, 1);

      /* Get value.  */
      nclua_fetchfield (L, -1, "value");
      value = (!lua_isnil (L, -1)) ? lua_tostring (L, -1) : "";
      if (value == NULL)
        {
          push_errmsg (L, STATUS_INVALID_FIELD, "value");
          goto fail;
        }
      lua_pop (L, 1);

      warn_extra_fields_in_event (L, -1);

      /* Execute attribution event.  */
      player = GETPLAYER (L);
      player->unprotectedSetPropertyValue (name, value);
      player->notifyPlayerListeners (action_code, name, type_code, value);
    }
  else if (nclua_streq (type, "selection"))
    {
      push_errmsg (L, STATUS_UNSUPPORTED_TYPE, "selection");
      goto fail;
    }
  else
    {
      push_errmsg (L, STATUS_UNKNOWN_TYPE, type);
      goto fail;
    }

  /* Success.  */
  lua_pushboolean (L, 1);
  return 1;

 fail:
  lua_pushboolean (L, 0);
  lua_insert (L, -2);
  return 2;
}

/* Posts pointer (cursor) event.  */

static int
l_post_pointer_event (lua_State * L)
{
  lua_pushboolean (L, 0);
  push_errmsg (L, STATUS_UNSUPPORTED_CLASS, "pointer");
  return 2;
}

/* Posts SI event.  */

static int
l_post_si_event (lua_State * L)
{
  lua_pushboolean (L, 0);
  push_errmsg (L, STATUS_UNSUPPORTED_CLASS, "si");
  return 2;
}

/* Posts SMS event.  */

static int
l_post_sms_event (lua_State * L)
{
  lua_pushboolean (L, 0);
  push_errmsg (L, STATUS_UNSUPPORTED_CLASS, "sms");
  return 2;
}

/* Posts TCP event. */

static int
l_post_tcp_event (lua_State * L)
{
  lua_pushboolean (L, 0);
  push_errmsg (L, STATUS_UNSUPPORTED_CLASS, "tcp");
  return 2;
}

/* Posts user event.  */

static int
l_post_user_event (lua_State * L)
{
  lua_pushboolean (L, 0);
  push_errmsg (L, STATUS_UNSUPPORTED_CLASS, "user");
  return 2;
}

/* event.uptime () -> ms:number

   Returns the number of milliseconds elapsed since last start.  */

static int
l_uptime (lua_State *L)
{
  LuaPlayer *player;

#ifndef NWARNING
  if (lua_gettop (L) > 0)
    nclua_warn_ignored_args (L);
#endif

  player = GETPLAYER (L);
  lua_pushnumber (L, player->getMediaTime() * 1000);
  return 1;
}

/* event.register2 ([pos:number], f:function, [class:string], ...), or
   event.register2 ([pos:number], f:function, [filter:table])

   Appends the function F to the listeners queue.
   If POS is given, then F is registered in position POS of listeners queue.
   If CLASS is given, then F called only for events of the given class.

   In the first form, any additional parameters are treated as class
   dependent filters.  In the second form, the table FILTER is used instead.

   Returns true if successful, otherwise return false plus error
   message.  */

static int
l_register2 (lua_State *L)
{
  int pos;
  int n;

  lua_rawgeti (L, LUA_ENVIRONINDEX, LUAEVENT_HOOKSQ_INDEX);
  n = lua_objlen (L, -1);

  /* Get pos.  */
  pos = luaL_optint (L, 1, -1);
  if (pos < 0 || pos > n + 1)
    pos = n + 1;

  return 0;
}


/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */


/* Index of the listeners table in event table.  */
#define LISTENERS_TABLE_INDEX -3

/* Index of the pending listeners table in event table.  */
#define PENDING_LISTENERS_TABLE_INDEX -4



/* TODO: REMOVE THIS!  */

enum
{
    REFTCPIN=6,
    REFTCPOUT,
};


// Dispatch event at top of stack.

static void int_dispatch (lua_State* L)
{
     lua_getfield (L, LUA_REGISTRYINDEX, LUAEVENT_TABLE_KEY);

    // [ ... | evt | env ]

    lua_rawgeti(L, -1, PENDING_LISTENERS_TABLE_INDEX);  // [ ... | evt | env | newlst ]
    if (!lua_isnil(L, -1)) {
        lua_pushvalue(L, -1);              // [ ... | evt | env | newlst | newlst ]
        lua_rawseti(L, -3, LISTENERS_TABLE_INDEX); // [ ... | evt | env | newlst ]
        lua_pushnil(L);                    // [ ... | evt | env | newlst | nil ]
        lua_rawseti(L, -3, PENDING_LISTENERS_TABLE_INDEX); // [  | evt | env | newlst ]
    }
    else {
        lua_pop(L, 1);                     // [ ... | evt | env ]
	    lua_rawgeti(L, -1, LISTENERS_TABLE_INDEX); // [ ... | evt | env | lst ]
    }

    // [ ... | evt | env | lst ]
    // iterate over all listeners and call each of them
	int len = lua_objlen(L, -1);
	for (int i=1; i<=len; i++)
    {
		lua_rawgeti(L, -1, i);         // [ ... | evt | env | lst | t ]
        lua_getfield(L, -1, "__func"); // [ ... | evt | env | lst | t | func ]
		lua_pushvalue(L, -5);          // [ ... | evt | env | lst | t | func | evt]

		lua_call(L, 1, 1);
		if (lua_toboolean(L, -1)) {
            lua_pop(L, 2);             // [ ... | evt | env | lst ]
            break;
        }
        lua_pop(L, 2);                 // [ ... | evt | env | lst ]
	}

    // [ ... | evt | env | lst ]
    lua_pop(L, 3);                     // [ ... ]
}

LUALIB_API int ext_postRef (lua_State* L, int ref)
{
    // [ ... ]
    lua_rawgeti(L, LUA_REGISTRYINDEX, ref);  // [ ... | evt ]
    luaL_unref(L, LUA_REGISTRYINDEX, ref);
    int_dispatch(L);                         // [ ... ]
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
    int_dispatch(L);  // [ ...]
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
    	int_dispatch(L);  // [ ... ]
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


/*********************************************************************
 * event.register
 *********************************************************************/

static void int_newlisteners (lua_State* L, int lst)
{
    // [ ... ]
    lua_newtable(L);               // [ ... | newlst ]
    int len = lua_objlen(L, lst);
    for (int i=1; i<=len; i++)
    {
        lua_rawgeti(L, lst, i);    // [ ... | newlst | hdlr ]
        lua_rawseti(L, -2, i);     // [ ... | newlst ]
    }
    // [ ... | newlst ]
}


#if 1
static int
l_register (lua_State *L)
{
    // [ [i] | func | [filter] ]

    // [i] -> i
    if (lua_type(L, 1) == LUA_TFUNCTION) {
	    lua_rawgeti(L, LUA_ENVIRONINDEX, LISTENERS_TABLE_INDEX);
            // [ func | [filter] | listeners ]
        lua_pushnumber(L, lua_objlen(L, -1)+1);
            // [ func | [filter] | listeners | #listeners+1 ]
        lua_insert(L, 1);
            // [ #listeners+1 | func | [filter] | listeners ]
        lua_pop(L, 1);
            // [ #listeners+1 | func | [filter] ]
    }

    // [ i | func | [filter] ]

    // [filter] -> filter
    if (lua_gettop(L) == 2)
        lua_newtable(L);    // [ i | func | filter ]

    // [ i | func | filter ]
	//luaL_checktype(L, 1, LUA_TNUMBER);
	//luaL_checktype(L, 2, LUA_TFUNCTION);
	//luaL_checktype(L, 3, LUA_TTABLE);
    if(!lua_isnumber(L, 1)){

		fprintf(stderr,"Lua conformity error: event module\n");
		fprintf(stderr,"Call to 'register' does not follow Ginga-NCL standard: bad argument #1 (number expected)\n");
		fprintf(stderr,"Chamada a 'register' nao segue padrao Ginga-NCL: argumento #1 (number esperado)\n");

	    	return 0;
    }
    if(!lua_isfunction(L,2)){

		fprintf(stderr,"Lua conformity error: event module\n");
		fprintf(stderr,"Call to 'register' does not follow Ginga-NCL standard: bad argument #2 (function expected)\n");
		fprintf(stderr,"Chamada a 'register' nao segue padrao Ginga-NCL: argumento #2 (function esperado)\n");

	    	return 0;
    }
    if(!lua_istable(L,3)){

		fprintf(stderr,"Lua conformity error: event module\n");
		fprintf(stderr,"Call to 'register' does not follow Ginga-NCL standard: bad argument #3 (table expected)\n");
		fprintf(stderr,"Chamada a 'register' nao segue padrao Ginga-NCL: argumento #3 (table esperado)\n");
	        return 0;
    }

    // filter.__func = func
    lua_pushvalue(L, 2);           // [ i | func | filter | func ]
    lua_setfield(L, -2, "__func"); // [ i | func | filter ]

    // creates newlisteners
    lua_rawgeti(L, LUA_ENVIRONINDEX, LISTENERS_TABLE_INDEX);    // [ i | func | filter | lst ]
    lua_rawgeti(L, LUA_ENVIRONINDEX, PENDING_LISTENERS_TABLE_INDEX); // [ i | func | filter | lst | ? ]
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);           // [ i | func | filter | lst ]
        int_newlisteners(L, 4);  // [ i | func | filter | lst | newlst ]
        lua_pushvalue(L, -1);    // [ i | func | filter | lst | newlst | newlst ]
        lua_rawseti(L, LUA_ENVIRONINDEX, PENDING_LISTENERS_TABLE_INDEX); // [ ewlst ]
    }

    // [ i | func | filter | lst | newlst ]
    lua_getglobal(L, "table");     // [ -> | table ]
    lua_getfield(L, -1, "insert"); // [ -> | table | tinsert ]
    lua_pushvalue(L, 5);           // [ -> | table | tinsert | newlst ]
    lua_pushvalue(L, 1);           // [ -> | table | tinsert | newlst | i ]
    lua_pushvalue(L, 3);           // [ -> | table | tinsert | newlst | i | filter ]

    if (lua_pcall(L, 3, 0, 0) != 0) {// [ -> | table ]
    	clog << "LUAEVENT l_register ERROR:: ";
    	clog << lua_tostring(L, -1) << endl;
    }

    // [ i | func | filter | lst | newlst | table ]
    return 0;
#endif
}

/*********************************************************************
 * event.unregister
 *********************************************************************/

static int l_unregister (lua_State* L)
{
	// [ func ]
	// luaL_checktype(L, 1, LUA_TFUNCTION);
	if(!lua_isfunction(L, 1)){

		fprintf(stderr,"Lua conformity error: event module\n");
		fprintf(stderr,"Call to 'unregister' does not follow Ginga-NCL standard: bad argument #1 (function expected)\n");
		fprintf(stderr,"Chamada a 'unregister' nao segue padrao Ginga-NCL: argumento #1 (function esperado)\n");

		return 0;
	}
    // creates newlisteners
    lua_rawgeti(L, LUA_ENVIRONINDEX, LISTENERS_TABLE_INDEX);    // [ func | lst ]
    lua_rawgeti(L, LUA_ENVIRONINDEX, PENDING_LISTENERS_TABLE_INDEX); // [ func | lst | ? ]
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);                        // [ func | lst ]
        int_newlisteners(L, 4);               // [ func | lst | newlst ]
        lua_pushvalue(L, -1);                 // [ func | lst | newlst | newlst ]
        lua_rawseti(L, LUA_ENVIRONINDEX, PENDING_LISTENERS_TABLE_INDEX); // [ ewlst ]
    }

    // [ func | lst | newlst ]

	int len = lua_objlen(L, 3);
    for (int i=len; i>=1; i--)
    {
	    lua_rawgeti(L, -1, i);         // [ func | lst | newlst | t ]
        lua_getfield(L, -1, "__func"); // [ func | lst | newlst | t | f' ]
	    if (!lua_equal(L, -3, -1)) {
            lua_getglobal(L, "table");     // [ -> | f' | table ]
            lua_getfield(L, -1, "remove"); // [ -> | f' | table | remove ]
            lua_pushvalue(L, 3);           // [ -> | f' | table | remove | newlst ]
            lua_pushnumber(L, i);          // [ -> | f' | table | remove | newlst | i ]

            if (lua_pcall(L, 2, 0, 0) != 0) {// [ -> | f' | table ]
            	clog << "LUAEVENT l_unregister ERROR:: ";
            	clog << lua_tostring(L, -1) << endl;
            }

            lua_pop(L, 1);                 // [ -> | f' ]
	    }
	    lua_pop(L, 2);                 // [ func | lst | newlst ]
	}

    // [ func | lst | newlst ]
    return 0;
}

/*********************************************************************
 * Funcoes exportadas pelo modulo.
 ********************************************************************/

static const struct luaL_Reg funcs[] = {
	{ "post",       l_post       },
	{ "timer",      l_timer      },
	{ "uptime",     l_uptime     },
	{ "register",   l_register   },
	{ "unregister", l_unregister },
	{ NULL,         NULL         }
};

/*********************************************************************
 * Funcao que carrega o modulo.
 * - cria as variaveis locais ao modulo
 ********************************************************************/

LUALIB_API int luaclose_event (lua_State* L)
{
	pthread_cancel(GETPLAYER(L)->tcp_thread_id);
	return 1;
}

LUALIB_API int luaopen_event (lua_State* L)
{
     // Create event table and store it in registry.
     lua_newtable (L);
     lua_pushvalue (L, -1);
     lua_setfield (L, LUA_REGISTRYINDEX, LUAEVENT_TABLE_KEY);

     // Replace the current environment by the event table.
     lua_replace (L, LUA_ENVIRONINDEX);

     // Create the listeners table.
     lua_newtable (L);
     lua_rawseti (L, LUA_ENVIRONINDEX, LISTENERS_TABLE_INDEX);

#if 0
    // env[NCLMAP] = {
    //     start  = PL_NOTIFY_START,
    //     stop   = PL_NOTIFY_STOP,
    //     pause  = PL_NOTIFY_PAUSE,
    //     resume = PL_NOTIFY_RESUME,
    //     abort  = PL_NOTIFY_ABORT,
	// }
	lua_newtable(L);                              // [ nclmap ]
	lua_pushnumber(L, Player::PL_NOTIFY_START);   // [ nclmap | START ]
	lua_setfield(L, -2, "start");                 // [ nclmap ]
	lua_pushnumber(L, Player::PL_NOTIFY_STOP);    // [ nclmap | STOP ]
	lua_setfield(L, -2, "stop");                  // [ nclmap ]
	lua_pushnumber(L, Player::PL_NOTIFY_PAUSE);   // [ nclmap | PAUSE ]
	lua_setfield(L, -2, "pause");                 // [ nclmap ]
	lua_pushnumber(L, Player::PL_NOTIFY_RESUME);  // [ nclmap | RESUME ]
	lua_setfield(L, -2, "resume");                // [ nclmap ]
	lua_pushnumber(L, Player::PL_NOTIFY_ABORT);   // [ nclmap | ABORT ]
	lua_setfield(L, -2, "abort");                 // [ nclmap ]
	lua_rawseti(L, LUA_ENVIRONINDEX, -REFNCLMAP); // [ ]
#endif

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

#if 0
static void* tcp_thread (void* data)
{
    LuaPlayer* player = (LuaPlayer*) data;
    player->tcp_running = true;
//clog << "TCP STARTED\n";
    while (1) {
    	SystemCompat::uSleep(500000);
        player->lock();
        lua_getfield(player->L, LUA_REGISTRYINDEX, LUAEVENT_TABLE_KEY);  // [ ... | env ]
        lua_rawgeti(player->L, -1, -REFTCPIN);       // [ ... | env | f_in ]

        if (lua_pcall(player->L, 0, 1, 0) != 0) {          // [ ... | env | count ]
        	clog << "LUAEVENT tcp_thread ERROR:: ";
        	clog << lua_tostring(player->L, -1) << endl;
        }

        int count = luaL_checknumber(player->L, -1); // [ ... | env | count ]
        lua_pop(player->L, 2);                       // [ ... ]
        player->unlock();
        if (count == 0) break;
    }
//clog << "TCP FINISHED\n";
    player->tcp_running = false;
    return NULL;
}
#endif
