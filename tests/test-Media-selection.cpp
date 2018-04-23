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
  for (int i = 0; i < samples.size (); i++)
    {
      Formatter *fmt;
      Document *doc;
      tests_parse_and_start (&fmt, &doc,
                             xstrbuild ("\
<ncl>\n\
  <head>\n\
    <connectorBase>\n\
      <causalConnector id='onSectionStart'>\n\
        <simpleCondition role='onSelection' key='$key'/>\n\
        <simpleAction role='start'/>\n\
      </causalConnector>\n\
    </connectorBase>\n\
  </head>\n\
  <body>\n\
    <port id='start0' component='m1'/>\n\
    <media id='m1' src='%s'>\n\
      <property name='focusIndex' value='0'/>\n\
    </media>\n\
    <media id='m2' src='%s'/>\n\
    <link xconnector='onSectionStart'>\n\
      <bind role='onSelection' component='m1'/>\n\
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
      Event *m1_selection = m1->getSelectionEvent ("");
      g_assert_nonnull (m1_selection);

      Media *m2 = cast (Media *, doc->getObjectById ("m2"));
      g_assert_nonnull (m2);
      Event *m2_lambda = m2->getLambda ();
      g_assert_nonnull (m2_lambda);

      // --------------------------------
      // check start document

      g_assert (body_lambda->getState () == Event::OCCURRING);
      g_assert (m1_lambda->getState () == Event::SLEEPING);
      g_assert (m1_selection->getState () == Event::SLEEPING);
      g_assert (m2_lambda->getState () == Event::SLEEPING);

      // when start document, m1 is OCCURRING
      fmt->sendTick (0, 0, 0);
      g_assert (body_lambda->getState () == Event::OCCURRING);
      g_assert (m1_lambda->getState () == Event::OCCURRING);
      g_assert (m1_selection->getState () == Event::SLEEPING);
      g_assert (m2_lambda->getState () == Event::SLEEPING);

      // --------------------------------
      // main check

      // when sendKey, m1_selection and m2_lambda are OCCURRING
      fmt->sendKey ("ENTER", true);
      g_assert (body_lambda->getState () == Event::OCCURRING);
      g_assert (m1_lambda->getState () == Event::OCCURRING);
      g_assert (m1_selection->getState () == Event::OCCURRING);
      g_assert (m2_lambda->getState () == Event::OCCURRING);

      delete fmt;
    }
  exit (EXIT_SUCCESS);
}
