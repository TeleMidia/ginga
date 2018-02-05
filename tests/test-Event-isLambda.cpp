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

  Event *e = m->getPresentationEvent ("@lambda");
  g_assert_nonnull (e);

  m->addPresentationEvent ("pres", 0, 0);
  Event *e1 = m->getPresentationEvent ("pres");
  g_assert_nonnull (e1);

  m->addAttributionEvent ("attr");
  Event *e2 = m->getAttributionEvent ("attr");
  g_assert_nonnull (e2);

  m->addSelectionEvent ("sel");
  Event *e3 = m->getSelectionEvent ("sel");
  g_assert_nonnull (e3);

  g_assert (e->isLambda ());
  g_assert_false (e1->isLambda ());
  g_assert_false (e2->isLambda ());
  g_assert_false (e3->isLambda ());

  delete m;

  exit (EXIT_SUCCESS);
}
