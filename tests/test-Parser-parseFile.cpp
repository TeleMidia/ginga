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
using namespace ::ginga;

int
main (void)
{
  string errmsg;
  GDir *dir;
  string path;
  const gchar *entry;

  // Check bad path.
  g_assert_null (Parser::parseFile ("nonexistent", 100, 100, nullptr));

  // Sanity checks.
  path = xpathbuildabs (ABS_TOP_SRCDIR, "tests-ncl");
  dir = g_dir_open (path.c_str (), 0, nullptr);
  g_assert_nonnull (dir);

  while ((entry = g_dir_read_name (dir)) != nullptr)
    {
      string entry_path;
      Document *doc;
      string errmsg;

      entry_path = xpathbuildabs (path, string (entry));
      if (!xstrhassuffix (entry_path, ".ncl"))
        continue;

      errmsg = "";
      doc = Parser::parseFile (entry_path, 100, 100, &errmsg);
      if (doc == nullptr)
        {
          g_printerr ("%s: %s\n", entry, errmsg.c_str ());
          g_assert_not_reached ();
        }
      else
        {
          g_assert (errmsg == "");
        }
      delete doc;
    }

  g_dir_close (dir);
  exit (EXIT_SUCCESS);
}
