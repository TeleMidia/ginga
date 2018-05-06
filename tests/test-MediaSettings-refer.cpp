/* Copyright (C) 2006-2018 PUC-Rio/Laboratorio TeleMidia

This file is part of Ginga (Ginga-NCL).

Ginga is free software: you can yistribute it and/or modify it
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

#define PASS(nclBuffer)                                                    \
  G_STMT_START                                                             \
  {                                                                        \
    Formatter *fmt;                                                        \
    Document *doc;                                                         \
                                                                           \
    tests_parse_and_start (&fmt, &doc, nclBuffer);                         \
                                                                           \
    Context *body = cast (Context *, doc->getRoot ());                     \
    g_assert_nonnull (body);                                               \
    Event *body_lambda = body->getLambda ();                               \
    g_assert_nonnull (body_lambda);                                        \
                                                                           \
    Media *m1 = cast (Media *, doc->getObjectByIdOrAlias ("m1"));          \
    g_assert_nonnull (m1);                                                 \
    Event *m1_lambda = m1->getLambda ();                                   \
    g_assert_nonnull (m1_lambda);                                          \
                                                                           \
    Media *r1 = cast (Media *, doc->getObjectByIdOrAlias ("r1"));          \
    g_assert_nonnull (r1);                                                 \
    Event *r1_lambda = r1->getLambda ();                                   \
    g_assert_nonnull (r1_lambda);                                          \
                                                                           \
    Media *m2 = cast (Media *, doc->getObjectById ("m2"));                 \
    g_assert_nonnull (m1);                                                 \
    Event *m2_lambda = m2->getLambda ();                                   \
    g_assert_nonnull (m2_lambda);                                          \
                                                                           \
    /* -------------------------------- */                                 \
    /* check start document */                                             \
    g_assert (body_lambda->getState () == Event::OCCURRING);               \
    g_assert (m1_lambda->getState () == Event::OCCURRING);                 \
    g_assert (r1_lambda->getState () == Event::OCCURRING);                 \
    g_assert (m2_lambda->getState () == Event::SLEEPING);                  \
                                                                           \
    g_assert (m1->getProperty ("uri") == r1->getProperty ("uri"));         \
    g_assert (m1->getProperty ("x") == r1->getProperty ("x"));             \
    g_assert (m1->getProperty ("x") == "z");                               \
    /* -------------------------------- */                                 \
    /* main check */                                                       \
                                                                           \
    /* when start document, m1 is OCCURRING */                             \
    fmt->sendTick (0 * GINGA_SECOND, 0 * GINGA_SECOND, 0);                 \
    fmt->sendTick (0 * GINGA_SECOND, 0 * GINGA_SECOND, 0);                 \
    g_assert (body_lambda->getState () == Event::OCCURRING);               \
    g_assert (m1_lambda->getState () == Event::OCCURRING);                 \
    g_assert (r1_lambda->getState () == Event::OCCURRING);                 \
    g_assert (m2_lambda->getState () == Event::OCCURRING);                 \
                                                                           \
    g_assert (m1->getProperty ("x") == r1->getProperty ("x"));             \
    g_assert (m1->getProperty ("x") == "k");                               \
                                                                           \
    delete fmt;                                                            \
  }                                                                        \
  G_STMT_END

