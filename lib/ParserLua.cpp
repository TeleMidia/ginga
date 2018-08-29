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
#include "ParserLua.h"

#include "Context.h"
#include "Media.h"
#include "MediaSettings.h"
#include "Switch.h"
#include "Predicate.h"

GINGA_BEGIN_DECLS
#include "aux-lua.h"
GINGA_END_DECLS

GINGA_NAMESPACE_BEGIN

/* TODO:

   - l_parse_media() is not dealing with labels

   - make l_parse_children to avoid duplicated code
   remember to test if property exists before adding to document
   make maps<> to help parsing and avoid big if/case statements
 */

// declarations
static int l_parse_context (lua_State *L);
static int l_parse_switch (lua_State *L);
static int l_parse_port (lua_State *L);
static int l_parse_media (lua_State *L);
static int l_parse_link (lua_State *L);
static int l_parse_predicate (lua_State *L);

// Helper function
static Event *
getEventStringAsEvent (string str, Context *parent)
{
  Object *obj;
  string id, evt;

  size_t at = str.find ('@');

  if (at != str.npos) // presentation
  {
    id = str.substr (0, at);
    evt = str.substr (at + 1, str.npos);
    obj = parent->getChildById (id);

    // only lambda should have '@' as prefix? this seems strange
    if (xstrcasecmp ("lambda", evt) == 0)
      evt = "@lambda";

    return obj->getEvent (Event::PRESENTATION, evt);
  }
  else if (str.find ('.') != str.npos) // attribution
  {
    at = str.find ('.');
    id = str.substr (0, at);
    evt = str.substr (at + 1, str.npos);
    obj = parent->getChildById (id);
    return obj->getEvent (Event::ATTRIBUTION, evt);
  }
  else // selection
  {
    at = str.find ('<');
    id = str.substr (0, at);
    evt = str.substr (at + 1, str.npos);
    evt.pop_back ();
    obj = parent->getChildById (id);
    return obj->getEvent (Event::SELECTION, evt);
  }
}

/* Parsing functions. */

/* parse_context (doc, [parent], tab, path) */
static int
l_parse_context (lua_State *L)
{
  Document *doc;
  Context *ctx;
  Composition *parent;
  const char *tag;
  const char *id;

  doc = (Document *) lua_touserdata (L, 1);
  g_assert_nonnull (doc);
  parent = (Composition *) lua_touserdata (L, 2);

  luaL_checktype (L, 3, LUA_TTABLE);
  lua_rawgeti (L, 3, 1);

  tag = luaL_checkstring (L, -1);

  if (!g_str_equal (tag, "context"))
  {
    lua_pushfstring (L, "unexpected tag: %s", tag);
    lua_error (L);
  }

  lua_rawgeti (L, 3, 2);
  id = luaL_checkstring (L, -1);

  if (parent == nullptr) // root
  {
    Context *root = doc->getRoot ();
    ctx = root;
    root->addAlias (string (id));
    parent = root;
  }
  else // non-root
  {
    ctx = new Context (id);
    parent->addChild (ctx);
  }

  lua_rawgeti (L, 3, 5);
  if (lua_isnil (L, -1) == 0) // children
  {
    lua_pushnil (L);
    while (lua_next (L, -2) != 0)
    {
      lua_rawgeti (L, -1, 1);
      string child = luaL_checkstring (L, -1);

      if (xstrcasecmp (child, "context") == 0)
      {
        lua_pushcfunction (L, l_parse_context);
        lua_pushlightuserdata (L, doc);
        lua_pushlightuserdata (L, ctx);
        lua_pushvalue (L, -5);
        lua_pushvalue (L, 4);
        lua_call (L, 4, 0);
        lua_pop (L, 2);
      }
      else if (xstrcasecmp (child, "switch") == 0)
      {
        lua_pushcfunction (L, l_parse_switch);
        lua_pushlightuserdata (L, doc);
        lua_pushlightuserdata (L, ctx);
        lua_pushvalue (L, -5);
        lua_pushvalue (L, 4);
        lua_call (L, 4, 0);
        lua_pop (L, 2);
      }
      else if (xstrcasecmp (child, "media") == 0)
      {
        lua_pushcfunction (L, l_parse_media);
        lua_pushlightuserdata (L, doc);
        lua_pushlightuserdata (L, ctx);
        lua_pushvalue (L, -5);
        lua_pushvalue (L, 4);
        lua_call (L, 4, 0);
        lua_pop (L, 2);
      }
      else
      {
        g_assert_not_reached (); // print error message first
      }
    }
  }

  lua_rawgeti (L, 3, 4);
  if (lua_isnil (L, -1) == 0) // ports
  {
    lua_pushnil (L);
    while (lua_next (L, -2) != 0)
    {
      lua_pushcfunction (L, l_parse_port);
      lua_pushlightuserdata (L, doc);
      lua_pushlightuserdata (L, ctx);
      lua_pushvalue (L, -4);
      lua_call (L, 3, 0);
      lua_pop (L, 1);
    }
  }

  lua_rawgeti (L, 3, 6);
  if (lua_isnil (L, -1) == 0) // links
  {
    lua_pushnil (L);
    while (lua_next (L, -2) != 0)
    {
      lua_pushcfunction (L, l_parse_link);
      lua_pushlightuserdata (L, doc);
      lua_pushlightuserdata (L, ctx);
      lua_pushvalue (L, -4);
      lua_call (L, 3, 0);
      lua_pop (L, 1);
    }
  }

  return 0;
}

