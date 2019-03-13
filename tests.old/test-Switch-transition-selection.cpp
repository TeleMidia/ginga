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
  // ABORT lambda from state OCCURRING
  // ABORT lambda from state PAUSED
  // ABORT lambda from state SLEEPING
  {
    Formatter *fmt;
    Event *body_lambda, *swt1_lambda, *swt1_sel, *m1_lambda, *m2_lambda;

    tests_create_document_with_switch_and_start (&fmt, &body_lambda,
                                                 &swt1_lambda, &swt1_sel,
                                                 &m1_lambda, &m2_lambda);
    // ABORT is not done
    g_assert_false (swt1_sel->transition (Event::ABORT));

    // after start, swt1_lambda is SLEEPING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (swt1_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::OCCURRING);
    g_assert (m2_lambda->getState () == Event::SLEEPING);
    g_assert (swt1_sel->getState () == Event::SLEEPING);

    delete fmt;
  }

  // START lambda from state OCCURRING
  // START lambda from state PAUSED
  // START lambda from state SLEEPING
  {
    Formatter *fmt;
    Event *body_lambda, *swt1_lambda, *swt1_sel, *m1_lambda, *m2_lambda;

    tests_create_document_with_switch_and_start (&fmt, &body_lambda,
                                                 &swt1_lambda, &swt1_sel,
                                                 &m1_lambda, &m2_lambda);
    // START is not done
    g_assert_false (swt1_lambda->transition (Event::START));

    // after start, swt1_lambda is OCCURRING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (swt1_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::OCCURRING);
    g_assert (m2_lambda->getState () == Event::SLEEPING);
    g_assert (swt1_sel->getState () == Event::SLEEPING);

    delete fmt;
  }

  // RESUME lambda from state OCCURRING
  // RESUME lambda from state PAUSED
  // RESUME lambda from state SLEEPING
  {
    Formatter *fmt;
    Event *body_lambda, *swt1_lambda, *swt1_sel, *m1_lambda, *m2_lambda;

    tests_create_document_with_switch_and_start (&fmt, &body_lambda,
                                                 &swt1_lambda, &swt1_sel,
                                                 &m1_lambda, &m2_lambda);
    // RESUME is not done
    g_assert_false (swt1_sel->transition (Event::RESUME));

    // after start, swt1_lambda is SLEEPING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (swt1_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::OCCURRING);
    g_assert (m2_lambda->getState () == Event::SLEEPING);
    g_assert (swt1_sel->getState () == Event::SLEEPING);

    delete fmt;
  }

  // START lambda from state OCCURRING
  // START lambda from state PAUSED
  // START lambda from state SLEEPING
  {
    Formatter *fmt;
    Event *body_lambda, *swt1_lambda, *swt1_sel, *m1_lambda, *m2_lambda;

    tests_create_document_with_switch_and_start (&fmt, &body_lambda,
                                                 &swt1_lambda, &swt1_sel,
                                                 &m1_lambda, &m2_lambda);

    // START is not done
    g_assert_false (swt1_lambda->transition (Event::START));

    // after start, swt1_lambda is OCCURRING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (swt1_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::OCCURRING);
    g_assert (m2_lambda->getState () == Event::SLEEPING);
    g_assert (swt1_sel->getState () == Event::SLEEPING);

    delete fmt;
  }

  // STOP lambda from state OCCURRING
  // STOP lambda from state PAUSED
  // STOP lambda from state SLEEPING
  {
    Formatter *fmt;
    Event *body_lambda, *swt1_lambda, *swt1_sel, *m1_lambda, *m2_lambda;

    tests_create_document_with_switch_and_start (&fmt, &body_lambda,
                                                 &swt1_lambda, &swt1_sel,
                                                 &m1_lambda, &m2_lambda);

    // STOP is not done
    g_assert_false (swt1_sel->transition (Event::STOP));

    // after STOP, swt1_lambda is SLEEPING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (swt1_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::OCCURRING);
    g_assert (m2_lambda->getState () == Event::SLEEPING);
    g_assert (swt1_sel->getState () == Event::SLEEPING);

    delete fmt;
  }

  exit (EXIT_SUCCESS);
}
