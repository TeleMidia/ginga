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
  {
    Formatter *fmt;
    Document *doc;
      tests_parse_and_start (&fmt, &doc, "\
<ncl>\n\
  <body/>\n\
</ncl>\n");

      Context *body = cast (Context *, doc->getRoot ());
      g_assert_nonnull (body);
      Event *body_lambda = body->getLambda ();
      g_assert_nonnull (body_lambda);


      // --------------------------------
      // check start document

      g_assert (body_lambda->getState () == Event::OCCURRING);

      // --------------------------------
      // main check

      fmt->sendTick (0, 0, 0);
      g_assert (body_lambda->getState () == Event::SLEEPING);

      delete fmt;
  }

  {
    Formatter *fmt;
    Document *doc;
      tests_parse_and_start (&fmt, &doc, "\
<ncl>\n\
  <head>\n\
    <regionBase>\n\
      <region id='reg1' left='0' top='0' width='50%%' height='50%%'/>\n\
      <region id='reg2' left='50%%' top='0' width='50%%' height='50%%'/>\n\
      <region id='reg3' left='0' top='50%%' width='50%%' height='50%%'/>\n\
      <region id='reg4' left='50%%' top='50%%' width='50%%' height='50%%'/>\n\
    </regionBase>\n\
    <descriptorBase>\n\
      <descriptor id='desc1' region='reg1'/>\n\
      <descriptor id='desc2' region='reg2'/>\n\
      <descriptor id='desc3' region='reg3'/>\n\
      <descriptor id='desc4' region='reg4'/>\n\
    </descriptorBase>\n\
  </head>\n\
  <body/>\n\
</ncl>\n");

      Context *body = cast (Context *, doc->getRoot ());
      g_assert_nonnull (body);
      Event *body_lambda = body->getLambda ();
      g_assert_nonnull (body_lambda);


      // --------------------------------
      // check start document

      g_assert (body_lambda->getState () == Event::OCCURRING);

      // --------------------------------
      // main check

      fmt->sendTick (0, 0, 0);
      g_assert (body_lambda->getState () == Event::SLEEPING);

      delete fmt;
  }

  exit (EXIT_SUCCESS);
}
