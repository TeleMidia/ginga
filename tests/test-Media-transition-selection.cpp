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
  // ABORT selection from state PAUSED
  // ABORT selection from state SLEEPING

  // PAUSE selection from state OCCURRING
  // PAUSE selection from state PAUSED
  // PAUSE selection from state SLEEPING

  // START selection from state OCCURRING
  {
    Formatter *fmt;
    Event *body_lambda, *m1_lambda, *m1_anchor_0s, *m1_label, *m1_prop,
        *m1_sel;

    tests_create_document_with_media_and_start (
        &fmt, &body_lambda, &m1_lambda, &m1_anchor_0s, &m1_label, &m1_prop,
        &m1_sel);

    // START is done and return true
    g_assert (m1_sel->transition (Event::START));

    // after START, m1_sel is OCCURRING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::OCCURRING);
    g_assert (m1_anchor_0s->getState () == Event::OCCURRING);
    g_assert (m1_label->getState () == Event::SLEEPING);
    g_assert (m1_prop->getState () == Event::SLEEPING);
    g_assert (m1_sel->getState () == Event::OCCURRING);

    // START is not done and return true
    g_assert_false (m1_sel->transition (Event::START));

    // when advance time, m1_sel is still OCCURRING
    fmt->sendTick (0, 0, 0);
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::OCCURRING);
    g_assert (m1_anchor_0s->getState () == Event::OCCURRING);
    g_assert (m1_label->getState () == Event::SLEEPING);
    g_assert (m1_prop->getState () == Event::SLEEPING);
    g_assert (m1_sel->getState () == Event::OCCURRING);

    delete fmt;
  }

  // START selection from state PAUSED
  // START selection from state SLEEPING
  {
    Formatter *fmt;
    Event *body_lambda, *m1_lambda, *m1_anchor_0s, *m1_label, *m1_prop,
        *m1_sel;

    tests_create_document_with_media_and_start (
        &fmt, &body_lambda, &m1_lambda, &m1_anchor_0s, &m1_label, &m1_prop,
        &m1_sel);

    // START is done
    g_assert (m1_sel->transition (Event::START));

    // after START, m1_sel is OCCURRING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::OCCURRING);
    g_assert (m1_anchor_0s->getState () == Event::OCCURRING);
    g_assert (m1_label->getState () == Event::SLEEPING);
    g_assert (m1_prop->getState () == Event::SLEEPING);
    g_assert (m1_sel->getState () == Event::OCCURRING);

    // when advance time, m1_sel is still OCCURRING
    fmt->sendTick (0, 0, 0);
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::OCCURRING);
    g_assert (m1_anchor_0s->getState () == Event::OCCURRING);
    g_assert (m1_label->getState () == Event::SLEEPING);
    g_assert (m1_prop->getState () == Event::SLEEPING);
    g_assert (m1_sel->getState () == Event::OCCURRING);

    delete fmt;
  }

  // STOP selection from state OCCURRING
  // STOP selection from state PAUSED
  // STOP selection from state SLEEPING

  exit (EXIT_SUCCESS);
}
