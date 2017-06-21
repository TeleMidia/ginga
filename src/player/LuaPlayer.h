/* Copyright (C) 2006-2017 PUC-Rio/Laboratorio TeleMidia

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
along with Ginga.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef LUAPLAYER_H
#define LUAPLAYER_H

#include "Player.h"

#include "mb/Display.h"
#include "mb/IEventListener.h"
using namespace ::ginga::mb;

GINGA_PLAYER_BEGIN

class LuaPlayer : public Player, IEventListener
{
public:
  LuaPlayer (const string &mrl);
  virtual ~LuaPlayer (void);
  bool doPlay (void);
  void doStop (void);

  void abort (void) override;
  void pause (void) override;
  bool play (void) override;
  void resume (void) override;
  void stop (void) override;

  virtual void setCurrentScope (const string &scopeId) override;
  bool setKeyHandler (bool isHandler);
  virtual void setProperty (const string &, const string &) override;

  void redraw (SDL_Renderer *) override;

  // IEventListener.
  void handleTickEvent (GingaTime, GingaTime, int) override {};
  void handleKeyEvent (SDL_EventType, SDL_Keycode) override;

private:
  ncluaw_t *_nw;                // the NCLua state
  SDL_Rect _init_rect;          // initial output rectangle
  bool _isKeyHandler;           // true if player has the focus
  string _scope;                // the label of the active anchor
};

GINGA_PLAYER_END

#endif // LUAPLAYER_H