// parse_switch (doc, parent, tab, path)
static int
l_parse_switch (lua_State *L)
{
  Switch *swtch;
  Document *doc;
  Composition *parent;
  const char *tag;
  const char *id;

  doc = (Document *) lua_touserdata (L, 1);
  g_assert_nonnull (doc);
  parent = (Composition *) lua_touserdata (L, 2);
  g_assert_nonnull (parent);

  luaL_checktype (L, 3, LUA_TTABLE);

  lua_rawgeti (L, 3, 1);
  tag = luaL_checkstring (L, -1);

  if (!g_str_equal (tag, "switch"))
  {
    lua_pushfstring (L, "unexpected tag: %s", tag);
    lua_error (L);
  }

  lua_rawgeti (L, 3, 2);
  id = luaL_checkstring (L, -1);

  if (parent == NULL) // error
  {
    lua_pushfstring (L, "parent missing: %s", id);
    lua_error (L);
  }

  swtch = new Switch (id);
  parent->addChild (swtch);

  lua_rawgeti (L, 3, 3);
  if (lua_isnil (L, -1) == 0) // children
  {
    lua_pushnil (L);
    while (lua_next (L, -2) != 0)
    {
      lua_rawgeti (L, -1, 1);
      string child = luaL_checkstring (L, -1);

      if (xstrcasecmp (child, "context") == 0)
      {
        lua_pushcfunction (L, l_parse_context);
        lua_pushlightuserdata (L, doc);
        lua_pushlightuserdata (L, swtch);
        lua_pushvalue (L, -5);
        lua_pushvalue (L, 4);
        lua_call (L, 4, 0);
        lua_pop (L, 2);
      }
      else if (xstrcasecmp (child, "switch") == 0)
      {
        lua_pushcfunction (L, l_parse_switch);
        lua_pushlightuserdata (L, doc);
        lua_pushlightuserdata (L, swtch);
        lua_pushvalue (L, -5);
        lua_pushvalue (L, 4);
        lua_call (L, 4, 0);
        lua_pop (L, 2);
      }
      else if (xstrcasecmp (child, "media") == 0)
      {
        lua_pushcfunction (L, l_parse_media);
        lua_pushlightuserdata (L, doc);
        lua_pushlightuserdata (L, swtch);
        lua_pushvalue (L, -5);
        lua_pushvalue (L, 4);
        lua_call (L, 4, 0);
        lua_pop (L, 2);
      }
      else
      {
        g_assert_not_reached (); // print error message first
      }
    }
  }

  lua_rawgeti (L, 3, 4);
  if (lua_isnil (L, -1) == 0) // rules
  {
    lua_pushnil (L);
    while (lua_next (L, -2) != 0)
    {
      lua_rawgeti (L, 10, 1);
      const string id_media = string (luaL_checkstring (L, -1));
      Object *obj = swtch->getChildById (id_media);

      lua_rawgeti (L, 10, 2);
      luaL_checktype (L, -1, LUA_TTABLE);

      Predicate *predicate = new Predicate (Predicate::CONJUNCTION);
      lua_pushcfunction (L, l_parse_predicate);
      lua_pushlightuserdata (L, predicate);
      lua_pushvalue (L, -3);
      lua_call (L, 2, 0);

      Predicate *pred = predicate->getChildren ()->front ();
      swtch->addRule (obj, pred);

      lua_pop (L, 3);
    }
  }

  return 0;
}

