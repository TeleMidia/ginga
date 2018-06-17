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
  // Port to media object
  {
    Formatter *fmt;
    Document *doc;
    tests_parse_and_start (&fmt, &doc, "\
<ncl>\n\
<body>\n\
  <port id='port0' component='m1'/>\n\
  <media id='m1'/>\n\
  <media id='m2'/>\n\
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

    Media *m2 = cast (Media *, doc->getObjectById ("m2"));
    g_assert_nonnull (m2);
    Event *m2_lambda = m2->getLambda ();
    g_assert_nonnull (m2_lambda);

    // --------------------------------
    // check start document

    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);
    g_assert (m2_lambda->getState () == Event::SLEEPING);

    fmt->sendTick (0, 0, 0);
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::OCCURRING);
    g_assert (m2_lambda->getState () == Event::SLEEPING);

    delete fmt;
  }

  // Port to a context
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
    <media id='m3'/>\n\
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

    Media *m3 = cast (Media *, doc->getObjectById ("m3"));
    g_assert_nonnull (m3);
    Event *m3_lambda = m3->getLambda ();
    g_assert_nonnull (m3_lambda);

    // --------------------------------
    // check start document

    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);
    g_assert (c1_lambda->getState () == Event::SLEEPING);
    g_assert (m2_lambda->getState () == Event::SLEEPING);

    fmt->sendTick (0, 0, 0);

    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::OCCURRING);
    g_assert (c1_lambda->getState () == Event::OCCURRING);
    g_assert (m2_lambda->getState () == Event::SLEEPING);
    g_assert (m3_lambda->getState () == Event::SLEEPING);

    fmt->sendTick (0, 0, 0);

    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::OCCURRING);
    g_assert (c1_lambda->getState () == Event::OCCURRING);
    g_assert (m2_lambda->getState () == Event::OCCURRING);
    g_assert (m3_lambda->getState () == Event::SLEEPING);

    delete fmt;
  }

  // Port to a switch
  {
    Formatter *fmt;
    Document *doc;
    tests_parse_and_start (&fmt, &doc, "\
<ncl>\n\
<head>\n\
  <ruleBase>\n\
    <rule id='rVarIsX' var='var' value='x' comparator='eq'/>\n\
    <rule id='rVarIsY' var='var' value='y' comparator='eq'/>\n\
  </ruleBase>\n\
</head>\n\
<body>\n\
  <port id='pSwitch' component='switch'/>\n\
  <media id='settings' type='application/x-ginga-settings'>\n\
    <property name='var' value='x'/>\n\
  </media>\n\
  <switch id='switch'>\n\
    <bindRule constituent='m1' rule='rVarIsX'/>\n\
    <bindRule constituent='m2' rule='rVarIsY'/>\n\
    <media id='m1'/>\n\
    <media id='m2'/>\n\
  </switch>\n\
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

    Media *m2 = cast (Media *, doc->getObjectById ("m2"));
    g_assert_nonnull (m2);
    Event *m2_lambda = m2->getLambda ();
    g_assert_nonnull (m2_lambda);

    // --------------------------------
    // check start document

    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);
    g_assert (m2_lambda->getState () == Event::SLEEPING);

    fmt->sendTick (0, 0, 0);

    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::OCCURRING);
    g_assert (m2_lambda->getState () == Event::SLEEPING);

    delete fmt;
  }

  exit (EXIT_SUCCESS);
}
