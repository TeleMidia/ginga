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
  Media *m;

  m = new Media ("m");
  g_assert_nonnull (m);
  g_assert (m->getId () == "m");
  g_assert_null (m->getDocument ());
  g_assert_null (m->getParent ());
  g_assert (m->getObjectTypeAsString () == "Media");
  g_assert (m->toString () != "");
  g_assert (m->getAliases ()->size () == 0);
  g_assert (m->getEvents ()->size () == 1);
  g_assert_nonnull (m->getLambda ());
  g_assert_false (m->isOccurring ());
  g_assert_false (m->isPaused ());
  g_assert_true (m->isSleeping ());
  g_assert (m->getProperty ("zIndex") == "");
  g_assert (m->getDelayedActions ()->size () == 0);

  // Media-only.
  g_assert_false (m->isFocused ());
  g_assert_false (m->getZ (nullptr, nullptr));

  delete m;

  exit (EXIT_SUCCESS);
}
