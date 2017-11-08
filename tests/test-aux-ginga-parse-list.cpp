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

#include "aux-ginga.h"

int
main (void)
{
  vector<string> v;

  g_assert (ginga::try_parse_list ("a,b,c", ',', 0, G_MAXSIZE, NULL));
  g_assert (!ginga::try_parse_list ("a,b,c", ',', 2, 2, NULL));
  g_assert (!ginga::try_parse_list ("a,b,c,d", ',', 1, 2, NULL));

  v = ginga::parse_list ("a,b,c", ',', 0, 3);
  g_assert (v.size () == 3);
  g_assert (v[0] == "a");
  g_assert (v[1] == "b");
  g_assert (v[2] == "c");

  v = ginga::parse_list ("", ';', 0, 3);
  g_assert (v.size () == 0);

  v = ginga::parse_list ("x", ';', 0, 3);
  g_assert (v.size () == 1);
  g_assert (v[0] == "x");

  v = ginga::parse_list ("x;y;z", ';', 2, 5);
  g_assert (v.size () == 3);
  g_assert (v[0] == "x");
  g_assert (v[1] == "y");
  g_assert (v[2] == "z");

  v = ginga::parse_list (" aa , bb , cc , dd", ',', 0, 10);
  g_assert (v.size () == 4);
  g_assert (v[0] == "aa");
  g_assert (v[1] == "bb");
  g_assert (v[2] == "cc");
  g_assert (v[3] == "dd");

  exit (EXIT_SUCCESS);
}