int
main (void)
{

  // referred object: defined first
  // reference: defined after
  // action set over: referred object
  // link onEndAttribution: referred object
  PASS ("<ncl>\n\
  <head>\n\
    <connectorBase>\n\
      <causalConnector id='onBeginSet'>\n\
        <simpleCondition role='onBegin'/>\n\
        <simpleAction role='set' value='$var'/>\n\
      </causalConnector>\n\
      <causalConnector id='onEndAttributionStart'>\n\
        <simpleCondition role='onEndAttribution'/>\n\
        <simpleAction role='start'/>\n\
      </causalConnector>\n\
    </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start' component='m0'/>\n\
    <media id='m0'/>\n\
    <media id='m1' type='application/x-ginga-settings'>\n\
      <property name='x' value='y'/>\n\
    </media>\n\
    <media id='r1' refer='m1'>\n\
      <property name='x' value='z'/>\n\
    </media>\n\
    <media id='m2'/>\n\
    <link xconnector='onBeginSet'>\n\
      <bind role='onBegin' component='m0'/>\n\
      <bind role='set' component='m1' interface='x'>\n\
        <bindParam name='var' value='k'/>\n\
      </bind>\n\
    </link>\n\
    <link xconnector='onEndAttributionStart'>\n\
      <bind role='onEndAttribution' component='m1' interface='x'/>\n\
      <bind role='start' component='m2'/>\n\
    </link>\n\
  </body>\n\
</ncl>\n");
  // referred object: defined first
  // reference: defined after
  // action set over: referred object
  // link onEndAttribution: reference
  PASS ("<ncl>\n\
  <head>\n\
    <connectorBase>\n\
      <causalConnector id='onBeginSet'>\n\
        <simpleCondition role='onBegin'/>\n\
        <simpleAction role='set' value='$var'/>\n\
      </causalConnector>\n\
      <causalConnector id='onEndAttributionStart'>\n\
        <simpleCondition role='onEndAttribution'/>\n\
        <simpleAction role='start'/>\n\
      </causalConnector>\n\
    </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start' component='m0'/>\n\
    <media id='m0'/>\n\
    <media id='m1' type='application/x-ginga-settings'>\n\
      <property name='x' value='y'/>\n\
    </media>\n\
    <media id='r1' refer='m1'>\n\
      <property name='x' value='z'/>\n\
    </media>\n\
    <media id='m2'/>\n\
    <link xconnector='onBeginSet'>\n\
      <bind role='onBegin' component='m0'/>\n\
      <bind role='set' component='m1' interface='x'>\n\
        <bindParam name='var' value='k'/>\n\
      </bind>\n\
    </link>\n\
    <link xconnector='onEndAttributionStart'>\n\
      <bind role='onEndAttribution' component='m1' interface='x'/>\n\
      <bind role='start' component='m2'/>\n\
    </link>\n\
  </body>\n\
</ncl>\n");
  // referred object: defined first
  // reference: defined after
  // action set over: reference
  // link onEndAttribution: referred object
  PASS ("<ncl>\n\
  <head>\n\
    <connectorBase>\n\
      <causalConnector id='onBeginSet'>\n\
        <simpleCondition role='onBegin'/>\n\
        <simpleAction role='set' value='$var'/>\n\
      </causalConnector>\n\
      <causalConnector id='onEndAttributionStart'>\n\
        <simpleCondition role='onEndAttribution'/>\n\
        <simpleAction role='start'/>\n\
      </causalConnector>\n\
    </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start' component='m0'/>\n\
    <media id='m0'/>\n\
    <media id='m1' type='application/x-ginga-settings'>\n\
      <property name='x' value='y'/>\n\
    </media>\n\
    <media id='r1' refer='m1'>\n\
      <property name='x' value='z'/>\n\
    </media>\n\
    <media id='m2'/>\n\
    <link xconnector='onBeginSet'>\n\
      <bind role='onBegin' component='m0'/>\n\
      <bind role='set' component='m1' interface='x'>\n\
        <bindParam name='var' value='k'/>\n\
      </bind>\n\
    </link>\n\
    <link xconnector='onEndAttributionStart'>\n\
      <bind role='onEndAttribution' component='m1' interface='x'/>\n\
      <bind role='start' component='m2'/>\n\
    </link>\n\
  </body>\n\
</ncl>\n");
  // referred object: defined first
  // reference: defined after
  // action set over: reference
  // link onEndAttribution: reference
  PASS ("<ncl>\n\
  <head>\n\
    <connectorBase>\n\
      <causalConnector id='onBeginSet'>\n\
        <simpleCondition role='onBegin'/>\n\
        <simpleAction role='set' value='$var'/>\n\
      </causalConnector>\n\
      <causalConnector id='onEndAttributionStart'>\n\
        <simpleCondition role='onEndAttribution'/>\n\
        <simpleAction role='start'/>\n\
      </causalConnector>\n\
    </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start' component='m0'/>\n\
    <media id='m0'/>\n\
    <media id='m1' type='application/x-ginga-settings'>\n\
      <property name='x' value='y'/>\n\
    </media>\n\
    <media id='r1' refer='m1'>\n\
      <property name='x' value='z'/>\n\
    </media>\n\
    <media id='m2'/>\n\
    <link xconnector='onBeginSet'>\n\
      <bind role='onBegin' component='m0'/>\n\
      <bind role='set' component='m1' interface='x'>\n\
        <bindParam name='var' value='k'/>\n\
      </bind>\n\
    </link>\n\
    <link xconnector='onEndAttributionStart'>\n\
      <bind role='onEndAttribution' component='m1' interface='x'/>\n\
      <bind role='start' component='m2'/>\n\
    </link>\n\
  </body>\n\
</ncl>\n");
  // referred object: defined after
  // reference: defined first
  // action set over: referred object
  // link onEndAttribution: referred object
  PASS ("<ncl>\n\
  <head>\n\
    <connectorBase>\n\
      <causalConnector id='onBeginSet'>\n\
        <simpleCondition role='onBegin'/>\n\
        <simpleAction role='set' value='$var'/>\n\
      </causalConnector>\n\
      <causalConnector id='onEndAttributionStart'>\n\
        <simpleCondition role='onEndAttribution'/>\n\
        <simpleAction role='start'/>\n\
      </causalConnector>\n\
    </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start' component='m0'/>\n\
    <media id='m0'/>\n\
    <media id='r1' refer='m1'>\n\
      <property name='x' value='y'/>\n\
    </media>\n\
    <media id='m1' type='application/x-ginga-settings'>\n\
      <property name='x' value='z'/>\n\
    </media>\n\
    <media id='m2'/>\n\
    <link xconnector='onBeginSet'>\n\
      <bind role='onBegin' component='m0'/>\n\
      <bind role='set' component='m1' interface='x'>\n\
        <bindParam name='var' value='k'/>\n\
      </bind>\n\
    </link>\n\
    <link xconnector='onEndAttributionStart'>\n\
      <bind role='onEndAttribution' component='m1' interface='x'/>\n\
      <bind role='start' component='m2'/>\n\
    </link>\n\
  </body>\n\
</ncl>\n");
  // referred object: defined after
  // reference: defined first
  // action set over: referred object
  // link onEndAttribution: reference
  PASS ("<ncl>\n\
  <head>\n\
    <connectorBase>\n\
      <causalConnector id='onBeginSet'>\n\
        <simpleCondition role='onBegin'/>\n\
        <simpleAction role='set' value='$var'/>\n\
      </causalConnector>\n\
      <causalConnector id='onEndAttributionStart'>\n\
        <simpleCondition role='onEndAttribution'/>\n\
        <simpleAction role='start'/>\n\
      </causalConnector>\n\
    </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start' component='m0'/>\n\
    <media id='m0'/>\n\
    <media id='r1' refer='m1'>\n\
      <property name='x' value='y'/>\n\
    </media>\n\
    <media id='m1' type='application/x-ginga-settings'>\n\
      <property name='x' value='z'/>\n\
    </media>\n\
    <media id='m2'/>\n\
    <link xconnector='onBeginSet'>\n\
      <bind role='onBegin' component='m0'/>\n\
      <bind role='set' component='m1' interface='x'>\n\
        <bindParam name='var' value='k'/>\n\
      </bind>\n\
    </link>\n\
    <link xconnector='onEndAttributionStart'>\n\
      <bind role='onEndAttribution' component='m1' interface='x'/>\n\
      <bind role='start' component='m2'/>\n\
    </link>\n\
  </body>\n\
</ncl>\n");
  // referred object: defined after
  // reference: defined first
  // action set over: reference
  // link onEndAttribution: referred object
  PASS ("<ncl>\n\
  <head>\n\
    <connectorBase>\n\
      <causalConnector id='onBeginSet'>\n\
        <simpleCondition role='onBegin'/>\n\
        <simpleAction role='set' value='$var'/>\n\
      </causalConnector>\n\
      <causalConnector id='onEndAttributionStart'>\n\
        <simpleCondition role='onEndAttribution'/>\n\
        <simpleAction role='start'/>\n\
      </causalConnector>\n\
    </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start' component='m0'/>\n\
    <media id='m0'/>\n\
    <media id='r1' refer='m1'>\n\
      <property name='x' value='y'/>\n\
    </media>\n\
    <media id='m1' type='application/x-ginga-settings'>\n\
      <property name='x' value='z'/>\n\
    </media>\n\
    <media id='m2'/>\n\
    <link xconnector='onBeginSet'>\n\
      <bind role='onBegin' component='m0'/>\n\
      <bind role='set' component='m1' interface='x'>\n\
        <bindParam name='var' value='k'/>\n\
      </bind>\n\
    </link>\n\
    <link xconnector='onEndAttributionStart'>\n\
      <bind role='onEndAttribution' component='m1' interface='x'/>\n\
      <bind role='start' component='m2'/>\n\
    </link>\n\
  </body>\n\
</ncl>\n");
  // referred object: defined after
  // reference: defined first
  // action set over: reference
  // link onEndAttribution: reference
  PASS ("<ncl>\n\
  <head>\n\
    <connectorBase>\n\
      <causalConnector id='onBeginSet'>\n\
        <simpleCondition role='onBegin'/>\n\
        <simpleAction role='set' value='$var'/>\n\
      </causalConnector>\n\
      <causalConnector id='onEndAttributionStart'>\n\
        <simpleCondition role='onEndAttribution'/>\n\
        <simpleAction role='start'/>\n\
      </causalConnector>\n\
    </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start' component='m0'/>\n\
    <media id='m0'/>\n\
    <media id='r1' refer='m1'>\n\
      <property name='x' value='y'/>\n\
    </media>\n\
    <media id='m1' type='application/x-ginga-settings'>\n\
      <property name='x' value='z'/>\n\
    </media>\n\
    <media id='m2'/>\n\
    <link xconnector='onBeginSet'>\n\
      <bind role='onBegin' component='m0'/>\n\
      <bind role='set' component='m1' interface='x'>\n\
        <bindParam name='var' value='k'/>\n\
      </bind>\n\
    </link>\n\
    <link xconnector='onEndAttributionStart'>\n\
      <bind role='onEndAttribution' component='m1' interface='x'/>\n\
      <bind role='start' component='m2'/>\n\
    </link>\n\
  </body>\n\
</ncl>\n");
  // referred object: defined first
  // reference: defined after in nested context
  // action set over: referred object
  // link onEndAttribution: referred object
  PASS ("<ncl>\n\
  <head>\n\
    <connectorBase>\n\
      <causalConnector id='onBeginSet'>\n\
        <simpleCondition role='onBegin'/>\n\
        <simpleAction role='set' value='$var'/>\n\
      </causalConnector>\n\
      <causalConnector id='onEndAttributionStart'>\n\
        <simpleCondition role='onEndAttribution'/>\n\
        <simpleAction role='start'/>\n\
      </causalConnector>\n\
    </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start' component='m0'/>\n\
    <media id='m0'/>\n\
    <media id='m1' type='application/x-ginga-settings'>\n\
      <property name='x' value='y'/>\n\
    </media>\n\
    <context id='ctx1'>\n\
      <port id='start1' component='r1'/>\n\
      <media id='r1' refer='m1'>\n\
        <property name='x' value='z'/>\n\
      </media>\n\
    </context>\n\
    <media id='m2'/>\n\
    <link xconnector='onBeginSet'>\n\
      <bind role='onBegin' component='m0'/>\n\
      <bind role='set' component='m1' interface='x'>\n\
        <bindParam name='var' value='k'/>\n\
      </bind>\n\
    </link>\n\
    <link xconnector='onEndAttributionStart'>\n\
      <bind role='onEndAttribution' component='m1' interface='x'/>\n\
      <bind role='start' component='m2'/>\n\
    </link>\n\
  </body>\n\
</ncl>\n");
  // referred object: defined first
  // reference: defined after in nested context
  // action set over: referred object
  // link onEndAttribution: reference
  PASS ("<ncl>\n\
  <head>\n\
    <connectorBase>\n\
      <causalConnector id='onBeginSet'>\n\
        <simpleCondition role='onBegin'/>\n\
        <simpleAction role='set' value='$var'/>\n\
      </causalConnector>\n\
      <causalConnector id='onEndAttributionStart'>\n\
        <simpleCondition role='onEndAttribution'/>\n\
        <simpleAction role='start'/>\n\
      </causalConnector>\n\
    </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start' component='m0'/>\n\
    <media id='m0'/>\n\
    <media id='m1' type='application/x-ginga-settings'>\n\
      <property name='x' value='y'/>\n\
    </media>\n\
    <context id='ctx1'>\n\
      <port id='start1' component='r1'/>\n\
      <media id='r1' refer='m1'>\n\
        <property name='x' value='z'/>\n\
      </media>\n\
    </context>\n\
    <media id='m2'/>\n\
    <link xconnector='onBeginSet'>\n\
      <bind role='onBegin' component='m0'/>\n\
      <bind role='set' component='m1' interface='x'>\n\
        <bindParam name='var' value='k'/>\n\
      </bind>\n\
    </link>\n\
    <link xconnector='onEndAttributionStart'>\n\
      <bind role='onEndAttribution' component='m1' interface='x'/>\n\
      <bind role='start' component='m2'/>\n\
    </link>\n\
  </body>\n\
</ncl>\n");
  // referred object: defined first
  // reference: defined after in nested context
  // action set over: reference
  // link onEndAttribution: referred object
  PASS ("<ncl>\n\
  <head>\n\
    <connectorBase>\n\
      <causalConnector id='onBeginSet'>\n\
        <simpleCondition role='onBegin'/>\n\
        <simpleAction role='set' value='$var'/>\n\
      </causalConnector>\n\
      <causalConnector id='onEndAttributionStart'>\n\
        <simpleCondition role='onEndAttribution'/>\n\
        <simpleAction role='start'/>\n\
      </causalConnector>\n\
    </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start' component='m0'/>\n\
    <media id='m0'/>\n\
    <media id='m1' type='application/x-ginga-settings'>\n\
      <property name='x' value='y'/>\n\
    </media>\n\
    <context id='ctx1'>\n\
      <port id='start1' component='r1'/>\n\
      <media id='r1' refer='m1'>\n\
        <property name='x' value='z'/>\n\
      </media>\n\
    </context>\n\
    <media id='m2'/>\n\
    <link xconnector='onBeginSet'>\n\
      <bind role='onBegin' component='m0'/>\n\
      <bind role='set' component='m1' interface='x'>\n\
        <bindParam name='var' value='k'/>\n\
      </bind>\n\
    </link>\n\
    <link xconnector='onEndAttributionStart'>\n\
      <bind role='onEndAttribution' component='m1' interface='x'/>\n\
      <bind role='start' component='m2'/>\n\
    </link>\n\
  </body>\n\
</ncl>\n");
  // referred object: defined first
  // reference: defined after in nested context
  // action set over: reference
  // link onEndAttribution: reference
  PASS ("<ncl>\n\
  <head>\n\
    <connectorBase>\n\
      <causalConnector id='onBeginSet'>\n\
        <simpleCondition role='onBegin'/>\n\
        <simpleAction role='set' value='$var'/>\n\
      </causalConnector>\n\
      <causalConnector id='onEndAttributionStart'>\n\
        <simpleCondition role='onEndAttribution'/>\n\
        <simpleAction role='start'/>\n\
      </causalConnector>\n\
    </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start' component='m0'/>\n\
    <media id='m0'/>\n\
    <media id='m1' type='application/x-ginga-settings'>\n\
      <property name='x' value='y'/>\n\
    </media>\n\
    <context id='ctx1'>\n\
      <port id='start1' component='r1'/>\n\
      <media id='r1' refer='m1'>\n\
        <property name='x' value='z'/>\n\
      </media>\n\
    </context>\n\
    <media id='m2'/>\n\
    <link xconnector='onBeginSet'>\n\
      <bind role='onBegin' component='m0'/>\n\
      <bind role='set' component='m1' interface='x'>\n\
        <bindParam name='var' value='k'/>\n\
      </bind>\n\
    </link>\n\
    <link xconnector='onEndAttributionStart'>\n\
      <bind role='onEndAttribution' component='m1' interface='x'/>\n\
      <bind role='start' component='m2'/>\n\
    </link>\n\
  </body>\n\
</ncl>\n");
  // referred object: defined first in nested context
  // reference: defined after
  // action set over: referred object
  // link onEndAttribution: referred object
  PASS ("<ncl>\n\
  <head>\n\
    <connectorBase>\n\
      <causalConnector id='onBeginSet'>\n\
        <simpleCondition role='onBegin'/>\n\
        <simpleAction role='set' value='$var'/>\n\
      </causalConnector>\n\
      <causalConnector id='onEndAttributionStart'>\n\
        <simpleCondition role='onEndAttribution'/>\n\
        <simpleAction role='start'/>\n\
      </causalConnector>\n\
    </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start' component='m0'/>\n\
    <media id='m0'/>\n\
    <context id='ctx1'>\n\
      <port id='start1' component='m1'/>\n\
      <media id='m1' type='application/x-ginga-settings'>\n\
        <property name='x' value='y'/>\n\
      </media>\n\
    </context>\n\
    <media id='r1' refer='m1'>\n\
      <property name='x' value='z'/>\n\
    </media>\n\
    <media id='m2'/>\n\
    <link xconnector='onBeginSet'>\n\
      <bind role='onBegin' component='m0'/>\n\
      <bind role='set' component='m1' interface='x'>\n\
        <bindParam name='var' value='k'/>\n\
      </bind>\n\
    </link>\n\
    <link xconnector='onEndAttributionStart'>\n\
      <bind role='onEndAttribution' component='m1' interface='x'/>\n\
      <bind role='start' component='m2'/>\n\
    </link>\n\
  </body>\n\
</ncl>\n");
  // referred object: defined first in nested context
  // reference: defined after
  // action set over: referred object
  // link onEndAttribution: reference
  PASS ("<ncl>\n\
  <head>\n\
    <connectorBase>\n\
      <causalConnector id='onBeginSet'>\n\
        <simpleCondition role='onBegin'/>\n\
        <simpleAction role='set' value='$var'/>\n\
      </causalConnector>\n\
      <causalConnector id='onEndAttributionStart'>\n\
        <simpleCondition role='onEndAttribution'/>\n\
        <simpleAction role='start'/>\n\
      </causalConnector>\n\
    </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start' component='m0'/>\n\
    <media id='m0'/>\n\
    <context id='ctx1'>\n\
      <port id='start1' component='m1'/>\n\
      <media id='m1' type='application/x-ginga-settings'>\n\
        <property name='x' value='y'/>\n\
      </media>\n\
    </context>\n\
    <media id='r1' refer='m1'>\n\
      <property name='x' value='z'/>\n\
    </media>\n\
    <media id='m2'/>\n\
    <link xconnector='onBeginSet'>\n\
      <bind role='onBegin' component='m0'/>\n\
      <bind role='set' component='m1' interface='x'>\n\
        <bindParam name='var' value='k'/>\n\
      </bind>\n\
    </link>\n\
    <link xconnector='onEndAttributionStart'>\n\
      <bind role='onEndAttribution' component='m1' interface='x'/>\n\
      <bind role='start' component='m2'/>\n\
    </link>\n\
  </body>\n\
</ncl>\n");
  // referred object: defined first in nested context
  // reference: defined after
  // action set over: reference
  // link onEndAttribution: referred object
  PASS ("<ncl>\n\
  <head>\n\
    <connectorBase>\n\
      <causalConnector id='onBeginSet'>\n\
        <simpleCondition role='onBegin'/>\n\
        <simpleAction role='set' value='$var'/>\n\
      </causalConnector>\n\
      <causalConnector id='onEndAttributionStart'>\n\
        <simpleCondition role='onEndAttribution'/>\n\
        <simpleAction role='start'/>\n\
      </causalConnector>\n\
    </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start' component='m0'/>\n\
    <media id='m0'/>\n\
    <context id='ctx1'>\n\
      <port id='start1' component='m1'/>\n\
      <media id='m1' type='application/x-ginga-settings'>\n\
        <property name='x' value='y'/>\n\
      </media>\n\
    </context>\n\
    <media id='r1' refer='m1'>\n\
      <property name='x' value='z'/>\n\
    </media>\n\
    <media id='m2'/>\n\
    <link xconnector='onBeginSet'>\n\
      <bind role='onBegin' component='m0'/>\n\
      <bind role='set' component='m1' interface='x'>\n\
        <bindParam name='var' value='k'/>\n\
      </bind>\n\
    </link>\n\
    <link xconnector='onEndAttributionStart'>\n\
      <bind role='onEndAttribution' component='m1' interface='x'/>\n\
      <bind role='start' component='m2'/>\n\
    </link>\n\
  </body>\n\
</ncl>\n");
  // referred object: defined first in nested context
  // reference: defined after
  // action set over: reference
  // link onEndAttribution: reference
  PASS ("<ncl>\n\
  <head>\n\
    <connectorBase>\n\
      <causalConnector id='onBeginSet'>\n\
        <simpleCondition role='onBegin'/>\n\
        <simpleAction role='set' value='$var'/>\n\
      </causalConnector>\n\
      <causalConnector id='onEndAttributionStart'>\n\
        <simpleCondition role='onEndAttribution'/>\n\
        <simpleAction role='start'/>\n\
      </causalConnector>\n\
    </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start' component='m0'/>\n\
    <media id='m0'/>\n\
    <context id='ctx1'>\n\
      <port id='start1' component='m1'/>\n\
      <media id='m1' type='application/x-ginga-settings'>\n\
        <property name='x' value='y'/>\n\
      </media>\n\
    </context>\n\
    <media id='r1' refer='m1'>\n\
      <property name='x' value='z'/>\n\
    </media>\n\
    <media id='m2'/>\n\
    <link xconnector='onBeginSet'>\n\
      <bind role='onBegin' component='m0'/>\n\
      <bind role='set' component='m1' interface='x'>\n\
        <bindParam name='var' value='k'/>\n\
      </bind>\n\
    </link>\n\
    <link xconnector='onEndAttributionStart'>\n\
      <bind role='onEndAttribution' component='m1' interface='x'/>\n\
      <bind role='start' component='m2'/>\n\
    </link>\n\
  </body>\n\
</ncl>\n");
  // referred object: defined first in nested context
  // reference: defined after in nested context
  // action set over: referred object
  // link onEndAttribution: referred object
  PASS ("<ncl>\n\
  <head>\n\
    <connectorBase>\n\
      <causalConnector id='onBeginSet'>\n\
        <simpleCondition role='onBegin'/>\n\
        <simpleAction role='set' value='$var'/>\n\
      </causalConnector>\n\
      <causalConnector id='onEndAttributionStart'>\n\
        <simpleCondition role='onEndAttribution'/>\n\
        <simpleAction role='start'/>\n\
      </causalConnector>\n\
    </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start0' component='m0'/>\n\
    <media id='m0'/>\n\
    <context id='ctx1'>\n\
      <port id='start1' component='m1'/>\n\
      <media id='m1' type='application/x-ginga-settings'>\n\
        <property name='x' value='y'/>\n\
      </media>\n\
    </context>\n\
    <context id='ctx2'>\n\
      <media id='r1' refer='m1'>\n\
        <property name='x' value='z'/>\n\
      </media>\n\
    </context>\n\
    <media id='m2'/>\n\
    <link xconnector='onBeginSet'>\n\
      <bind role='onBegin' component='m0'/>\n\
      <bind role='set' component='m1' interface='x'>\n\
        <bindParam name='var' value='k'/>\n\
      </bind>\n\
    </link>\n\
    <link xconnector='onEndAttributionStart'>\n\
      <bind role='onEndAttribution' component='m1' interface='x'/>\n\
      <bind role='start' component='m2'/>\n\
    </link>\n\
  </body>\n\
</ncl>\n");
  // referred object: defined first in nested context
  // reference: defined after in nested context
  // action set over: referred object
  // link onEndAttribution over: reference
  PASS ("<ncl>\n\
  <head>\n\
    <connectorBase>\n\
      <causalConnector id='onBeginSet'>\n\
        <simpleCondition role='onBegin'/>\n\
        <simpleAction role='set' value='$var'/>\n\
      </causalConnector>\n\
      <causalConnector id='onEndAttributionStart'>\n\
        <simpleCondition role='onEndAttribution'/>\n\
        <simpleAction role='start'/>\n\
      </causalConnector>\n\
    </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start' component='m0'/>\n\
    <media id='m0'/>\n\
    <context id='ctx1'>\n\
      <port id='prop1' component='m1' interface='x'/>\n\
      <media id='m1' type='application/x-ginga-settings'>\n\
        <property name='x' value='y'/>\n\
      </media>\n\
    </context>\n\
    <context id='ctx2'>\n\
      <media id='r1' refer='m1'>\n\
        <property name='x' value='z'/>\n\
      </media>\n\
    </context>\n\
    <media id='m2'/>\n\
    <link xconnector='onBeginSet'>\n\
      <bind role='onBegin' component='m0'/>\n\
      <bind role='set' component='ctx1' interface='prop1'>\n\
        <bindParam name='var' value='k'/>\n\
      </bind>\n\
    </link>\n\
    <link xconnector='onEndAttributionStart'>\n\
      <bind role='onEndAttribution' component='m1' interface='x'/>\n\
      <bind role='start' component='m2'/>\n\
    </link>\n\
  </body>\n\
</ncl>\n");
  // referred object: defined first in nested context
  // reference: defined after in nested context
  // action set over: reference
  // link onEndAttribution: referred object
  PASS ("<ncl>\n\
  <head>\n\
    <connectorBase>\n\
      <causalConnector id='onBeginSet'>\n\
        <simpleCondition role='onBegin'/>\n\
        <simpleAction role='set' value='$var'/>\n\
      </causalConnector>\n\
      <causalConnector id='onEndAttributionStart'>\n\
        <simpleCondition role='onEndAttribution'/>\n\
        <simpleAction role='start'/>\n\
      </causalConnector>\n\
    </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start' component='m0'/>\n\
    <media id='m0'/>\n\
    <context id='ctx1'>\n\
      <media id='m1' type='application/x-ginga-settings'>\n\
        <property name='x' value='y'/>\n\
      </media>\n\
    </context>\n\
    <context id='ctx2'>\n\
      <port id='start2' component='r1'/>\n\
      <media id='r1' refer='m1'>\n\
        <property name='x' value='z'/>\n\
      </media>\n\
    </context>\n\
    <media id='m2'/>\n\
    <link xconnector='onBeginSet'>\n\
      <bind role='onBegin' component='m0'/>\n\
      <bind role='set' component='m1' interface='x'>\n\
        <bindParam name='var' value='k'/>\n\
      </bind>\n\
    </link>\n\
    <link xconnector='onEndAttributionStart'>\n\
      <bind role='onEndAttribution' component='m1' interface='x'/>\n\
      <bind role='start' component='m2'/>\n\
    </link>\n\
  </body>\n\
</ncl>\n");
  //   // referred object: defined first in nested context
  //   // reference: defined after in nested context
  //   // action set over: reference
  //   // link onEndAttribution: reference
  PASS ("<ncl>\n\
  <head>\n\
    <connectorBase>\n\
      <causalConnector id='onBeginSet'>\n\
        <simpleCondition role='onBegin'/>\n\
        <simpleAction role='set' value='$var'/>\n\
      </causalConnector>\n\
      <causalConnector id='onEndAttributionStart'>\n\
        <simpleCondition role='onEndAttribution'/>\n\
        <simpleAction role='start'/>\n\
      </causalConnector>\n\
    </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start' component='m0'/>\n\
    <media id='m0'/>\n\
    <context id='ctx1'>\n\
      <media id='m1' type='application/x-ginga-settings'>\n\
        <property name='x' value='y'/>\n\
      </media>\n\
    </context>\n\
    <context id='ctx2'>\n\
      <port id='start2' component='r1'/>\n\
      <media id='r1' refer='m1'>\n\
        <property name='x' value='z'/>\n\
      </media>\n\
    </context>\n\
    <media id='m2'/>\n\
    <link xconnector='onBeginSet'>\n\
      <bind role='onBegin' component='m0'/>\n\
      <bind role='set' component='m1' interface='x'>\n\
        <bindParam name='var' value='k'/>\n\
      </bind>\n\
    </link>\n\
    <link xconnector='onEndAttributionStart'>\n\
      <bind role='onEndAttribution' component='m1' interface='x'/>\n\
      <bind role='start' component='m2'/>\n\
    </link>\n\
  </body>\n\
</ncl>\n");
  // reference: defined first
  // action set over: referred object
  // link onEndAttribution: referred object
  PASS ("<ncl>\n\
  <head>\n\
    <connectorBase>\n\
      <causalConnector id='onBeginSet'>\n\
        <simpleCondition role='onBegin'/>\n\
        <simpleAction role='set' value='$var'/>\n\
      </causalConnector>\n\
      <causalConnector id='onEndAttributionStart'>\n\
        <simpleCondition role='onEndAttribution'/>\n\
        <simpleAction role='start'/>\n\
      </causalConnector>\n\
    </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start' component='m0'/>\n\
    <media id='m0'/>\n\
    <media id='r1' refer='m1'>\n\
      <property name='x' value='y'/>\n\
    </media>\n\
    <media id='m1' type='application/x-ginga-settings'>\n\
      <property name='x' value='z'/>\n\
    </media>\n\
    <media id='m2'/>\n\
    <link xconnector='onBeginSet'>\n\
      <bind role='onBegin' component='m0'/>\n\
      <bind role='set' component='m1' interface='x'>\n\
        <bindParam name='var' value='k'/>\n\
      </bind>\n\
    </link>\n\
    <link xconnector='onEndAttributionStart'>\n\
      <bind role='onEndAttribution' component='m1' interface='x'/>\n\
      <bind role='start' component='m2'/>\n\
    </link>\n\
  </body>\n\
</ncl>\n");
  // referred object: defined after
  // reference: defined first
  // action set over: referred object
  // link onEndAttribution: reference
  PASS ("<ncl>\n\
  <head>\n\
    <connectorBase>\n\
      <causalConnector id='onBeginSet'>\n\
        <simpleCondition role='onBegin'/>\n\
        <simpleAction role='set' value='$var'/>\n\
      </causalConnector>\n\
      <causalConnector id='onEndAttributionStart'>\n\
        <simpleCondition role='onEndAttribution'/>\n\
        <simpleAction role='start'/>\n\
      </causalConnector>\n\
    </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start' component='m0'/>\n\
    <media id='m0'/>\n\
    <media id='r1' refer='m1'>\n\
      <property name='x' value='y'/>\n\
    </media>\n\
    <media id='m1' type='application/x-ginga-settings'>\n\
      <property name='x' value='z'/>\n\
    </media>\n\
    <media id='m2'/>\n\
    <link xconnector='onBeginSet'>\n\
      <bind role='onBegin' component='m0'/>\n\
      <bind role='set' component='m1' interface='x'>\n\
        <bindParam name='var' value='k'/>\n\
      </bind>\n\
    </link>\n\
    <link xconnector='onEndAttributionStart'>\n\
      <bind role='onEndAttribution' component='m1' interface='x'/>\n\
      <bind role='start' component='m2'/>\n\
    </link>\n\
  </body>\n\
</ncl>\n");
  // referred object: defined after
  // reference: defined first
  // action set over: reference
  // link onEndAttribution: referred object
  PASS ("<ncl>\n\
  <head>\n\
    <connectorBase>\n\
      <causalConnector id='onBeginSet'>\n\
        <simpleCondition role='onBegin'/>\n\
        <simpleAction role='set' value='$var'/>\n\
      </causalConnector>\n\
      <causalConnector id='onEndAttributionStart'>\n\
        <simpleCondition role='onEndAttribution'/>\n\
        <simpleAction role='start'/>\n\
      </causalConnector>\n\
    </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start' component='m0'/>\n\
    <media id='m0'/>\n\
    <media id='r1' refer='m1'>\n\
      <property name='x' value='y'/>\n\
    </media>\n\
    <media id='m1' type='application/x-ginga-settings'>\n\
      <property name='x' value='z'/>\n\
    </media>\n\
    <media id='m2'/>\n\
    <link xconnector='onBeginSet'>\n\
      <bind role='onBegin' component='m0'/>\n\
      <bind role='set' component='m1' interface='x'>\n\
        <bindParam name='var' value='k'/>\n\
      </bind>\n\
    </link>\n\
    <link xconnector='onEndAttributionStart'>\n\
      <bind role='onEndAttribution' component='m1' interface='x'/>\n\
      <bind role='start' component='m2'/>\n\
    </link>\n\
  </body>\n\
</ncl>\n");
  // referred object: defined after
  // reference: defined first
  // action set over: reference
  // link onEndAttribution: reference
  PASS ("<ncl>\n\
  <head>\n\
    <connectorBase>\n\
      <causalConnector id='onBeginSet'>\n\
        <simpleCondition role='onBegin'/>\n\
        <simpleAction role='set' value='$var'/>\n\
      </causalConnector>\n\
      <causalConnector id='onEndAttributionStart'>\n\
        <simpleCondition role='onEndAttribution'/>\n\
        <simpleAction role='start'/>\n\
      </causalConnector>\n\
    </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start' component='m0'/>\n\
    <media id='m0'/>\n\
    <media id='r1' refer='m1'>\n\
      <property name='x' value='y'/>\n\
    </media>\n\
    <media id='m1' type='application/x-ginga-settings'>\n\
      <property name='x' value='z'/>\n\
    </media>\n\
    <media id='m2'/>\n\
    <link xconnector='onBeginSet'>\n\
      <bind role='onBegin' component='m0'/>\n\
      <bind role='set' component='m1' interface='x'>\n\
        <bindParam name='var' value='k'/>\n\
      </bind>\n\
    </link>\n\
    <link xconnector='onEndAttributionStart'>\n\
      <bind role='onEndAttribution' component='m1' interface='x'/>\n\
      <bind role='start' component='m2'/>\n\
    </link>\n\
  </body>\n\
</ncl>\n");
  // referred object: defined first
  // reference: defined after in nested context
  // action set over: referred object
  // link onEndAttribution: referred object
  PASS ("<ncl>\n\
  <head>\n\
    <connectorBase>\n\
      <causalConnector id='onBeginSet'>\n\
        <simpleCondition role='onBegin'/>\n\
        <simpleAction role='set' value='$var'/>\n\
      </causalConnector>\n\
      <causalConnector id='onEndAttributionStart'>\n\
        <simpleCondition role='onEndAttribution'/>\n\
        <simpleAction role='start'/>\n\
      </causalConnector>\n\
    </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start' component='m0'/>\n\
    <media id='m0'/>\n\
    <media id='m1' type='application/x-ginga-settings'>\n\
      <property name='x' value='y'/>\n\
    </media>\n\
    <context id='ctx1'>\n\
      <port id='start1' component='r1'/>\n\
      <media id='r1' refer='m1'>\n\
        <property name='x' value='z'/>\n\
      </media>\n\
    </context>\n\
    <media id='m2'/>\n\
    <link xconnector='onBeginSet'>\n\
      <bind role='onBegin' component='m0'/>\n\
      <bind role='set' component='m1' interface='x'>\n\
        <bindParam name='var' value='k'/>\n\
      </bind>\n\
    </link>\n\
    <link xconnector='onEndAttributionStart'>\n\
      <bind role='onEndAttribution' component='m1' interface='x'/>\n\
      <bind role='start' component='m2'/>\n\
    </link>\n\
  </body>\n\
</ncl>\n");
  // referred object: defined first
  // reference: defined after in nested context
  // action set over: referred object
  // link onEndAttribution: reference
  PASS ("<ncl>\n\
  <head>\n\
    <connectorBase>\n\
      <causalConnector id='onBeginSet'>\n\
        <simpleCondition role='onBegin'/>\n\
        <simpleAction role='set' value='$var'/>\n\
      </causalConnector>\n\
      <causalConnector id='onEndAttributionStart'>\n\
        <simpleCondition role='onEndAttribution'/>\n\
        <simpleAction role='start'/>\n\
      </causalConnector>\n\
    </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start' component='m0'/>\n\
    <media id='m0'/>\n\
    <media id='m1' type='application/x-ginga-settings'>\n\
      <property name='x' value='y'/>\n\
    </media>\n\
    <context id='ctx1'>\n\
      <port id='start1' component='r1'/>\n\
      <media id='r1' refer='m1'>\n\
        <property name='x' value='z'/>\n\
      </media>\n\
    </context>\n\
    <media id='m2'/>\n\
    <link xconnector='onBeginSet'>\n\
      <bind role='onBegin' component='m0'/>\n\
      <bind role='set' component='m1' interface='x'>\n\
        <bindParam name='var' value='k'/>\n\
      </bind>\n\
    </link>\n\
    <link xconnector='onEndAttributionStart'>\n\
      <bind role='onEndAttribution' component='m1' interface='x'/>\n\
      <bind role='start' component='m2'/>\n\
    </link>\n\
  </body>\n\
</ncl>\n");
  // referred object: defined first
  // reference: defined after in nested context
  // action set over: reference
  // link onEndAttribution: referred object
  PASS ("<ncl>\n\
  <head>\n\
    <connectorBase>\n\
      <causalConnector id='onBeginSet'>\n\
        <simpleCondition role='onBegin'/>\n\
        <simpleAction role='set' value='$var'/>\n\
      </causalConnector>\n\
      <causalConnector id='onEndAttributionStart'>\n\
        <simpleCondition role='onEndAttribution'/>\n\
        <simpleAction role='start'/>\n\
      </causalConnector>\n\
    </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start' component='m0'/>\n\
    <media id='m0'/>\n\
    <media id='m1' type='application/x-ginga-settings'>\n\
      <property name='x' value='y'/>\n\
    </media>\n\
    <context id='ctx1'>\n\
      <port id='start1' component='r1'/>\n\
      <media id='r1' refer='m1'>\n\
        <property name='x' value='z'/>\n\
      </media>\n\
    </context>\n\
    <media id='m2'/>\n\
    <link xconnector='onBeginSet'>\n\
      <bind role='onBegin' component='m0'/>\n\
      <bind role='set' component='m1' interface='x'>\n\
        <bindParam name='var' value='k'/>\n\
      </bind>\n\
    </link>\n\
    <link xconnector='onEndAttributionStart'>\n\
      <bind role='onEndAttribution' component='m1' interface='x'/>\n\
      <bind role='start' component='m2'/>\n\
    </link>\n\
  </body>\n\
</ncl>\n");
  // referred object: defined first
  // reference: defined after in nested context
  // action set over: reference
  // link onEndAttribution: reference
  PASS ("<ncl>\n\
  <head>\n\
    <connectorBase>\n\
      <causalConnector id='onBeginSet'>\n\
        <simpleCondition role='onBegin'/>\n\
        <simpleAction role='set' value='$var'/>\n\
      </causalConnector>\n\
      <causalConnector id='onEndAttributionStart'>\n\
        <simpleCondition role='onEndAttribution'/>\n\
        <simpleAction role='start'/>\n\
      </causalConnector>\n\
    </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start' component='m0'/>\n\
    <media id='m0'/>\n\
    <media id='m1' type='application/x-ginga-settings'>\n\
      <property name='x' value='y'/>\n\
    </media>\n\
    <context id='ctx1'>\n\
      <port id='start1' component='r1'/>\n\
      <media id='r1' refer='m1'>\n\
        <property name='x' value='z'/>\n\
      </media>\n\
    </context>\n\
    <media id='m2'/>\n\
    <link xconnector='onBeginSet'>\n\
      <bind role='onBegin' component='m0'/>\n\
      <bind role='set' component='m1' interface='x'>\n\
        <bindParam name='var' value='k'/>\n\
      </bind>\n\
    </link>\n\
    <link xconnector='onEndAttributionStart'>\n\
      <bind role='onEndAttribution' component='m1' interface='x'/>\n\
      <bind role='start' component='m2'/>\n\
    </link>\n\
  </body>\n\
</ncl>\n");
  // referred object: defined first in nested context
  // reference: defined after
  // action set over: referred object
  // link onEndAttribution: referred object
  PASS ("<ncl>\n\
  <head>\n\
    <connectorBase>\n\
      <causalConnector id='onBeginSet'>\n\
        <simpleCondition role='onBegin'/>\n\
        <simpleAction role='set' value='$var'/>\n\
      </causalConnector>\n\
      <causalConnector id='onEndAttributionStart'>\n\
        <simpleCondition role='onEndAttribution'/>\n\
        <simpleAction role='start'/>\n\
      </causalConnector>\n\
    </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start' component='m0'/>\n\
    <media id='m0'/>\n\
    <context id='ctx1'>\n\
      <port id='start1' component='m1'/>\n\
      <media id='m1' type='application/x-ginga-settings'>\n\
        <property name='x' value='y'/>\n\
      </media>\n\
    </context>\n\
    <media id='r1' refer='m1'>\n\
      <property name='x' value='z'/>\n\
    </media>\n\
    <media id='m2'/>\n\
    <link xconnector='onBeginSet'>\n\
      <bind role='onBegin' component='m0'/>\n\
      <bind role='set' component='m1' interface='x'>\n\
        <bindParam name='var' value='k'/>\n\
      </bind>\n\
    </link>\n\
    <link xconnector='onEndAttributionStart'>\n\
      <bind role='onEndAttribution' component='m1' interface='x'/>\n\
      <bind role='start' component='m2'/>\n\
    </link>\n\
  </body>\n\
</ncl>\n");
  // referred object: defined first in nested context
  // reference: defined after
  // action set over: referred object
  // link onEndAttribution: reference
  PASS ("<ncl>\n\
  <head>\n\
    <connectorBase>\n\
      <causalConnector id='onBeginSet'>\n\
        <simpleCondition role='onBegin'/>\n\
        <simpleAction role='set' value='$var'/>\n\
      </causalConnector>\n\
      <causalConnector id='onEndAttributionStart'>\n\
        <simpleCondition role='onEndAttribution'/>\n\
        <simpleAction role='start'/>\n\
      </causalConnector>\n\
    </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start' component='m0'/>\n\
    <media id='m0'/>\n\
    <context id='ctx1'>\n\
      <port id='start1' component='m1'/>\n\
      <media id='m1' type='application/x-ginga-settings'>\n\
        <property name='x' value='y'/>\n\
      </media>\n\
    </context>\n\
    <media id='r1' refer='m1'>\n\
      <property name='x' value='z'/>\n\
    </media>\n\
    <media id='m2'/>\n\
    <link xconnector='onBeginSet'>\n\
      <bind role='onBegin' component='m0'/>\n\
      <bind role='set' component='m1' interface='x'>\n\
        <bindParam name='var' value='k'/>\n\
      </bind>\n\
    </link>\n\
    <link xconnector='onEndAttributionStart'>\n\
      <bind role='onEndAttribution' component='m1' interface='x'/>\n\
      <bind role='start' component='m2'/>\n\
    </link>\n\
  </body>\n\
</ncl>\n");
  // referred object: defined first in nested context
  // reference: defined after
  // action set over: reference
  // link onEndAttribution: referred object
  PASS ("<ncl>\n\
  <head>\n\
    <connectorBase>\n\
      <causalConnector id='onBeginSet'>\n\
        <simpleCondition role='onBegin'/>\n\
        <simpleAction role='set' value='$var'/>\n\
      </causalConnector>\n\
      <causalConnector id='onEndAttributionStart'>\n\
        <simpleCondition role='onEndAttribution'/>\n\
        <simpleAction role='start'/>\n\
      </causalConnector>\n\
    </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start' component='m0'/>\n\
    <media id='m0'/>\n\
    <context id='ctx1'>\n\
      <port id='start1' component='m1'/>\n\
      <media id='m1' type='application/x-ginga-settings'>\n\
        <property name='x' value='y'/>\n\
      </media>\n\
    </context>\n\
    <media id='r1' refer='m1'>\n\
      <property name='x' value='z'/>\n\
    </media>\n\
    <media id='m2'/>\n\
    <link xconnector='onBeginSet'>\n\
      <bind role='onBegin' component='m0'/>\n\
      <bind role='set' component='m1' interface='x'>\n\
        <bindParam name='var' value='k'/>\n\
      </bind>\n\
    </link>\n\
    <link xconnector='onEndAttributionStart'>\n\
      <bind role='onEndAttribution' component='m1' interface='x'/>\n\
      <bind role='start' component='m2'/>\n\
    </link>\n\
  </body>\n\
</ncl>\n");
  // referred object: defined first in nested context
  // reference: defined after
  // action set over: reference
  // link onEndAttribution: reference
  PASS ("<ncl>\n\
  <head>\n\
    <connectorBase>\n\
      <causalConnector id='onBeginSet'>\n\
        <simpleCondition role='onBegin'/>\n\
        <simpleAction role='set' value='$var'/>\n\
      </causalConnector>\n\
      <causalConnector id='onEndAttributionStart'>\n\
        <simpleCondition role='onEndAttribution'/>\n\
        <simpleAction role='start'/>\n\
      </causalConnector>\n\
    </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start' component='m0'/>\n\
    <media id='m0'/>\n\
    <context id='ctx1'>\n\
      <port id='start1' component='m1'/>\n\
      <media id='m1' type='application/x-ginga-settings'>\n\
        <property name='x' value='y'/>\n\
      </media>\n\
    </context>\n\
    <media id='r1' refer='m1'>\n\
      <property name='x' value='z'/>\n\
    </media>\n\
    <media id='m2'/>\n\
    <link xconnector='onBeginSet'>\n\
      <bind role='onBegin' component='m0'/>\n\
      <bind role='set' component='m1' interface='x'>\n\
        <bindParam name='var' value='k'/>\n\
      </bind>\n\
    </link>\n\
    <link xconnector='onEndAttributionStart'>\n\
      <bind role='onEndAttribution' component='m1' interface='x'/>\n\
      <bind role='start' component='m2'/>\n\
    </link>\n\
  </body>\n\
</ncl>\n");
  // referred object: defined first in nested context
  // reference: defined after in nested context
  // action set over: referred object
  // link onEndAttribution: referred object
  PASS ("<ncl>\n\
  <head>\n\
    <connectorBase>\n\
      <causalConnector id='onBeginSet'>\n\
        <simpleCondition role='onBegin'/>\n\
        <simpleAction role='set' value='$var'/>\n\
      </causalConnector>\n\
      <causalConnector id='onEndAttributionStart'>\n\
        <simpleCondition role='onEndAttribution'/>\n\
        <simpleAction role='start'/>\n\
      </causalConnector>\n\
    </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start0' component='m0'/>\n\
    <media id='m0'/>\n\
    <context id='ctx1'>\n\
      <port id='start1' component='m1'/>\n\
      <media id='m1' type='application/x-ginga-settings'>\n\
        <property name='x' value='y'/>\n\
      </media>\n\
    </context>\n\
    <context id='ctx2'>\n\
      <media id='r1' refer='m1'>\n\
        <property name='x' value='z'/>\n\
      </media>\n\
    </context>\n\
    <media id='m2'/>\n\
    <link xconnector='onBeginSet'>\n\
      <bind role='onBegin' component='m0'/>\n\
      <bind role='set' component='m1' interface='x'>\n\
        <bindParam name='var' value='k'/>\n\
      </bind>\n\
    </link>\n\
    <link xconnector='onEndAttributionStart'>\n\
      <bind role='onEndAttribution' component='m1' interface='x'/>\n\
      <bind role='start' component='m2'/>\n\
    </link>\n\
  </body>\n\
</ncl>\n");
  // referred object: defined first in nested context
  // reference: defined after in nested context
  // action set over: referred object
  // link onEndAttribution: reference
  PASS ("<ncl>\n\
  <head>\n\
    <connectorBase>\n\
      <causalConnector id='onBeginSet'>\n\
        <simpleCondition role='onBegin'/>\n\
        <simpleAction role='set' value='$var'/>\n\
      </causalConnector>\n\
      <causalConnector id='onEndAttributionStart'>\n\
        <simpleCondition role='onEndAttribution'/>\n\
        <simpleAction role='start'/>\n\
      </causalConnector>\n\
    </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start0' component='m0'/>\n\
    <media id='m0'/>\n\
    <context id='ctx1'>\n\
      <port id='start1' component='m1'/>\n\
      <media id='m1' type='application/x-ginga-settings'>\n\
        <property name='x' value='y'/>\n\
      </media>\n\
    </context>\n\
    <context id='ctx2'>\n\
      <media id='r1' refer='m1'>\n\
        <property name='x' value='z'/>\n\
      </media>\n\
    </context>\n\
    <media id='m2'/>\n\
    <link xconnector='onBeginSet'>\n\
      <bind role='onBegin' component='m0'/>\n\
      <bind role='set' component='m1' interface='x'>\n\
        <bindParam name='var' value='k'/>\n\
      </bind>\n\
    </link>\n\
    <link xconnector='onEndAttributionStart'>\n\
      <bind role='onEndAttribution' component='m1' interface='x'/>\n\
      <bind role='start' component='m2'/>\n\
    </link>\n\
  </body>\n\
</ncl>\n");
  // referred object: defined first in nested context
  // reference: defined after in nested context
  // action set over: reference
  // link onEndAttribution: referred object
  PASS ("<ncl>\n\
  <head>\n\
    <connectorBase>\n\
      <causalConnector id='onBeginSet'>\n\
        <simpleCondition role='onBegin'/>\n\
        <simpleAction role='set' value='$var'/>\n\
      </causalConnector>\n\
      <causalConnector id='onEndAttributionStart'>\n\
        <simpleCondition role='onEndAttribution'/>\n\
        <simpleAction role='start'/>\n\
      </causalConnector>\n\
    </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start' component='m0'/>\n\
    <media id='m0'/>\n\
    <context id='ctx1'>\n\
      <media id='m1' type='application/x-ginga-settings'>\n\
        <property name='x' value='y'/>\n\
      </media>\n\
    </context>\n\
    <context id='ctx2'>\n\
      <port id='start2' component='r1'/>\n\
      <media id='r1' refer='m1'>\n\
        <property name='x' value='z'/>\n\
      </media>\n\
    </context>\n\
    <media id='m2'/>\n\
    <link xconnector='onBeginSet'>\n\
      <bind role='onBegin' component='m0'/>\n\
      <bind role='set' component='m1' interface='x'>\n\
        <bindParam name='var' value='k'/>\n\
      </bind>\n\
    </link>\n\
    <link xconnector='onEndAttributionStart'>\n\
      <bind role='onEndAttribution' component='m1' interface='x'/>\n\
      <bind role='start' component='m2'/>\n\
    </link>\n\
  </body>\n\
</ncl>\n");
  // referred object: defined first in nested context
  // reference: defined after in nested context
  // action set over: reference
  // link onEndAttribution: reference
  PASS ("<ncl>\n\
  <head>\n\
    <connectorBase>\n\
      <causalConnector id='onBeginSet'>\n\
        <simpleCondition role='onBegin'/>\n\
        <simpleAction role='set' value='$var'/>\n\
      </causalConnector>\n\
      <causalConnector id='onEndAttributionStart'>\n\
        <simpleCondition role='onEndAttribution'/>\n\
        <simpleAction role='start'/>\n\
      </causalConnector>\n\
    </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start' component='m0'/>\n\
    <media id='m0'/>\n\
    <context id='ctx1'>\n\
      <media id='m1' type='application/x-ginga-settings'>\n\
        <property name='x' value='y'/>\n\
      </media>\n\
    </context>\n\
    <context id='ctx2'>\n\
      <port id='start2' component='r1'/>\n\
      <media id='r1' refer='m1'>\n\
        <property name='x' value='z'/>\n\
      </media>\n\
    </context>\n\
    <media id='m2'/>\n\
    <link xconnector='onBeginSet'>\n\
      <bind role='onBegin' component='m0'/>\n\
      <bind role='set' component='m1' interface='x'>\n\
        <bindParam name='var' value='k'/>\n\
      </bind>\n\
    </link>\n\
    <link xconnector='onEndAttributionStart'>\n\
      <bind role='onEndAttribution' component='m1' interface='x'/>\n\
      <bind role='start' component='m2'/>\n\
    </link>\n\
  </body>\n\
</ncl>\n");

  exit (EXIT_SUCCESS);
}
