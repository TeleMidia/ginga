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
  Ginga *ginga;
  string tmp, errmsg;

  ginga = Ginga::create (nullptr);
  g_assert_nonnull (ginga);
  ginga->setOptionInt ("width", 800);
  g_assert (ginga->getOptionInt ("width") == 800);
  ginga->setOptionInt ("height", 600);
  g_assert (ginga->getOptionInt ("height") == 600);
  tmp = tests_write_tmp_file ("<ncl/>");
  g_assert_true (ginga->start (tmp, &errmsg));
  ginga->resize (640, 480);
  g_assert (ginga->getOptionInt ("width") == 640);
  g_assert (ginga->getOptionInt ("height") == 480);
  g_remove (tmp.c_str ());

  exit (EXIT_SUCCESS);
}