// parse_port (doc, parent, port)
static int
l_parse_port (lua_State *L)
{
  Event *event;
  Document *doc;
  Context *parent;
  string str;

  doc = (Document *) lua_touserdata (L, 1);
  g_assert_nonnull (doc);
  parent = (Context *) lua_touserdata (L, 2);
  g_assert_nonnull (parent);
  str = string (luaL_checkstring (L, -1));

  event = getEventStringAsEvent (str, parent);
  g_assert_nonnull (event);
  parent->addPort (event);

  return 0;
}

// parse_media (doc, parent, tab, path)
static int
l_parse_media (lua_State *L)
{
  Media *media;
  Document *doc;
  Composition *parent;
  const char *tag;
  const char *id;
  const char *name;
  const char *value;
  const char *path;
  string src, str;
  Time begin, end;

  doc = (Document *) lua_touserdata (L, 1);
  g_assert_nonnull (doc);
  parent = (Composition *) lua_touserdata (L, 2);

  luaL_checktype (L, 3, LUA_TTABLE);
  lua_rawgeti (L, 3, 1);
  tag = luaL_checkstring (L, -1);

  if (!g_str_equal (tag, "media"))
  {
    lua_pushfstring (L, "unexpected tag: %s", tag);
    lua_error (L);
  }

  lua_rawgeti (L, 3, 2);
  id = luaL_checkstring (L, -1);
  media = new Media (id);

  if (parent == NULL) // error
  {
    lua_pushfstring (L, "parent missing: %s", id);
    lua_error (L);
  }

  lua_rawgeti (L, 3, 3);
  if (lua_isnil (L, -1) == 0) // have property list
  {
    lua_pushnil (L);
    while (lua_next (L, 7) != 0)
    {
      name = lua_tolstring (L, -2, 0);
      value = lua_tolstring (L, -1, 0);

      if (xstrcasecmp ("src", name) == 0)
      {
        name = "uri";
        src = string (value);

        // resolve relative dir
        if (src != "" && !xpathisuri (src) && !xpathisabs (src))
        {
          path = luaL_checkstring (L, 4);
          string dir = xpathdirname (path);
          src = xurifromsrc (src, dir);
          value = src.c_str ();
        }
      }
      else if ((xstrcasecmp ("transIn", name) == 0)
               || (xstrcasecmp ("transOut", name) == 0))
      {
        string aux;

        lua_getfield (L, 9, "type");
        if (lua_isnil (L, -1) == 0)
        {
          aux = lua_tolstring (L, -1, 0);
          str = "{type='" + aux + "',";
        }
        else
        {
          g_assert_not_reached ();
        }

        lua_getfield (L, 9, "subtype");
        if (lua_isnil (L, -1) == 0)
          aux = lua_tolstring (L, -1, 0);
        else
          aux = "";
        str += "subtype='" + aux + "',";

        lua_getfield (L, 9, "dur");
        if (lua_isnil (L, -1) == 0)
          aux = lua_tolstring (L, -1, 0);
        else
          aux = "0";
        str += "dur='" + aux + "',";

        lua_getfield (L, 9, "startProgress");
        if (lua_isnil (L, -1) == 0)
          aux = lua_tolstring (L, -1, 0);
        else
          aux = "0";
        str += "startProgress='" + aux + "',";

        lua_getfield (L, 9, "endProgress");
        if (lua_isnil (L, -1) == 0)
          aux = lua_tolstring (L, -1, 0);
        else
          aux = "0";
        str += "endProgress='" + aux + "',";

        lua_getfield (L, 9, "direction");
        if (lua_isnil (L, -1) == 0)
          aux = lua_tolstring (L, -1, 0);
        else
          aux = "forward";
        str += "direction='" + aux + "',";

        lua_getfield (L, 9, "fadeColor");
        if (lua_isnil (L, -1) == 0)
          aux = lua_tolstring (L, -1, 0);
        else
          aux = "";
        str += "fadeColor='" + aux + "',";

        lua_getfield (L, 9, "horzRepeat");
        if (lua_isnil (L, -1) == 0)
          aux = lua_tolstring (L, -1, 0);
        else
          aux = "0";
        str += "horzRepeat='" + aux + "',";

        lua_getfield (L, 9, "vertRepeat");
        if (lua_isnil (L, -1) == 0)
          aux = lua_tolstring (L, -1, 0);
        else
          aux = "0";
        str += "vertRepeat='" + aux + "',";

        lua_getfield (L, 9, "borderWidth");
        if (lua_isnil (L, -1) == 0)
          aux = lua_tolstring (L, -1, 0);
        else
          aux = "0";
        str += "borderWidth='" + aux + "',";

        lua_getfield (L, 9, "borderColor");
        if (lua_isnil (L, -1) == 0)
          aux = lua_tolstring (L, -1, 0);
        else
          aux = "";
        str += "borderColor='" + aux + "',";

        str += "}";
        value = str.c_str ();
        lua_pop (L, 11);
      }

      media->addAttributionEvent (name);
      media->setProperty (name, value);
      lua_pop (L, 1);
    }
  }

  lua_pop (L, 3);

  lua_rawgeti (L, 3, 4);
  if (lua_isnil (L, -1) == 0) // have area list
  {
    lua_pushnil (L);
    while (lua_next (L, 5) != 0)
    {
      lua_rawgeti (L, -1, 1);
      name = lua_tolstring (L, -1, nullptr);

      lua_pop (L, 1);
      lua_rawgeti (L, -1, 2);
      value = lua_tolstring (L, -1, nullptr);

      begin = 0;
      if ((value != NULL)
          && (unlikely (!ginga::try_parse_time (value, &begin))))
      {
        lua_pushfstring (L, "bad attr: %s", value);
        lua_error (L);
      }

      lua_pop (L, 1);
      lua_rawgeti (L, -1, 3);
      value = lua_tolstring (L, -1, 0);

      end = GINGA_TIME_NONE;
      if ((value != NULL)
          && (unlikely (!ginga::try_parse_time (value, &end))))
      {
        lua_pushfstring (L, "bad attr: %s", value);
        lua_error (L);
      }

      media->addPresentationEvent (name, begin, end);
      lua_pop (L, 2);
    }
  }

  parent->addChild (media);
  return 0;
}

