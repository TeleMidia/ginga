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
#include <iostream>
#include <vector>
#include <iterator>
int
main (void)
{
  for (int i = 2; i < samples_uris.size (); i++)
    {
      printf ("### %s\n", samples_uris[i]);
      Formatter *fmt;
      Document *doc;
      Player *p;
      tests_parse_and_start (&fmt, &doc, xstrbuild ("\
<ncl>\n\
  <body>\n\
    <port id='start' component='m'/>\n\
    <media id='m' src='%s'/>\n\
  </body>\n\
</ncl>\n",
                                                    samples_uris[i]));

      Context *body = cast (Context *, doc->getRoot ());
      g_assert_nonnull (body);
      Event *body_lambda = body->getLambda ();
      g_assert_nonnull (body_lambda);

      Media *m = cast (Media *, doc->getObjectById ("m"));
      g_assert_nonnull (m);
      Event *m_lambda = m->getLambda ();
      g_assert_nonnull (m_lambda);

      // test start the document
      g_assert (body_lambda->getState () == Event::OCCURRING);
      g_assert (m_lambda->getState () == Event::SLEEPING);

      fmt->sendTick (0, 0, 0);

      g_assert (body_lambda->getState () == Event::OCCURRING);
      g_assert (m_lambda->getState () == Event::OCCURRING);

      // --------------------------------
      // main check
      g_assert (m->getProperty ("uri")
                == xstrbuild ("file:%s", samples_uris[i]));
      g_assert (m->getProperty ("type") == samples_mimes[i]);

      delete m;
    }
  exit (EXIT_SUCCESS);
}