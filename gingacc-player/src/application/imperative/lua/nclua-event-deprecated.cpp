#define BAD_ARGUMENT "bad argument"
#define NOT_IMPLEMENTED "not implemented"

static int l_post_edit_event (lua_State *L);
static int l_post_key_event (lua_State *L);
static int l_post_ncl_event (lua_State *L);
static int l_post_pointer_event (lua_State *L);
static int l_post_si_event (lua_State *L);
static int l_post_sms_event (lua_State *L);
static int l_post_tcp_event (lua_State *L);
static int l_post_user_event (lua_State *L);
static int l_register_edit_event (lua_State *L);
static int l_register_key_event (lua_State *L);
static int l_register_ncl_event (lua_State *L);
static int l_register_pointer_event (lua_State *L);
static int l_register_si_event (lua_State *L);
static int l_register_sms_event (lua_State *L);
static int l_register_tcp_event (lua_State *L);
static int l_register_user_event (lua_State *L);

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

/* event.post ([dst:string], evt:event) -> status:boolean, errmsg:string
 *
 * Posts event EVT to the NCL player if DST == 'out', or to the NCLua player
 * itself if DST == 'in'.  Returns true if successful; otherwise returns
 * false plus error message.
 */

static int l_post (lua_State* L)
{
  int nresults;
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

  /* This must come after the above luaL_checktype() call; otherwise, we
     might return false in cases where trowing an exception would be more
     appropriate.  For example, if DST != "out" or DST != "in" and #2 is not
     a table.  */

  if (unlikely (!streq (dst, "out") && !streq (dst, "in")))
    goto error_bad_argument;    /* invalid destination */

  if (unlikely (lua_gettop (L) > 2))
    {
      _nclua_warning_extra_arguments (L);
      lua_settop (L, 2);
    }

  /* Post event to itself.  */
  if (streq (dst, "in"))
    {
      nclua_t *nc = nclua_get_nclua_state (L);
      LuaPlayer *player = (LuaPlayer *) nclua_get_user_data (nc, NULL);
      GingaScreenID id = player->getScreenId ();
      ILocalScreenManager *m = player->getScreenManager ();
      IInputEvent* evt;
      int ref;

      ref = luaL_ref (L, LUA_REGISTRYINDEX);
      evt = m->createApplicationEvent (id, ref, (void *) L);
      player->im->postInputEvent (evt);

      lua_pushboolean (L, TRUE);
      nresults = 1;
      goto tail;
    }

  /* Post event to NCL.  */
  if (unlikely (!ncluax_getstringfield (L, -1, "class", &cls)))
    goto error_bad_argument;    /* invalid class */

  handler = class_to_handler (cls, POST_HANDLER);
  if (unlikely (handler == NULL))
    goto error_bad_argument;    /* unknown class */

  nresults = handler (L);

 tail:
  return nresults;

 error_bad_argument:
  lua_pushboolean (L, FALSE);
  lua_pushstring (L, BAD_ARGUMENT);
  nresults = 2;
  goto tail;
}


/* Prints warning message if event table at INDEX is not empty.  Before
   performing the check, removes from table all fields indexed by the
   strings in the NULL-terminated varargs argument.  */

static void
warn_extra_fields_in_event (lua_State *L, int index, ...)
{
  va_list args;
  const char *k;
  int saved_top;
  int t;

  saved_top = lua_gettop (L);
  t = ncluax_abs (L, index);

  va_start (args, index);
  while ((k = va_arg (args, char *)) != NULL)
    ncluax_unsetfield (L, t, k);
  va_end (args);

  lua_pushnil (L);
  if (unlikely (lua_next (L, t)) != 0)
    _nclua_warning (L, 1, "ignoring extra fields in event");

  lua_settop (L, saved_top);
}

/* Posts NCL edit event at index 2.  Returns true if successful.
   Otherwise, returns false plus error message.  */

static int
l_post_edit_event (lua_State *L)
{
  lua_pushboolean (L, FALSE);
  lua_pushstring (L, NOT_IMPLEMENTED);
  return 2;
}

/* Posts key event.  */

static int
l_post_key_event (lua_State * L)
{
  lua_pushboolean (L, FALSE);
  lua_pushstring (L, NOT_IMPLEMENTED);
  return 2;
}

/* Posts NCL (presentation, attribution, or selection) event.  */

static int
l_post_ncl_event (lua_State *L)
{
  nclua_t *nc;
  LuaPlayer *player;

  const char *type;
  const char *action;
  int action_code;

  nc = nclua_get_nclua_state (L);
  assert (nc != NULL);

  player = (LuaPlayer *) nclua_get_user_data (nc, NULL);

  if (unlikely (!ncluax_getstringfield (L, 2, "type", &type)))
    goto error_bad_argument;    /* invalid type */

  if (unlikely (!ncluax_getstringfield (L, 2, "action", &action)))
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

      warn_extra_fields_in_event (L, 2, "class", "type",
                                  "action", "label", NULL);

      player->exec (Player::TYPE_PRESENTATION, action_code, string (label));
    }
  else if (streq (type, "attribution"))
    {
      const char *name;
      const char *value;

      if (unlikely (!ncluax_getstringfield (L, 2, "name", &name)))
        goto error_bad_argument; /* invalid name */

      if (unlikely (!ncluax_getstringfield (L, 2, "value", &value)))
        goto error_bad_argument; /* invalid value */

      warn_extra_fields_in_event (L, 2, "class", "type",
                                  "action", "name", "value", NULL);

      player->exec (Player::TYPE_ATTRIBUTION, action_code,
                    string (name), string (value));
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
  lua_pushstring (L, BAD_ARGUMENT);
  goto error;

 error_not_implemented:
  lua_pushstring (L, NOT_IMPLEMENTED);
  goto error;
}

