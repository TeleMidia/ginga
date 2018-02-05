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
  Document *doc;

  doc = new Document ();
  g_assert_nonnull (doc);
  g_assert (doc->getObjects ()->size () == 2);
  g_assert (doc->getMedias ()->size () == 1);
  g_assert (doc->getContexts ()->size () == 1);
  g_assert (doc->getSwitches ()->size () == 0);

  Context *root = doc->getRoot ();
  g_assert_nonnull (root);
  g_assert (root->getId () == "__root__");

  MediaSettings *settings = doc->getSettings ();
  g_assert_nonnull (settings);
  g_assert (settings->getId () == "__settings__");

  delete doc;

  exit (EXIT_SUCCESS);
}
