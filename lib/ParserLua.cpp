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

// Parsing functions.

// parse_context (doc, [parent], t)
static int
l_parse_context (lua_State *L)
{
  Document *doc;
  Composition *parent;
  const char *tag;
  const char *id;

  doc = (Document *) lua_touserdata (L, 1);
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
      root->addAlias (string (id));
    }
  else // non-root
    {
      g_assert_not_reached ();
    }

  return 0;
}

// External API.

/// Helper function used by Parser::parseBuffer() and Parser::parseFile().
static Document *
process (lua_State *L, string *errmsg)
{
  Document *doc;

  doc = new Document ();
  lua_pushcfunction (L, l_parse_context);
  lua_pushlightuserdata (L, doc);
  lua_pushlightuserdata (L, nullptr);
  lua_pushvalue (L, 1);
  if (unlikely (lua_pcall (L, 3, 0, 0) != LUA_OK))
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
 * @return The resulting #Document if successful, or \c nullptr otherwise.
 */
Document *
ParserLua::parseBuffer (const void *buf, size_t size, string *errmsg)
{
  lua_State *L;
  char *str;
  int err;
  Document *doc;

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

  doc = process (L, errmsg);

done:
  g_free (str);
  lua_close (L);
  return doc;
}

/**
 * @brief Parses NCL-ltab from Lua script.
 * @param path File path.
 * @param[out] errmsg Variable to store the error message (if any).
 * @return The resulting #Document if successful, or \c nullptr otherwise.
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

  doc = process (L, errmsg);

done:
  lua_close (L);
  return doc;
}

GINGA_NAMESPACE_END