// parse_link (doc, parent, tab)
static int
l_parse_link (lua_State *L)
{
  Document *doc;
  Context *parent;
  list<Action> conditions;
  list<Action> actions;

  doc = (Document *) lua_touserdata (L, 1);
  g_assert_nonnull (doc);
  parent = (Context *) lua_touserdata (L, 2);

  luaL_checktype (L, 3, LUA_TTABLE);

  lua_rawgeti (L, 3, 1); // condition table
  luaL_checktype (L, 4, LUA_TTABLE);
  lua_pushnil (L);
  while (lua_next (L, 4) != 0)
  {
    Action act;

    lua_rawgeti (L, 6, 1); // transition
    string transition = luaL_checkstring (L, -1);

    if (xstrcasecmp (transition, "set") == 0)
      transition = "start";

    act.transition = Event::getStringAsTransition (transition);

    lua_rawgeti (L, 6, 2); // event
    string event = luaL_checkstring (L, -1);
    act.event = getEventStringAsEvent (event, parent);

    lua_rawgeti (L, 6, 3); // predicate
    act.predicate = nullptr;
    if (!lua_isnil (L, -1))
    {
      luaL_checktype (L, -1, LUA_TTABLE);

      Predicate *predicate = new Predicate (Predicate::CONJUNCTION);
      lua_pushcfunction (L, l_parse_predicate);
      lua_pushlightuserdata (L, predicate);
      lua_pushvalue (L, -3);
      lua_call (L, 2, 0);

      Predicate *pred = predicate->getChildren ()->front ();
      act.predicate = pred;
    }

    conditions.push_back (act);
    lua_pop (L, 4);
  }

  lua_rawgeti (L, 3, 2); // action table
  luaL_checktype (L, 5, LUA_TTABLE);
  lua_pushnil (L);
  while (lua_next (L, 5) != 0)
  {
    Action act;

    lua_rawgeti (L, 7, 1); // transition
    string transition = luaL_checkstring (L, -1);

    if (xstrcasecmp (transition, "set") == 0)
      transition = "start";

    act.transition = Event::getStringAsTransition (transition);

    lua_rawgeti (L, 7, 2); // event
    string event = luaL_checkstring (L, -1);
    act.event = getEventStringAsEvent (event, parent);

    lua_rawgeti (L, 7, 3); // value
    if (!lua_isnil (L, -1))
      act.value = luaL_checkstring (L, -1);

    lua_rawgeti (L, 7, 4); // parameter list
    if (!lua_isnil (L, -1))
    {
      luaL_checktype (L, 11, LUA_TTABLE);
      lua_getfield (L, 11, "delay");
      if (!lua_isnil (L, -1))
      {
        string str = luaL_checkstring (L, -1);
        act.delay = str;
      }

      lua_getfield (L, 11, "duration");
      if (!lua_isnil (L, -1))
      {
        string str = luaL_checkstring (L, -1);
        act.duration = str;
      }

      lua_pop (L, 2);
    }

    act.predicate = nullptr;

    actions.push_back (act);
    lua_pop (L, 5);
  }

  parent->addLink (conditions, actions);
  return 0;
}

