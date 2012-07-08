/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware
Ginga (Ginga-NCL).

Direitos Autorais Reservados (c) 2012 PUC-Rio/Laboratorio TeleMidia

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

Copyright: 2012 PUC-RIO/LABORATORIO TELEMIDIA, All Rights Reserved.

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

#ifndef LUAPLAYER_EVENT_H
#define LUAPLAYER_EVENT_H

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>

#include "player/LuaPlayer.h"

#undef  ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof (x) / sizeof (x[0]))

#undef  ASSERT_NOT_REACHED
#define ASSERT_NOT_REACHED assert (!"reached")

#undef  STREQ
#define STREQ(x,y) (strcmp (x, y) == 0)


// Constants.

#define EVENT_FIELD_CLASS           "class"

// NCL
#define EVENT_NCL_CLASS             "ncl"
#define EVENT_NCL_FIELD_ACTION      "action"
#define EVENT_NCL_FIELD_LABEL       "label"
#define EVENT_NCL_FIELD_NAME        "name"
#define EVENT_NCL_FIELD_TYPE        "type"
#define EVENT_NCL_FIELD_VALUE       "value"
#define EVENT_NCL_TYPE_ATTRIBUTION  "attribution"
#define EVENT_NCL_TYPE_PRESENTATION "presentation"
#define EVENT_NCL_TYPE_SELECTION    "selection"
#define EVENT_NCL_ACTION_ABORT      "abort"
#define EVENT_NCL_ACTION_PAUSE      "pause"
#define EVENT_NCL_ACTION_RESUME     "resume"
#define EVENT_NCL_ACTION_START      "start"
#define EVENT_NCL_ACTION_STOP       "stop"

// Key
#define EVENT_KEY_CLASS             "key"
#define EVENT_KEY_FIELD_KEY         "key"
#define EVENT_KEY_FIELD_TYPE        "type"
#define EVENT_KEY_TYPE_PRESS        "press"
#define EVENT_KEY_TYPE_RELEASE      "release"

// User
#define EVENT_USER_CLASS            "user"


// Error messages.

#define event_errpush_invalid_field(L, f) \
     lua_pushfstring (L, "invalid event %s", f)

#define event_errpush_unknown_field(L, f, v) \
     lua_pushfstring (L, "unknown event %s '%s'", f, v)


// Auxiliary functions.

typedef struct
{
     const char *key;
     void *value;
} event_map_t;

static int event_map_compare (const void *p1, const void *p2)
{
     return strcmp (((event_map_t *) p1)->key,
                    ((event_map_t *) p2)->key);
}

static const event_map_t *event_map_get (const event_map_t map[],
                                         size_t size, const char *key)
{
     event_map_t e = {key, NULL};
     return (const event_map_t *)
          bsearch (&e, map, size, sizeof (event_map_t), event_map_compare);
}


// NCL class.

// Type name to code.
static const event_map_t event_ncl_type_map[] =
{
     // KEEP THIS SORTED ALPHABETICALLY.
     { EVENT_NCL_TYPE_ATTRIBUTION,  (void *) Player::TYPE_ATTRIBUTION  },
     { EVENT_NCL_TYPE_PRESENTATION, (void *) Player::TYPE_PRESENTATION },
     { EVENT_NCL_TYPE_SELECTION,    (void *) Player::TYPE_SELECTION    },
};

// Action name to code.
static const event_map_t event_ncl_action_map[] =
{
     // KEEP THIS SORTED ALPHABETICALLY.
     { EVENT_NCL_ACTION_ABORT,  (void *) Player::PL_NOTIFY_ABORT  },
     { EVENT_NCL_ACTION_PAUSE,  (void *) Player::PL_NOTIFY_PAUSE  },
     { EVENT_NCL_ACTION_RESUME, (void *) Player::PL_NOTIFY_RESUME },
     { EVENT_NCL_ACTION_START,  (void *) Player::PL_NOTIFY_START  },
     { EVENT_NCL_ACTION_STOP,   (void *) Player::PL_NOTIFY_STOP   },
};

#define _event_ncl_get_type_entry(type)                         \
     event_map_get (event_ncl_type_map,                         \
                    ARRAY_SIZE (event_ncl_type_map), type)

#define _event_ncl_get_action_entry(action)                     \
     event_map_get (event_ncl_action_map,                       \
                    ARRAY_SIZE (event_ncl_action_map), action)

// Returns true if type name TYPE is valid.
#define event_ncl_check_type(type) \
     (_event_ncl_get_type_entry (type) != NULL)

