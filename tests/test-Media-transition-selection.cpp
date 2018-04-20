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
  // ABORT selection when Media is OCCURRING.
  // ABORT selection when Media is PAUSED.
  // ABORT selection when Media is SLEEPING.

  // PAUSE selection when Media is OCCURRING.
  // PAUSE selection when Media is PAUSED.
  // PAUSE selection when Media is SLEEPING.

  // START selection when Media is OCCURRING.
  {
    Formatter *fmt;
    Document *doc;
    tests_parse_and_start (&fmt, &doc, "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
    <causalConnector id='onKeySelectionStop'>\n\
      <simpleCondition role='onSelection'/>\n\
      <simpleAction role='stop'/>\n\
    </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body>\n\
  <port id='start' component='m1'/>\n\
  <media id='m1'>\n\
   <property name='focusIndex' value='0'/>\n\
  </media>\n\
  <link xconnector='onKeySelectionStop'>\n\
    <bind role='onSelection' component='m1'/>\n\
    <bind role='stop' component='m1'/>\n\
  </link>\n\
 </body>\n\
</ncl>");

    Context *body = cast (Context *, doc->getRoot ());
    g_assert_nonnull (body);
    Event *body_lambda = body->getLambda ();
    g_assert_nonnull (body_lambda);

    Media *m1 = cast (Media *, body->getChildById ("m1"));
    g_assert_nonnull (m1);
    puts(m1->toString ().c_str ());
    Event *m1_lambda = m1->getLambda ();
    g_assert_nonnull (m1_lambda);
    Event *m1_selection = m1->getSelectionEvent ("");
    g_assert_nonnull (m1_selection);

    // --------------------------------
    // check start document

    // when document is started, only the body@lambda is OCCURING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);
    g_assert (m1_selection->getState () == Event::SLEEPING);

    // START is done and return true
    g_assert (m1_lambda->transition (Event::START));

    // after START lambda is in OCCURRING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::OCCURRING);
    g_assert (m1_selection->getState () == Event::SLEEPING);

    // advance time
    fmt->sendTick (0, 0, 0);

    // when advance time, anchors events go to OCCURRING
    // and properties events are SLEEPING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_selection->getState () == Event::SLEEPING);
    g_assert (m1_lambda->getState () == Event::OCCURRING);

    // --------------------------------
    // main check

    // START is done
    g_assert (m1_selection->transition (Event::START));

    // after START, selection is OCCURRING
    g_assert (body_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::OCCURRING);
    g_assert (m1_selection->getState () == Event::OCCURRING);

    delete fmt;
  }

  // START selection when Media is PAUSED.
  // START selection when Media is SLEEPING.

  // STOP selection when Media is OCCURRING.
  // STOP selection when Media is PAUSED.
  // STOP selection when Media is SLEEPING.

  exit (EXIT_SUCCESS);
}
