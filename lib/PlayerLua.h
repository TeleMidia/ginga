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

#ifndef PLAYER_LUA_H
#define PLAYER_LUA_H

#include "Player.h"

#if defined WITH_NCLUA && WITH_NCLUA
#include <ncluaw.h>
#endif

GINGA_NAMESPACE_BEGIN

class PlayerLua : public Player
{
public:
  PlayerLua (Formatter *, Media *);
  ~PlayerLua ();
  void start () override;
  void stop () override;
  void pause () override;
  void resume () override;
  void redraw (cairo_t *) override;
  void sendKeyEvent (const string &, bool) override;
  void sendPresentationEvent (const string &, const string &) override;

protected:
  virtual bool doSetProperty (Property, const string &,
                              const string &) override;

private:
  ncluaw_t *_nw;     // the NCLua state
  Rect _init_rect;   // initial output rectangle
  string _pwd;       // script's working dir
  string _saved_pwd; // saved working dir

  void pwdSave (const string &);
  void pwdSave ();
  void pwdRestore ();
};

GINGA_NAMESPACE_END

#endif // PLAYER_LUA_H
