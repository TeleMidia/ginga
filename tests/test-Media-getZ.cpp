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
  for (guint i = 0; i < samples.size (); i++)
    {
      Formatter *fmt;
      Document *doc;
      int m1_zindex, m1_zorder;
      int m2_zindex, m2_zorder;

      tests_parse_and_start (&fmt, &doc,
                             xstrbuild ("\
<ncl>\n\
  <head>\n\
    <connectorBase>\n\
      <causalConnector id='onBeginSet'>\n\
        <simpleCondition role='onBegin'/>\n\
        <simpleAction role='set' value='$var'/>\n\
      </causalConnector>\n\
    </connectorBase>\n\
  </head>\n\
  <body>\n\
    <port id='startTimer' component='timer'/>\n\
    <port id='start1' component='m1'/>\n\
    <port id='start2' component='m2'/>\n\
    <media id='timer'>\n\
      <area id='a1' begin='1s'/>\n\
      <area id='a2' begin='2s'/>\n\
    </media>\n\
    <media id='m1' src='%s'>\n\
      <property name='zIndex' value='1'/>\n\
    </media>\n\
    <media id='m2' src='%s'>\n\
      <property name='zIndex' value='2'/>\n\
    </media>\n\
    <link xconnector='onBeginSet'>\n\
      <bind role='onBegin' component='timer' interface='a1'/>\n\
      <bind role='set' component='m1' interface='zIndex'>\n\
        <bindParam name='var' value='2'/>\n\
      </bind>\n\
      <bind role='set' component='m2' interface='zIndex'>\n\
        <bindParam name='var' value='1'/>\n\
      </bind>\n\
    </link>\n\
    <link xconnector='onBeginSet'>\n\
      <bind role='onBegin' component='timer' interface='a2'/>\n\
      <bind role='set' component='m1' interface='zIndex'>\n\
        <bindParam name='var' value='1'/>\n\
      </bind>\n\
      <bind role='set' component='m2' interface='zIndex'>\n\
        <bindParam name='var' value='1'/>\n\
      </bind>\n\
    </link>\n\
  </body>\n\
</ncl>\n",
                                        samples[i].uri, samples[i].uri));

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

      // when start document, m1 is OCCURRING
      fmt->sendTick (0, 0, 0);
      g_assert (body_lambda->getState () == Event::OCCURRING);
      g_assert (m1_lambda->getState () == Event::OCCURRING);
      g_assert (m2_lambda->getState () == Event::OCCURRING);

      // --------------------------------
      // main check
      g_assert (m1->getProperty ("zIndex") == "1");
      g_assert (m2->getProperty ("zIndex") == "2");
      g_assert_true (m1->getZ (&m1_zindex, &m1_zorder));
      g_assert_cmpint (m1_zindex, ==, 1);
      g_assert_true (m2->getZ (&m2_zindex, &m2_zorder));
      g_assert_cmpint (m2_zindex, ==, 2);

      fmt->sendTick (1 * GINGA_SECOND, 1 * GINGA_SECOND, 0);
      g_assert (m1->getProperty ("zIndex") == "2");
      g_assert (m2->getProperty ("zIndex") == "1");
      g_assert_true (m1->getZ (&m1_zindex, &m1_zorder));
      g_assert_true (m2->getZ (&m2_zindex, &m2_zorder));
      g_assert_true (m1->getZ (&m1_zindex, &m1_zorder));
      g_assert_cmpint (m1_zindex, ==, 2);
      g_assert_true (m2->getZ (&m2_zindex, &m2_zorder));
      g_assert_cmpint (m2_zindex, ==, 1);

      fmt->sendTick (1 * GINGA_SECOND, 1 * GINGA_SECOND, 0);
      g_assert (m1->getProperty ("zIndex") == "1");
      g_assert (m2->getProperty ("zIndex") == "1");
      g_assert_true (m1->getZ (&m1_zindex, &m1_zorder));
      g_assert_true (m2->getZ (&m2_zindex, &m2_zorder));
      g_assert_true (m1->getZ (&m1_zindex, &m1_zorder));
      g_assert_cmpint (m1_zindex, ==, 1);
      g_assert_true (m2->getZ (&m2_zindex, &m2_zorder));
      g_assert_cmpint (m2_zindex, ==, 1);

      delete fmt;
    }

  exit (EXIT_SUCCESS);
}
