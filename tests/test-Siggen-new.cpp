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

  Formatter *fmt;
  Document *doc;
  Player *p;
  tests_parse_and_start (&fmt, &doc, "\
<ncl>\n\
  <head></head>\n\
  <body>\n\
    <port id='start' component='m'/>\n\
    <port id='start2' component='m2' />\n\
    <port id='start3' component='m3' />\n\
    <port id='start4' component='m4' />\n\
    <port id='start5' component='m5'/>\n\
    <port id='start6' component='m6'/>\n\
    <port id='start7' component='m7'/>\n\
    <port id='start8' component='m8'/>\n\
    <port id='start9' component='m9'/>\n\
    <port id='start10' component='m10'/>\n\
    <port id='start11' component='m11'/>\n\
    <port id='start12' component='m12'/>\n\
    <port id='start13' component='m13'/>\n\
    <media id='m' type='application/x-ginga-siggen'>\n\
      <property name='freq' value='20000'/>\n\
      <property name='volume' value='0'/>\n\
    </media>\n\
    <media id='m2' type='application/x-ginga-siggen'>\n\
      <property name='freq' value='20150'/>\n\
      <property name='volume' value='0'/>\n\
    </media>\n\
    <media id='m3' type='application/x-ginga-siggen'>\n\
      <property name='freq' value='20300'/>\n\
      <property name='volume' value='0'/>\n\
    </media>\n\
    <media id='m4' type='application/x-ginga-siggen'>\n\
      <property name='freq' value='20450'/>\n\
      <property name='volume' value='0'/>\n\
    </media>\n\
    <media id='m5' type='application/x-ginga-siggen'>\n\
      <property name='freq' value='20600'/>\n\
      <property name='volume' value='0'/>\n\
    </media>\n\
    <media id='m6' type='application/x-ginga-siggen'>\n\
      <property name='freq' value='20750'/>\n\
      <property name='volume' value='0'/>\n\
    </media>\n\
    <media id='m7' type='application/x-ginga-siggen'>\n\
      <property name='freq' value='20900'/>\n\
      <property name='volume' value='0'/>\n\
    </media>\n\
    <media id='m8' type='application/x-ginga-siggen'>\n\
      <property name='freq' value='21050'/>\n\
      <property name='volume' value='0'/>\n\
    </media>\n\
    <media id='m9' type='application/x-ginga-siggen'>\n\
      <property name='freq' value='21200'/>\n\
      <property name='volume' value='0'/>\n\
    </media>\n\
    <media id='m10' type='application/x-ginga-siggen'>\n\
      <property name='freq' value='21350'/>\n\
      <property name='volume' value='0'/>\n\
    </media>\n\
    <media id='m11' type='application/x-ginga-siggen'>\n\
      <property name='freq' value='21500'/>\n\
      <property name='volume' value='0'/>\n\
    </media>\n\
    <media id='m12' type='application/x-ginga-siggen'>\n\
      <property name='freq' value='21650'/>\n\
      <property name='volume' value='0'/>\n\
    </media>\n\
    <media id='m13' type='application/x-ginga-siggen'>\n\
      <property name='freq' value='19500'/>\n\
      <property name='volume' value='0.5'/>\n\
    </media>\n\
  </body>\n\
</ncl>\n");

  Context *body = cast (Context *, doc->getRoot ());
  g_assert_nonnull (body);

  Media *m1 = cast (Media *, body->getChildById ("m"));
  g_assert_nonnull (m1);
  Media *m2 = cast (Media *, body->getChildById ("m2"));
  g_assert_nonnull (m2);
  Media *m3 = cast (Media *, body->getChildById ("m3"));
  g_assert_nonnull (m3);
  Media *m4 = cast (Media *, body->getChildById ("m4"));
  g_assert_nonnull (m4);
  Media *m5 = cast (Media *, body->getChildById ("m5"));
  g_assert_nonnull (m5);
  Media *m6 = cast (Media *, body->getChildById ("m6"));
  g_assert_nonnull (m6);
  Media *m7 = cast (Media *, body->getChildById ("m7"));
  g_assert_nonnull (m7);
  Media *m8 = cast (Media *, body->getChildById ("m8"));
  g_assert_nonnull (m8);
  Media *m9 = cast (Media *, body->getChildById ("m9"));
  g_assert_nonnull (m9);
  Media *m10 = cast (Media *, body->getChildById ("m10"));
  g_assert_nonnull (m10);
  Media *m11 = cast (Media *, body->getChildById ("m11"));
  g_assert_nonnull (m11);
  Media *m12 = cast (Media *, body->getChildById ("m12"));
  g_assert_nonnull (m12);
  Media *m13 = cast (Media *, body->getChildById ("m13"));
  g_assert_nonnull (m12);
  fmt->sendTick (0, 0, 0);

  delete fmt;

  exit (EXIT_SUCCESS);
}
