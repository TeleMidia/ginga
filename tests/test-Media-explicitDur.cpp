/* Copyright (Ctx) 2006-2017 PUC-Rio/Laboratorio TeleMidia

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
  // explicitDur in media
  {
    Formatter *fmt;
    Document *doc;
    tests_parse_and_start (&fmt, &doc, "\
<ncl>\n\
  <head>\n\
  </head>\n\
  <body>\n\
    <port id='port1' component='m1'/>\n\
    <media id='m1'>\n\
      <property name='explicitDur' value='3s'/>\n\
    </media>\n\
  </body>\n\
</ncl>\n");

    Event *root_lambda = doc->getRoot ()->getLambda ();
    g_assert_nonnull (root_lambda);
    Media *m1 = cast (Media *, doc->getObjectById ("m1"));
    g_assert_nonnull (m1);
    Event *m1_lambda = m1->getLambda ();
    g_assert_nonnull (m1_lambda);

    // --------------------------------
    // check start document

    // when start the document, only the lambda@root is OCCURING
    g_assert (root_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);

    // in next reaction, m1@lambda is OCCURRING
    fmt->sendTick (0, 0, 0);
    g_assert (root_lambda->getState () == Event::OCCURRING);
    g_assert (m1_lambda->getState () == Event::OCCURRING);

    // --------------------------------
    // main check

    // after 3 seconds, m1 is SLEEPING
    fmt->sendTick (4 * GINGA_SECOND, 4 * GINGA_SECOND, 0);
    fmt->sendTick (0, 0, 0);
    g_assert (root_lambda->getState () == Event::SLEEPING);
    g_assert (m1_lambda->getState () == Event::SLEEPING);

    // in next reaction, fmt is stoped
    fmt->sendTick (0, 0, 0);
    g_assert (fmt->getState () == GINGA_STATE_STOPPED);

    delete fmt;
  }

  // explicitDur in media from diferent types
  {
    for (auto sample : samples)
      {
        printf ("uri=%s\n", sample.uri);
        Formatter *fmt;
        Document *doc;
        tests_parse_and_start (&fmt, &doc,
                               xstrbuild ("\
  <ncl>\n\
    <head>\n\
    </head>\n\
    <body>\n\
      <port id='start1' component='m1'/>\n\
      <port id='start2' component='m2'/>\n\
      <media id='m1' src='%s'>\n\
        <property name='explicitDur' value='1s'/>\n\
        <property name='bounds' value='0%%,0%%,50%%,50%%'/>\n\
      </media>\n\
      <media id='m2' src='%s'>\n\
        <property name='explicitDur' value='3s'/>\n\
        <property name='bounds' value='50%%,50%%,50%%,50%%'/>\n\
      </media>\n\
    </body>\n\
  </ncl>\n",
                                          sample.uri, sample.uri));

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
        g_assert (m2_lambda->getState () == Event::OCCURRING);

        // --------------------------------
        // main check

        fmt->sendTick (1.01 * GINGA_SECOND, 1.01 * GINGA_SECOND, 0);
        g_assert (m1_lambda->getState () == Event::SLEEPING);
        g_assert (m2_lambda->getState () == Event::OCCURRING);

        fmt->sendTick (2 * GINGA_SECOND, 2 * GINGA_SECOND, 0);
        g_assert (m1_lambda->getState () == Event::SLEEPING);
        g_assert (m2_lambda->getState () == Event::SLEEPING);

        delete fmt;
      }
  }

  exit (EXIT_SUCCESS);
}
