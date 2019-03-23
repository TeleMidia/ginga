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

#include "LuaAPI.h"
#include "Document.h"

extern "C" {
int luaopen_ginga (lua_State *L);
}

static int
l_createDocument (lua_State *L)
{
  LuaAPI::Document_push (L, new Document (L));
  return 1;
}

int
luaopen_ginga (lua_State *L)
{
  lua_newtable (L);
  lua_pushcfunction (L, l_createDocument);
  lua_setfield (L, -2, "createDocument");

  return 1;
}
