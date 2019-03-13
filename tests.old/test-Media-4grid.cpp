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
  <regionBase>\n\
    <region id='reg1' top='0%%' left='0%%' width='50%%' height='50%%'/>\n\
    <region id='reg2' top='0%%' left='50%%' width='50%%' height='50%%'/>\n\
    <region id='reg3' bottom='0%%' right='0%%' width='50%%' height='50%%'/>\n\
    <region id='reg4' bottom='0%%' right='50%%' width='50%%' height='50%%'/>\n\
  </regionBase>\n\
  <descriptorBase>\n\
    <descriptor id='desctx' region='reg1'/>\n\
    <descriptor id='desc2' region='reg2'/>\n\
    <descriptor id='desc3' region='reg3'/>\n\
    <descriptor id='desc4' region='reg4'/>\n\
  </descriptorBase>\n\
</head>\n\
<body>\n\
  <port id='start1' component='m1'/>\n\
  <port id='start2' component='m2'/>\n\
  <media id='m1' src='%s' descriptor='desctx'/>\n\
  <media id='m2' src='%s' descriptor='desc2'/>\n\
  <port id='startctx' component='ctx'/>\n\
  <context id='ctx'>\n\
    <port id='start3' component='m3'/>\n\
    <port id='start4' component='m4'/>\n\
    <media id='m3' src='%s' descriptor='desc3'/>\n\
    <media id='m4' src='%s' descriptor='desc4'/>\n\
  </context>\n\
</body>\n\
</ncl>\n",
                                        sample.uri, sample.uri, sample.uri,
                                        sample.uri));

      Context *body = cast (Context *, doc->getRoot ());
      g_assert_nonnull (body);
      Event *body_lambda = body->getLambda ();
      g_assert_nonnull (body_lambda);

      Context *ctx = cast (Context *, doc->getObjectById ("ctx"));
      g_assert_nonnull (ctx);
      Event *ctx_lambda = ctx->getLambda ();
      g_assert_nonnull (ctx_lambda);

      Media *m1 = cast (Media *, doc->getObjectById ("m1"));
      g_assert_nonnull (m1);
      Event *m1_lambda = m1->getLambda ();
      g_assert_nonnull (m1_lambda);

      Media *m2 = cast (Media *, doc->getObjectById ("m2"));
      g_assert_nonnull (m2);
      Event *m2_lambda = m2->getLambda ();
      g_assert_nonnull (m2_lambda);

      Media *m3 = cast (Media *, doc->getObjectById ("m3"));
      g_assert_nonnull (m3);
      Event *m3_lambda = m3->getLambda ();
      g_assert_nonnull (m3_lambda);

      Media *m4 = cast (Media *, doc->getObjectById ("m4"));
      g_assert_nonnull (m4);
      Event *m4_lambda = m4->getLambda ();
      g_assert_nonnull (m4_lambda);

      // --------------------------------
      // check start document

      // when start the document, only the lambda@root is OCCURING
      g_assert (body_lambda->getState () == Event::OCCURRING);
      g_assert (m1_lambda->getState () == Event::SLEEPING);
      g_assert (m2_lambda->getState () == Event::SLEEPING);
      g_assert (ctx_lambda->getState () == Event::SLEEPING);
      g_assert (m3_lambda->getState () == Event::SLEEPING);
      g_assert (m4_lambda->getState () == Event::SLEEPING);

      fmt->sendTick (0, 0, 0);

      g_assert (body_lambda->getState () == Event::OCCURRING);
      g_assert (m1_lambda->getState () == Event::OCCURRING);
      g_assert (m2_lambda->getState () == Event::OCCURRING);
      g_assert (ctx_lambda->getState () == Event::OCCURRING);
      g_assert (m3_lambda->getState () == Event::SLEEPING);
      g_assert (m4_lambda->getState () == Event::SLEEPING);

      // in next reaction, lambda@ctx1 and its anchors
      // are OCCURRING, and its properties are SLEEPING
      fmt->sendTick (0, 0, 0);
      g_assert (body_lambda->getState () == Event::OCCURRING);
      g_assert (m1_lambda->getState () == Event::OCCURRING);
      g_assert (m2_lambda->getState () == Event::OCCURRING);
      g_assert (ctx_lambda->getState () == Event::OCCURRING);
      g_assert (m3_lambda->getState () == Event::OCCURRING);
      g_assert (m4_lambda->getState () == Event::OCCURRING);

      // --------------------------------
      // main check

      g_assert (m1->getProperty ("top") == "0%");
      g_assert (m1->getProperty ("left") == "0%");
      g_assert (m1->getProperty ("width") == "50%");
      g_assert (m1->getProperty ("height") == "50%");

      g_assert (m2->getProperty ("top") == "0%");
      g_assert (m2->getProperty ("width") == "50%");
      g_assert (m2->getProperty ("left") == "50%");
      g_assert (m2->getProperty ("height") == "50%");

      g_assert (m3->getProperty ("bottom") == "0%");
      g_assert (m3->getProperty ("right") == "0%");
      g_assert (m3->getProperty ("width") == "50%");
      g_assert (m3->getProperty ("height") == "50%");

      g_assert (m4->getProperty ("bottom") == "0%");
      g_assert (m4->getProperty ("right") == "50%");
      g_assert (m4->getProperty ("width") == "50%");
      g_assert (m4->getProperty ("height") == "50%");

      delete fmt;
    }

  exit (EXIT_SUCCESS);
}
