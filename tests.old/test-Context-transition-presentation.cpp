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
  {
    Formatter *fmt;
    Event *body_lambda, *c1_lambda, *c1_prop, *c1_sel, *m1_lambda,
        *m2_lambda;

    tests_create_document_with_context_and_start (
        &fmt, &body_lambda, &c1_lambda, &c1_prop, &c1_sel, &m1_lambda,
        &m2_lambda);

    // ABORT is done
    g_assert_true (c1_lambda->transition (Event::ABORT));

    // after ABORT c1_lambda, m1_lambda and m2_lambda are SLEEPING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (c1_lambda->getState () == Event::SLEEPING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);
    g_assert (m2_lambda->getState () == Event::SLEEPING);
    g_assert (c1_prop->getState () == Event::SLEEPING);
    g_assert (c1_sel->getState () == Event::SLEEPING);
    g_assert (c1_sel->getState () == Event::SLEEPING);

    delete fmt;
  }

  // ABORT lambda from state PAUSED
  {
    Formatter *fmt;
    Event *body_lambda, *c1_lambda, *c1_prop, *c1_sel, *m1_lambda,
        *m2_lambda;

    tests_create_document_with_context_and_start (
        &fmt, &body_lambda, &c1_lambda, &c1_prop, &c1_sel, &m1_lambda,
        &m2_lambda);

    // PAUSE is done
    g_assert_true (c1_lambda->transition (Event::PAUSE));

    // after PAUSE, c1_lambda, m1_lambda and m2_lambda are PAUSED
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (c1_lambda->getState () == Event::PAUSED);
    g_assert (c1_prop->getState () == Event::SLEEPING);
    g_assert (c1_sel->getState () == Event::SLEEPING);
    g_assert (m1_lambda->getState () == Event::PAUSED);
    g_assert (m2_lambda->getState () == Event::PAUSED);

    // ABORT is done
    g_assert_true (c1_lambda->transition (Event::ABORT));

    // after ABORT c1_lambda, m1_lambda and m2_lambda are SLEEPING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (c1_lambda->getState () == Event::SLEEPING);
    g_assert (c1_prop->getState () == Event::SLEEPING);
    g_assert (c1_sel->getState () == Event::SLEEPING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);
    g_assert (m2_lambda->getState () == Event::SLEEPING);

    delete fmt;
  }

  // ABORT lambda from state SLEEPING
  {
    Formatter *fmt;
    Event *body_lambda, *c1_lambda, *c1_prop, *c1_sel, *m1_lambda,
        *m2_lambda;

    tests_create_document_with_context_and_start (
        &fmt, &body_lambda, &c1_lambda, &c1_prop, &c1_sel, &m1_lambda,
        &m2_lambda);

    // STOP is done
    g_assert_true (c1_lambda->transition (Event::STOP));

    // after STOP c1_lambda, m1_lambda and m2_lambda are SLEEPING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (c1_lambda->getState () == Event::SLEEPING);
    g_assert (c1_prop->getState () == Event::SLEEPING);
    g_assert (c1_sel->getState () == Event::SLEEPING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);
    g_assert (m2_lambda->getState () == Event::SLEEPING);

    // ABORT is not done
    g_assert_false (c1_lambda->transition (Event::ABORT));

    // after ABORT c1_lambda, m1_lambda and m2_lambda are still SLEEPING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (c1_lambda->getState () == Event::SLEEPING);
    g_assert (c1_prop->getState () == Event::SLEEPING);
    g_assert (c1_sel->getState () == Event::SLEEPING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);
    g_assert (m2_lambda->getState () == Event::SLEEPING);

    delete fmt;
  }

  // PAUSE lambda from state OCCURRING
  {
    Formatter *fmt;
    Event *body_lambda, *c1_lambda, *c1_prop, *c1_sel, *m1_lambda,
        *m2_lambda;

    tests_create_document_with_context_and_start (
        &fmt, &body_lambda, &c1_lambda, &c1_prop, &c1_sel, &m1_lambda,
        &m2_lambda);

    // PAUSE is done
    g_assert_true (c1_lambda->transition (Event::PAUSE));

    // after PAUSE c1_lambda, m1_lambda and m2_lambda are PAUSED
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (c1_lambda->getState () == Event::PAUSED);
    g_assert (c1_prop->getState () == Event::SLEEPING);
    g_assert (c1_sel->getState () == Event::SLEEPING);
    g_assert (m1_lambda->getState () == Event::PAUSED);
    g_assert (m2_lambda->getState () == Event::PAUSED);

    delete fmt;
  }

  // PAUSE lambda from state PAUSED
  {
    Formatter *fmt;
    Event *body_lambda, *c1_lambda, *c1_prop, *c1_sel, *m1_lambda,
        *m2_lambda;

    tests_create_document_with_context_and_start (
        &fmt, &body_lambda, &c1_lambda, &c1_prop, &c1_sel, &m1_lambda,
        &m2_lambda);

    // PAUSE is done
    g_assert_true (c1_lambda->transition (Event::PAUSE));

    // after PAUSE c1_lambda, m1_lambda and m2_lambda are PAUSED
    g_assert (c1_lambda->getState () == Event::PAUSED);
    g_assert (c1_prop->getState () == Event::SLEEPING);
    g_assert (c1_sel->getState () == Event::SLEEPING);
    g_assert (m1_lambda->getState () == Event::PAUSED);
    g_assert (m2_lambda->getState () == Event::PAUSED);

    // PAUSE is not done
    g_assert_false (c1_lambda->transition (Event::PAUSE));

    // after PAUSE c1_lambda, m1_lambda and m2_lambda are still PAUSED
    g_assert (c1_lambda->getState () == Event::PAUSED);
    g_assert (c1_prop->getState () == Event::SLEEPING);
    g_assert (c1_sel->getState () == Event::SLEEPING);
    g_assert (m1_lambda->getState () == Event::PAUSED);
    g_assert (m2_lambda->getState () == Event::PAUSED);

    delete fmt;
  }

  // PAUSE lambda from state SLEEPING

  {
    Formatter *fmt;
    Event *body_lambda, *c1_lambda, *c1_prop, *c1_sel, *m1_lambda,
        *m2_lambda;

    tests_create_document_with_context_and_start (
        &fmt, &body_lambda, &c1_lambda, &c1_prop, &c1_sel, &m1_lambda,
        &m2_lambda);

    // STOP is done
    g_assert_true (c1_lambda->transition (Event::STOP));

    // after STOP c1_lambda, m1_lambda and m2_lambda are SLEEPING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (c1_lambda->getState () == Event::SLEEPING);
    g_assert (c1_prop->getState () == Event::SLEEPING);
    g_assert (c1_sel->getState () == Event::SLEEPING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);
    g_assert (m2_lambda->getState () == Event::SLEEPING);

    // PAUSE is not done
    g_assert_false (c1_lambda->transition (Event::PAUSE));

    // after PAUSE c1_lambda, m1_lambda and m2_lambda still SLEEPING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (c1_lambda->getState () == Event::SLEEPING);
    g_assert (c1_prop->getState () == Event::SLEEPING);
    g_assert (c1_sel->getState () == Event::SLEEPING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);
    g_assert (m2_lambda->getState () == Event::SLEEPING);

    delete fmt;
  }

  // RESUME lambda from state OCCURRING
  {
    Formatter *fmt;
    Event *body_lambda, *c1_lambda, *c1_prop, *c1_sel, *m1_lambda,
        *m2_lambda;

    tests_create_document_with_context_and_start (
        &fmt, &body_lambda, &c1_lambda, &c1_prop, &c1_sel, &m1_lambda,
        &m2_lambda);

    // RESUME is done
    g_assert_false (c1_lambda->transition (Event::RESUME));

    // after RESUME, anchors events go to OCCURRING
    // and properties events are SLEEPING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (c1_lambda->getState () == Event::OCCURRING);
    g_assert (c1_prop->getState () == Event::SLEEPING);
    g_assert (c1_sel->getState () == Event::SLEEPING);
    g_assert (m1_lambda->getState () == Event::OCCURRING);
    g_assert (m2_lambda->getState () == Event::OCCURRING);

    delete fmt;
  }

  // RESUME lambda from state PAUSED
  {
    Formatter *fmt;
    Event *body_lambda, *c1_lambda, *c1_prop, *c1_sel, *m1_lambda,
        *m2_lambda;

    tests_create_document_with_context_and_start (
        &fmt, &body_lambda, &c1_lambda, &c1_prop, &c1_sel, &m1_lambda,
        &m2_lambda);

    // PAUSE is done
    g_assert_true (c1_lambda->transition (Event::PAUSE));

    // after PAUSE, anchors events go to PAUSED
    // and properties events are SLEEPING
    g_assert (c1_lambda->getState () == Event::PAUSED);
    g_assert (c1_prop->getState () == Event::SLEEPING);
    g_assert (c1_sel->getState () == Event::SLEEPING);
    g_assert (m1_lambda->getState () == Event::PAUSED);
    g_assert (m2_lambda->getState () == Event::PAUSED);

    // RESUME is done
    g_assert_true (c1_lambda->transition (Event::RESUME));

    // after RESUME, anchors events go to OCCURRING
    // and properties events are SLEEPING
    g_assert (c1_lambda->getState () == Event::OCCURRING);
    g_assert (c1_prop->getState () == Event::SLEEPING);
    g_assert (c1_sel->getState () == Event::SLEEPING);
    g_assert (m1_lambda->getState () == Event::OCCURRING);
    g_assert (m2_lambda->getState () == Event::OCCURRING);

    delete fmt;
  }

  // RESUME lambda from state SLEEPING
  {
    Formatter *fmt;
    Event *body_lambda, *c1_lambda, *c1_prop, *c1_sel, *m1_lambda,
        *m2_lambda;

    tests_create_document_with_context_and_start (
        &fmt, &body_lambda, &c1_lambda, &c1_prop, &c1_sel, &m1_lambda,
        &m2_lambda);

    // STOP is done
    g_assert_true (c1_lambda->transition (Event::STOP));

    // after STOP c1_lambda, m1_lambda and m2_lambda are SLEEPING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (c1_lambda->getState () == Event::SLEEPING);
    g_assert (c1_prop->getState () == Event::SLEEPING);
    g_assert (c1_sel->getState () == Event::SLEEPING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);
    g_assert (m2_lambda->getState () == Event::SLEEPING);

    // RESUME is not done
    g_assert_false (c1_lambda->transition (Event::RESUME));

    // after PAUSE c1_lambda, m1_lambda and m2_lambda still SLEEPING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (c1_lambda->getState () == Event::SLEEPING);
    g_assert (c1_prop->getState () == Event::SLEEPING);
    g_assert (c1_sel->getState () == Event::SLEEPING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);
    g_assert (m2_lambda->getState () == Event::SLEEPING);

    delete fmt;
  }

  // START lambda from state OCCURRING
  {
    Formatter *fmt;
    Event *body_lambda, *c1_lambda, *c1_prop, *c1_sel, *m1_lambda,
        *m2_lambda;

    tests_create_document_with_context_and_start (
        &fmt, &body_lambda, &c1_lambda, &c1_prop, &c1_sel, &m1_lambda,
        &m2_lambda);

    // START is not done
    g_assert_false (c1_lambda->transition (Event::START));

    // after start, c1_lambda, m1_lambda and m2_lambda are PAUSED
    // are OCCURRING, and its properties are SLEEPING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (c1_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::OCCURRING);
    g_assert (m2_lambda->getState () == Event::OCCURRING);
    g_assert (c1_prop->getState () == Event::SLEEPING);
    g_assert (c1_sel->getState () == Event::SLEEPING);

    delete fmt;
  }

  // START lambda from state PAUSED
  {
    Formatter *fmt;
    Event *body_lambda, *c1_lambda, *c1_prop, *c1_sel, *m1_lambda,
        *m2_lambda;

    tests_create_document_with_context_and_start (
        &fmt, &body_lambda, &c1_lambda, &c1_prop, &c1_sel, &m1_lambda,
        &m2_lambda);

    // PAUSE is done
    g_assert_true (c1_lambda->transition (Event::PAUSE));

    // after PAUSE c1_lambda, m1_lambda and m2_lambda are PAUSED
    g_assert (c1_lambda->getState () == Event::PAUSED);
    g_assert (c1_prop->getState () == Event::SLEEPING);
    g_assert (c1_sel->getState () == Event::SLEEPING);
    g_assert (m1_lambda->getState () == Event::PAUSED);
    g_assert (m2_lambda->getState () == Event::PAUSED);

    // START is done
    g_assert_true (c1_lambda->transition (Event::START));

    // after start, c1_lambda, m1_lambda and m2_lambda are PAUSED
    // are OCCURRING, and its properties are SLEEPING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (c1_lambda->getState () == Event::OCCURRING);
    g_assert (c1_prop->getState () == Event::SLEEPING);
    g_assert (c1_sel->getState () == Event::SLEEPING);
    g_assert (m1_lambda->getState () == Event::PAUSED);
    g_assert (m2_lambda->getState () == Event::PAUSED);

    fmt->sendTick (0, 0, 0);
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (c1_lambda->getState () == Event::OCCURRING);
    g_assert (c1_prop->getState () == Event::SLEEPING);
    g_assert (c1_sel->getState () == Event::SLEEPING);
    g_assert (m1_lambda->getState () == Event::OCCURRING);
    g_assert (m2_lambda->getState () == Event::OCCURRING);

    delete fmt;
  }

  // START lambda from state SLEEPING
  {
    Formatter *fmt;
    Event *body_lambda, *c1_lambda, *c1_prop, *c1_sel, *m1_lambda,
        *m2_lambda;

    tests_create_document_with_context_and_start (
        &fmt, &body_lambda, &c1_lambda, &c1_prop, &c1_sel, &m1_lambda,
        &m2_lambda);

    // STOP is done
    g_assert (c1_lambda->transition (Event::STOP));

    // after STOP, c1_lambda, m1_lambda and m2_lambda are PAUSED
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (c1_lambda->getState () == Event::SLEEPING);
    g_assert (c1_prop->getState () == Event::SLEEPING);
    g_assert (c1_sel->getState () == Event::SLEEPING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);
    g_assert (m2_lambda->getState () == Event::SLEEPING);

    // START is done
    g_assert_true (c1_lambda->transition (Event::START));

    // after START, c1_lambda is SLEEPING
    g_assert (c1_lambda->getState () == Event::OCCURRING);
    g_assert (c1_prop->getState () == Event::SLEEPING);
    g_assert (c1_sel->getState () == Event::SLEEPING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);
    g_assert (m2_lambda->getState () == Event::SLEEPING);

    // after advance time, c1_lambda, m1_lambda and m2_lambda are PAUSED
    // are OCCURRING, and its properties are SLEEPING
    fmt->sendTick (0, 0, 0);
    g_assert (c1_lambda->getState () == Event::OCCURRING);
    g_assert (c1_prop->getState () == Event::SLEEPING);
    g_assert (c1_sel->getState () == Event::SLEEPING);
    g_assert (m1_lambda->getState () == Event::OCCURRING);
    g_assert (m2_lambda->getState () == Event::OCCURRING);

    delete fmt;
  }

  // STOP c1_lambda  from state OCCURRING
  {
    Formatter *fmt;
    Event *body_lambda, *c1_lambda, *c1_prop, *c1_sel, *m1_lambda,
        *m2_lambda;

    tests_create_document_with_context_and_start (
        &fmt, &body_lambda, &c1_lambda, &c1_prop, &c1_sel, &m1_lambda,
        &m2_lambda);

    // STOP is done
    g_assert_true (c1_lambda->transition (Event::STOP));

    // after STOP c1_lambda, m1_lambda and m2_lambda are SLEEPING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (c1_lambda->getState () == Event::SLEEPING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);
    g_assert (m2_lambda->getState () == Event::SLEEPING);
    g_assert (c1_prop->getState () == Event::SLEEPING);
    g_assert (c1_sel->getState () == Event::SLEEPING);

    delete fmt;
  }

  // STOP c1_lambda  from state PAUSED
  {
    Formatter *fmt;
    Event *body_lambda, *c1_lambda, *c1_prop, *c1_sel, *m1_lambda,
        *m2_lambda;

    tests_create_document_with_context_and_start (
        &fmt, &body_lambda, &c1_lambda, &c1_prop, &c1_sel, &m1_lambda,
        &m2_lambda);

    // PAUSE is done
    g_assert_true (c1_lambda->transition (Event::PAUSE));

    // after PAUSE c1_lambda, m1_lambda and m2_lambda are PAUSED
    g_assert (c1_lambda->getState () == Event::PAUSED);
    g_assert (c1_prop->getState () == Event::SLEEPING);
    g_assert (c1_sel->getState () == Event::SLEEPING);
    g_assert (m1_lambda->getState () == Event::PAUSED);
    g_assert (m2_lambda->getState () == Event::PAUSED);

    // STOP is done
    g_assert_true (c1_lambda->transition (Event::STOP));

    // after STOP c1_lambda, m1_lambda and m2_lambda are still SLEEPING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (c1_lambda->getState () == Event::SLEEPING);
    g_assert (c1_prop->getState () == Event::SLEEPING);
    g_assert (c1_sel->getState () == Event::SLEEPING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);
    g_assert (m2_lambda->getState () == Event::SLEEPING);

    delete fmt;
  }

  // STOP c1_lambda  from state SLEEPING
  {
    Formatter *fmt;
    Event *body_lambda, *c1_lambda, *c1_prop, *c1_sel, *m1_lambda,
        *m2_lambda;

    tests_create_document_with_context_and_start (
        &fmt, &body_lambda, &c1_lambda, &c1_prop, &c1_sel, &m1_lambda,
        &m2_lambda);

    // STOP is done
    g_assert (c1_lambda->transition (Event::STOP));

    // after STOP c1_lambda, m1_lambda and m2_lambda SLEEPING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (c1_lambda->getState () == Event::SLEEPING);
    g_assert (c1_prop->getState () == Event::SLEEPING);
    g_assert (c1_sel->getState () == Event::SLEEPING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);
    g_assert (m2_lambda->getState () == Event::SLEEPING);

    // STOP is not done
    g_assert_false (c1_lambda->transition (Event::STOP));

    // after STOP c1_lambda, m1_lambda and m2_lambda still SLEEPING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (c1_lambda->getState () == Event::SLEEPING);
    g_assert (c1_prop->getState () == Event::SLEEPING);
    g_assert (c1_sel->getState () == Event::SLEEPING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);
    g_assert (m2_lambda->getState () == Event::SLEEPING);

    delete fmt;
  }

  exit (EXIT_SUCCESS);
}
