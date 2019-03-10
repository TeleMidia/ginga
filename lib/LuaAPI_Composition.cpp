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
#include "Composition.h"

#include "Context.h"
#include "Switch.h"

Composition *
LuaAPI::Composition_check (lua_State *L, int i)
{
  g_return_val_if_fail (L != NULL, NULL);

  if (luaL_testudata (L, i, LuaAPI::_CONTEXT))
    {
      Context *ctx = LuaAPI::Context_check (L, i);
      return cast (Composition *, ctx);
    }

  if (luaL_testudata (L, i, LuaAPI::_SWITCH))
    {
      Switch *swtch = LuaAPI::Switch_check (L, i);
      return cast (Composition *, swtch);
    }

  luaL_argerror (L, 3, "Ginga.Context or Ginga.Switch expected");
  return NULL;
}

void
LuaAPI::Composition_call (lua_State *L, Composition *comp,
                          const char *name, int nargs, int nresults)
{
  g_return_if_fail (L != NULL);
  g_return_if_fail (comp != NULL);
  g_return_if_fail (name != NULL);
  g_return_if_fail (nargs >= 0);
  g_return_if_fail (nresults >= 0);

  LuaAPI::_callLuaWrapper (L, comp, name, nargs, nresults);
}
