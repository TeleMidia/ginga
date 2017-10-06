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

GINGA_PLAYER_BEGIN

class LuaPlayer : public Player, public IGingaInternalEventListener
{
public:
  LuaPlayer (GingaInternal *, const string &, const string &);
  virtual ~LuaPlayer (void);
  void start (void) override;
  void stop (void) override;
  void pause (void) override;
  void resume (void) override;
  void redraw (cairo_t *) override;
  void redrawGL () override;

  // IGingaInternalEventListener.
  void handleTickEvent (GingaTime, GingaTime, int) override {};
  void handleKeyEvent (const string &, bool) override;

protected:
  virtual bool doSetProperty (PlayerProperty, const string &,
                              const string &) override;
private:
  ncluaw_t *_nw;                // the NCLua state
  GingaRect _init_rect;         // initial output rectangle
  string _pwd;                  // script's working dir
  string _saved_pwd;            // saved working dir

  void pwdSave (const string &);
  void pwdSave ();
  void pwdRestore ();
};

GINGA_PLAYER_END

#endif // LUAPLAYER_H
