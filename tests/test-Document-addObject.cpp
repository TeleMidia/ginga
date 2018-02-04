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

  Context *root = doc->getRoot ();
  g_assert_nonnull (root);
  g_assert_false (doc->addObject (root));
  g_assert (doc->getObjects ()->size () == 2);

  MediaSettings *settings = doc->getSettings ();
  g_assert_nonnull (settings);
  g_assert_false (doc->addObject (settings));
  g_assert (doc->getObjects ()->size () == 2);

  // Add media.
  {
    Media *m = new Media ("m");
    g_assert (doc->addObject (m));
    g_assert (doc->getObjects ()->size () == 3);
    g_assert (doc->getMedias ()->size () == 2);
    auto it = doc->getMedias ()->find (m);
    g_assert (it != doc->getMedias ()->end ());
  }

  // Add context.
  {
    Context *c = new Context ("c");
    g_assert (doc->addObject (c));
    g_assert (doc->getObjects ()->size () == 4);
    g_assert (doc->getContexts ()->size () == 2);
    auto it = doc->getContexts ()->find (c);
    g_assert (it != doc->getContexts ()->end ());
  }

  // Add switch.
  {
    Switch *s = new Switch ("s");
    g_assert (doc->addObject (s));
    g_assert (doc->getObjects ()->size () == 5);
    g_assert (doc->getSwitches ()->size () == 1);
    auto it = doc->getSwitches ()->find (s);
    g_assert (it != doc->getSwitches ()->end ());
  }

  delete doc;

  exit (EXIT_SUCCESS);
}
