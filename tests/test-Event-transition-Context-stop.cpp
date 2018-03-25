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
  // Presentation events ---------------------------------------------------

  // @c1_lambda: STOP from state OCCURRING.
  {
    Formatter *fmt;
    Document *doc;
    tests_parse_and_start (&fmt, &doc, "\
<ncl>\n\
<body>\n\
  <port id='p1' component='c1'/>\n\
  <context id='c1'>\n\
    <property name='p1' value='0'/>\n\
    <port id='port1' component='m1'/>\n\
    <port id='port2' component='m2'/>\n\
    <media id='m1'/>\n\
    <media id='m2'/>\n\
  </context>\n\
</body>\n\
</ncl>");

    Context *body = cast (Context *, doc->getRoot ());
    g_assert_nonnull (body);
    Event *body_lambda = body->getLambda ();
    g_assert_nonnull (body_lambda);

    Context *c1 = cast (Context *, body->getChildById ("c1"));
    g_assert_nonnull (c1);
    Event *c1_lambda = c1->getLambda ();
    g_assert_nonnull (c1_lambda);

    Event *c1_p1 = c1->getAttributionEvent ("p1");
    g_assert_nonnull (c1_p1);

    auto iter = c1->getPorts ()->begin ();
    Event *c1_port1 = *iter;
    g_assert_nonnull (c1_port1);
    iter++;
    Event *c1_port2 = *iter;
    g_assert_nonnull (c1_port2);

    // --------------------------------
    // check start document

    // when document is started, only the body@lambda is OCCURING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (c1_p1->getState () == Event::SLEEPING);
    g_assert (c1_port1->getState () == Event::SLEEPING);
    g_assert (c1_port2->getState () == Event::SLEEPING);
    g_assert (c1_lambda->getState () == Event::SLEEPING);

    // after advance time, c1@lambda is OCCURRING, and its
    // anchors and properties are SLEEPING
    fmt->sendTick (0, 0, 0);
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (c1_lambda->getState () == Event::OCCURRING);
    g_assert (c1_port1->getState () == Event::SLEEPING);
    g_assert (c1_port2->getState () == Event::SLEEPING);
    g_assert (c1_p1->getState () == Event::SLEEPING);

    // after advance time, c1@lambda and its anchors
    // are OCCURRING, and its properties are SLEEPING
    fmt->sendTick (0, 0, 0);
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (c1_lambda->getState () == Event::OCCURRING);
    g_assert (c1_port1->getState () == Event::OCCURRING);
    g_assert (c1_port2->getState () == Event::OCCURRING);
    g_assert (c1_p1->getState () == Event::SLEEPING);

    // --------------------------------
    // main check

    // STOP is done and return true
    g_assert_true (c1_lambda->transition (Event::STOP));

    // after STOP all events are SLEEPING
    g_assert (c1_lambda->getState () == Event::SLEEPING);
    g_assert (c1_port1->getState () == Event::SLEEPING);
    g_assert (c1_port2->getState () == Event::SLEEPING);
    g_assert (c1_p1->getState () == Event::SLEEPING);

    delete fmt;
  }

  // @c1_lambda: STOP from state PAUSED.
  {
    Formatter *fmt;
    Document *doc;
    tests_parse_and_start (&fmt, &doc, "\
<ncl>\n\
<body>\n\
  <port id='p1' component='c1'/>\n\
  <context id='c1'>\n\
    <property name='p1' value='0'/>\n\
    <port id='port1' component='m1'/>\n\
    <port id='port2' component='m2'/>\n\
    <media id='m1'/>\n\
    <media id='m2'/>\n\
  </context>\n\
</body>\n\
</ncl>");

    Context *body = cast (Context *, doc->getRoot ());
    g_assert_nonnull (body);
    Event *body_lambda = body->getLambda ();
    g_assert_nonnull (body_lambda);

    Context *c1 = cast (Context *, body->getChildById ("c1"));
    g_assert_nonnull (c1);
    Event *c1_lambda = c1->getLambda ();
    g_assert_nonnull (c1_lambda);

    Event *c1_p1 = c1->getAttributionEvent ("p1");
    g_assert_nonnull (c1_p1);

    auto iter = c1->getPorts ()->begin ();
    Event *c1_port1 = *iter;
    g_assert_nonnull (c1_port1);
    iter++;
    Event *c1_port2 = *iter;
    g_assert_nonnull (c1_port2);

    // --------------------------------
    // check start document

    // when document is started, only the body@lambda is OCCURING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (c1_p1->getState () == Event::SLEEPING);
    g_assert (c1_port1->getState () == Event::SLEEPING);
    g_assert (c1_port2->getState () == Event::SLEEPING);
    g_assert (c1_lambda->getState () == Event::SLEEPING);

    // after advance time, c1@lambda is OCCURRING, and its
    // anchors and properties are SLEEPING
    fmt->sendTick (0, 0, 0);
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (c1_lambda->getState () == Event::OCCURRING);
    g_assert (c1_port1->getState () == Event::SLEEPING);
    g_assert (c1_port2->getState () == Event::SLEEPING);
    g_assert (c1_p1->getState () == Event::SLEEPING);

    // after advance time, c1@lambda and its anchors
    // are OCCURRING, and its properties are SLEEPING
    fmt->sendTick (0, 0, 0);
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (c1_lambda->getState () == Event::OCCURRING);
    g_assert (c1_port1->getState () == Event::OCCURRING);
    g_assert (c1_port2->getState () == Event::OCCURRING);
    g_assert (c1_p1->getState () == Event::SLEEPING);

    // --------------------------------
    // main check

    // PAUSE is done and return true
    g_assert_true (c1_lambda->transition (Event::PAUSE));

    // after PAUSE all events are PAUSED
    g_assert (c1_lambda->getState () == Event::PAUSED);
    g_assert (c1_port1->getState () == Event::PAUSED);
    g_assert (c1_port2->getState () == Event::PAUSED);
    g_assert (c1_p1->getState () == Event::SLEEPING);

    // STOP is done and return true
    g_assert_true (c1_lambda->transition (Event::STOP));

    // after STOP all events are still SLEEPING
    g_assert (c1_lambda->getState () == Event::SLEEPING);
    g_assert (c1_port1->getState () == Event::SLEEPING);
    g_assert (c1_port2->getState () == Event::SLEEPING);
    g_assert (c1_p1->getState () == Event::SLEEPING);

    delete fmt;
  }

  // @c1_lambda: STOP from state SLEEPING.

  {
    Formatter *fmt;
    Document *doc;
    tests_parse_and_start (&fmt, &doc, "\
<ncl>\n\
<body>\n\
  <port id='p1' component='c1'/>\n\
  <context id='c1'>\n\
    <property name='p1' value='0'/>\n\
    <port id='port1' component='m1'/>\n\
    <port id='port2' component='m2'/>\n\
    <media id='m1'/>\n\
    <media id='m2'/>\n\
  </context>\n\
</body>\n\
</ncl>");

    Context *body = cast (Context *, doc->getRoot ());
    g_assert_nonnull (body);
    Event *body_lambda = body->getLambda ();
    g_assert_nonnull (body_lambda);

    Context *c1 = cast (Context *, body->getChildById ("c1"));
    g_assert_nonnull (c1);
    Event *c1_lambda = c1->getLambda ();
    g_assert_nonnull (c1_lambda);

    Event *c1_p1 = c1->getAttributionEvent ("p1");
    g_assert_nonnull (c1_p1);

    auto iter = c1->getPorts ()->begin ();
    Event *c1_port1 = *iter;
    g_assert_nonnull (c1_port1);
    iter++;
    Event *c1_port2 = *iter;
    g_assert_nonnull (c1_port2);

    // --------------------------------
    // check start document

    // when document is started, only the body@lambda is OCCURING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (c1_p1->getState () == Event::SLEEPING);
    g_assert (c1_port1->getState () == Event::SLEEPING);
    g_assert (c1_port2->getState () == Event::SLEEPING);
    g_assert (c1_lambda->getState () == Event::SLEEPING);

    // after advance time, c1@lambda is OCCURRING, and its
    // anchors and properties are SLEEPING
    fmt->sendTick (0, 0, 0);
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (c1_lambda->getState () == Event::OCCURRING);
    g_assert (c1_port1->getState () == Event::SLEEPING);
    g_assert (c1_port2->getState () == Event::SLEEPING);
    g_assert (c1_p1->getState () == Event::SLEEPING);

    // after advance time, c1@lambda and its anchors
    // are OCCURRING, and its properties are SLEEPING
    fmt->sendTick (0, 0, 0);
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (c1_lambda->getState () == Event::OCCURRING);
    g_assert (c1_port1->getState () == Event::OCCURRING);
    g_assert (c1_port2->getState () == Event::OCCURRING);
    g_assert (c1_p1->getState () == Event::SLEEPING);

    // --------------------------------
    // main check

    // STOP is done and return true
    g_assert (c1_lambda->transition (Event::STOP));

    // after STOP all events SLEEPING
    g_assert (c1_lambda->getState () == Event::SLEEPING);
    g_assert (c1_port1->getState () == Event::SLEEPING);
    g_assert (c1_port2->getState () == Event::SLEEPING);
    g_assert (c1_p1->getState () == Event::SLEEPING);

    // STOP is not done and return false
    g_assert_false (c1_lambda->transition (Event::STOP));

    // after STOP all events still SLEEPING
    g_assert (c1_lambda->getState () == Event::SLEEPING);
    g_assert (c1_port1->getState () == Event::SLEEPING);
    g_assert (c1_port2->getState () == Event::SLEEPING);
    g_assert (c1_p1->getState () == Event::SLEEPING);

    delete fmt;
  }

  exit (EXIT_SUCCESS);
}
