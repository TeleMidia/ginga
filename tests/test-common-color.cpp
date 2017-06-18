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

#include "ginga.h"

#define CHECK_COLOR(c,_r,_g,_b,_a)              \
  g_assert ((c).r == (_r)                       \
            && (c).g == (_g)                    \
            && (c).b == (_b)                    \
            && (c).a == (_a))

int
main (void)
{
  SDL_Color c;

  // unknown color
  g_assert (!_ginga_parse_color ("*** unknown ***", &c));

  // black
  c = ginga_parse_color ("black");
  CHECK_COLOR (c, 0, 0, 0, 255);

  c = ginga_parse_color ("#0");
  CHECK_COLOR (c, 0, 0, 0, 255);

  c = ginga_parse_color ("#00");
  CHECK_COLOR (c, 0, 0, 0, 255);

  c = ginga_parse_color ("#000000");
  CHECK_COLOR (c, 0, 0, 0, 255);

  c = ginga_parse_color ("rgb(0,0,0)");
  CHECK_COLOR (c, 0, 0, 0, 255);

  c = ginga_parse_color (" rgb (0,0,0) ");
  CHECK_COLOR (c, 0, 0, 0, 255);

  c = ginga_parse_color (" rgb( 0 , 0 ,\t 0) ");
  CHECK_COLOR (c, 0, 0, 0, 255);

  c = ginga_parse_color ("rgba(0,0,0,127)");
  CHECK_COLOR (c, 0, 0, 0, 127);

  // white
  c = ginga_parse_color ("white");
  CHECK_COLOR (c, 255, 255, 255, 255);

  c = ginga_parse_color ("#ffffff");
  CHECK_COLOR (c, 255, 255, 255, 255);

  // red
  c = ginga_parse_color ("red");
  CHECK_COLOR (c, 255, 0, 0, 255);

  c = ginga_parse_color ("#ff0000");
  CHECK_COLOR (c, 255, 0, 0, 255);

  c = ginga_parse_color ("#ff000000");
  CHECK_COLOR (c, 255, 0, 0, 0);

  exit (0);
}