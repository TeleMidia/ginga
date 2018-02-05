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

  // Presentation.
  {
    Event *e = new Event (Event::PRESENTATION, m, "pres");
    g_assert_nonnull (e);
    g_assert (e->getType () == Event::PRESENTATION);
    g_assert (e->getObject () == m);
    g_assert (e->getId () == "pres");
    g_assert (e->getFullId () == "m@pres");
    g_assert (e->getState () == Event::SLEEPING);
    g_assert (e->getLabel () == "");
    g_assert (!e->hasLabel ());
    g_assert_false (e->isLambda ());
    TRACE ("\n%s", e->toString ().c_str ());
    delete e;
  }

  // Attribution.
  {
    Event *e = new Event (Event::ATTRIBUTION, m, "attr");
    g_assert_nonnull (e);
    g_assert (e->getType () == Event::ATTRIBUTION);
    g_assert (e->getObject () == m);
    g_assert (e->getId () == "attr");
    g_assert (e->getFullId () == "m.attr");
    g_assert (e->getState () == Event::SLEEPING);
    g_assert_false (e->isLambda ());
    TRACE ("\n%s", e->toString ().c_str ());
    delete e;
  }

  // Selection.
  {
    Event *e = new Event (Event::SELECTION, m, "sel");
    g_assert_nonnull (e);
    g_assert (e->getType () == Event::SELECTION);
    g_assert (e->getObject () == m);
    g_assert (e->getId () == "sel");
    g_assert (e->getFullId () == "m<sel>");
    g_assert (e->getState () == Event::SLEEPING);
    g_assert_false (e->isLambda ());
    TRACE ("\n%s", e->toString ().c_str ());
    delete e;
  }

  delete m;

  exit (EXIT_SUCCESS);
}
