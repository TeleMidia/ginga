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
#include "Media.h"
#include "LuaAPI.h"

#include "Document.h"
#include "Context.h"
#include "Switch.h"
#include "StateMachine.h"
#include "Player.h"

GINGA_NAMESPACE_BEGIN

Media::Media (Document *doc, const string &id) : Object (doc, id)
{
  LuaAPI::Object_attachWrapper (_L, this, doc, Object::MEDIA, id);
}

Media::~Media ()
{
  Player *player;

  player = _getPlayer ();
  if (player != NULL)
    delete player;

  LuaAPI::Object_detachWrapper (_L, this);
}

bool
Media::isFocused ()
{
  bool status;

  LuaAPI::Media_call (_L, this, "isFocused", 0, 1);
  status = lua_toboolean (_L, -1);
  lua_pop (_L, 1);

  return status;
}

Player *
Media::_getPlayer ()
{
  Player *player = NULL;

  LuaAPI::Media_call (_L, this, "_getPlayer", 0, 1);
  if (!lua_isnil (_L, -1))
    {
      player = LuaAPI::Player_check (_L, -1);
    }
  lua_pop (_L, 1);

  return player;
}

GINGA_NAMESPACE_END
