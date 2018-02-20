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

  // @lambda: STOP from state OCCURRING.
  {
    Formatter *fmt;
    Document *doc;
    tests_parse_and_start (&fmt, &doc, "\
<ncl>\n\
 <body>\n\
  <port id='start' component='m1'/>\n\
  <media id='m1'>\n\
   <property name='p1' value='0'/>\n\
   <area id='a1'/>\n\
   <area id='a2'/>\n\
  </media>\n\
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
    Event *m1_a1 = m1->getPresentationEvent ("a1");
    g_assert_nonnull (m1_a1);
    Event *m1_a2 = m1->getPresentationEvent ("a1");
    g_assert_nonnull (m1_a2);
    Event *m1_p1 = m1->getAttributionEvent ("p1");
    g_assert_nonnull (m1_p1);

    // --------------------------------
    // check start document

    // when document is started, only the body@lambda is OCCURING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);
    g_assert (m1_a1->getState () == Event::SLEEPING);
    g_assert (m1_a2->getState () == Event::SLEEPING);
    g_assert (m1_p1->getState () == Event::SLEEPING);

    // START is done and return true
    g_assert (m1_lambda->transition (Event::START));

    // after START lambda is in OCCURRING and
    // anchors are in SLEEPING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::OCCURRING);
    g_assert (m1_a1->getState () == Event::SLEEPING);
    g_assert (m1_a2->getState () == Event::SLEEPING);
    g_assert (m1_p1->getState () == Event::SLEEPING);

    // advance time
    fmt->sendTick (0, 0, 0);

    // when advance time, anchors events go to OCCURRING
    // and properties events are SLEEPING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::OCCURRING);
    g_assert (m1_a1->getState () == Event::OCCURRING);
    g_assert (m1_a2->getState () == Event::OCCURRING);
    g_assert (m1_p1->getState () == Event::SLEEPING);

    // --------------------------------
    // main check

    // STOP is done and return true
    g_assert_true (m1_lambda->transition (Event::STOP));

    // after STOP all events are SLEEPING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);
    g_assert (m1_a1->getState () == Event::SLEEPING);
    g_assert (m1_a2->getState () == Event::SLEEPING);
    g_assert (m1_p1->getState () == Event::SLEEPING);

    delete fmt;
  }

  // @lambda: STOP from state PAUSED.
  {
    Formatter *fmt;
    Document *doc;
    tests_parse_and_start (&fmt, &doc, "\
<ncl>\n\
 <body>\n\
  <port id='start' component='m1'/>\n\
  <media id='m1'>\n\
   <property name='p1' value='0'/>\n\
   <area id='a1'/>\n\
   <area id='a2'/>\n\
  </media>\n\
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
    Event *m1_a1 = m1->getPresentationEvent ("a1");
    g_assert_nonnull (m1_a1);
    Event *m1_a2 = m1->getPresentationEvent ("a1");
    g_assert_nonnull (m1_a2);
    Event *m1_p1 = m1->getAttributionEvent ("p1");
    g_assert_nonnull (m1_p1);

    // --------------------------------
    // check start document

    // when document is started, only the body@lambda is OCCURING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);
    g_assert (m1_a1->getState () == Event::SLEEPING);
    g_assert (m1_a2->getState () == Event::SLEEPING);
    g_assert (m1_p1->getState () == Event::SLEEPING);

    // START is done and return true
    g_assert (m1_lambda->transition (Event::START));

    // after START lambda is in OCCURRING and
    // anchors are in SLEEPING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::OCCURRING);
    g_assert (m1_a1->getState () == Event::SLEEPING);
    g_assert (m1_a2->getState () == Event::SLEEPING);
    g_assert (m1_p1->getState () == Event::SLEEPING);

    // advance time
    fmt->sendTick (0, 0, 0);

    // when advance time, anchors events go to OCCURRING
    // and properties events are SLEEPING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::OCCURRING);
    g_assert (m1_a1->getState () == Event::OCCURRING);
    g_assert (m1_a2->getState () == Event::OCCURRING);
    g_assert (m1_p1->getState () == Event::SLEEPING);

    // --------------------------------
    // main check

    // PAUSE is done and return true
    g_assert_true (m1_lambda->transition (Event::PAUSE));

    // after PAUSE lambda and anchors are PAUSED and
    // properties are in SLEEPING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::PAUSED);
    g_assert (m1_a1->getState () == Event::PAUSED);
    g_assert (m1_a2->getState () == Event::PAUSED);
    g_assert (m1_p1->getState () == Event::SLEEPING);

    // STOP is done and return true
    g_assert_true (m1_lambda->transition (Event::STOP));

    // after STOP all events are SLEEPING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);
    g_assert (m1_a1->getState () == Event::SLEEPING);
    g_assert (m1_a2->getState () == Event::SLEEPING);
    g_assert (m1_p1->getState () == Event::SLEEPING);

    delete fmt;
  }

  // @lambda: STOP from state SLEEPING.
  {
    Formatter *fmt;
    Document *doc;
    tests_parse_and_start (&fmt, &doc, "\
<ncl>\n\
 <body>\n\
  <port id='start' component='m1'/>\n\
  <media id='m1'>\n\
   <property name='p1' value='0'/>\n\
   <area id='a1'/>\n\
   <area id='a2'/>\n\
  </media>\n\
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
    Event *m1_a1 = m1->getPresentationEvent ("a1");
    g_assert_nonnull (m1_a1);
    Event *m1_a2 = m1->getPresentationEvent ("a1");
    g_assert_nonnull (m1_a2);
    Event *m1_p1 = m1->getAttributionEvent ("p1");
    g_assert_nonnull (m1_p1);

    // --------------------------------
    // check start document

    // when document is started, only the body@lambda is OCCURING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);
    g_assert (m1_a1->getState () == Event::SLEEPING);
    g_assert (m1_a2->getState () == Event::SLEEPING);
    g_assert (m1_p1->getState () == Event::SLEEPING);

    // START is done and return true
    g_assert (m1_lambda->transition (Event::START));

    // after START lambda is in OCCURRING and
    // anchors are in SLEEPING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::OCCURRING);
    g_assert (m1_a1->getState () == Event::SLEEPING);
    g_assert (m1_a2->getState () == Event::SLEEPING);
    g_assert (m1_p1->getState () == Event::SLEEPING);

    // advance time
    fmt->sendTick (0, 0, 0);

    // when advance time, anchors events go to OCCURRING
    // and properties events are SLEEPING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::OCCURRING);
    g_assert (m1_a1->getState () == Event::OCCURRING);
    g_assert (m1_a2->getState () == Event::OCCURRING);
    g_assert (m1_p1->getState () == Event::SLEEPING);

    // --------------------------------
    // main check

   // STOP is done and return true
    g_assert_true (m1_lambda->transition (Event::STOP));

    // after STOP all events are SLEEPING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);
    g_assert (m1_a1->getState () == Event::SLEEPING);
    g_assert (m1_a2->getState () == Event::SLEEPING);
    g_assert (m1_p1->getState () == Event::SLEEPING);

    // STOP is done and return false
    g_assert_false (m1_lambda->transition (Event::STOP));

    // after STOP all events are still SLEEPING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);
    g_assert (m1_a1->getState () == Event::SLEEPING);
    g_assert (m1_a2->getState () == Event::SLEEPING);
    g_assert (m1_p1->getState () == Event::SLEEPING);

    delete fmt;
  }

  // Attribution events ----------------------------------------------------

  // Selection events ------------------------------------------------------

  exit (EXIT_SUCCESS);
}
