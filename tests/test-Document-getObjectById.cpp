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

#define N 1024

int
main (void)
{
  Document *doc;
  Media *m[N];

  doc = new Document ();
  g_assert_nonnull (doc);
  g_assert (doc->getObjects ()->size () == 2);

  Context *root = doc->getRoot ();
  g_assert_nonnull (root);
  g_assert (doc->getObjectById ("__root__") == root);

  MediaSettings *settings = doc->getSettings ();
  g_assert_nonnull (settings);
  g_assert (doc->getObjectById ("__settings__") == settings);

  for (size_t i = 0; i < N; i++)
    {
      m[i] = new Media (xstrbuild ("m%d", (int) i));
      doc->addObject (m[i]);
    }
  g_assert (doc->getObjects ()->size () == N + 2);

  for (size_t i = 0; i < N; i++)
    {
      string id = xstrbuild ("m%d", (int) i);
      g_assert (doc->getObjectById (id) == m[i]);
    }

  g_assert_null (doc->getObjectById ("a"));
  g_assert_null (doc->getObjectById ("b"));
  g_assert_null (doc->getObjectById ("c"));

  delete doc;

  exit (EXIT_SUCCESS);
}
