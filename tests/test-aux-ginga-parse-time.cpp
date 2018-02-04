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
  g_assert (!ginga::try_parse_time ("*** invalid ***", NULL));
  g_assert (ginga::parse_time ("0") == 0 * GINGA_SECOND);
  g_assert (ginga::parse_time ("1s") == 1 * GINGA_SECOND);
  g_assert (ginga::parse_time ("00:00:50") == 50 * GINGA_SECOND);
  g_assert (ginga::parse_time ("00:01:00") == 60 * GINGA_SECOND);
  g_assert (ginga::parse_time ("01:00:00") == 3600 * GINGA_SECOND);
  exit (EXIT_SUCCESS);
}
