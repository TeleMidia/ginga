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
  // Port to media object.
  {
    Formatter *fmt;
    Document *doc;
    tests_parse_and_start (&fmt, &doc, "\
<ncl>\n\
<body>\n\
  <port id='port0a' component='m1'/>\n\
  <media id='m1'/>\n\
</body>\n\
</ncl>");

    Context *body = cast (Context *, doc->getRoot ());
    g_assert_nonnull (body);
    Event *body_lambda = body->getLambda ();
    g_assert_nonnull (body_lambda);

    Media *m1 = cast (Media *, doc->getObjectById ("m1"));
    g_assert_nonnull (m1);
    Event *m1_lambda = m1->getLambda ();
    g_assert_nonnull (m1_lambda);

    // --------------------------------
    // check start document

    // When document is started, only the body@lambda is OCCURING.
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);

    // after advance time
    fmt->sendTick (0, 0, 0);

    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::OCCURRING);

    delete fmt;
  }

  // Port to a nested media object.
  {
    Formatter *fmt;
    Document *doc;
    tests_parse_and_start (&fmt, &doc, "\
<ncl>\n\
<body>\n\
  <port id='port0a' component='m1'/>\n\
  <port id='port0b' component='c1' interface='port1'/>\n\
  <media id='m1'/>\n\
  <context id='c1'>\n\
    <port id='port1' component='m2'/>\n\
    <media id='m2'/>\n\
  </context>\n\
</body>\n\
</ncl>");

    Context *body = cast (Context *, doc->getRoot ());
    g_assert_nonnull (body);
    Event *body_lambda = body->getLambda ();
    g_assert_nonnull (body_lambda);

    Context *c1 = cast (Context *, doc->getObjectById ("c1"));
    g_assert_nonnull (c1);
    Event *c1_lambda = c1->getLambda ();
    g_assert_nonnull (c1_lambda);

    Media *m1 = cast (Media *, doc->getObjectById ("m1"));
    g_assert_nonnull (m1);
    Event *m1_lambda = m1->getLambda ();
    g_assert_nonnull (m1_lambda);

    Media *m2 = cast (Media *, doc->getObjectById ("m2"));
    g_assert_nonnull (m2);
    Event *m2_lambda = m2->getLambda ();
    g_assert_nonnull (m2_lambda);

    // --------------------------------
    // check start document

    // When document is started, only the body@lambda is OCCURRING.
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (c1_lambda->getState () == Event::SLEEPING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);
    g_assert (m2_lambda->getState () == Event::SLEEPING);

    // After advancing time, m2 begins, because it is pointed by the started
    // port.
    fmt->sendTick (0, 0, 0);

    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::OCCURRING);
    g_assert (c1_lambda->getState () == Event::OCCURRING);
    g_assert (m2_lambda->getState () == Event::OCCURRING);

    delete fmt;
  }

  // Port to a context.
  {
    Formatter *fmt;
    Document *doc;
    tests_parse_and_start (&fmt, &doc, "\
<ncl>\n\
<body>\n\
  <port id='port0a' component='m1'/>\n\
  <port id='port0b' component='c1'/>\n\
  <media id='m1'/>\n\
  <context id='c1'>\n\
    <port id='port1' component='m2'/>\n\
    <media id='m2'/>\n\
  </context>\n\
</body>\n\
</ncl>");

    Context *body = cast (Context *, doc->getRoot ());
    g_assert_nonnull (body);
    Event *body_lambda = body->getLambda ();
    g_assert_nonnull (body_lambda);

    Context *c1 = cast (Context *, doc->getObjectById ("c1"));
    g_assert_nonnull (c1);
    Event *c1_lambda = c1->getLambda ();
    g_assert_nonnull (c1_lambda);

    Media *m1 = cast (Media *, doc->getObjectById ("m1"));
    g_assert_nonnull (m1);
    Event *m1_lambda = m1->getLambda ();
    g_assert_nonnull (m1_lambda);

    Media *m2 = cast (Media *, doc->getObjectById ("m2"));
    g_assert_nonnull (m2);
    Event *m2_lambda = m2->getLambda ();
    g_assert_nonnull (m2_lambda);

    // --------------------------------
    // check start document

    // When the document is started, only the body@lambda is OCCURRING.
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);
    g_assert (c1_lambda->getState () == Event::SLEEPING);
    g_assert (m2_lambda->getState () == Event::SLEEPING);

    // After advancing time, m2 does not begin, because it is not pointed by
    // the started port.
    fmt->sendTick (0, 0, 0);

    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::OCCURRING);
    g_assert (c1_lambda->getState () == Event::OCCURRING);
    g_assert (m2_lambda->getState () == Event::SLEEPING);

    fmt->sendTick (0, 0, 0);

    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::OCCURRING);
    g_assert (c1_lambda->getState () == Event::OCCURRING);
    g_assert (m2_lambda->getState () == Event::OCCURRING);

    delete fmt;
  }

  // Port over a nested context.
  {
    Formatter *fmt;
    Document *doc;
    tests_parse_and_start (&fmt, &doc, "\
<ncl>\n\
<body>\n\
  <port id='port0' component='m1'/>\n\
  <port id='port1' component='c1' interface='port2'/>\n\
  <media id='m1'/>\n\
  <context id='c1'>\n\
    <port id='port2' component='c2'/>\n\
    <property name='prop' value='0'/>\n\
    <context id='c2'>\n\
      <port id='port3' component='m2'/>\n\
      <property name='prop' value='0'/>\n\
      <media id='m2'/>\n\
    </context>\n\
  </context>\n\
</body>\n\
</ncl>");

    Context *body = cast (Context *, doc->getRoot ());
    g_assert_nonnull (body);
    Event *body_lambda = body->getLambda ();
    g_assert_nonnull (body_lambda);

    Context *c1 = cast (Context *, doc->getObjectById ("c1"));
    g_assert_nonnull (c1);
    Event *c1_lambda = c1->getLambda ();
    g_assert_nonnull (c1_lambda);
    Event *c1_port2 = *c1->getPorts ()->begin ();
    g_assert_nonnull (c1_port2);
    Event *c1_prop = c1->getAttributionEvent ("prop");
    g_assert_nonnull (c1_prop);

    Context *c2 = cast (Context *, doc->getObjectById ("c2"));
    g_assert_nonnull (c2);
    Event *c2_lambda = c2->getLambda ();
    g_assert_nonnull (c2_lambda);
    Event *c2_port3 = *c2->getPorts ()->begin ();
    g_assert_nonnull (c2_port3);
    Event *c2_prop = c2->getAttributionEvent ("prop");
    g_assert_nonnull (c2_prop);

    // --------------------------------
    // check start document

    // When document is started, only the body@lambda is OCCURRING.
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (c2_lambda->getState () == Event::SLEEPING);
    g_assert (c1_port2->getState () == Event::SLEEPING);
    g_assert (c1_prop->getState () == Event::SLEEPING);
    g_assert (c2_port3->getState () == Event::SLEEPING);
    g_assert (c2_prop->getState () == Event::SLEEPING);

    // After advancing time, c2@lambda is OCCURRING, and its anchors
    // and properties are SLEEPING.
    fmt->sendTick (0, 0, 0);

    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (c1_port2->getState () == Event::OCCURRING);
    g_assert (c1_prop->getState () == Event::SLEEPING);
    g_assert (c2_lambda->getState () == Event::OCCURRING);
    g_assert (c2_port3->getState () == Event::SLEEPING);
    g_assert (c2_prop->getState () == Event::SLEEPING);

    // After advancing time, c2@lambda and its anchors are OCCURRING, and
    // its properties are SLEEPING.
    fmt->sendTick (0, 0, 0);
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (c2_lambda->getState () == Event::OCCURRING);
    g_assert (c1_port2->getState () == Event::OCCURRING);
    g_assert (c1_prop->getState () == Event::SLEEPING);
    g_assert (c2_port3->getState () == Event::OCCURRING);
    g_assert (c2_prop->getState () == Event::SLEEPING);

    delete fmt;
  }

  exit (EXIT_SUCCESS);
}
