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

  // @lambda: START from state OCCURRING.
  {
    Formatter *fmt;
    Document *doc;
    tests_parse_and_start (&fmt, &doc, "\
<ncl>\n\
  <head>\n\
  </head>\n\
  <body>\n\
    <port id='start1' component='ctx1'/>\n\
    <context id='ctx1'>\n\
      <port id='port1' component='m1'/>\n\
      <media id='m1'>\n\
        <property name='explicitDur' value='3s'/>\n\
      </media>\n\
    </context>\n\
  </body>\n\
</ncl>\n");

    Event *root_lambda = doc->getRoot ()->getLambda ();
    g_assert_nonnull (root_lambda);
    Context *ctx1 = cast (Context *, doc->getObjectById ("ctx1"));
    g_assert_nonnull (ctx1);
    Event *ctx1_lambda = ctx1->getLambda ();
    g_assert_nonnull (ctx1_lambda);
    Event *m1_lambda = *ctx1->getPorts ()->begin ();
    g_assert_nonnull (m1_lambda);

    // --------------------------------
    // check start document

    // when start the document, only the lambda@root is OCCURING
    g_assert (root_lambda->getState () == Event::OCCURRING);
    g_assert (ctx1_lambda->getState () == Event::SLEEPING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);

    fmt->sendTick (0, 0, 0);
    g_assert (root_lambda->getState () == Event::OCCURRING);
    g_assert (ctx1_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);

    // in next reaction, lambda@ctx1 and its anchors
    // are OCCURRING, and its properties are SLEEPING
    fmt->sendTick (0, 0, 0);
    g_assert (root_lambda->getState () == Event::OCCURRING);
    g_assert (ctx1_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::OCCURRING);

    // --------------------------------
    // main check

    // after 4 second, m1 is SLEEPING
    fmt->sendTick (4 * GINGA_SECOND, 4 * GINGA_SECOND, 0);
    g_assert (root_lambda->getState () == Event::OCCURRING);
    g_assert (ctx1_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);

    // in next reaction, ctx1 is SLEEPING
    fmt->sendTick (0, 0, 0);
    g_assert (root_lambda->getState () == Event::OCCURRING);
    g_assert (ctx1_lambda->getState () == Event::SLEEPING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);

    // in next reaction, root is SLEEPING
    fmt->sendTick (0, 0, 0);
    g_assert (root_lambda->getState () == Event::SLEEPING);
    g_assert (ctx1_lambda->getState () == Event::SLEEPING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);

    // in next reaction, fmt is stoped
    fmt->sendTick (0, 0, 0);
    g_assert (fmt->getState () == GINGA_STATE_STOPPED);

    delete fmt;
  }

  exit (EXIT_SUCCESS);
}