// Returns true if action name ACTION is value.
#define event_ncl_check_action(action) \
     (_event_ncl_get_action_entry (action) != NULL)

// Gets the type value associated with type name TYPE.
#define event_ncl_get_type_value(type) \
     ((int)(_event_ncl_get_type_entry (type))->value)

// Gets the action value associated with type name ACTION.
#define event_ncl_get_action_value(action) \
     ((int)(_event_ncl_get_action_entry (action))->value)

// Gets the type name associated with type value TYPE.
static const char *event_ncl_get_type_name (int type)
{
     switch (type)
     {
     case Player::TYPE_ATTRIBUTION:  return EVENT_NCL_TYPE_ATTRIBUTION;
     case Player::TYPE_PRESENTATION: return EVENT_NCL_TYPE_PRESENTATION;
     case Player::TYPE_SELECTION:    return EVENT_NCL_TYPE_SELECTION;
     }
     ASSERT_NOT_REACHED;
     return NULL;
}

// Gets the action name associated with action value ACTION.
static const char *event_ncl_get_action_name (int action)
{
     switch (action)
     {
     case Player::PL_NOTIFY_ABORT:  return EVENT_NCL_ACTION_ABORT;
     case Player::PL_NOTIFY_PAUSE:  return EVENT_NCL_ACTION_PAUSE;
     case Player::PL_NOTIFY_RESUME: return EVENT_NCL_ACTION_RESUME;
     case Player::PL_NOTIFY_START:  return EVENT_NCL_ACTION_START;
     case Player::PL_NOTIFY_STOP:   return EVENT_NCL_ACTION_STOP;
     }
     ASSERT_NOT_REACHED;
     return NULL;
}

// Checks if the event at top of stack is valid NCL event.
// If successful, pushes true onto stack.
// Otherwise, pushes false plus error message.

static int event_check_ncl_event (lua_State *L)
{
     const char *type;
     const char *action;

     lua_getfield (L, -1, EVENT_NCL_FIELD_TYPE);
     if (!lua_isstring (L, -1))
     {
          lua_pushboolean (L, 0);
          event_errpush_invalid_field (L, EVENT_NCL_FIELD_TYPE);
          return 2;
     }

     type = lua_tostring (L, -1);
     if (!event_ncl_check_type (type))
     {
          lua_pushboolean (L, 0);
          event_errpush_unknown_field (L, EVENT_NCL_FIELD_TYPE, type);
          return 2;
     }
     lua_pop (L, 1);

     lua_getfield (L, -1, EVENT_NCL_FIELD_ACTION);
     if (!lua_isstring (L, -1))
     {
          lua_pushboolean (L, 0);
          event_errpush_invalid_field (L, EVENT_NCL_FIELD_ACTION);
          return 2;
     }

     action = lua_tostring (L, -1);
     if (!event_ncl_check_action (action))
     {
          lua_pushboolean (L, 0);
          event_errpush_unknown_field (L, EVENT_NCL_FIELD_ACTION, action);
     }
     lua_pop (L, 1);

     if (STREQ (type, EVENT_NCL_TYPE_ATTRIBUTION))
     {
          lua_getfield (L, -1, EVENT_NCL_FIELD_NAME);
          if (!lua_isstring (L, -1))
          {
               lua_pushboolean (L, 0);
               event_errpush_invalid_field (L, EVENT_NCL_FIELD_NAME);
               return 2;
          }
          lua_pop (L, 1);

          lua_getfield (L, -1, EVENT_NCL_FIELD_VALUE);
          if (!lua_isstring (L, -1))
          {
               lua_pushboolean (L, 0);
               event_errpush_invalid_field (L, EVENT_NCL_FIELD_VALUE);
               return 2;
          }
          lua_pop (L, 1);
     }
     else if (STREQ (type, EVENT_NCL_TYPE_PRESENTATION)
              || STREQ (type, EVENT_NCL_TYPE_SELECTION))
     {
          lua_getfield (L, -1, EVENT_NCL_FIELD_LABEL);
          if (!lua_isstring (L, -1))
          {
               lua_pushboolean (L, 0);
               event_errpush_invalid_field (L, EVENT_NCL_FIELD_LABEL);
               return 2;
          }
          lua_pop (L, 1);
     }
     else
     {
          ASSERT_NOT_REACHED;
     }

     lua_pushboolean (L, 1);
     return 1;
}

// Receives the NCL event at top of stack.
// If successful, pushes true onto stack.
// Otherwise, pushes false plus error message.
//
// This function assumes that event is a valid NCL event.

