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

#ifndef SVG_PLAYER_H
#define SVG_PLAYER_H

#include "Player.h"
#include "mb/Display.h"
using namespace ginga::mb;

GINGA_PLAYER_BEGIN

class SvgPlayer : public Player
{
public:
  SvgPlayer (const string &uri) : Player (uri) {}
  ~SvgPlayer (void) {}
  void setProperty (const string &, const string &) override;
  void redraw (SDL_Renderer *) override;

private:
  bool dirty = true;            // true if texture should be reloaded
  void reload (SDL_Renderer *);
};

GINGA_PLAYER_END

#endif // SVG_PLAYER_H
