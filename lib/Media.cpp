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
#include "Event.h"
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

// Private.

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

// TODO --------------------------------------------------------------------

void
Media::sendKey (const string &key, bool press)
{
  list<Event *> buf;

  if (_getPlayer () == NULL)
    return;

  // if (press && xstrhasprefix (key, "CURSOR_") && this->isFocused ())
  //   {
  //     string next;
  //
  //     if ((key == "CURSOR_UP"
  //          && (next = this->getProperty ("moveUp")) != "")
  //         || ((key == "CURSOR_DOWN"
  //              && (next = this->getProperty ("moveDown")) != ""))
  //         || ((key == "CURSOR_LEFT"
  //              && (next = this->getProperty ("moveLeft")) != ""))
  //         || ((key == "CURSOR_RIGHT"
  //              && (next = this->getProperty ("moveRight")) != "")))
  //       {
  //         this->getDocument ()->getSettings ()
  //           ->setPropertyString ("_nextFocus", next);
  //       }
  //   }

  // Pass key to player.
  // if (this->isFocused ())
  //   _getPlayer ()->sendKeyEvent (key, press);

  set<Event *> events;
  this->getEvents (&events);

  // Collect the events to be triggered.
  for (auto evt: events)
    {
      if (evt->getType () != Event::SELECTION)
        continue;

      string expected = evt->getId ();
      if (expected[0] == '$')
        expected = ""; // A param could not be resolved.  Should we generate
                       // an error?

      if (!((expected == "" && key == "ENTER" && this->isFocused ())
            || (expected != "" && key == expected)))
        {
          continue;
        }

      buf.push_back (evt);
    }

  // Run collected events.
  for (Event *evt : buf)
    this->getDocument ()
      ->evalAction (evt, press ? Event::START : Event::STOP);
}

GINGA_NAMESPACE_END
