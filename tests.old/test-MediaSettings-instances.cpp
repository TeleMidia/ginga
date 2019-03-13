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
#include "iostream"

int
main (void)
{
  // two MediaSettings instances in same context
  {
    Formatter *fmt;
    Document *doc;
    tests_parse_and_start (&fmt, &doc, "\
<ncl>\n\
  <head>\n\
    <connectorBase>\n\
      <causalConnector id='onBeginSet'>\n\
        <connectorParam name='var'/>\n\
        <simpleCondition role='onBegin'/>\n\
        <simpleAction role='set' value='$var'/>\n\
      </causalConnector>\n\
    </connectorBase>\n\
  </head>\n\
  <body>\n\
    <port id='start1' component='m1'/>\n\
    <media id='m1'/>\n\
    <media id='settings1' type='application/x-ginga-settings'>\n\
      <property name='p1' value='v1'/>\n\
    </media>\n\
    <media id='settings2' type='application/x-ginga-settings'>\n\
      <property name='p2' value='v2'/>\n\
    </media>\n\
    <link xconnector='onBeginSet'>\n\
      <bind role='onBegin' component='m1'/>\n\
      <bind role='set' component='settings2' interface='p1'>\n\
        <bindParam name='var' value='new1'/>\n\
      </bind>\n\
      <bind role='set' component='settings2' interface='p2'>\n\
        <bindParam name='var' value='new2'/>\n\
      </bind>\n\
    </link>\n\
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

    MediaSettings *s1
        = cast (MediaSettings *, doc->getObjectByIdOrAlias ("settings1"));
    g_assert_nonnull (s1);
    Event *s1_lambda = s1->getLambda ();
    g_assert_nonnull (s1_lambda);

    MediaSettings *s2
        = cast (MediaSettings *, doc->getObjectByIdOrAlias ("settings2"));
    g_assert_nonnull (s2);
    Event *s2_lambda = s2->getLambda ();
    g_assert_nonnull (s2_lambda);

    // --------------------------------
    // check start document

    // when document is started, only the body@lambda is OCCURING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);
    g_assert (s1_lambda->getState () == Event::OCCURRING);
    g_assert (s2_lambda->getState () == Event::OCCURRING);

    g_assert (s1->getProperty ("p1") == "v1");
    g_assert (s1->getProperty ("p2") == "v2");
    g_assert (s2->getProperty ("p1") == "v1");
    g_assert (s2->getProperty ("p2") == "v2");

    // advance time
    fmt->sendTick (0, 0, 0);

    // when advance time m1 and m2 are OCCURRING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::OCCURRING);
    g_assert (s1_lambda->getState () == Event::OCCURRING);
    g_assert (s2_lambda->getState () == Event::OCCURRING);

    // --------------------------------
    // main check

    g_assert (s1->getProperty ("p1") == "new1");
    g_assert (s1->getProperty ("p2") == "new2");
    g_assert (s2->getProperty ("p1") == "new1");
    g_assert (s2->getProperty ("p2") == "new2");

    delete fmt;
  }

  // two MediaSettings instances in diferent contexts
  {
    Formatter *fmt;
    Document *doc;
    tests_parse_and_start (&fmt, &doc, "\
<ncl>\n\
  <head>\n\
    <connectorBase>\n\
      <causalConnector id='onBeginSet'>\n\
        <connectorParam name='var'/>\n\
        <simpleCondition role='onBegin'/>\n\
        <simpleAction role='set' value='$var'/>\n\
      </causalConnector>\n\
    </connectorBase>\n\
  </head>\n\
  <body>\n\
    <port id='start' component='ctx1'/>\n\
    <media id='settings1' type='application/x-ginga-settings'>\n\
      <property name='p1' value='v1'/>\n\
    </media>\n\
    <context id='ctx1'>\n\
      <port id='start1' component='m1'/>\n\
      <media id='m1'/>\n\
      <media id='settings2' type='application/x-ginga-settings'>\n\
        <property name='p2' value='v2'/>\n\
      </media>\n\
      <link xconnector='onBeginSet'>\n\
        <bind role='onBegin' component='m1'/>\n\
        <bind role='set' component='settings2' interface='p1'>\n\
          <bindParam name='var' value='new1'/>\n\
        </bind>\n\
        <bind role='set' component='settings2' interface='p2'>\n\
          <bindParam name='var' value='new2'/>\n\
        </bind>\n\
      </link>\n\
    </context>\n\
  </body>\n\
</ncl>");

    Context *body = cast (Context *, doc->getRoot ());
    g_assert_nonnull (body);
    Event *body_lambda = body->getLambda ();
    g_assert_nonnull (body_lambda);

    Context *ctx1 = cast (Context *, doc->getObjectById ("ctx1"));
    g_assert_nonnull (ctx1);
    Event *ctx1_lambda = ctx1->getLambda ();
    g_assert_nonnull (ctx1_lambda);

    Media *m1 = cast (Media *, doc->getObjectById ("m1"));
    g_assert_nonnull (m1);
    Event *m1_lambda = m1->getLambda ();
    g_assert_nonnull (m1_lambda);

    MediaSettings *s1
        = cast (MediaSettings *, doc->getObjectByIdOrAlias ("settings1"));
    g_assert_nonnull (s1);
    Event *s1_lambda = s1->getLambda ();
    g_assert_nonnull (s1_lambda);

    MediaSettings *s2
        = cast (MediaSettings *, doc->getObjectByIdOrAlias ("settings2"));
    g_assert_nonnull (s2);
    Event *s2_lambda = s2->getLambda ();
    g_assert_nonnull (s2_lambda);

    // --------------------------------
    // check start document

    // when document is started, only the body@lambda is OCCURING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (ctx1_lambda->getState () == Event::SLEEPING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);
    g_assert (s1_lambda->getState () == Event::OCCURRING);
    g_assert (s2_lambda->getState () == Event::OCCURRING);

    g_assert (s1->getProperty ("p1") == "v1");
    g_assert (s1->getProperty ("p2") == "v2");
    g_assert (s2->getProperty ("p1") == "v1");
    g_assert (s2->getProperty ("p2") == "v2");

    // advance time
    fmt->sendTick (0, 0, 0);

    // when advance time m1 and m2 are OCCURRING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (ctx1_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);
    g_assert (s1_lambda->getState () == Event::OCCURRING);
    g_assert (s2_lambda->getState () == Event::OCCURRING);

    // advance time
    fmt->sendTick (0, 0, 0);

    // when advance time m1 and m2 are OCCURRING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (ctx1_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::OCCURRING);
    g_assert (s1_lambda->getState () == Event::OCCURRING);
    g_assert (s2_lambda->getState () == Event::OCCURRING);

    // --------------------------------
    // main check

    g_assert (s1->getProperty ("p1") == "new1");
    g_assert (s1->getProperty ("p2") == "new2");
    g_assert (s2->getProperty ("p1") == "new1");
    g_assert (s2->getProperty ("p2") == "new2");

    delete fmt;
  }

  exit (EXIT_SUCCESS);
}
