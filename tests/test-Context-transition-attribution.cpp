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
  {
    // ABORT prop from state OCCURRING
    // ABORT prop from state PAUSED
    // ABORT prop from state SLEEPING

    // PAUSE prop from state OCCURRING
    // PAUSE prop from state PAUSED
    // PAUSE prop from state SLEEPING

    // START prop from state OCCURRING
    // START prop from state PAUSED
    // START prop from state SLEEPING
    {
      Formatter *fmt;
      Event *body_lambda, *c1_lambda, *c1_prop, *m1_lambda, *m2_lambda;

      tests_create_document_with_context_and_start (
          &fmt, &body_lambda, &c1_lambda, &c1_prop, &m1_lambda, &m2_lambda);

      // START AttributionEvent is done and return true
      g_assert_true (c1_prop->transition (Event::START));

      // after START AttributionEvent is OCCURRING
      g_assert (body_lambda->getState () == Event::OCCURRING);
      g_assert (c1_lambda->getState () == Event::OCCURRING);
      g_assert (m1_lambda->getState () == Event::OCCURRING);
      g_assert (m2_lambda->getState () == Event::OCCURRING);
      g_assert (c1_prop->getState () == Event::OCCURRING);

      // when advance time AttributionEvent is SLEEPING
      fmt->sendTick (0, 0, 0);
      g_assert (body_lambda->getState () == Event::OCCURRING);
      g_assert (c1_lambda->getState () == Event::OCCURRING);
      g_assert (m1_lambda->getState () == Event::OCCURRING);
      g_assert (m2_lambda->getState () == Event::OCCURRING);
      g_assert (c1_prop->getState () == Event::SLEEPING);

      delete fmt;
    }

    // STOP lambda from state OCCURRING
    // STOP lambda from state PAUSED
    // STOP lambda from state SLEEPING
  }

  exit (EXIT_SUCCESS);
}
