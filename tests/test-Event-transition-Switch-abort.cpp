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
    tests_parse_and_start (&fmt, &doc, "\
<ncl>\n\
<head>\n\
  <ruleBase>\n\
    <rule id='r1' var='var1' value='m1' comparator='eq'/>\n\
    <rule id='r2' var='var1' value='m2' comparator='eq'/>\n\
  </ruleBase>\n\
</head>\n\
<body>\n\
  <port id='p1' component='s1'/>\n\
  <media id='stgs' type='application/x-ginga-settings'>\n\
    <property name='var1' value='m1'/>\n\
  </media>\n\
  <switch id='s1'>\n\
    <bindRule constituent='m1' rule='r1'/>\n\
    <bindRule constituent='m2' rule='r2'/>\n\
    <media id='m1'/>\n\
    <media id='m2'/>\n\
  </switch>\n\
</body>\n\
</ncl>");

    Context *body = cast (Context *, doc->getRoot ());
    g_assert_nonnull (body);
    Event *body_lambda = body->getLambda ();
    g_assert_nonnull (body_lambda);

    MediaSettings *stgs
        = cast (MediaSettings *, doc->getObjectByIdOrAlias ("stgs"));
    g_assert_nonnull (stgs);
    Event *stgs_lambda = stgs->getLambda ();
    g_assert_nonnull (stgs_lambda);

    Switch *swt = cast (Switch *, body->getChildById ("s1"));
    g_assert_nonnull (swt);
    Event *swt_lambda = swt->getLambda ();
    g_assert_nonnull (swt_lambda);

    Media *m1 = cast (Media *, swt->getChildById ("m1"));
    g_assert_nonnull (m1);
    Event *m1_lambda = m1->getLambda ();
    g_assert_nonnull (m1_lambda);

    Media *m2 = cast (Media *, swt->getChildById ("m2"));
    g_assert_nonnull (m2);
    Event *m2_lambda = m2->getLambda ();
    g_assert_nonnull (m2_lambda);

    // --------------------------------
    // check start document

    // When the document is started, only the body@lambda is OCCURRING.
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (swt_lambda->getState () == Event::SLEEPING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);
    g_assert (m2_lambda->getState () == Event::SLEEPING);

    // after advance time, s1@lambda is OCCURRING
    fmt->sendTick (0, 0, 0);
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (swt_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::OCCURRING);
    g_assert (m2_lambda->getState () == Event::SLEEPING);

    // --------------------------------
    // main check

    // ABORT is done
    g_assert (swt_lambda->transition (Event::ABORT));

    // after start, s1@lambda is SLEEPING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (swt_lambda->getState () == Event::SLEEPING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);
    g_assert (m2_lambda->getState () == Event::SLEEPING);

    delete fmt;
  }

  // @lambda: ABORT from state SLEEPING.

  // Attribution events ----------------------------------------------------

  // ABORT from state SLEEPING.

  exit (EXIT_SUCCESS);
}
