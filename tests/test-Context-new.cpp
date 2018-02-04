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
  Context *c;

  c = new Context ("c");
  g_assert_nonnull (c);
  g_assert (c->getId () == "c");
  g_assert_null (c->getDocument ());
  g_assert_null (c->getParent ());
  g_assert (c->getObjectTypeAsString () == "Context");
  g_assert (c->toString () != "");
  g_assert (c->getAliases ()->size () == 0);
  g_assert (c->getEvents ()->size () == 1);
  g_assert_nonnull (c->getLambda ());
  g_assert_false (c->isOccurring ());
  g_assert_false (c->isPaused ());
  g_assert_true (c->isSleeping ());
  g_assert (c->getDelayedActions ()->size () == 0);

  // Composition-only.
  g_assert (c->getChildren ()->size () == 0);

  // Context-only.
  g_assert (c->getPorts ()->size () == 0);
  g_assert (c->getLinks ()->size () == 0);
  g_assert (c->getLinksStatus () == true);

  delete c;

  exit (EXIT_SUCCESS);
}
