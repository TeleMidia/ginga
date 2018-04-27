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
  // ABORT selection from state OCCURRING
  {
    // not applicable
  } // ABORT selection from state PAUSED

  {
    // not applicable
  } // ABORT selection from state SLEEPING

  {
    Formatter *fmt;
    Event *body_lambda, *c1_lambda, *c1_prop, *c1_sel, *m1_lambda,
        *m2_lambda;

    tests_create_document_with_context_and_start (
        &fmt, &body_lambda, &c1_lambda, &c1_prop, &c1_sel, &m1_lambda,
        &m2_lambda);

    // ABORT is not done and return false
    g_assert_false (c1_sel->transition (Event::ABORT));

    // after advance time, c1_lambda, m1_lambda and m2_lambda are PAUSED
    // are OCCURRING, and its properties are SLEEPING
    fmt->sendTick (0, 0, 0);
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (c1_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::OCCURRING);
    g_assert (m2_lambda->getState () == Event::OCCURRING);
    g_assert (c1_prop->getState () == Event::SLEEPING);
    g_assert (c1_sel->getState () == Event::SLEEPING);

    delete fmt;
  }

  // PAUSE selection from state OCCURRING
  {
    // not applicable
  }

  // PAUSE selection from state PAUSED
  {
    // not applicable
  }

  // PAUSE selection from state SLEEPING
  {
    Formatter *fmt;
    Event *body_lambda, *c1_lambda, *c1_prop, *c1_sel, *m1_lambda,
        *m2_lambda;

    tests_create_document_with_context_and_start (
        &fmt, &body_lambda, &c1_lambda, &c1_prop, &c1_sel, &m1_lambda,
        &m2_lambda);

    // PAUSE is not done and return false
    g_assert_false (c1_sel->transition (Event::PAUSE));

    // after advance time, c1_lambda, m1_lambda and m2_lambda are PAUSED
    // are OCCURRING, and its properties are SLEEPING
    fmt->sendTick (0, 0, 0);
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (c1_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::OCCURRING);
    g_assert (m2_lambda->getState () == Event::OCCURRING);
    g_assert (c1_prop->getState () == Event::SLEEPING);
    g_assert (c1_sel->getState () == Event::SLEEPING);

    delete fmt;
  }
  // START selection from state OCCURRING
  {
    // not applicable
  }

  // START selection from state PAUSED
  {
    // not applicable
  }

  // START selection from state SLEEPING
  {
    Formatter *fmt;
    Event *body_lambda, *c1_lambda, *c1_prop, *c1_sel, *m1_lambda,
        *m2_lambda;

    tests_create_document_with_context_and_start (
        &fmt, &body_lambda, &c1_lambda, &c1_prop, &c1_sel, &m1_lambda,
        &m2_lambda);

    // START is not done and return false
    g_assert_false (c1_sel->transition (Event::START));

    // after advance time, c1_lambda, m1_lambda and m2_lambda are PAUSED
    // are OCCURRING, and its properties are SLEEPING
    fmt->sendTick (0, 0, 0);
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (c1_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::OCCURRING);
    g_assert (m2_lambda->getState () == Event::OCCURRING);
    g_assert (c1_prop->getState () == Event::SLEEPING);
    g_assert (c1_sel->getState () == Event::SLEEPING);

    delete fmt;
  }

  // STOP selection from state OCCURRING
  // STOP selection from state PAUSED
  // STOP selection from state SLEEPING
  {
    Formatter *fmt;
    Event *body_lambda, *c1_lambda, *c1_prop, *c1_sel, *m1_lambda,
        *m2_lambda;

    tests_create_document_with_context_and_start (
        &fmt, &body_lambda, &c1_lambda, &c1_prop, &c1_sel, &m1_lambda,
        &m2_lambda);

    // START is not done and return false
    g_assert_false (c1_sel->transition (Event::STOP));

    // after advance time, c1_lambda, m1_lambda and m2_lambda are PAUSED
    // are OCCURRING, and its properties are SLEEPING
    fmt->sendTick (0, 0, 0);
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (c1_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::OCCURRING);
    g_assert (m2_lambda->getState () == Event::OCCURRING);
    g_assert (c1_prop->getState () == Event::SLEEPING);
    g_assert (c1_sel->getState () == Event::SLEEPING);

    delete fmt;
  }

  exit (EXIT_SUCCESS);
}
