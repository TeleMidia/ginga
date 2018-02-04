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

int
main (void)
{
  g_assert (xstrcasecmp ("", "") == 0);
  g_assert (xstrcasecmp ("testing", "TESTING") == 0);
  g_assert (xstrcasecmp ("test", "TeStING") < 0);
  g_assert (xstrcasecmp ("testing", "test") > 0);
  g_assert (xstrcasecmp ("", "test") < 0);
  g_assert (xstrcasecmp ("test ing", "test") > 0);
  exit (EXIT_SUCCESS);
}
