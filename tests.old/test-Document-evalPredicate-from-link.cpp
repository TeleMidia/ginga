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
  // Predicate::ATOM from assessmentStatement is true
  {
    Formatter *fmt;
    Document *doc;
    tests_parse_and_start (&fmt, &doc, "\
<ncl>\n\
<head>\n\
  <connectorBase>\n\
  <causalConnector id='onBeginPropertyTestStart'>\n\
    <connectorParam name='val'/>\n\
    <compoundCondition operator='and'>\n\
      <simpleCondition role='onBegin'/>\n\
      <assessmentStatement comparator='eq'>\n\
        <attributeAssessment role='propertyTest'/>\n\
        <valueAssessment value='$val'/>\n\
      </assessmentStatement>\n\
    </compoundCondition>\n\
    <simpleAction role='start' max='unbounded' qualifier='seq'/>\n\
  </causalConnector>\n\
  </connectorBase>\n\
</head>\n\
<body id='body'>\n\
  <port id='p1' component='m1' />\n\
  <media id='m1' />\n\
  <media id='m2' />\n\
  <media id='noSettings' type='application/x-ginga-settings'>\n\
    <property name='turn' value='x'/>\n\
  </media>\n\
  <link xconnector='onBeginPropertyTestStart' >\n\
    <bind component='m1' role='onBegin'/>\n\
    <bind component='noSettings' interface='turn' role='propertyTest'>\n\
      <bindParam name='val' value='x'/>\n\
    </bind>\n\
    <bind component='m2' role='start'/>\n\
  </link>\n\
</body>\n\
</ncl>");

    Context *body = cast (Context *, doc->getRoot ());
    g_assert_nonnull (body);
    Event *body_lambda = body->getLambda ();
    g_assert_nonnull (body_lambda);

    Media *m1 = cast (Media *, body->getChildById ("m1"));
    g_assert_nonnull (m1);
    Event *m1_lambda = m1->getLambda ();
    g_assert_nonnull (m1_lambda);

    Media *m2 = cast (Media *, body->getChildById ("m2"));
    g_assert_nonnull (m2);
    Event *m2_lambda = m2->getLambda ();
    g_assert_nonnull (m2_lambda);

    // --------------------------------
    // check start document

    // when document is started, only the body@lambda is OCCURING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);
    g_assert (m2_lambda->getState () == Event::SLEEPING);

    // advance time to trigger the link
    // the link triger because if $__settings__.turn is equals to x
    // the $__settings__.turn parameter is assigned as param by the
    // propertyTest role and by not by the onBegin role
    fmt->sendTick (0, 0, 0);

    // when advance time m1 and m2 are OCCURRING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::OCCURRING);
    g_assert (m2_lambda->getState () == Event::OCCURRING);

    delete fmt;
  }

  // Predicate::ATOM from assessmentStatement is false
  {
    Formatter *fmt;
    Document *doc;
    tests_parse_and_start (&fmt, &doc, "\
<ncl>\n\
<head>\n\
  <connectorBase>\n\
  <causalConnector id='onBeginPropertyTestStart'>\n\
    <connectorParam name='val'/>\n\
    <compoundCondition operator='and'>\n\
      <simpleCondition role='onBegin'/>\n\
      <assessmentStatement comparator='eq'>\n\
        <attributeAssessment role='propertyTest'/>\n\
        <valueAssessment value='$val'/>\n\
      </assessmentStatement>\n\
    </compoundCondition>\n\
    <simpleAction role='start' max='unbounded' qualifier='seq'/>\n\
  </causalConnector>\n\
  </connectorBase>\n\
</head>\n\
<body id='body'>\n\
  <port id='p1' component='m1' />\n\
  <media id='m1' />\n\
  <media id='m2' />\n\
  <media id='noSettings' type='application/x-ginga-settings'>\n\
    <property name='turn' value='x'/>\n\
  </media>\n\
  <link xconnector='onBeginPropertyTestStart' >\n\
    <bind component='m1' role='onBegin'/>\n\
    <bind component='noSettings' interface='turn' role='propertyTest'>\n\
      <bindParam name='val' value='y'/>\n\
    </bind>\n\
    <bind component='m2' role='start'/>\n\
  </link>\n\
</body>\n\
</ncl>");

    Context *body = cast (Context *, doc->getRoot ());
    g_assert_nonnull (body);
    Event *body_lambda = body->getLambda ();
    g_assert_nonnull (body_lambda);

    Media *m1 = cast (Media *, body->getChildById ("m1"));
    g_assert_nonnull (m1);
    Event *m1_lambda = m1->getLambda ();
    g_assert_nonnull (m1_lambda);

    Media *m2 = cast (Media *, body->getChildById ("m2"));
    g_assert_nonnull (m2);
    Event *m2_lambda = m2->getLambda ();
    g_assert_nonnull (m2_lambda);

    // --------------------------------
    // check start document

    // when document is started, only the body@lambda is OCCURING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);
    g_assert (m2_lambda->getState () == Event::SLEEPING);

    // advance time to trigger the link
    // the link not triger because $__settings__.turn is diferent to x
    // the $__settings__.turn parameter is assigned as param by the
    // propertyTest role and by not by the onBegin role
    fmt->sendTick (0, 0, 0);

    // when advance time, anchors events go to OCCURRING
    // and properties events are SLEEPING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::OCCURRING);
    g_assert (m2_lambda->getState () == Event::SLEEPING);

    delete fmt;
  }

  // Predicate::DISJUNCTION from assessmentStatement is true
  {}

  // Predicate::DISJUNCTION from assessmentStatement is false
  {}

  // Predicate::CONJUNCTION from assessmentStatement is true
  {}

  // Predicate::CONJUNCTION from assessmentStatement is false
  {
  }

  exit (EXIT_SUCCESS);
}
