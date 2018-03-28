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
  for (auto sample : samples)
    {
      Formatter *fmt;
      Document *doc;
      tests_parse_and_start (&fmt, &doc,
                             xstrbuild ("\
<ncl>\n\
  <head>\n\
    <connectorBase>\n\
      <causalConnector id='onBeginSet'>\n\
        <connectorParam name='var'/>\n\
        <connectorParam name='duration'/>\n\
        <simpleCondition role='onBegin'/>\n\
        <simpleAction role='set' value='$var' duration='$duration'/>\n\
      </causalConnector>\n\
    </connectorBase>\n\
  </head>\n\
  <body>\n\
    <port id='start1' component='m1'/>\n\
    <port id='start2' component='m2'/>\n\
    <port id='start3' component='m3'/>\n\
    <media id='m1' src='%s'>\n\
      <property name='width' value='50%%'/>\n\
      <property name='height' value='50%%'/>\n\
      <property name='background' value='red'/>\n\
      <property name='focusIndex' value='0'/>\n\
      <property name='moveUp' value='1'/>\n\
      <property name='moveDown' value='1'/>\n\
      <property name='moveLeft' value='1'/>\n\
      <property name='moveRight' value='1'/>\n\
    </media>\n\
    <media id='m2' src='%s'>\n\
      <property name='width' value='50%%'/>\n\
      <property name='height' value='50%%'/>\n\
      <property name='bottom' value='0'/>\n\
      <property name='right' value='0'/>\n\
      <property name='background' value='green'/>\n\
      <property name='focusIndex' value='1'/>\n\
      <property name='moveUp' value='0'/>\n\
      <property name='moveDown' value='0'/>\n\
      <property name='moveLeft' value='0'/>\n\
      <property name='moveRight' value='0'/>\n\
    </media>\n\
    <media id='m3'>\n\
      <area id='a1' begin='1s'/>\n\
      <area id='a2' begin='2s'/>\n\
      <area id='a3' begin='3s'/>\n\
      <area id='a4' begin='4s'/>\n\
    </media>\n\
    <media id='settings' type='application/x-ginga-settings'>\n\
      <property name='service.currentFocus' value='1' />\n\
    </media>\n\
    <link xconnector='onBeginSet'>\n\
      <bind role='onBegin' component='m3' interface='a1'/>\n\
      <bind role='set' component='settings' interface='service.currentFocus'>\n\
        <bindParam name='var' value='0'/>\n\
      </bind>\n\
    </link>\n\
    <link xconnector='onBeginSet'>\n\
      <bind role='onBegin' component='m3' interface='a2'/>\n\
      <bind role='set' component='settings' interface='service.currentFocus'>\n\
        <bindParam name='var' value='1'/>\n\
      </bind>\n\
    </link>\n\
    <link xconnector='onBeginSet'>\n\
      <bind role='onBegin' component='m3' interface='a3'/>\n\
      <bind role='set' component='settings' interface='service.currentFocus'>\n\
        <bindParam name='var' value='0'/>\n\
      </bind>\n\
    </link>\n\
    <link xconnector='onBeginSet'>\n\
      <bind role='onBegin' component='m3' interface='a4'/>\n\
      <bind role='set' component='settings' interface='service.currentFocus'>\n\
        <bindParam name='var' value='1'/>\n\
      </bind>\n\
    </link>\n\
  </body>\n\
</ncl>\n",
                                        sample.uri, sample.uri));

      Context *body = cast (Context *, doc->getRoot ());
      g_assert_nonnull (body);
      Event *body_lambda = body->getLambda ();
      g_assert_nonnull (body_lambda);

      Media *settings = cast (MediaSettings *, doc->getSettings ());
      g_assert_nonnull (settings);
      Event *settings_lambda = settings->getLambda ();
      g_assert_nonnull (settings_lambda);
      g_assert (settings->getProperty ("service.currentFocus") == "1");

      Media *m1 = cast (Media *, doc->getObjectById ("m1"));
      g_assert_nonnull (m1);
      Event *m1_lambda = m1->getLambda ();
      g_assert_nonnull (m1_lambda);
      g_assert (m1->getProperty ("focusIndex") == "0");

      Media *m2 = cast (Media *, doc->getObjectById ("m2"));
      g_assert_nonnull (m2);
      Event *m2_lambda = m2->getLambda ();
      g_assert_nonnull (m2_lambda);
      g_assert (m2->getProperty ("focusIndex") == "1");

      Media *m3 = cast (Media *, doc->getObjectById ("m3"));
      g_assert_nonnull (m3);
      Event *m3_lambda = m3->getLambda ();
      g_assert_nonnull (m3_lambda);
      g_assert (m3->getProperty ("focusIndex") == "");

      // --------------------------------
      // check start document

      g_assert (body_lambda->getState () == Event::OCCURRING);
      g_assert (settings_lambda->getState () == Event::OCCURRING);
      g_assert (m1_lambda->getState () == Event::SLEEPING);
      g_assert (m2_lambda->getState () == Event::SLEEPING);
      g_assert (m3_lambda->getState () == Event::SLEEPING);

      fmt->sendTick (0, 0, 0);
      g_assert (body_lambda->getState () == Event::OCCURRING);
      g_assert (settings_lambda->getState () == Event::OCCURRING);
      g_assert (m1_lambda->getState () == Event::OCCURRING);
      g_assert (m2_lambda->getState () == Event::OCCURRING);
      g_assert (m3_lambda->getState () == Event::OCCURRING);

      // --------------------------------
      // main check

      fmt->sendTick (1 * GINGA_SECOND, 1 * GINGA_SECOND, 0);
      g_assert (settings->getProperty ("service.currentFocus") == "0");
      g_assert (m1->isFocused ());
      g_assert_false (m2->isFocused ());
      g_assert_false (m3->isFocused ());

      fmt->sendTick (1 * GINGA_SECOND, 1 * GINGA_SECOND, 0);
      g_assert (settings->getProperty ("service.currentFocus") == "1");
      g_assert_false (m1->isFocused ());
      g_assert (m2->isFocused ());
      g_assert_false (m3->isFocused ());

      fmt->sendTick (1 * GINGA_SECOND, 1 * GINGA_SECOND, 0);
      g_assert (settings->getProperty ("service.currentFocus") == "0");
      g_assert (m1->isFocused ());
      g_assert_false (m2->isFocused ());
      g_assert_false (m3->isFocused ());

      fmt->sendTick (1 * GINGA_SECOND, 1 * GINGA_SECOND, 0);
      g_assert (settings->getProperty ("service.currentFocus") == "1");
      g_assert_false (m1->isFocused ());
      g_assert (m2->isFocused ());
      g_assert_false (m3->isFocused ());

      delete fmt;
    }

  exit (EXIT_SUCCESS);
}