// parse_predicate (parent, tab)
static int
l_parse_predicate (lua_State *L)
{
  Predicate::Type type;
  Predicate::Test test;
  string str;
  Predicate *it;

  Predicate *parent = (Predicate *) lua_touserdata (L, 1);

  luaL_checktype (L, 2, LUA_TTABLE);
  lua_rawgeti (L, 2, 1);

  if (lua_isboolean (L, -1) == 1)
  {
    if (lua_toboolean (L, -1) == 1)
      type = Predicate::VERUM;
    else
      type = Predicate::FALSUM;
  }
  else
  {
    str = luaL_checkstring (L, -1);

    if (xstrcasecmp (str, "not") == 0)
      type = Predicate::NEGATION;
    else if (xstrcasecmp (str, "and") == 0)
      type = Predicate::CONJUNCTION;
    else if (xstrcasecmp (str, "or") == 0)
      type = Predicate::DISJUNCTION;
    else
      type = Predicate::ATOM;
  }

  it = new Predicate (type);

  switch (type)
  {
  case Predicate::ATOM:
    {
      string left = str;

      lua_rawgeti (L, 2, 2);
      str = luaL_checkstring (L, -1);

      lua_rawgeti (L, 2, 3);
      string right = luaL_checkstring (L, -1);

      if (xstrcasecmp (str, "==") == 0)
        test = Predicate::EQ;
      else if (xstrcasecmp (str, "!=") == 0)
        test = Predicate::NE;
      else if (xstrcasecmp (str, "<") == 0)
        test = Predicate::LT;
      else if (xstrcasecmp (str, "<=") == 0)
        test = Predicate::LE;
      else if (xstrcasecmp (str, ">") == 0)
        test = Predicate::GT;
      else if (xstrcasecmp (str, ">=") == 0)
        test = Predicate::GE;
      else
        g_assert_not_reached ();

      it->setTest (left, test, right);
      break;
    }
  case Predicate::FALSUM:
  case Predicate::VERUM:
    break;
  case Predicate::CONJUNCTION:
  case Predicate::DISJUNCTION:
    lua_pushcfunction (L, l_parse_predicate); // children
    lua_pushlightuserdata (L, it);
    lua_rawgeti (L, 2, 3);
    lua_call (L, 2, 0);
  case Predicate::NEGATION: // fall through
    lua_pushcfunction (L, l_parse_predicate);
    lua_pushlightuserdata (L, it);
    lua_rawgeti (L, 2, 2);
    lua_call (L, 2, 0);
    break;
  default:
    g_assert_not_reached ();
  }

  parent->addChild (it);
  return 0;
}

