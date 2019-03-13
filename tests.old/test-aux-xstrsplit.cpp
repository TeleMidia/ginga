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
  list<string> list;

  list.push_back ("");
  g_assert (xstrsplit (" ", ' ') == list);
  list.clear ();

  list.push_back ("hello world");
  g_assert (xstrsplit ("hello world", 'a') == list);
  list.clear ();

  list.push_back ("hello");
  list.push_back ("world");
  g_assert (xstrsplit ("hello world", ' ') == list);
  list.clear ();

  list.push_back ("hell");
  list.push_back ("o");
  list.push_back ("world");
  g_assert (xstrsplit ("hell|o|world", '|') == list);
  list.clear ();

  list.push_back ("h");
  list.push_back ("llo world");
  g_assert (xstrsplit ("hello world", 'e') == list);
  list.clear ();

  list.push_back ("he");
  list.push_back ("");
  list.push_back ("o wor");
  list.push_back ("d");
  g_assert (xstrsplit ("hello world", 'l') == list);
  list.clear ();

  exit (EXIT_SUCCESS);
}