static int event_receive_ncl_event (lua_State *L)
{
     int type;
     int action;
     const char *name;
     const char *value;

     nclua_t *nc;
     LuaPlayer *player;

     nc = nclua_get_nclua_state (L);
     player = (LuaPlayer *) nclua_get_user_data (nc, NULL);

     lua_getfield (L, -1, EVENT_NCL_FIELD_TYPE);
     type = event_ncl_get_type_value (lua_tostring (L, -1));
     lua_pop (L, 1);

     lua_getfield (L, -1, EVENT_NCL_FIELD_ACTION);
     action = event_ncl_get_action_value (lua_tostring (L, -1));
     lua_pop (L, 1);

     switch (type)
     {
     case Player::TYPE_ATTRIBUTION:
          lua_getfield (L, -1, EVENT_NCL_FIELD_NAME);
          name = lua_tostring (L, -1);
          lua_pop (L, 1);

          lua_getfield (L, -1, EVENT_NCL_FIELD_VALUE);
          value = lua_tostring (L, -1);
          lua_pop (L, 1);

          player->notifyPlayerListeners (action, string (name),
                                         type, string (value));
          break;

     case Player::TYPE_PRESENTATION:
          lua_getfield (L, -1, EVENT_NCL_FIELD_LABEL);
          name = lua_tostring (L, -1);
          lua_pop (L, 1);

          player->notifyPlayerListeners (action, string (name));
          break;

     case Player::TYPE_SELECTION:
          ASSERT_NOT_REACHED;
          break;                // TODO: not implemented
     }

     lua_pushboolean (L, 1);
     return 1;
}

// Sends NCL event with the given parameters to NCLua state NC.

#define event_send_ncl_presentation_event(nc, action, label)    \
     event_send_ncl_event (nc, Player::TYPE_PRESENTATION,       \
                           action, label, NULL)

#define event_send_ncl_attribution_event(nc, action, name, value)       \
     event_send_ncl_event (nc, Player::TYPE_ATTRIBUTION,                \
                           action, name, value)

static void event_send_ncl_event (nclua_t *nc, int type, int action,
                                  const char *name, const char *value)
{
     lua_State *L;

     L = nclua_get_lua_state (nc);

     lua_createtable (L, 0, 5);

     lua_pushstring (L, EVENT_NCL_CLASS);
     lua_setfield (L, -2, EVENT_FIELD_CLASS);

     lua_pushstring (L, event_ncl_get_type_name (type));
     lua_setfield (L, -2, EVENT_NCL_FIELD_TYPE);

     lua_pushstring (L, event_ncl_get_action_name (action));
     lua_setfield (L, -2, EVENT_NCL_FIELD_ACTION);

     switch (type)
     {
     case Player::TYPE_ATTRIBUTION:
          assert (name != NULL);
          lua_pushstring (L, name);
          lua_setfield (L, -2, EVENT_NCL_FIELD_NAME);

          assert (value != NULL);
          lua_pushstring (L, value);
          lua_setfield (L, -2, EVENT_NCL_FIELD_VALUE);
          break;

     case Player::TYPE_PRESENTATION:
     case Player::TYPE_SELECTION:
          assert (name != NULL);
          lua_pushstring (L, name);
          lua_setfield (L, -2, EVENT_NCL_FIELD_LABEL);
          break;
     default:
          ASSERT_NOT_REACHED;
     }

     nclua_send (nc, L);
}


// Key class.

// Checks if the event at top of stack is a valid key event.
// If successful, pushes true onto stack.
// Otherwise, pushes false plus error message.

static int event_check_key_event (lua_State *L)
{
     const char *type;
     const char *key;

     lua_getfield (L, -1, EVENT_KEY_FIELD_TYPE);
     if (!lua_isstring (L, -1))
     {
          lua_pushboolean (L, 0);
          event_errpush_invalid_field (L, EVENT_KEY_FIELD_TYPE);
          return 2;
     }

     type = lua_tostring (L, -1);
     if (!STREQ (type, EVENT_KEY_TYPE_PRESS)
         && !STREQ (type, EVENT_KEY_TYPE_RELEASE))
     {
          lua_pushboolean (L, 0);
          event_errpush_unknown_field (L, EVENT_KEY_FIELD_TYPE, type);
          return 2;
     }
     lua_pop (L, 1);

     lua_getfield (L, -1, EVENT_KEY_FIELD_KEY);
     if (!lua_isstring (L, -1))
     {
          lua_pushboolean (L, 0);
          event_errpush_invalid_field (L, EVENT_KEY_FIELD_KEY);
          return 2;
     }

     // TODO: Check if key is valid.

     key = lua_tostring (L, -1);
     lua_pop (L, 1);

     lua_pushboolean (L, 1);
     return 1;
}

