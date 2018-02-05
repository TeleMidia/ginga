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

#ifndef PLAYER_TEXT_H
#define PLAYER_TEXT_H

#include "Player.h"

GINGA_NAMESPACE_BEGIN

class PlayerText : public Player
{
public:
  static cairo_surface_t *renderSurface (const string &, const string &,
                                         const string &, const string &,
                                         const string &, Color, Color, Rect,
                                         const string &, const string &,
                                         bool, Rect *);

  PlayerText (Formatter *, Media *);
  ~PlayerText ();
  void reload () override;

protected:
  bool doSetProperty (Property, const string &, const string &) override;

private:
  struct
  {
    Color fontColor;
    Color fontBgColor;
    string fontFamily;
    string fontSize;
    string fontStyle;
    string fontVariant;
    string fontWeight;
    string horzAlign;
    string vertAlign;
  } _prop;
};

GINGA_NAMESPACE_END

#endif // PLAYER_TEXT
