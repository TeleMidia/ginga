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
#include "Context.h"
#include "LuaAPI.h"

#include "Document.h"

GINGA_NAMESPACE_BEGIN

void
Context::getPorts (list<Event *> *ports)
{
  lua_Integer len;
  lua_Integer i;

  g_return_if_fail (ports != NULL);

  LuaAPI::Context_call (_L, this, "getPorts", 0, 1);
  g_assert (lua_type (_L, -1) == LUA_TTABLE);

  len = luaL_len (_L, -1);
  for (i = 1; i <= len; i++)
    {
      const char *qualId;
      Event *evt;

      lua_rawgeti (_L, -1, i);
      qualId = luaL_checkstring (_L, -1);
      evt = this->getDocument ()->getEvent (qualId);
      if (evt != NULL)
        ports->push_back (evt);
      lua_pop (_L, 1);
    }
}

bool
Context::addPort (Event *event)
{
  bool status;

  g_return_val_if_fail (event != NULL, false);

  LuaAPI::Event_push (_L, event);
  LuaAPI::Context_call (_L, this, "addPort", 1, 1);
  status = lua_toboolean (_L, -1);
  lua_pop (_L, 1);

  return status;
}

bool
Context::removePort (Event *event)
{
  bool status;

  g_return_val_if_fail (event != NULL, false);

  LuaAPI::Event_push (_L, event);
  LuaAPI::Context_call (_L, this, "removePort", 1, 1);
  status = lua_toboolean (_L, -1);
  lua_pop (_L, 1);

  return status;
}

// TODO --------------------------------------------------------------------

Context::Context (Document *doc, const string &id) : Composition (doc, id)
{
  LuaAPI::Object_attachWrapper (_L, this, doc, Object::CONTEXT, id);
}

Context::~Context ()
{
  for (auto link: _links)
    {
      for (auto &cond : link.first)
        if (cond.predicate != NULL)
          delete cond.predicate;
      for (auto &act : link.second)
        if (act.predicate != NULL)
          delete act.predicate;
    }

  LuaAPI::Object_detachWrapper (_L, this);
}

const list<pair<list<Action>, list<Action> > > *
Context::getLinks ()
{
  return &_links;
}

void
Context::addLink (list<Action> conds, list<Action> acts)
{
  g_assert (conds.size () > 0);
  g_assert (acts.size () > 0);
  _links.push_back (std::make_pair (conds, acts));
}

GINGA_NAMESPACE_END
