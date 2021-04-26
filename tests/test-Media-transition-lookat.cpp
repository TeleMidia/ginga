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

  // START lookAt from state SLEEPING
  {
    Formatter *fmt;
    Document *doc;

    tests_parse_and_start (&fmt, &doc, "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='onLookAtStart'>\n\
    <simpleCondition role='onLookAt'/>\n\
    <simpleAction role='start'/>\n\
   </causalConnector>\n\
   <causalConnector id='onLookAwayStart'>\n\
    <simpleCondition role='onLookAway'/>\n\
    <simpleAction role='start'/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body id='body'>\n\
  <port id='start' component='m1'/>\n\
  <media id='m1'/>\n\
  <media id='m2'/>\n\
  <media id='m3'/>\n\
  <link xconnector='onLookAtStart'>\n\
   <bind role='onLookAt' component='m1'/>\n\
   <bind role='start' component='m2'/>\n\
  </link>\n\
  <link xconnector='onLookAwayStart'>\n\
   <bind role='onLookAway' component='m1'/>\n\
   <bind role='start' component='m3'/>\n\
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

    // when document is started, only the body_lambda is OCCURING
    g_assert_cmpint ((body_lambda)->getState (), ==, Event::OCCURRING);
    g_assert_cmpint ((m1_lambda)->getState (), ==, Event::SLEEPING);
    g_assert_cmpint ((m2_lambda)->getState (), ==, Event::SLEEPING);
    g_assert_cmpint ((m3_lambda)->getState (), ==, Event::SLEEPING);

    // when advance time, m1_lambda is OCCURRING
    (fmt)->sendTick (0, 0, 0);
    g_assert_cmpint ((body_lambda)->getState (), ==, Event::OCCURRING);
    g_assert_cmpint ((m1_lambda)->getState (), ==, Event::OCCURRING);
    g_assert_cmpint ((m2_lambda)->getState (), ==, Event::SLEEPING);
    g_assert_cmpint ((m3_lambda)->getState (), ==, Event::SLEEPING);

    // START is done
    Event *evtOnLookAt = m1->getLookAtEvent ("@lambda");
    g_assert_nonnull (evtOnLookAt);

    // after START, m1_onLooAt m2 OCCURRING
    doc->evalAction (evtOnLookAt, Event::START);
    (fmt)->sendTick (0, 0, 0);
    g_assert_cmpint ((body_lambda)->getState (), ==, Event::OCCURRING);
    g_assert_cmpint ((m1_lambda)->getState (), ==, Event::OCCURRING);
    g_assert_cmpint ((m2_lambda)->getState (), ==, Event::OCCURRING);
    g_assert_cmpint ((m3_lambda)->getState (), ==, Event::SLEEPING);

    doc->evalAction (evtOnLookAt, Event::STOP);
    (fmt)->sendTick (0, 0, 0);

    // after START, m1_onLooAway m3 are OCCURRING
    (fmt)->sendTick (0, 0, 0);
    g_assert_cmpint ((body_lambda)->getState (), ==, Event::OCCURRING);
    g_assert_cmpint ((m1_lambda)->getState (), ==, Event::OCCURRING);
    g_assert_cmpint ((m2_lambda)->getState (), ==, Event::OCCURRING);
    g_assert_cmpint ((m3_lambda)->getState (), ==, Event::OCCURRING);

    delete fmt;
  }

  exit (EXIT_SUCCESS);
}
