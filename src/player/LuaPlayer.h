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
#include "mb/InputManager.h"
using namespace ::ginga::mb;

GINGA_PLAYER_BEGIN

class LuaPlayer : public Player, public IInputEventListener
{
private:
  GINGA_MUTEX_DEFN ();
  GINGA_COND_DEFN (DisplayJob);

  ncluaw_t *nw;          // the NCLua state
  bool hasExecuted;      // true if script was executed
  bool isKeyHandler;     // true if player has the focus
  string scope;          // the label of the active anchor
  InputManager *im;

  static bool displayJobCallbackWrapper (DisplayJob *,
                                         SDL_Renderer *, void *);
  bool displayJobCallback (DisplayJob *, SDL_Renderer *);

public:
  LuaPlayer (const string &mrl);
  virtual ~LuaPlayer (void);
  bool doPlay (void);
  void doStop (void);

  // Player interface.
  void abort (void);
  void pause (void);
  bool play (void);
  void resume (void);
  void stop (void);
  virtual bool hasPresented (void);
  void setCurrentScope (const string &scopeId);
  bool setKeyHandler (bool isHandler);
  virtual void setPropertyValue (const string &name, const string &value);

  // Input event callback.
  bool userEventReceived (InputEvent *evt);
};

GINGA_PLAYER_END

#endif // LUAPLAYER_H
