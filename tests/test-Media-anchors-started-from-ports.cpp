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
  // start in anchor with begin nonzero and link waiting for this anchor end
  for (int i = 0; i < samples.size (); i++)
    {
      Formatter *fmt;
      Document *doc;
      tests_parse_and_start (&fmt, &doc,
                             xstrbuild ("\
<ncl>\n\
  <head>\n\
    <connectorBase>\n\
      <causalConnector id='onBeginStart'>\n\
        <simpleCondition role='onBegin'/>\n\
        <simpleAction role='start'/>\n\
      </causalConnector>\n\
      <causalConnector id='onEndStart'>\n\
        <simpleCondition role='onEnd'/>\n\
        <simpleAction role='start'/>\n\
      </causalConnector>\n\
    </connectorBase>\n\
  </head>\n\
  <body>\n\
    <port id='start0' component='m1' interface='a2'/>\n\
    <media id='m1' src='%s'>\n\
      <area id='a1' begin='10s'/>\n\
      <area id='a2' end='20s'/>\n\
      <area id='a3' begin='30s'/>\n\
    </media>\n\
    <media id='m2' src='%s'/>\n\
    <link xconnector='onEndStart'>\n\
      <bind role='onEnd' component='m1' interface='a2'/>\n\
      <bind role='start' component='m2'/>\n\
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

      fmt->sendTick (0, 0, 0);
      g_assert (body_lambda->getState () == Event::OCCURRING);
      g_assert (m1_lambda->getState () == Event::OCCURRING);
      g_assert (m2_lambda->getState () == Event::SLEEPING);

      fmt->sendTick (20 * GINGA_SECOND, 20 * GINGA_SECOND, 0);
      g_assert (body_lambda->getState () == Event::OCCURRING);
      g_assert (m1_lambda->getState () == Event::OCCURRING);
      g_assert (m2_lambda->getState () == Event::OCCURRING);

      delete fmt;
    }

  // start in anchor with begin nonzero and link waiting another anchor with
  // begin after the begin of the first anchor
  for (int i = 0; i < samples.size (); i++)
    {
      Formatter *fmt;
      Document *doc;
      tests_parse_and_start (&fmt, &doc,
                             xstrbuild ("\
<ncl>\n\
  <head>\n\
    <connectorBase>\n\
      <causalConnector id='onBeginStart'>\n\
        <simpleCondition role='onBegin'/>\n\
        <simpleAction role='start'/>\n\
      </causalConnector>\n\
      <causalConnector id='onEndStart'>\n\
        <simpleCondition role='onEnd'/>\n\
        <simpleAction role='start'/>\n\
      </causalConnector>\n\
    </connectorBase>\n\
  </head>\n\
  <body>\n\
    <port id='start0' component='m1' interface='a1'/>\n\
    <media id='m1' src='%s'>\n\
      <area id='a1' begin='10s'/>\n\
      <area id='a2' begin='20s'/>\n\
      <area id='a3' begin='30s'/>\n\
    </media>\n\
    <media id='m2' src='%s'/>\n\
    <link xconnector='onBeginStart'>\n\
      <bind role='onBegin' component='m1' interface='a2'/>\n\
      <bind role='start' component='m2'/>\n\
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

      fmt->sendTick (0, 0, 0);
      g_assert (body_lambda->getState () == Event::OCCURRING);
      g_assert (m1_lambda->getState () == Event::OCCURRING);
      g_assert (m2_lambda->getState () == Event::SLEEPING);

      fmt->sendTick (20 * GINGA_SECOND, 20 * GINGA_SECOND, 0);
      g_assert (body_lambda->getState () == Event::OCCURRING);
      g_assert (m1_lambda->getState () == Event::OCCURRING);
      g_assert (m2_lambda->getState () == Event::OCCURRING);

      delete fmt;
    }

  // start in anchor with begin nonzero and link waiting another anchor
  // begin with time before the begin of the first anchor
  for (int i = 0; i < samples.size (); i++)
    {
      Formatter *fmt;
      Document *doc;
      tests_parse_and_start (&fmt, &doc,
                             xstrbuild ("\
<ncl>\n\
  <head>\n\
    <connectorBase>\n\
    <causalConnector id='onBeginStart'>\n\
      <simpleCondition role='onBegin'/>\n\
      <simpleAction role='start'/>\n\
    </causalConnector>\n\
    <causalConnector id='onEndStart'>\n\
      <simpleCondition role='onEnd'/>\n\
      <simpleAction role='start'/>\n\
    </causalConnector>\n\
  </connectorBase>\n\
  </head>\n\
  <body>\n\
    <port id='start0' component='m1' interface='a2'/>\n\
    <media id='m1' src='%s'>\n\
      <area id='a1' begin='10s'/>\n\
      <area id='a2' begin='20s'/>\n\
      <area id='a3' begin='30s'/>\n\
    </media>\n\
    <media id='m2' src='%s'/>\n\
    <link xconnector='onBeginStart'>\n\
      <bind role='onBegin' component='m1' interface='a1'/>\n\
      <bind role='start' component='m2'/>\n\
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

      fmt->sendTick (0, 0, 0);
      g_assert (body_lambda->getState () == Event::OCCURRING);
      g_assert (m1_lambda->getState () == Event::OCCURRING);
      g_assert (m2_lambda->getState () == Event::SLEEPING);

      fmt->sendTick (20 * GINGA_SECOND, 20 * GINGA_SECOND, 0);
      g_assert (body_lambda->getState () == Event::OCCURRING);
      g_assert (m1_lambda->getState () == Event::OCCURRING);
      g_assert (m2_lambda->getState () == Event::SLEEPING);

      delete fmt;
    }

  // start in anchor with non-zero end and link waiting
  // an anchor end with time after the end of the first anchor
  for (int i = 0; i < samples.size (); i++)
    {
      Formatter *fmt;
      Document *doc;
      tests_parse_and_start (&fmt, &doc,
                             xstrbuild ("\
<ncl>\n\
  <head>\n\
    <connectorBase>\n\
    <causalConnector id='onBeginStart'>\n\
      <simpleCondition role='onBegin'/>\n\
      <simpleAction role='start'/>\n\
    </causalConnector>\n\
    <causalConnector id='onEndStart'>\n\
      <simpleCondition role='onEnd'/>\n\
      <simpleAction role='start'/>\n\
    </causalConnector>\n\
  </connectorBase>\n\
  </head>\n\
  <body>\n\
    <port id='start0' component='m1' interface='a1'/>\n\
    <media id='m1' src='%s'>\n\
      <area id='a1' begin='10s' end='20s'/>\n\
      <area id='a2' begin='15s' end='25s'/>\n\
    </media>\n\
    <media id='m2' src='%s'/>\n\
    <link xconnector='onEndStart'>\n\
      <bind role='onEnd' component='m1' interface='a2'/>\n\
      <bind role='start' component='m2'/>\n\
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

      fmt->sendTick (0, 0, 0);
      g_assert (body_lambda->getState () == Event::OCCURRING);
      g_assert (m1_lambda->getState () == Event::OCCURRING);
      g_assert (m2_lambda->getState () == Event::SLEEPING);

      fmt->sendTick (25 * GINGA_SECOND, 25 * GINGA_SECOND, 0);
      g_assert (m1_lambda->getState () == Event::SLEEPING);
      g_assert (m2_lambda->getState () == Event::SLEEPING);

      delete fmt;
    }

  // start in anchor with non-zero end and link waiting an anchor with end
  // time the end before to the first anchor
  for (int i = 0; i < samples.size (); i++)
    {
      Formatter *fmt;
      Document *doc;
      tests_parse_and_start (&fmt, &doc,
                             xstrbuild ("\
<ncl>\n\
  <head>\n\
    <connectorBase>\n\
    <causalConnector id='onBeginStart'>\n\
      <simpleCondition role='onBegin'/>\n\
      <simpleAction role='start'/>\n\
    </causalConnector>\n\
    <causalConnector id='onEndStart'>\n\
      <simpleCondition role='onEnd'/>\n\
      <simpleAction role='start'/>\n\
    </causalConnector>\n\
  </connectorBase>\n\
  </head>\n\
  <body>\n\
    <port id='start0' component='m1' interface='a1'/>\n\
    <media id='m1' src='%s'>\n\
      <area id='a1' begin='10s' end='20s'/>\n\
      <area id='a2' begin='5s' end='15s'/>\n\
    </media>\n\
    <media id='m2' src='%s'/>\n\
    <link xconnector='onEndStart'>\n\
      <bind role='onEnd' component='m1' interface='a2'/>\n\
      <bind role='start' component='m2'/>\n\
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

      fmt->sendTick (0, 0, 0);
      g_assert (body_lambda->getState () == Event::OCCURRING);
      g_assert (m1_lambda->getState () == Event::OCCURRING);
      g_assert (m2_lambda->getState () == Event::SLEEPING);

      fmt->sendTick (20 * GINGA_SECOND, 20 * GINGA_SECOND, 0);
      g_assert (body_lambda->getState () == Event::OCCURRING);
      g_assert (m1_lambda->getState () == Event::SLEEPING);
      g_assert (m2_lambda->getState () == Event::OCCURRING);

      delete fmt;
    }

  // start an anchor and start another anchor
  for (int i = 0; i < samples.size (); i++)
    {
      Formatter *fmt;
      Document *doc;
      tests_parse_and_start (&fmt, &doc,
                             xstrbuild ("\
<ncl>\n\
  <head>\n\
    <connectorBase>\n\
    <causalConnector id='onBeginStart'>\n\
      <simpleCondition role='onBegin'/>\n\
      <simpleAction role='start'/>\n\
    </causalConnector>\n\
    <causalConnector id='onEndStart'>\n\
      <simpleCondition role='onEnd'/>\n\
      <simpleAction role='start'/>\n\
    </causalConnector>\n\
  </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start0' component='m1' interface='a1'/>\n\
    <media id='m1' src='%s'>\n\
      <area id='a1' begin='10s'/>\n\
      <area id='a2' begin='20s'/>\n\
    </media>\n\
    <media id='m2' src='%s'/>\n\
    <link xconnector='onBeginStart'>\n\
      <bind role='onBegin' component='body'/>\n\
      <bind role='start' component='m1'/>\n\
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
      g_assert (m1_lambda->getState () == Event::OCCURRING);
      g_assert (m2_lambda->getState () == Event::SLEEPING);

      fmt->sendTick (0, 0, 0);
      g_assert (body_lambda->getState () == Event::OCCURRING);
      g_assert (m1_lambda->getState () == Event::OCCURRING);
      g_assert (m2_lambda->getState () == Event::SLEEPING);

      fmt->sendTick (10 * GINGA_SECOND, 10 * GINGA_SECOND, 0);
      g_assert (body_lambda->getState () == Event::OCCURRING);
      g_assert (m1_lambda->getState () == Event::OCCURRING);
      g_assert (m2_lambda->getState () == Event::SLEEPING);

      delete fmt;
    }

  exit (EXIT_SUCCESS);
}
