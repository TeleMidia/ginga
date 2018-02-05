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
  Switch *s;

  s = new Switch ("s");
  g_assert_nonnull (s);
  g_assert (s->getId () == "s");
  g_assert_null (s->getDocument ());
  g_assert_null (s->getParent ());
  g_assert (s->getObjectTypeAsString () == "Switch");
  g_assert (s->toString () != "");
  g_assert (s->getAliases ()->size () == 0);
  g_assert (s->getEvents ()->size () == 1);
  g_assert_nonnull (s->getLambda ());
  g_assert_false (s->isOccurring ());
  g_assert_false (s->isPaused ());
  g_assert_true (s->isSleeping ());
  g_assert (s->getDelayedActions ()->size () == 0);

  // Composition-only.
  g_assert (s->getChildren ()->size () == 0);

  // Switch-only.
  g_assert (s->getRules ()->size () == 0);

  delete s;

  exit (EXIT_SUCCESS);
}
