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
#include "Document.h"
#include "Media.h"
#include "MediaSettings.h"
#include "Switch.h"

GINGA_BEGIN_DECLS
#include "aux-lua.h"
GINGA_END_DECLS

GINGA_NAMESPACE_BEGIN

// Helper function
Event *
getEventStringAsEvent (string str, Context *parent)
{
  Object *obj;
  string id, evt;

  size_t at = str.find ('@');

  if (at != str.npos)           // presentation
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
  else                          // selection
    {
      printf("Selection Event\n");           // to be done
    }
}


// Parsing functions.

// parse_port (doc, parent, port)
static int
l_parse_port (lua_State *L)
{
  Object *obj;
  Event *event;
  Document *doc;
  Context *parent;
  string str;

  doc = (Document *) lua_touserdata (L, 1);
  parent = (Context *) lua_touserdata (L, 2);
  g_assert_nonnull (parent);
  str = string (luaL_checkstring (L, -1));

  event = getEventStringAsEvent (str, parent);
  g_assert_nonnull (event);
  parent->addPort (event);

  return 0;
}

// label not implemented for area list
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
  string src;
  Time begin, end;

  doc = (Document *) lua_touserdata (L, 1);
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
          name = lua_tolstring (L, -2, 0);

          lua_rawgeti (L, -1, 1);
          value = lua_tolstring (L, -1, 0);

          begin = 0;
          if ((value != NULL)
              && (unlikely (!ginga::try_parse_time (value, &begin))))
            {
              lua_pushfstring (L, "bad attr: %s", value);
              lua_error (L);
            }

          lua_pop (L, 1);
          lua_rawgeti (L, -1, 2);
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

  if (parent == NULL) // error
    {
      lua_pushfstring (L, "parent missing: %s", id);
      lua_error (L);
    }
  else // add parent
    {
      parent->addChild (media);
    }

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

  lua_rawgeti (L, 3, 1);        // condition list
  luaL_checktype (L, 4, LUA_TTABLE);
  lua_pushnil (L);
  while (lua_next (L, 4) != 0)
    {
      lua_rawgeti (L, 6, 1);
      string transition = luaL_checkstring (L, -1);

      lua_rawgeti (L, 6, 2);
      string event = luaL_checkstring (L, -1);

      lua_rawgeti (L, 6, 3);
      string predicate;
      if (!lua_isnil (L, -1))
        predicate = luaL_checkstring (L, -1);

      Action act;
      act.event = getEventStringAsEvent (event, parent);

      if (xstrcasecmp (transition, "set") == 0)
        transition = "start";

      act.transition =  Event::getStringAsTransition (transition);
      act.predicate = NULL;     // predicate still missing

      conditions.push_back (act);
      lua_pop (L, 4);
    }

  lua_rawgeti (L, 3, 2);        // action list
  luaL_checktype (L, 5, LUA_TTABLE);
  lua_pushnil (L);
  while (lua_next (L, 5) != 0)
    {
      Action act;

      lua_rawgeti (L, 7, 1);
      string transition = luaL_checkstring (L, -1);

      lua_rawgeti (L, 7, 2);
      string event = luaL_checkstring (L, -1);

      lua_rawgeti (L, 7, 3);
      if (!lua_isnil (L, -1))
        act.value = luaL_checkstring (L, -1);

      act.event = getEventStringAsEvent (event, parent);

      if (xstrcasecmp (transition, "set") == 0)
        transition = "start";

      act.transition =  Event::getStringAsTransition (transition);

      actions.push_back (act);
      lua_pop (L, 4);
    }

  parent->addLink (conditions, actions);

  return 0;
}

// parse_context (doc, [parent], tab, path)
static int
l_parse_context (lua_State *L)
{
  Document *doc;
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

  if (parent == NULL) // root
    {
      Context *root = doc->getRoot ();
      root->addAlias (string (id));
      parent = root;
    }
  else // non-root
    {
      g_assert_not_reached ();
    }

  lua_rawgeti (L, 3, 4);
  if (lua_isnil (L, -1) == 0) // media table
    {
      lua_pushnil (L);
      while (lua_next (L, -2) != 0)
        {
          lua_pushcfunction (L, l_parse_media);
          lua_pushlightuserdata (L, doc);
          lua_pushlightuserdata (L, parent);
          lua_pushvalue (L, -4);
          lua_pushvalue (L, 4);
          lua_call (L, 4, 0);
          lua_pop (L, 1);
        }
    }

  lua_rawgeti (L, 3, 3);
  if (lua_isnil (L, -1) == 0) // port table
    {
      lua_pushnil (L);
      while (lua_next (L, -2) != 0)
        {
          lua_pushcfunction (L, l_parse_port);
          lua_pushlightuserdata (L, doc);
          lua_pushlightuserdata (L, parent);
          lua_pushvalue (L, -4);
          lua_call (L, 3, 0);
          lua_pop (L, 1);
        }
    }

  lua_rawgeti (L, 3, 5);
  if (lua_isnil (L, -1) == 0) // link table
    {
      lua_pushnil (L);
      while (lua_next (L, -2) != 0)
        {
          lua_pushcfunction (L, l_parse_link);
          lua_pushlightuserdata (L, doc);
          lua_pushlightuserdata (L, parent);
          lua_pushvalue (L, -4);
          lua_call (L, 3, 0);
          lua_pop (L, 1);
        }
    }

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
  lua_pushlightuserdata (L, NULL);
  lua_pushvalue (L, 1);
  lua_pushstring (L, path.c_str ());
  if (unlikely (lua_pcall (L, 4, 0, 0) != LUA_OK))
    {
      delete doc;
      tryset (errmsg, g_strdup (luaL_checkstring (L, -1)));
      return NULL;
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

  doc = NULL;
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

  doc = NULL;
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
