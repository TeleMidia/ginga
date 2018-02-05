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
  map<string, string> tab;

  g_assert (ginga::try_parse_table ("{a='x',b='y'}", NULL));
  g_assert (!ginga::try_parse_table ("a,b,c", NULL));
  g_assert (!ginga::try_parse_table ("{a=x}", NULL));

  tab = ginga::parse_table ("{a='1',  b='2' , c='3'  }");
  g_assert (tab.size () == 3);
  g_assert (tab["a"] == "1");
  g_assert (tab["b"] == "2");
  g_assert (tab["c"] == "3");

  tab = ginga::parse_table ("{}");
  g_assert (tab.size () == 0);

  tab = ginga::parse_table ("{a=' x '}");
  g_assert (tab.size () == 1);
  g_assert (tab["a"] == " x ");

  exit (EXIT_SUCCESS);
}
