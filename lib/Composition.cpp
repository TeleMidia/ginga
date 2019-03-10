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
#include "Composition.h"
#include "LuaAPI.h"

#include "Document.h"

GINGA_NAMESPACE_BEGIN

Composition::Composition (Document *doc, const string &id)
  : Object (doc, id)
{
}

Composition::~Composition ()
{
}

void
Composition::getChildren (set<Object *> *children)
{
  lua_Integer len;
  lua_Integer i;

  g_return_if_fail (children != NULL);

  LuaAPI::Composition_call (_L, this, "getChildren", 0, 1);
  g_assert (lua_type (_L, -1) == LUA_TTABLE);

  len = luaL_len (_L, -1);
  for (i = 1; i <= len; i++)
    {
      Object *obj;

      lua_rawgeti (_L, -1, i);
      obj = LuaAPI::Object_check (_L, -1);
      children->insert (obj);
      lua_pop (_L, 1);
    }
}

Object *
Composition::getChild (const string &id)
{
  Object *obj = NULL;

  lua_pushstring (_L, id.c_str ());
  LuaAPI::Composition_call (_L, this, "getChild", 1, 1);
  if (!lua_isnil (_L, -1))
    {
      obj = LuaAPI::Object_check (_L, -1);
    }
  lua_pop (_L, 1);

  return obj;
}

void
Composition::addChild (Object *child)
{
  g_return_if_fail (child != NULL);

  LuaAPI::Object_push (_L, child);
  LuaAPI::Composition_call (_L, this, "addChild", 1, 0);
}

void
Composition::removeChild (Object *child)
{
  g_return_if_fail (child != NULL);

  LuaAPI::Object_push (_L, child);
  LuaAPI::Composition_call (_L, this, "removeChild", 1, 0);
}

GINGA_NAMESPACE_END
