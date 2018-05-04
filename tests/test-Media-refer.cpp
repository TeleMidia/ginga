/* Copyright (C) 2006-2018 PUC-Rio/Laboratorio TeleMidia

This file is part of Ginga (Ginga-NCL).

Ginga is free software: you can yistribute it and/or modify it
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
  // referred object: defined first
  // reference: defined after
  // init document: start referred object
  // link on: referred object
  "<ncl>\n\
  <head>\n\
  <connectorBase>\n\
    <causalConnector id='onBeginStart'>\n\
      <simpleCondition role='onBegin'/>\n\
      <simpleAction role='start'/>\n\
    </causalConnector>\n\
  </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start' component='m1'/>\n\
    <media id='m1' src='http://raw.github.com/telemidia/ginga/master/tests-ncl/samples/gnu.png'>\n\
      <property name='x' value='y'/>\n\
    </media>\n\
    <media id='r1' refer='m1'>\n\
      <property name='x' value='z'/>\n\
    </media>\n\
    <media id='m2'/>\n\
    <link xconnector='onBeginStart'>\n\
      <bind role='onBegin' component='m1'/>\n\
      <bind role='start' component='m2'/>\n\
    </link>\n\
  </body>\n\
</ncl>\n",
  // referred object: defined first
  // reference: defined after
  // init document: start referred object
  // link on: reference
  "<ncl>\n\
  <head>\n\
  <connectorBase>\n\
    <causalConnector id='onBeginStart'>\n\
      <simpleCondition role='onBegin'/>\n\
      <simpleAction role='start'/>\n\
    </causalConnector>\n\
  </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start' component='m1'/>\n\
    <media id='m1' src='http://raw.github.com/telemidia/ginga/master/tests-ncl/samples/gnu.png'>\n\
      <property name='x' value='y'/>\n\
    </media>\n\
    <media id='r1' refer='m1'>\n\
      <property name='x' value='z'/>\n\
    </media>\n\
    <media id='m2'/>\n\
    <link xconnector='onBeginStart'>\n\
      <bind role='onBegin' component='r1'/>\n\
      <bind role='start' component='m2'/>\n\
    </link>\n\
  </body>\n\
</ncl>\n",
  // referred object: defined first
  // reference: defined after
  // init document: start reference
  // link on: referred object
  "<ncl>\n\
  <head>\n\
  <connectorBase>\n\
    <causalConnector id='onBeginStart'>\n\
      <simpleCondition role='onBegin'/>\n\
      <simpleAction role='start'/>\n\
    </causalConnector>\n\
  </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start' component='r1'/>\n\
    <media id='m1' src='http://raw.github.com/telemidia/ginga/master/tests-ncl/samples/gnu.png'>\n\
      <property name='x' value='y'/>\n\
    </media>\n\
    <media id='r1' refer='m1'>\n\
      <property name='x' value='z'/>\n\
    </media>\n\
    <media id='m2'/>\n\
    <link xconnector='onBeginStart'>\n\
      <bind role='onBegin' component='m1'/>\n\
      <bind role='start' component='m2'/>\n\
    </link>\n\
  </body>\n\
</ncl>\n",
  // referred object: defined first
  // reference: defined after
  // init document: start reference
  // link on: reference
  "<ncl>\n\
  <head>\n\
  <connectorBase>\n\
    <causalConnector id='onBeginStart'>\n\
      <simpleCondition role='onBegin'/>\n\
      <simpleAction role='start'/>\n\
    </causalConnector>\n\
  </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start' component='r1'/>\n\
    <media id='m1' src='http://raw.github.com/telemidia/ginga/master/tests-ncl/samples/gnu.png'>\n\
      <property name='x' value='y'/>\n\
    </media>\n\
    <media id='r1' refer='m1'>\n\
      <property name='x' value='z'/>\n\
    </media>\n\
    <media id='m2'/>\n\
    <link xconnector='onBeginStart'>\n\
      <bind role='onBegin' component='r1'/>\n\
      <bind role='start' component='m2'/>\n\
    </link>\n\
  </body>\n\
</ncl>\n",
  // referred object: defined after
  // reference: defined first
  // init document: start referred object
  // link on: referred object
  "<ncl>\n\
  <head>\n\
  <connectorBase>\n\
    <causalConnector id='onBeginStart'>\n\
      <simpleCondition role='onBegin'/>\n\
      <simpleAction role='start'/>\n\
    </causalConnector>\n\
  </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start' component='m1'/>\n\
    <media id='r1' refer='m1'>\n\
      <property name='x' value='y'/>\n\
    </media>\n\
    <media id='m1' src='http://raw.github.com/telemidia/ginga/master/tests-ncl/samples/gnu.png'>\n\
      <property name='x' value='z'/>\n\
    </media>\n\
    <media id='m2'/>\n\
    <link xconnector='onBeginStart'>\n\
      <bind role='onBegin' component='m1'/>\n\
      <bind role='start' component='m2'/>\n\
    </link>\n\
  </body>\n\
</ncl>\n",
  // referred object: defined after
  // reference: defined first
  // init document: start referred object
  // link on: reference
  "<ncl>\n\
  <head>\n\
  <connectorBase>\n\
    <causalConnector id='onBeginStart'>\n\
      <simpleCondition role='onBegin'/>\n\
      <simpleAction role='start'/>\n\
    </causalConnector>\n\
  </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start' component='m1'/>\n\
    <media id='r1' refer='m1'>\n\
      <property name='x' value='y'/>\n\
    </media>\n\
    <media id='m1' src='http://raw.github.com/telemidia/ginga/master/tests-ncl/samples/gnu.png'>\n\
      <property name='x' value='z'/>\n\
    </media>\n\
    <media id='m2'/>\n\
    <link xconnector='onBeginStart'>\n\
      <bind role='onBegin' component='r1'/>\n\
      <bind role='start' component='m2'/>\n\
    </link>\n\
  </body>\n\
</ncl>\n",
  // referred object: defined after
  // reference: defined first
  // init document: start reference
  // link on: referred object
  "<ncl>\n\
  <head>\n\
  <connectorBase>\n\
    <causalConnector id='onBeginStart'>\n\
      <simpleCondition role='onBegin'/>\n\
      <simpleAction role='start'/>\n\
    </causalConnector>\n\
  </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start' component='r1'/>\n\
    <media id='r1' refer='m1'>\n\
      <property name='x' value='y'/>\n\
    </media>\n\
    <media id='m1' src='http://raw.github.com/telemidia/ginga/master/tests-ncl/samples/gnu.png'>\n\
      <property name='x' value='z'/>\n\
    </media>\n\
    <media id='m2'/>\n\
    <link xconnector='onBeginStart'>\n\
      <bind role='onBegin' component='m1'/>\n\
      <bind role='start' component='m2'/>\n\
    </link>\n\
  </body>\n\
</ncl>\n",
  // referred object: defined after
  // reference: defined first
  // init document: start reference
  // link on: reference
  "<ncl>\n\
  <head>\n\
  <connectorBase>\n\
    <causalConnector id='onBeginStart'>\n\
      <simpleCondition role='onBegin'/>\n\
      <simpleAction role='start'/>\n\
    </causalConnector>\n\
  </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start' component='r1'/>\n\
    <media id='r1' refer='m1'>\n\
      <property name='x' value='y'/>\n\
    </media>\n\
    <media id='m1' src='http://raw.github.com/telemidia/ginga/master/tests-ncl/samples/gnu.png'>\n\
      <property name='x' value='z'/>\n\
    </media>\n\
    <media id='m2'/>\n\
    <link xconnector='onBeginStart'>\n\
      <bind role='onBegin' component='r1'/>\n\
      <bind role='start' component='m2'/>\n\
    </link>\n\
  </body>\n\
</ncl>\n",
  // referred object: defined first
  // reference: defined after in nested context
  // init document: start referred object
  // link on: referred object
  "<ncl>\n\
  <head>\n\
  <connectorBase>\n\
    <causalConnector id='onBeginStart'>\n\
      <simpleCondition role='onBegin'/>\n\
      <simpleAction role='start'/>\n\
    </causalConnector>\n\
  </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start' component='m1'/>\n\
    <media id='m1' src='http://raw.github.com/telemidia/ginga/master/tests-ncl/samples/gnu.png'>\n\
      <property name='x' value='y'/>\n\
    </media>\n\
    <context id='ctx1'>\n\
      <port id='start1' component='r1'/>\n\
      <media id='r1' refer='m1'>\n\
        <property name='x' value='z'/>\n\
      </media>\n\
    </context>\n\
    <media id='m2'/>\n\
    <link xconnector='onBeginStart'>\n\
      <bind role='onBegin' component='m1'/>\n\
      <bind role='start' component='m2'/>\n\
    </link>\n\
  </body>\n\
</ncl>\n",
  // referred object: defined first
  // reference: defined after in nested context
  // init document: start referred object
  // link on: reference
  "<ncl>\n\
  <head>\n\
  <connectorBase>\n\
    <causalConnector id='onBeginStart'>\n\
      <simpleCondition role='onBegin'/>\n\
      <simpleAction role='start'/>\n\
    </causalConnector>\n\
  </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start' component='m1'/>\n\
    <media id='m1' src='http://raw.github.com/telemidia/ginga/master/tests-ncl/samples/gnu.png'>\n\
      <property name='x' value='y'/>\n\
    </media>\n\
    <context id='ctx1'>\n\
      <port id='start1' component='r1'/>\n\
      <media id='r1' refer='m1'>\n\
        <property name='x' value='z'/>\n\
      </media>\n\
    </context>\n\
    <media id='m2'/>\n\
    <link xconnector='onBeginStart'>\n\
      <bind role='onBegin' component='ctx1' interface='start1'/>\n\
      <bind role='start' component='m2'/>\n\
    </link>\n\
  </body>\n\
</ncl>\n",
  // referred object: defined first
  // reference: defined after in nested context
  // init document: start reference
  // link on: referred object
  "<ncl>\n\
  <head>\n\
  <connectorBase>\n\
    <causalConnector id='onBeginStart'>\n\
      <simpleCondition role='onBegin'/>\n\
      <simpleAction role='start'/>\n\
    </causalConnector>\n\
  </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start' component='ctx1' interface='start1'/>\n\
    <media id='m1' src='http://raw.github.com/telemidia/ginga/master/tests-ncl/samples/gnu.png'>\n\
      <property name='x' value='y'/>\n\
    </media>\n\
    <context id='ctx1'>\n\
      <port id='start1' component='r1'/>\n\
      <media id='r1' refer='m1'>\n\
        <property name='x' value='z'/>\n\
      </media>\n\
    </context>\n\
    <media id='m2'/>\n\
    <link xconnector='onBeginStart'>\n\
      <bind role='onBegin' component='m1'/>\n\
      <bind role='start' component='m2'/>\n\
    </link>\n\
  </body>\n\
</ncl>\n",
  // referred object: defined first
  // reference: defined after in nested context
  // init document: start reference
  // link on: reference
  "<ncl>\n\
  <head>\n\
  <connectorBase>\n\
    <causalConnector id='onBeginStart'>\n\
      <simpleCondition role='onBegin'/>\n\
      <simpleAction role='start'/>\n\
    </causalConnector>\n\
  </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start' component='ctx1' interface='start1'/>\n\
    <media id='m1' src='http://raw.github.com/telemidia/ginga/master/tests-ncl/samples/gnu.png'>\n\
      <property name='x' value='y'/>\n\
    </media>\n\
    <context id='ctx1'>\n\
      <port id='start1' component='r1'/>\n\
      <media id='r1' refer='m1'>\n\
        <property name='x' value='z'/>\n\
      </media>\n\
    </context>\n\
    <media id='m2'/>\n\
    <link xconnector='onBeginStart'>\n\
      <bind role='onBegin' component='ctx1' interface='start1'/>\n\
      <bind role='start' component='m2'/>\n\
    </link>\n\
  </body>\n\
</ncl>\n",
  // referred object: defined first in nested context
  // reference: defined after
  // init document: start referred object
  // link on: referred object
  "<ncl>\n\
  <head>\n\
  <connectorBase>\n\
    <causalConnector id='onBeginStart'>\n\
      <simpleCondition role='onBegin'/>\n\
      <simpleAction role='start'/>\n\
    </causalConnector>\n\
  </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start' component='ctx1' interface='start1'/>\n\
    <context id='ctx1'>\n\
      <port id='start1' component='m1'/>\n\
      <media id='m1' src='http://raw.github.com/telemidia/ginga/master/tests-ncl/samples/gnu.png'>\n\
        <property name='x' value='y'/>\n\
      </media>\n\
    </context>\n\
    <media id='r1' refer='m1'>\n\
      <property name='x' value='z'/>\n\
    </media>\n\
    <media id='m2'/>\n\
    <link xconnector='onBeginStart'>\n\
      <bind role='onBegin' component='ctx1' interface='start1'/>\n\
      <bind role='start' component='m2'/>\n\
    </link>\n\
  </body>\n\
</ncl>\n",
  // referred object: defined first in nested context
  // reference: defined after
  // init document: start referred object
  // link on: reference
  "<ncl>\n\
  <head>\n\
  <connectorBase>\n\
    <causalConnector id='onBeginStart'>\n\
      <simpleCondition role='onBegin'/>\n\
      <simpleAction role='start'/>\n\
    </causalConnector>\n\
  </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start' component='ctx1' interface='start1'/>\n\
    <context id='ctx1'>\n\
      <port id='start1' component='m1'/>\n\
      <media id='m1' src='http://raw.github.com/telemidia/ginga/master/tests-ncl/samples/gnu.png'>\n\
        <property name='x' value='y'/>\n\
      </media>\n\
    </context>\n\
    <media id='r1' refer='m1'>\n\
      <property name='x' value='z'/>\n\
    </media>\n\
    <media id='m2'/>\n\
    <link xconnector='onBeginStart'>\n\
      <bind role='onBegin' component='r1'/>\n\
      <bind role='start' component='m2'/>\n\
    </link>\n\
  </body>\n\
</ncl>\n",
  // referred object: defined first in nested context
  // reference: defined after
  // init document: start reference
  // link on: referred object
  "<ncl>\n\
  <head>\n\
  <connectorBase>\n\
    <causalConnector id='onBeginStart'>\n\
      <simpleCondition role='onBegin'/>\n\
      <simpleAction role='start'/>\n\
    </causalConnector>\n\
  </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start' component='r1'/>\n\
    <context id='ctx1'>\n\
      <port id='start1' component='m1'/>\n\
      <media id='m1' src='http://raw.github.com/telemidia/ginga/master/tests-ncl/samples/gnu.png'>\n\
        <property name='x' value='y'/>\n\
      </media>\n\
    </context>\n\
    <media id='r1' refer='m1'>\n\
      <property name='x' value='z'/>\n\
    </media>\n\
    <media id='m2'/>\n\
    <link xconnector='onBeginStart'>\n\
      <bind role='onBegin' component='ctx1' interface='start1'/>\n\
      <bind role='start' component='m2'/>\n\
    </link>\n\
  </body>\n\
</ncl>\n",
  // referred object: defined first in nested context
  // reference: defined after
  // init document: start reference
  // link on: reference
  "<ncl>\n\
  <head>\n\
  <connectorBase>\n\
    <causalConnector id='onBeginStart'>\n\
      <simpleCondition role='onBegin'/>\n\
      <simpleAction role='start'/>\n\
    </causalConnector>\n\
  </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start' component='r1'/>\n\
    <context id='ctx1'>\n\
      <port id='start1' component='m1'/>\n\
      <media id='m1' src='http://raw.github.com/telemidia/ginga/master/tests-ncl/samples/gnu.png'>\n\
        <property name='x' value='y'/>\n\
      </media>\n\
    </context>\n\
    <media id='r1' refer='m1'>\n\
      <property name='x' value='z'/>\n\
    </media>\n\
    <media id='m2'/>\n\
    <link xconnector='onBeginStart'>\n\
      <bind role='onBegin' component='r1'/>\n\
      <bind role='start' component='m2'/>\n\
    </link>\n\
  </body>\n\
</ncl>\n",
  // referred object: defined first in nested context
  // reference: defined after in nested context
  // init document: start referred object
  // link on: referred object
  "<ncl>\n\
  <head>\n\
  <connectorBase>\n\
    <causalConnector id='onBeginStart'>\n\
      <simpleCondition role='onBegin'/>\n\
      <simpleAction role='start'/>\n\
    </causalConnector>\n\
  </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start' component='ctx1'  interface='start1'/>\n\
    <context id='ctx1'>\n\
      <port id='start1' component='m1'/>\n\
      <media id='m1' src='http://raw.github.com/telemidia/ginga/master/tests-ncl/samples/gnu.png'>\n\
        <property name='x' value='y'/>\n\
      </media>\n\
    </context>\n\
    <context id='ctx2'>\n\
      <media id='r1' refer='m1'>\n\
        <property name='x' value='z'/>\n\
      </media>\n\
    </context>\n\
    <media id='m2'/>\n\
    <link xconnector='onBeginStart'>\n\
      <bind role='onBegin' component='ctx1' interface='start1'/>\n\
      <bind role='start' component='m2'/>\n\
    </link>\n\
  </body>\n\
</ncl>\n",
  // referred object: defined first in nested context
  // reference: defined after in nested context
  // init document: start referred object
  // link on: reference
  "<ncl>\n\
  <head>\n\
  <connectorBase>\n\
    <causalConnector id='onBeginStart'>\n\
      <simpleCondition role='onBegin'/>\n\
      <simpleAction role='start'/>\n\
    </causalConnector>\n\
  </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start' component='ctx1'  interface='start1'/>\n\
    <context id='ctx1'>\n\
      <port id='start1' component='m1'/>\n\
      <media id='m1' src='http://raw.github.com/telemidia/ginga/master/tests-ncl/samples/gnu.png'>\n\
        <property name='x' value='y'/>\n\
      </media>\n\
    </context>\n\
    <context id='ctx2'>\n\
      <media id='r1' refer='m1'>\n\
        <property name='x' value='z'/>\n\
      </media>\n\
      <link xconnector='onBeginStart'>\n\
        <bind role='onBegin' component='r1'/>\n\
        <bind role='start' component='m2'/>\n\
      </link>\n\
      <media id='m2'/>\n\
    </context>\n\
  </body>\n\
</ncl>\n",
  // referred object: defined first in nested context
  // reference: defined after in nested context
  // init document: start reference
  // link on: referred object
  "<ncl>\n\
  <head>\n\
  <connectorBase>\n\
    <causalConnector id='onBeginStart'>\n\
      <simpleCondition role='onBegin'/>\n\
      <simpleAction role='start'/>\n\
    </causalConnector>\n\
  </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start' component='ctx2'  interface='start2'/>\n\
    <context id='ctx1'>\n\
      <media id='m1' src='http://raw.github.com/telemidia/ginga/master/tests-ncl/samples/gnu.png'>\n\
        <property name='x' value='y'/>\n\
      </media>\n\
      <media id='m2'/>\n\
      <link xconnector='onBeginStart'>\n\
        <bind role='onBegin' component='m1'/>\n\
        <bind role='start' component='m2'/>\n\
      </link>\n\
    </context>\n\
    <context id='ctx2'>\n\
      <port id='start2' component='r1'/>\n\
      <media id='r1' refer='m1'>\n\
        <property name='x' value='z'/>\n\
      </media>\n\
    </context>\n\
  </body>\n\
</ncl>\n",
  // referred object: defined first in nested context
  // reference: defined after in nested context
  // init document: start reference
  // link on: reference
  "<ncl>\n\
  <head>\n\
  <connectorBase>\n\
    <causalConnector id='onBeginStart'>\n\
      <simpleCondition role='onBegin'/>\n\
      <simpleAction role='start'/>\n\
    </causalConnector>\n\
  </connectorBase>\n\
  </head>\n\
  <body id='body'>\n\
    <port id='start' component='ctx2'  interface='start2'/>\n\
    <context id='ctx1'>\n\
      <media id='m1' src='http://raw.github.com/telemidia/ginga/master/tests-ncl/samples/gnu.png'>\n\
        <property name='x' value='y'/>\n\
      </media>\n\
    </context>\n\
    <context id='ctx2'>\n\
      <port id='start2' component='r1'/>\n\
      <media id='r1' refer='m1'>\n\
        <property name='x' value='z'/>\n\
      </media>\n\
      <media id='m2'/>\n\
      <link xconnector='onBeginStart'>\n\
        <bind role='onBegin' component='r1'/>\n\
        <bind role='start' component='m2'/>\n\
      </link>\n\
    </context>\n\
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

      Media *m2 = cast (Media *, doc->getObjectById ("m2"));
      g_assert_nonnull (m1);
      Event *m2_lambda = m2->getLambda ();
      g_assert_nonnull (m2_lambda);

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
      g_assert (m2_lambda->getState () == Event::OCCURRING);

      // --------------------------------
      // main check

      g_assert (m1->getProperty ("uri") == r1->getProperty ("uri"));
      g_assert (m1->getProperty ("x") == r1->getProperty ("x"));
      g_assert (m1->getProperty ("x") == "z");
      delete fmt;
    }

  exit (EXIT_SUCCESS);
}