/* Posts pointer (cursor) event.  */

static int
l_post_pointer_event (lua_State * L)
{
  lua_pushboolean (L, FALSE);
  lua_pushstring (L, NOT_IMPLEMENTED);
  return 2;
}

/* Posts SI event.  */

static int
l_post_si_event (lua_State * L)
{
  lua_pushboolean (L, FALSE);
  lua_pushstring (L, NOT_IMPLEMENTED);
  return 2;
}

/* Posts SMS event.  */

static int
l_post_sms_event (lua_State * L)
{
  lua_pushboolean (L, FALSE);
  lua_pushstring (L, NOT_IMPLEMENTED);
  return 2;
}

/* Posts TCP event. */

static int
l_post_tcp_event (lua_State * L)
{
  lua_pushboolean (L, FALSE);
  lua_pushstring (L, NOT_IMPLEMENTED);
  return 2;
}

/* Posts user event.  */

static int
l_post_user_event (lua_State * L)
{
  lua_pushboolean (L, FALSE);
  lua_pushstring (L, NOT_IMPLEMENTED);
  return 2;
}

/* event.uptime () -> ms:number
 *
 * Returns the number of milliseconds elapsed since last start.
 */

static int
l_uptime (lua_State *L)
{
  nclua_t *nc = nclua_get_nclua_state (L);
  LuaPlayer *player = (LuaPlayer *) nclua_get_user_data (nc, NULL);

  if (unlikely (lua_gettop (L) > 0))
    _nclua_warning_extra_arguments (L);

  lua_pushnumber (L, player->getMediaTime() * 1000);

  return 1;
}


/* event.register ([pos:number], f:function, [class:string], ...)
 *     -> status:boolean, [errmsg:string]; or
 *
 * event.register ([pos:number], f:function, [filter:table])
 *     -> status:boolean, [errmsg:string]
 *
 * Appends the function F to the handler list.
 * If POS is given, then F is registered in position POS of handler list.
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
  int nresults;
  int pos;
  int n;

  _nclua_get_registry_data (L, _NCLUA_REGISTRY_HANDLER_LIST);
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
      _nclua_get_registry_data (L, _NCLUA_REGISTRY_EMPTY_TABLE);
      lua_insert (L, 3);

      /* fall-through */

    case LUA_TTABLE:
      if (unlikely (lua_gettop (L) > 3))
        {
          _nclua_warning_extra_arguments (L);
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

  _nclua_get_registry_data (L, _NCLUA_REGISTRY_HANDLER_LIST);

  /* Creates {f, {filter}}.  */
  lua_createtable (L, 2, 0);
  lua_pushvalue (L, 2);
  lua_rawseti (L, -2, 1);
  lua_pushvalue (L, 3);
  lua_rawseti (L, -2, 2);

  /* Insert {f, {filter}} into handler list,
     shifting up any other elements, if necessary.  */

  ncluax_tableinsert (L, -2, pos);

  /* Success.  */
  lua_pushboolean (L, TRUE);
  nresults = 1;

 tail:
  return nresults;

  /* Failure.  */
 error:
  lua_pushboolean (L, FALSE);
  lua_insert (L, -2);
  nresults = 2;
  goto tail;

 error_bad_argument:
  lua_pushstring (L, BAD_ARGUMENT);
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
  lua_pushstring (L, NOT_IMPLEMENTED);
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
      _nclua_warning_extra_arguments (L);
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
  lua_pushstring (L, BAD_ARGUMENT);
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
      _nclua_warning_extra_arguments (L);
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
  lua_pushstring (L, BAD_ARGUMENT);
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
  lua_pushstring (L, NOT_IMPLEMENTED);
  return 2;
}

/* event.register (pos:number, f:function, 'si', ...)
       -> filter:table or false, [errmsg:string]

   Creates a filter for SI events with the given parameters.  */

static int
l_register_si_event (lua_State *L)
{
  lua_pushboolean (L, FALSE);
  lua_pushstring (L, NOT_IMPLEMENTED);
  return 2;
}

/* event.register (pos:number, f:function, 'sms', ...)
       -> filter:table or false, [errmsg:string]

   Creates a filter for SMS events with the given parameters.  */

static int
l_register_sms_event (lua_State *L)
{
  lua_pushboolean (L, FALSE);
  lua_pushstring (L, NOT_IMPLEMENTED);
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
  lua_pushstring (L, NOT_IMPLEMENTED);
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
      _nclua_warning_extra_arguments (L);
      lua_settop (L, 3);
    }

  lua_createtable (L, 0, 1);
  lua_pushvalue (L, 3);
  lua_setfield (L, -2, "class");

  return 1;
}

/* DEPRECATED */

void
nclua_sendx (nclua_t *nc)
{
  lua_State *L = (lua_State *) nclua_get_lua_state (nc);
  _nclua_notify (L);
  lua_pop (L, 1);
}
