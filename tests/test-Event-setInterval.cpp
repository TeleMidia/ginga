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

#define CHECK_INTERVAL(e, exp_begin, exp_end)                              \
  G_STMT_START                                                             \
  {                                                                        \
    Time begin, end;                                                       \
    (e)->getInterval (&begin, &end);                                       \
    g_assert (begin == (exp_begin));                                       \
    g_assert (end == (exp_end));                                           \
  }                                                                        \
  G_STMT_END

int
main (void)
{
  Media *m;

  m = new Media ("m");
  g_assert_nonnull (m);

  m->addPresentationEvent ("pres", 0, 0);
  Event *e = m->getPresentationEvent ("pres");
  g_assert_nonnull (e);
  CHECK_INTERVAL (e, 0, 0);

  e->setInterval (10, 600);
  CHECK_INTERVAL (e, 10, 600);

  e->setInterval (GINGA_TIME_NONE, 0);
  CHECK_INTERVAL (e, GINGA_TIME_NONE, 0);

  e->setInterval (GINGA_TIME_NONE, GINGA_TIME_NONE);
  CHECK_INTERVAL (e, GINGA_TIME_NONE, GINGA_TIME_NONE);

  delete m;

  exit (EXIT_SUCCESS);
}
