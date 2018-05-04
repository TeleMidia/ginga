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
  // ABORT lambda from state OCCURRING.
  {
    Formatter *fmt;
    Event *body_lambda, *m1_lambda, *m1_anchor_0s, *m1_label, *m1_prop, *m1_sel;

    tests_create_document_with_media_and_start (
        &fmt, &body_lambda, &m1_lambda, &m1_anchor_0s, &m1_label, &m1_prop, &m1_sel);

    // ABORT is done
    g_assert_true (m1_lambda->transition (Event::ABORT));

    // after ABORT all events are SLEEPING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);
    g_assert (m1_anchor_0s->getState () == Event::SLEEPING);
    g_assert (m1_label->getState () == Event::SLEEPING);
    g_assert (m1_prop->getState () == Event::SLEEPING);
    g_assert (m1_sel->getState () ==Event::SLEEPING);

    delete fmt;
  }

  // ABORT lambda from state PAUSED
  {
    Formatter *fmt;
    Event *body_lambda, *m1_lambda, *m1_anchor_0s, *m1_label, *m1_prop, *m1_sel;

    tests_create_document_with_media_and_start (
        &fmt, &body_lambda, &m1_lambda, &m1_anchor_0s, &m1_label, &m1_prop, &m1_sel);

    // PAUSE is done
    g_assert_true (m1_lambda->transition (Event::PAUSE));

    // after PAUSE, m1_lambda and m1_anchor0s are PAUSED
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::PAUSED);
    g_assert (m1_anchor_0s->getState () == Event::PAUSED);
    g_assert (m1_label->getState () == Event::SLEEPING);
    g_assert (m1_prop->getState () == Event::SLEEPING);
    g_assert (m1_sel->getState () ==Event::SLEEPING);

    // PAUSE is done
    g_assert_true (m1_lambda->transition (Event::ABORT));

    // after PAUSE, all events are SLEEPING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);
    g_assert (m1_anchor_0s->getState () == Event::SLEEPING);
    g_assert (m1_label->getState () == Event::SLEEPING);
    g_assert (m1_prop->getState () == Event::SLEEPING);
    g_assert (m1_sel->getState () ==Event::SLEEPING);

    delete fmt;
  }

  // ABORT lambda from state SLEEPING
  {
    Formatter *fmt;
    Event *body_lambda, *m1_lambda, *m1_anchor_0s, *m1_label, *m1_prop, *m1_sel;

    tests_create_document_with_media_and_start (
        &fmt, &body_lambda, &m1_lambda, &m1_anchor_0s, &m1_label, &m1_prop, &m1_sel);

    // STOP is done
    g_assert_true (m1_lambda->transition (Event::STOP));

    // after STOP all events are SLEEPING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);
    g_assert (m1_anchor_0s->getState () == Event::SLEEPING);
    g_assert (m1_label->getState () == Event::SLEEPING);
    g_assert (m1_prop->getState () == Event::SLEEPING);
    g_assert (m1_sel->getState () ==Event::SLEEPING);

    // ABORT is not done
    g_assert_false (m1_lambda->transition (Event::ABORT));

    // after ABORT all events are still SLEEPING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);
    g_assert (m1_anchor_0s->getState () == Event::SLEEPING);
    g_assert (m1_label->getState () == Event::SLEEPING);
    g_assert (m1_prop->getState () == Event::SLEEPING);
    g_assert (m1_sel->getState () ==Event::SLEEPING);

    delete fmt;
  }

  // PAUSE lambda from state OCCURRING.
  {
    Formatter *fmt;
    Event *body_lambda, *m1_lambda, *m1_anchor_0s, *m1_label, *m1_prop, *m1_sel;

    tests_create_document_with_media_and_start (
        &fmt, &body_lambda, &m1_lambda, &m1_anchor_0s, &m1_label, &m1_prop, &m1_sel);
    // --------------------------------
    // main check

    // PAUSE is done
    g_assert_true (m1_lambda->transition (Event::PAUSE));

    // after PAUSE, m1_lambda and m1_anchor0s are PAUSED
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::PAUSED);
    g_assert (m1_anchor_0s->getState () == Event::PAUSED);
    g_assert (m1_label->getState () == Event::SLEEPING);
    g_assert (m1_prop->getState () == Event::SLEEPING);
    g_assert (m1_sel->getState () ==Event::SLEEPING);

    delete fmt;
  }

  // PAUSE lambda from state PAUSED
  {
    Formatter *fmt;
    Event *body_lambda, *m1_lambda, *m1_anchor_0s, *m1_label, *m1_prop, *m1_sel;

    tests_create_document_with_media_and_start (
        &fmt, &body_lambda, &m1_lambda, &m1_anchor_0s, &m1_label, &m1_prop, &m1_sel);

    // PAUSE is done
    g_assert_true (m1_lambda->transition (Event::PAUSE));

    // after PAUSE, m1_lambda and m1_anchor0s are PAUSED
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::PAUSED);
    g_assert (m1_anchor_0s->getState () == Event::PAUSED);
    g_assert (m1_label->getState () == Event::SLEEPING);
    g_assert (m1_prop->getState () == Event::SLEEPING);
    g_assert (m1_sel->getState () ==Event::SLEEPING);

    // PAUSE is done
    g_assert_false (m1_lambda->transition (Event::PAUSE));

    // after PAUSE, m1_lambda and m1_anchor0s are  still PAUSED
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::PAUSED);
    g_assert (m1_anchor_0s->getState () == Event::PAUSED);
    g_assert (m1_label->getState () == Event::SLEEPING);
    g_assert (m1_prop->getState () == Event::SLEEPING);
    g_assert (m1_sel->getState () ==Event::SLEEPING);

    delete fmt;
  }

  // PAUSE lambda from state SLEEPING
  {
    Formatter *fmt;
    Event *body_lambda, *m1_lambda, *m1_anchor_0s, *m1_label, *m1_prop, *m1_sel;

    tests_create_document_with_media_and_start (
        &fmt, &body_lambda, &m1_lambda, &m1_anchor_0s, &m1_label, &m1_prop, &m1_sel);

    // STOP is done
    g_assert_true (m1_lambda->transition (Event::STOP));

    // after STOP all events are SLEEPING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);
    g_assert (m1_anchor_0s->getState () == Event::SLEEPING);
    g_assert (m1_label->getState () == Event::SLEEPING);
    g_assert (m1_prop->getState () == Event::SLEEPING);
    g_assert (m1_sel->getState () ==Event::SLEEPING);

    // PAUSE is not done
    g_assert_false (m1_lambda->transition (Event::PAUSE));

    // after PAUSE, m1_lambda and m1_anchor0s are still SLEEPING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);
    g_assert (m1_anchor_0s->getState () == Event::SLEEPING);
    g_assert (m1_label->getState () == Event::SLEEPING);
    g_assert (m1_prop->getState () == Event::SLEEPING);
    g_assert (m1_sel->getState () ==Event::SLEEPING);

    delete fmt;
  }

  // RESUME lambda from state OCCURRING.
  {
    Formatter *fmt;
    Event *body_lambda, *m1_lambda, *m1_anchor_0s, *m1_label, *m1_prop, *m1_sel;

    tests_create_document_with_media_and_start (
        &fmt, &body_lambda, &m1_lambda, &m1_anchor_0s, &m1_label, &m1_prop, &m1_sel);

    // RESUME is not done
    g_assert_false (m1_lambda->transition (Event::RESUME));

    // after RESUME m1_lambda and m1_anchor_0s are OCCURRING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::OCCURRING);
    g_assert (m1_anchor_0s->getState () == Event::OCCURRING);
    g_assert (m1_label->getState () == Event::SLEEPING);
    g_assert (m1_prop->getState () == Event::SLEEPING);
    g_assert (m1_sel->getState () ==Event::SLEEPING);

    delete fmt;
  }

  // RESUME lambda from state PAUSED
  {
    Formatter *fmt;
    Event *body_lambda, *m1_lambda, *m1_anchor_0s, *m1_label, *m1_prop, *m1_sel;

    tests_create_document_with_media_and_start (
        &fmt, &body_lambda, &m1_lambda, &m1_anchor_0s, &m1_label, &m1_prop, &m1_sel);

    // PAUSE is done
    g_assert (m1_lambda->transition (Event::PAUSE));

    // after PAUSE, m1_lambda and m1_anchor0s are PAUSED
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::PAUSED);
    g_assert (m1_anchor_0s->getState () == Event::PAUSED);
    g_assert (m1_label->getState () == Event::SLEEPING);
    g_assert (m1_prop->getState () == Event::SLEEPING);
    g_assert (m1_sel->getState () ==Event::SLEEPING);

    // RESUME is done
    g_assert (m1_lambda->transition (Event::RESUME));

    // after RESUME m1_lambda and m1_anchor_0s are OCCURRING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::OCCURRING);
    g_assert (m1_anchor_0s->getState () == Event::OCCURRING);
    g_assert (m1_label->getState () == Event::SLEEPING);
    g_assert (m1_prop->getState () == Event::SLEEPING);
    g_assert (m1_sel->getState () ==Event::SLEEPING);

    delete fmt;
  }

  // START lambda from state SLEEPING
  {
    Formatter *fmt;
    Event *body_lambda, *m1_lambda, *m1_anchor_0s, *m1_label, *m1_prop, *m1_sel;

    tests_create_document_with_media_and_start (
        &fmt, &body_lambda, &m1_lambda, &m1_anchor_0s, &m1_label, &m1_prop, &m1_sel);

    // STOP is done
    g_assert_true (m1_lambda->transition (Event::STOP));

    // after STOP all events are SLEEPING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);
    g_assert (m1_anchor_0s->getState () == Event::SLEEPING);
    g_assert (m1_label->getState () == Event::SLEEPING);
    g_assert (m1_prop->getState () == Event::SLEEPING);
    g_assert (m1_sel->getState () ==Event::SLEEPING);

    // RESUME is not done
    g_assert_false (m1_lambda->transition (Event::RESUME));

    // after RESUME all events are still SLEEPING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);
    g_assert (m1_anchor_0s->getState () == Event::SLEEPING);
    g_assert (m1_label->getState () == Event::SLEEPING);
    g_assert (m1_prop->getState () == Event::SLEEPING);
    g_assert (m1_sel->getState () ==Event::SLEEPING);

    delete fmt;
  }

  // START lambda from state SLEEPING
  {
    Formatter *fmt;
    Event *body_lambda, *m1_lambda, *m1_anchor_0s, *m1_label, *m1_prop, *m1_sel;

    tests_create_document_with_media_and_start (
        &fmt, &body_lambda, &m1_lambda, &m1_anchor_0s, &m1_label, &m1_prop, &m1_sel);

    // STOP is done
    g_assert_true (m1_lambda->transition (Event::STOP));

    // after STOP m1_lambada and m1_anchor_0s are SLEEPING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);
    g_assert (m1_anchor_0s->getState () == Event::SLEEPING);
    g_assert (m1_label->getState () == Event::SLEEPING);
    g_assert (m1_prop->getState () == Event::SLEEPING);
    g_assert (m1_sel->getState () ==Event::SLEEPING);

    // START is done
    g_assert_true (m1_lambda->transition (Event::START));

    // after START m1_lambada and m1_anchor_0s are OCCURRING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::OCCURRING);
    g_assert (m1_anchor_0s->getState () == Event::SLEEPING);
    g_assert (m1_label->getState () == Event::SLEEPING);
    g_assert (m1_prop->getState () == Event::SLEEPING);
    g_assert (m1_sel->getState () ==Event::SLEEPING);

    // advance time
    fmt->sendTick (0, 0, 0);

    // when advance time, timed m1_lambda and m1@a1 go to OCCURRING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::OCCURRING);
    g_assert (m1_anchor_0s->getState () == Event::OCCURRING);
    g_assert (m1_label->getState () == Event::SLEEPING);
    g_assert (m1_prop->getState () == Event::SLEEPING);
    g_assert (m1_sel->getState () ==Event::SLEEPING);

    delete fmt;
  }

  // STOP lambda from state OCCURRING.
  {
    Formatter *fmt;
    Event *body_lambda, *m1_lambda, *m1_anchor_0s, *m1_label, *m1_prop, *m1_sel;

    tests_create_document_with_media_and_start (
        &fmt, &body_lambda, &m1_lambda, &m1_anchor_0s, &m1_label, &m1_prop, &m1_sel);

    // STOP is done
    g_assert_true (m1_lambda->transition (Event::STOP));

    // after STOP all events are SLEEPING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);
    g_assert (m1_anchor_0s->getState () == Event::SLEEPING);
    g_assert (m1_label->getState () == Event::SLEEPING);
    g_assert (m1_prop->getState () == Event::SLEEPING);
    g_assert (m1_sel->getState () ==Event::SLEEPING);

    delete fmt;
  }

  // STOP lambda from state PAUSED
  {
    Formatter *fmt;
    Event *body_lambda, *m1_lambda, *m1_anchor_0s, *m1_label, *m1_prop, *m1_sel;

    tests_create_document_with_media_and_start (
        &fmt, &body_lambda, &m1_lambda, &m1_anchor_0s, &m1_label, &m1_prop, &m1_sel);

    // PAUSE is done
    g_assert_true (m1_lambda->transition (Event::PAUSE));

    // after PAUSE, m1_lambda and m1_anchor_0s are PAUSED
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::PAUSED);
    g_assert (m1_anchor_0s->getState () == Event::PAUSED);
    g_assert (m1_label->getState () == Event::SLEEPING);
    g_assert (m1_prop->getState () == Event::SLEEPING);
    g_assert (m1_sel->getState () ==Event::SLEEPING);

    // STOP is done
    g_assert_true (m1_lambda->transition (Event::STOP));

    // after STOP all events are SLEEPING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);
    g_assert (m1_anchor_0s->getState () == Event::SLEEPING);
    g_assert (m1_label->getState () == Event::SLEEPING);
    g_assert (m1_prop->getState () == Event::SLEEPING);
    g_assert (m1_sel->getState () ==Event::SLEEPING);

    delete fmt;
  }

  // STOP lambda from state SLEEPING
  {
    Formatter *fmt;
    Event *body_lambda, *m1_lambda, *m1_anchor_0s, *m1_label, *m1_prop, *m1_sel;

    tests_create_document_with_media_and_start (
        &fmt, &body_lambda, &m1_lambda, &m1_anchor_0s, &m1_label, &m1_prop, &m1_sel);

    // STOP is done
    g_assert_true (m1_lambda->transition (Event::STOP));

    // after STOP, m1_lambda and m1_anchor_0s are SLEEPING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);
    g_assert (m1_anchor_0s->getState () == Event::SLEEPING);
    g_assert (m1_label->getState () == Event::SLEEPING);
    g_assert (m1_prop->getState () == Event::SLEEPING);
    g_assert (m1_sel->getState () ==Event::SLEEPING);

    // STOP is not done
    g_assert_false (m1_lambda->transition (Event::STOP));

    // after STOP, m1_lambda and m1_anchor_0s are still SLEEPING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);
    g_assert (m1_anchor_0s->getState () == Event::SLEEPING);
    g_assert (m1_label->getState () == Event::SLEEPING);
    g_assert (m1_prop->getState () == Event::SLEEPING);
    g_assert (m1_sel->getState () ==Event::SLEEPING);

    delete fmt;
  }

  exit (EXIT_SUCCESS);
}
