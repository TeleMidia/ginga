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
  string path;
  Document *doc;
  Formatter *fmt;
  string errmsg = "";

  path = xpathbuildabs (
      ABS_TOP_SRCDIR, "tests-ncl/test-player-siggen-bit-13freq-100hz.ncl");

  fmt = new Formatter (nullptr);
  g_assert_nonnull (fmt);
  fmt->start (path, &errmsg);
  doc = fmt->getDocument ();
  g_assert_nonnull (doc);

  char dataToSend [] = { 1, 2, 3, 4, 5, 6};
  fmt->sendTick (4 * GINGA_SECOND, 4 * GINGA_SECOND, 0);

  Context *body = cast (Context *, doc->getRoot ());
  g_assert_nonnull (body);

  Media *m1 = cast (Media *, body->getChildById ("m"));
  g_assert_nonnull (m1);

  m1->setProperty ("volume", "0");

  while (true)
    ;
}