// Receives the key event at top of stack.
// If successful, pushes true onto stack.
// Otherwise, pushes false plus error message.
//
// This function assumes that event is a valid key event.

static int event_receive_key_event (lua_State *L)
{
     // TODO: not implemented.
     lua_pushboolean (L, 1);
     return 1;
}

// Sends key event with the given parameters to the NCLua state NC.

static void event_send_key_event (nclua_t *nc, const char *key, int press)
{
     lua_State *L;
     const char *type;

     L = nclua_get_lua_state (nc);

     lua_createtable (L, 0, 3);

     lua_pushstring (L, EVENT_KEY_CLASS);
     lua_setfield (L, -2, EVENT_FIELD_CLASS);

     lua_pushstring (L, key);
     lua_setfield (L, -2, EVENT_KEY_FIELD_KEY);

     type = (press) ? EVENT_KEY_TYPE_PRESS : EVENT_KEY_TYPE_RELEASE;
     lua_pushstring (L, type);
     lua_setfield (L, -2, EVENT_KEY_FIELD_TYPE);

     nclua_send (nc, L);
}


// User class

// Checks if the event at top of stack is a valid user event.
// If successful, pushes true onto stack.
// Otherwise, pushes false plus error message.

static int event_check_user_event (lua_State *L)
{
     lua_pushboolean (L, 1);
     return 1;
}

// Receives the user event at top of stack.
// If successful, pushes true onto stack.
// Otherwise, pushes false plus error message.

static int event_receive_user_event (lua_State *L)
{
     // TODO: not implemented
     lua_pushboolean (L, 1);
     return 1;
}


// Generic event handling.

typedef struct
{
     const char *cls;            // event class
     lua_CFunction check_func;   // pointer to check function
     lua_CFunction receive_func; // pointer receive function
} event_class_map_t;

static const event_class_map_t event_class_map[] =
{
     // KEEP THIS SORTED ALPHABETICALLY.
     { EVENT_KEY_CLASS,  event_check_key_event,  event_receive_key_event  },
     { EVENT_NCL_CLASS,  event_check_ncl_event,  event_receive_ncl_event  },
     { EVENT_USER_CLASS, event_check_user_event, event_receive_user_event },
};

static int event_class_map_compare (const void *p1, const void *p2)
{
     return strcmp (((event_class_map_t *) p1)->cls,
                    ((event_class_map_t *) p2)->cls);
}

static const event_class_map_t *event_class_map_get (const char *cls)
{
     event_class_map_t key = {cls, NULL, NULL};
     return (const event_class_map_t *)
          bsearch (&key, event_class_map, ARRAY_SIZE (event_class_map),
                   sizeof (event_class_map_t), event_class_map_compare);
}

static int event_handle_event (lua_State *L, ptrdiff_t offset)
{
     const event_class_map_t *entry;
     const char *cls;
     lua_CFunction handler;

     luaL_checktype (L, -1, LUA_TTABLE);

     lua_getfield (L, -1, EVENT_FIELD_CLASS);
     if (!lua_isstring (L, -1))
     {
          lua_pushboolean (L, 0);
          event_errpush_invalid_field (L, EVENT_FIELD_CLASS);
          return 2;
     }

     cls = lua_tostring (L, -1);
     lua_pop (L, 1);

     entry = event_class_map_get (cls);
     if (entry == NULL)
     {
          lua_pushboolean (L, 0);
          event_errpush_unknown_field (L, EVENT_FIELD_CLASS, cls);
          return 2;
     }

     handler = *(lua_CFunction *)((char *) entry + offset);
     return handler (L);
}

// Checks if event at top of stack is valid.
// If successful, pushes true onto stack.
// Otherwise, pushes false plus error message.

static int event_check_event (lua_State *L)
{
     return event_handle_event (L, offsetof (event_class_map_t,
                                             check_func));
}

// Receives the event at top of stack.
// If successful, pushes true onto stack.
// Otherwise, pushes false plus error message.

static int event_receive_event (lua_State *L)
{
     return event_handle_event (L, offsetof (event_class_map_t,
                                             receive_func));
}

#endif // LUAPLAYER_EVENT_H

// Local variables:
// mode: c++
// c-file-style: "k&r"
// End:
