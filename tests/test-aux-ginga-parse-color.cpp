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

#include "tests.h"

// clang-format off
GINGA_PRAGMA_DIAG_IGNORE (-Wfloat-equal)
// clang-format on

#define CHECK_COLOR(c, _r, _g, _b, _a)                                     \
  g_assert ((c).red * 255 == (_r) && (c).green * 255 == (_g)               \
            && (c).blue * 255 == (_b) && (c).alpha * 255 == (_a))

int
main (void)
{
  Color c;

  // unknown color
  g_assert (!ginga::try_parse_color ("*** unknown ***", &c));

  // black
  c = ginga::parse_color ("black");
  CHECK_COLOR (c, 0, 0, 0, 255);

  c = ginga::parse_color ("#000000");
  CHECK_COLOR (c, 0, 0, 0, 255);

  c = ginga::parse_color ("rgb(0,0,0)");
  CHECK_COLOR (c, 0, 0, 0, 255);

  c = ginga::parse_color ("rgb(0%,0%,0%)");
  CHECK_COLOR (c, 0, 0, 0, 255);

  // c = ginga_parse_color ("rgba(0,0,0,.5)");
  // CHECK_COLOR (c, 0, 0, 0, 128);

  // c = ginga_parse_color ("rgba(0,0,0,50%)");
  // CHECK_COLOR (c, 0, 0, 0, 128);

  // white
  c = ginga::parse_color ("white");
  CHECK_COLOR (c, 255, 255, 255, 255);

  c = ginga::parse_color ("#ffffff");
  CHECK_COLOR (c, 255, 255, 255, 255);

  // red
  c = ginga::parse_color ("red");
  CHECK_COLOR (c, 255, 0, 0, 255);

  c = ginga::parse_color ("#ff0000");
  CHECK_COLOR (c, 255, 0, 0, 255);

  exit (EXIT_SUCCESS);
}
