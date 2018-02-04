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

  // @lambda: ABORT from state OCCURRING.
  {
    Formatter *fmt;
    Document *doc;
    PARSE_AND_START (&fmt, &doc, "\
<ncl>\n\
<body>\n\
  <context id='c'>\n\
    <property name='p1' value='0'/>\n\
    <port id='port1' component='m1'/>\n\
    <port id='port2' component='m2'/>\n\
    <media id='m1'/>\n\
    <media id='m2'/>\n\
  </context>\n\
</body>\n\
</ncl>");

    Context *c = cast (Context *, doc->getObjectById ("c"));
    g_assert_nonnull (c);

    Event *lambda = c->getLambda ();
    g_assert_nonnull (lambda);
    auto iter = c->getPorts ()->begin ();
    Event *port1 = *iter;
    g_assert_nonnull (port1);
    iter++;
    Event *port2 = *iter;
    g_assert_nonnull (port2);
    Event *p1 = c->getAttributionEvent ("p1");

    // before START lambda, anchors events an properties
    // events are in SLEEPING
    g_assert (lambda->getState () == Event::SLEEPING);
    g_assert (port1->getState () == Event::SLEEPING);
    g_assert (port2->getState () == Event::SLEEPING);
    g_assert (p1->getState () == Event::SLEEPING);

    // START
    g_assert (lambda->transition (Event::START));

    // after START, lambda is in OCCURRING and
    // anchors are in SLEEPING
    g_assert (lambda->getState () == Event::OCCURRING);
    g_assert (port1->getState () == Event::SLEEPING);
    g_assert (port2->getState () == Event::SLEEPING);
    g_assert (p1->getState () == Event::SLEEPING);

    // advance time
    fmt->sendTick (1, 1, 1);

    // when advance time, anchors events go to OCCURRING
    // and properties events are SLEEPING
    g_assert (lambda->getState () == Event::OCCURRING);
    g_assert (port1->getState () == Event::OCCURRING);
    g_assert (port2->getState () == Event::OCCURRING);
    g_assert (p1->getState () == Event::SLEEPING);

    // --------------------------------
    // main check

    // ABORT is done and return true
    g_assert_true (lambda->transition (Event::ABORT));

    // after ABORT all events are SLEEPING
    g_assert (lambda->getState () == Event::SLEEPING);
    g_assert (port1->getState () == Event::SLEEPING);
    g_assert (port2->getState () == Event::SLEEPING);
    g_assert (p1->getState () == Event::SLEEPING);

    delete fmt;
  }

  // @lambda: ABORT from state PAUSED.
  {
    Formatter *fmt;
    Document *doc;
    PARSE_AND_START (&fmt, &doc, "\
<ncl>\n\
<body>\n\
  <context id='c'>\n\
    <property name='p1' value='0'/>\n\
    <port id='port1' component='m1'/>\n\
    <port id='port2' component='m2'/>\n\
    <media id='m1'/>\n\
    <media id='m2'/>\n\
  </context>\n\
</body>\n\
</ncl>");

    Context *c = cast (Context *, doc->getObjectById ("c"));
    g_assert_nonnull (c);

    Event *lambda = c->getLambda ();
    g_assert_nonnull (lambda);
    auto iter = c->getPorts ()->begin ();
    Event *port1 = *iter;
    g_assert_nonnull (port1);
    iter++;
    Event *port2 = *iter;
    g_assert_nonnull (port2);
    Event *p1 = c->getAttributionEvent ("p1");

    // before START lambda, anchors events an properties
    // events are in SLEEPING
    g_assert (lambda->getState () == Event::SLEEPING);
    g_assert (port1->getState () == Event::SLEEPING);
    g_assert (port2->getState () == Event::SLEEPING);
    g_assert (p1->getState () == Event::SLEEPING);

    // START
    g_assert (lambda->transition (Event::START));

    // after START, lambda is in OCCURRING and
    // anchors are in SLEEPING
    g_assert (lambda->getState () == Event::OCCURRING);
    g_assert (port1->getState () == Event::SLEEPING);
    g_assert (port2->getState () == Event::SLEEPING);
    g_assert (p1->getState () == Event::SLEEPING);

    // advance time
    fmt->sendTick (1, 1, 1);

    // when advance time, anchors events go to OCCURRING
    // and properties events are SLEEPING
    g_assert (lambda->getState () == Event::OCCURRING);
    g_assert (port1->getState () == Event::OCCURRING);
    g_assert (port2->getState () == Event::OCCURRING);
    g_assert (p1->getState () == Event::SLEEPING);

    // --------------------------------
    // setup
    g_assert_true (lambda->transition (Event::PAUSE));
    g_assert (lambda->getState () == Event::PAUSED);
    g_assert (port1->getState () == Event::PAUSED);
    g_assert (port2->getState () == Event::PAUSED);
    g_assert (p1->getState () == Event::SLEEPING);

    // --------------------------------
    // main check

    // ABORT is done and return true
    g_assert_true (lambda->transition (Event::ABORT));

    // after ABORT all events are SLEEPING
    g_assert (lambda->getState () == Event::SLEEPING);
    g_assert (port1->getState () == Event::SLEEPING);
    g_assert (port2->getState () == Event::SLEEPING);
    g_assert (p1->getState () == Event::SLEEPING);

    delete fmt;
  }

  // @lambda: ABORT from state SLEEPING.
  {
    Formatter *fmt;
    Document *doc;
    PARSE_AND_START (&fmt, &doc, "\
<ncl>\n\
<body>\n\
  <context id='c'/>\n\
</body>\n\
</ncl>");

    Context *c = cast (Context *, doc->getObjectById ("c"));
    g_assert_nonnull (c);

    Event *lambda = c->getLambda ();
    g_assert_nonnull (lambda);

    g_assert (lambda->getState () == Event::SLEEPING);

    // ABORT is not done and return false
    g_assert_false (lambda->transition (Event::ABORT));
    g_assert (lambda->getState () == Event::SLEEPING);

    delete fmt;
  }

  // Attribution events ----------------------------------------------------
}
