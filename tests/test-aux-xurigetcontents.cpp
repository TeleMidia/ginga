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

#define AUTHORS_FILE_CONTENT \
  "PUC-Rio/TeleMidia Lab.    www.telemidia.puc-rio.br\n"
#define AUTHORS_FILE_REMOTE_URI \
  "http://raw.githubusercontent.com/TeleMidia/ginga/master/AUTHORS"
#define AUTHORS_FILE_LOCAL_RELATIVE_PATH \
  TOP_SRCDIR "/AUTHORS"

int
main (void)
{
  // Unsuccessful tests --------------------------------------------------------

  // invalid uri.
  {
    // It must return false and not change the contents string.
    string empty  = "empty";
    g_assert (xurigetcontents ("unknown", empty) == false);
    g_assert (empty == "empty");  // doesn't change the string passed as param.
  }

  // relative path. (also invalid uri.)
  {
    // It must return false and not change the contents string.
    string empty = "empty";
    g_assert (xurigetcontents (
                AUTHORS_FILE_LOCAL_RELATIVE_PATH, empty) == false);
    g_assert (empty == "empty");  // doesn't change the string passed as param.
  }

  // valid fullpath. (also invalid uri.)
  {
    // It must return false and not change the contents string.
    string empty = "empty";
    string abs = xpathmakeabs (AUTHORS_FILE_LOCAL_RELATIVE_PATH);
    g_assert (xurigetcontents (abs, empty) == false);
    g_assert (empty == "empty");
  }

  // Successful tests ----------------------------------------------------------

  // local file:// complete uri.
  {
    // It must return true and work just fine.
    GError *err = nullptr;
    string content = "";
    string abs = xpathmakeabs (AUTHORS_FILE_LOCAL_RELATIVE_PATH);
    gchar *uri_local = g_filename_to_uri (abs.c_str (), NULL, &err);
    g_assert_null (err);

    g_assert (xurigetcontents (uri_local, content));
    g_assert (content == AUTHORS_FILE_CONTENT);

    g_free (uri_local);
  }

  // remote http:// uri.
  {
    // It must return true and work just fine.
    string content = "";
    g_assert (xurigetcontents (AUTHORS_FILE_REMOTE_URI, content));
    g_assert (content == AUTHORS_FILE_CONTENT);
  }

  exit (EXIT_SUCCESS);
}
