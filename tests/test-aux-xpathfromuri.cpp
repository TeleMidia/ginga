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

static string
xbuild_filename (const string &s)
{
  gchar *gfilename = g_build_filename (s.c_str (), NULL);
  string filename = gfilename;
  g_free (gfilename);
  return filename;
}

int
main (void)
{
  g_assert (xpathfromuri ("file:/a") ==  xbuild_filename ("/a"));
  g_assert (xpathfromuri ("file:/full/path") ==
            xbuild_filename ("/full/path") );
  g_assert (xpathfromuri ("file:/base/relative/path") ==
            xbuild_filename ("/base/relative/path") );

  exit (EXIT_SUCCESS);
}
