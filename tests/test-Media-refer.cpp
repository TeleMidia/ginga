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

vector<string> ncl_with_refer_samples = {
  // media followed by refer and start media
  "<ncl>\n\
  <body>\n\
    <port id='start' component='m1'/>\n\
    <media id='m1' src='http://raw.github.com/telemidia/ginga/master/tests-ncl/samples/gnu.png'>\n\
      <property name='background' value='red'/>\n\
    </media>\n\
    <media id='r1' refer='m1'>\n\
      <property name='background' value='green'/>\n\
    </media>\n\
  </body>\n\
</ncl>\n",
  // media followed by refer and start refer
  "<ncl>\n\
  <body>\n\
    <port id='start' component='r1'/>\n\
    <media id='m1' src='http://raw.github.com/telemidia/ginga/master/tests-ncl/samples/gnu.png'>\n\
      <property name='background' value='red'/>\n\
    </media>\n\
    <media id='r1' refer='m1'>\n\
      <property name='background' value='green'/>\n\
    </media>\n\
  </body>\n\
</ncl>\n",
  // refer followed by media and start media
  "<ncl>\n\
  <body>\n\
    <port id='start' component='m1'/>\n\
    <media id='r1' refer='m1'>\n\
      <property name='background' value='red'/>\n\
    </media>\n\
    <media id='m1' src='http://raw.github.com/telemidia/ginga/master/tests-ncl/samples/gnu.png'>\n\
      <property name='background' value='green'/>\n\
    </media>\n\
  </body>\n\
</ncl>\n",
  // refer followed by media and start refer
  "<ncl>\n\
  <body>\n\
    <port id='start' component='r1'/>\n\
    <media id='r1' refer='m1'>\n\
      <property name='background' value='red'/>\n\
    </media>\n\
    <media id='m1' src='http://raw.github.com/telemidia/ginga/master/tests-ncl/samples/gnu.png'>\n\
      <property name='background' value='green'/>\n\
    </media>\n\
  </body>\n\
</ncl>\n"
};

int
main (void)
{
  for (guint i = 0; i < ncl_with_refer_samples.size (); i++)
    {
      Formatter *fmt;
      Document *doc;
      tests_parse_and_start (&fmt, &doc, ncl_with_refer_samples[i]);

      Context *body = cast (Context *, doc->getRoot ());
      g_assert_nonnull (body);
      Event *body_lambda = body->getLambda ();
      g_assert_nonnull (body_lambda);

      Media *m1 = cast (Media *, doc->getObjectById ("m1"));
      g_assert_nonnull (m1);
      Event *m1_lambda = m1->getLambda ();
      g_assert_nonnull (m1_lambda);

      Media *r1 = cast (Media *, doc->getObjectByIdOrAlias ("r1"));
      g_assert_nonnull (r1);
      Event *r1_lambda = r1->getLambda ();
      g_assert_nonnull (r1_lambda);

      // --------------------------------
      // check start document

      g_assert (body_lambda->getState () == Event::OCCURRING);
      g_assert (m1_lambda->getState () == Event::SLEEPING);
      g_assert (r1_lambda->getState () == Event::SLEEPING);

      // when start document, m1 is OCCURRING
      fmt->sendTick (0, 0, 0);
      g_assert (body_lambda->getState () == Event::OCCURRING);
      g_assert (m1_lambda->getState () == Event::OCCURRING);
      g_assert (r1_lambda->getState () == Event::OCCURRING);

      // --------------------------------
      // main check

      g_assert (m1->getProperty ("uri") == r1->getProperty ("uri"));
      g_assert (m1->getProperty ("background")
                == r1->getProperty ("background"));
      delete fmt;
    }

  exit (EXIT_SUCCESS);
}
