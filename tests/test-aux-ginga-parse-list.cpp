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
  list<string> lst;

  g_assert (ginga::try_parse_list ("a,b,c", ',', 0, G_MAXSIZE, NULL));
  g_assert (!ginga::try_parse_list ("a,b,c", ',', 2, 2, NULL));
  g_assert (!ginga::try_parse_list ("a,b,c,d", ',', 1, 2, NULL));

  lst = ginga::parse_list ("a,b,c", ',', 0, 3);
  g_assert (lst.size () == 3);
  auto it = lst.begin ();
  g_assert (*it++ == "a");
  g_assert (*it++ == "b");
  g_assert (*it++ == "c");
  g_assert (it == lst.end ());

  lst = ginga::parse_list ("", ';', 0, 3);
  g_assert (lst.size () == 0);

  lst = ginga::parse_list ("x", ';', 0, 3);
  g_assert (lst.size () == 1);
  it = lst.begin ();
  g_assert (*it++ == "x");
  g_assert (it == lst.end ());

  lst = ginga::parse_list ("x;y;z", ';', 2, 5);
  g_assert (lst.size () == 3);
  it = lst.begin ();
  g_assert (*it++ == "x");
  g_assert (*it++ == "y");
  g_assert (*it++ == "z");
  g_assert (it == lst.end ());

  lst = ginga::parse_list (" aa , bb , cc , dd", ',', 0, 10);
  g_assert (lst.size () == 4);
  it = lst.begin ();
  g_assert (*it++ == "aa");
  g_assert (*it++ == "bb");
  g_assert (*it++ == "cc");
  g_assert (*it++ == "dd");
  g_assert (it == lst.end ());

  exit (EXIT_SUCCESS);
}