// External API.

/// Helper function used by Parser::parseBuffer() and Parser::parseFile().
static Document *
process (lua_State *L, const string &path, string *errmsg)
{
  Document *doc;

  doc = new Document ();
  lua_pushcfunction (L, l_parse_context);
  lua_pushlightuserdata (L, doc);
  lua_pushlightuserdata (L, nullptr);
  lua_pushvalue (L, 1);
  lua_pushstring (L, path.c_str ());
  if (unlikely (lua_pcall (L, 4, 0, 0) != LUA_OK))
  {
    delete doc;
    tryset (errmsg, g_strdup (luaL_checkstring (L, -1)));
    return nullptr;
  }

  return doc;
}

/**
 * @brief Parses NCL-ltab from memory buffer.
 * @fn ParserLua::parseBuffer
 * @param buf Buffer.
 * @param size Buffer size in bytes.
 * @param[out] errmsg Variable to store the error message (if any).
 * @return The resulting #Document if successful, or null otherwise.
 */
Document *
ParserLua::parseBuffer (const void *buf, size_t size, string *errmsg)
{
  lua_State *L;
  char *str;
  int err;
  Document *doc;
  string path = "";

  L = luaL_newstate ();
  g_assert_nonnull (L);
  luaL_openlibs (L);

  str = g_strndup ((const gchar *) buf, size);
  g_assert_nonnull (str);

  doc = nullptr;
  err = luaL_dostring (L, str);
  if (unlikely (err != LUA_OK))
  {
    tryset (errmsg, g_strdup (luaL_checkstring (L, -1)));
    goto done;
  }

  doc = process (L, path, errmsg);

 done:
  g_free (str);
  lua_close (L);
  return doc;
}

/**
 * @brief Parses NCL-ltab from Lua script.
 * @param path File path.
 * @param[out] errmsg Variable to store the error message (if any).
 * @return The resulting #Document if successful, or null otherwise.
 */
Document *
ParserLua::parseFile (const string &path, string *errmsg)
{
  lua_State *L;
  int err;
  Document *doc;

  L = luaL_newstate ();
  g_assert_nonnull (L);
  luaL_openlibs (L);

  doc = nullptr;
  err = luaL_dofile (L, path.c_str ());
  if (unlikely (err != LUA_OK))
  {
    tryset (errmsg, g_strdup (luaL_checkstring (L, -1)));
    goto done;
  }

  doc = process (L, path, errmsg);

 done:
  lua_close (L);
  return doc;
}

GINGA_NAMESPACE_END
