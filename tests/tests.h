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

#ifndef TESTS_H
#define TESTS_H

#include "aux-ginga.h"
#include "ginga.h"

#include "Context.h"
#include "Document.h"
#include "Event.h"
#include "Media.h"
#include "MediaSettings.h"
#include "Parser.h"
#include "ParserLua.h"
#include "Switch.h"

typedef struct
{
  const char *mime;
  const char *uri;
} sample;

vector<const char *> samples_keys
    = { "RED", "GREEN", "YELLOW", "BLUE", "INFO" };
vector<sample> samples = {
  { "application/x-ginga-timer", "" },
  { "audio/mp3", ABS_TOP_SRCDIR "/tests-ncl/samples/arcade.mp3" },
  { "image/png", ABS_TOP_SRCDIR "/tests-ncl/samples/gnu.png" },
  { "application/x-ginga-NCLua",
    ABS_TOP_SRCDIR "/tests-ncl/samples/fps.lua" },
  { "image/svg+xml", ABS_TOP_SRCDIR "/tests-ncl/samples/vector.svg" },
  { "text/plain", ABS_TOP_SRCDIR "/tests-ncl/samples/text.txt" },
  { "video/ogg", ABS_TOP_SRCDIR "/tests-ncl/samples/clock.ogv" },
  { "text/html", ABS_TOP_SRCDIR "/tests-ncl/samples/page.html" },
};

static G_GNUC_UNUSED string
tests_write_tmp_file (const string &buf, const string &file_ext = "ncl")
{
  string path;
  gchar *filename;
  gint fd;
  GError *error = nullptr;

  string file_name = string ("ginga-tests-XXXXXX.") + file_ext;

  // g_file_open_tmp should follow the rules for mkdtemp() templates
  fd = g_file_open_tmp (file_name.c_str (), &filename, &error);
  if (unlikely (error != nullptr))
    {
      ERROR ("*** Unexpected error: %s", error->message);
      g_error_free (error);
    }
  g_close (fd, nullptr);

  path = filename;
  g_assert (g_file_set_contents (path.c_str (), buf.c_str (), -1, nullptr));

  g_free (filename);
  return path;
}

static G_GNUC_UNUSED void
tests_parse_and_start (Formatter **fmt, Document **doc, const string &buf,
                       const string &file_ext = "ncl")
{
  string errmsg;
  string file;

  tryset (fmt, new Formatter (nullptr));
  g_assert_nonnull (*fmt);

  file = tests_write_tmp_file (buf, file_ext);
  if (!(*fmt)->start (file, &errmsg))
    {
      g_printerr ("*** Unexpected error: %s", errmsg.c_str ());
      g_assert_not_reached ();
    }

  tryset (doc, (*fmt)->getDocument ());
  g_assert_nonnull (*doc);
  g_assert (g_remove (file.c_str ()) == 0);
}

static G_GNUC_UNUSED void
tests_create_document (Document **doc, Context **root,
                       MediaSettings **settings)
{
  tryset (doc, new Document ());
  g_assert_nonnull (doc);

  tryset (root, (*doc)->getRoot ());
  g_assert_nonnull (root);
  g_assert ((*doc)->getObjectById ("__root__") == *root);

  tryset (settings, (*doc)->getSettings ());
  g_assert_nonnull (settings);
  g_assert ((*doc)->getObjectById ("__settings__") == *settings);
}

static G_GNUC_UNUSED void
tests_create_document_with_media_and_start (Formatter **fmt,
                                            Event **body_lambda,
                                            Event **m1_lambda,
                                            Event **m1_anchor_0s,
                                            Event **m1_anchor_label,
                                            Event **m1_prop, Event **m1_sel)
{
  Document *doc;
  Context *body;
  Media *m1;

  tests_parse_and_start (fmt, &doc, "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='onSelectionStart'>\n\
    <simpleCondition role='onSelection'/>\n\
    <simpleAction role='start'/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body id='body'>\n\
  <port id='start' component='m1'/>\n\
  <media id='m1'>\n\
   <property name='p1' value='0'/>\n\
   <area id='a1' begin='0s'/>\n\
   <area id='a2' label='l'/>\n\
  </media>\n\
  <link xconnector='onSelectionStart'>\n\
   <bind role='onSelection' component='m1'/>\n\
   <bind role='start' component='body'/>\n\
  </link>\n\
 </body>\n\
</ncl>");

  body = cast (Context *, doc->getRoot ());
  g_assert_nonnull (body);
  *body_lambda = body->getLambda ();
  g_assert_nonnull (*body_lambda);

  m1 = cast (Media *, body->getChildById ("m1"));
  g_assert_nonnull (m1);
  *m1_lambda = m1->getLambda ();
  g_assert_nonnull (*m1_lambda);
  *m1_anchor_0s = m1->getPresentationEvent ("a1");
  g_assert_nonnull (*m1_anchor_0s);
  *m1_anchor_label = m1->getPresentationEvent ("a2");
  g_assert_nonnull (*m1_anchor_label);
  *m1_prop = m1->getAttributionEvent ("p1");
  g_assert_nonnull (*m1_prop);
  *m1_sel = m1->getSelectionEvent ("");
  g_assert_nonnull (*m1_sel);

  // --------------------------------
  // check start document

  // when document is started, only the body_lambda is OCCURING
  g_assert_cmpint ((*body_lambda)->getState (), ==, Event::OCCURRING);
  g_assert_cmpint ((*m1_lambda)->getState (), ==, Event::SLEEPING);
  g_assert_cmpint ((*m1_anchor_0s)->getState (), ==, Event::SLEEPING);
  g_assert_cmpint ((*m1_anchor_label)->getState (), ==, Event::SLEEPING);
  g_assert_cmpint ((*m1_prop)->getState (), ==, Event::SLEEPING);
  g_assert_cmpint ((*m1_sel)->getState (), ==, Event::SLEEPING);

  // when advance time, m1_lambda is OCCURRING
  (*fmt)->sendTick (0, 0, 0);
  g_assert_cmpint ((*body_lambda)->getState (), ==, Event::OCCURRING);
  g_assert_cmpint ((*m1_lambda)->getState (), ==, Event::OCCURRING);
  g_assert_cmpint ((*m1_anchor_0s)->getState (), ==, Event::SLEEPING);
  g_assert_cmpint ((*m1_anchor_label)->getState (), ==, Event::SLEEPING);
  g_assert_cmpint ((*m1_prop)->getState (), ==, Event::SLEEPING);
  g_assert_cmpint ((*m1_sel)->getState (), ==, Event::SLEEPING);

  // when advance time, m1_anchor_0s is OCCURRING
  (*fmt)->sendTick (0, 0, 0);
  g_assert_cmpint ((*body_lambda)->getState (), ==, Event::OCCURRING);
  g_assert_cmpint ((*m1_lambda)->getState (), ==, Event::OCCURRING);
  g_assert_cmpint ((*m1_anchor_0s)->getState (), ==, Event::OCCURRING);
  g_assert_cmpint ((*m1_anchor_label)->getState (), ==, Event::SLEEPING);
  g_assert_cmpint ((*m1_prop)->getState (), ==, Event::SLEEPING);
  g_assert_cmpint ((*m1_sel)->getState (), ==, Event::SLEEPING);
}

static G_GNUC_UNUSED void
tests_create_document_with_context_and_start (
    Formatter **fmt, Event **body_lambda, Event **c1_lambda,
    Event **c1_prop, Event **m1_lambda, Event **m2_lambda)
{
  Document *doc;
  Context *body, *c1;
  Media *m1, *m2;

  tests_parse_and_start (fmt, &doc, "\
<ncl>\n\
<body>\n\
  <port id='p1' component='c1'/>\n\
  <context id='c1'>\n\
    <property name='p1' value='0'/>\n\
    <port id='port1' component='m1'/>\n\
    <port id='port2' component='m2'/>\n\
    <media id='m1'/>\n\
    <media id='m2'/>\n\
  </context>\n\
</body>\n\
</ncl>");

  body = cast (Context *, doc->getRoot ());
  g_assert_nonnull (body);
  *body_lambda = body->getLambda ();
  g_assert_nonnull (*body_lambda);

  c1 = cast (Context *, body->getChildById ("c1"));
  g_assert_nonnull (c1);
  *c1_lambda = c1->getLambda ();
  g_assert_nonnull (c1_lambda);
  *c1_prop = c1->getAttributionEvent ("p1");
  g_assert_nonnull (c1_prop);

  m1 = cast (Media *, c1->getChildById ("m1"));
  g_assert_nonnull (m1);
  *m1_lambda = m1->getLambda ();
  g_assert_nonnull (m1_lambda);

  m2 = cast (Media *, c1->getChildById ("m2"));
  g_assert_nonnull (m2);
  *m2_lambda = m2->getLambda ();
  g_assert_nonnull (m2_lambda);

  // --------------------------------
  // check start document

  // when document is started, only the body_lambda is OCCURING
  g_assert_cmpint ((*body_lambda)->getState (), ==, Event::OCCURRING);
  g_assert_cmpint ((*c1_lambda)->getState (), ==, Event::SLEEPING);
  g_assert_cmpint ((*m1_lambda)->getState (), ==, Event::SLEEPING);
  g_assert_cmpint ((*m2_lambda)->getState (), ==, Event::SLEEPING);
  g_assert_cmpint ((*c1_prop)->getState (), ==, Event::SLEEPING);

  // when advance time, c1_lambda is OCCURRING
  (*fmt)->sendTick (0, 0, 0);
  g_assert_cmpint ((*body_lambda)->getState (), ==, Event::OCCURRING);
  g_assert_cmpint ((*c1_lambda)->getState (), ==, Event::OCCURRING);
  g_assert_cmpint ((*m1_lambda)->getState (), ==, Event::SLEEPING);
  g_assert_cmpint ((*m2_lambda)->getState (), ==, Event::SLEEPING);
  g_assert_cmpint ((*c1_prop)->getState (), ==, Event::SLEEPING);

  // when advance time, m1_lambda and m2_lambda are OCCURRING
  (*fmt)->sendTick (0, 0, 0);
  g_assert_cmpint ((*body_lambda)->getState (), ==, Event::OCCURRING);
  g_assert_cmpint ((*c1_lambda)->getState (), ==, Event::OCCURRING);
  g_assert_cmpint ((*m1_lambda)->getState (), ==, Event::OCCURRING);
  g_assert_cmpint ((*m2_lambda)->getState (), ==, Event::OCCURRING);
  g_assert_cmpint ((*c1_prop)->getState (), ==, Event::SLEEPING);
}

static G_GNUC_UNUSED void
tests_create_document_with_switch_and_start (
    Formatter **fmt, Event **body_lambda, Event **swt1_lambda,
    Event **swt1_sel, Event **m1_lambda, Event **m2_lambda)
{
  Document *doc;
  Context *body;
  Switch *swt1;
  Media *m1, *m2;

  tests_parse_and_start (fmt, &doc, "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='onSelectionStart'>\n\
    <simpleCondition role='onSelection'/>\n\
    <simpleAction role='start'/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
  <ruleBase>\n\
    <rule id='r1' var='var1' value='m1' comparator='eq'/>\n\
    <rule id='r2' var='var1' value='m2' comparator='eq'/>\n\
  </ruleBase>\n\
</head>\n\
<body id='body'>\n\
  <port id='p1' component='swt1'/>\n\
  <media id='stgs' type='application/x-ginga-settings'>\n\
    <property name='var1' value='m1'/>\n\
  </media>\n\
  <switch id='swt1'>\n\
    <bindRule constituent='m1' rule='r1'/>\n\
    <bindRule constituent='m2' rule='r2'/>\n\
    <media id='m1'/>\n\
    <media id='m2'/>\n\
  </switch>\n\
  <link xconnector='onSelectionStart'>\n\
   <bind role='onSelection' component='swt1'/>\n\
   <bind role='start' component='body'/>\n\
  </link>\n\
</body>\n\
</ncl>");

  body = cast (Context *, doc->getRoot ());
  g_assert_nonnull (body);
  *body_lambda = body->getLambda ();
  g_assert_nonnull (*body_lambda);

  swt1 = cast (Switch *, body->getChildById ("swt1"));
  g_assert_nonnull (swt1);
  *swt1_lambda = swt1->getLambda ();
  g_assert_nonnull (swt1_lambda);
  // *swt1_sel = swt1->getSelectionEvent ("");
  // g_assert_nonnull (*m1_sel);

  m1 = cast (Media *, swt1->getChildById ("m1"));
  g_assert_nonnull (m1);
  *m1_lambda = m1->getLambda ();
  g_assert_nonnull (m1_lambda);

  m2 = cast (Media *, swt1->getChildById ("m2"));
  g_assert_nonnull (m2);
  *m2_lambda = m2->getLambda ();
  g_assert_nonnull (m2_lambda);

  // --------------------------------
  // check start document

  // when document is started, only the body_lambda is OCCURING
  g_assert_cmpint ((*body_lambda)->getState (), ==, Event::OCCURRING);
  g_assert_cmpint ((*swt1_lambda)->getState (), ==, Event::SLEEPING);
  g_assert_cmpint ((*m1_lambda)->getState (), ==, Event::SLEEPING);
  g_assert_cmpint ((*m2_lambda)->getState (), ==, Event::SLEEPING);
  // g_assert_cmpint ((*swt1_sel)->getState (), ==, Event::SLEEPING);

  // when advance time, c1_lambda is OCCURRING
  (*fmt)->sendTick (0, 0, 0);
  g_assert_cmpint ((*body_lambda)->getState (), ==, Event::OCCURRING);
  g_assert_cmpint ((*swt1_lambda)->getState (), ==, Event::OCCURRING);
  g_assert_cmpint ((*m1_lambda)->getState (), ==, Event::OCCURRING);
  g_assert_cmpint ((*m2_lambda)->getState (), ==, Event::SLEEPING);
  // g_assert_cmpint ((*swt1_sel)->getState (), ==, Event::SLEEPING);
}

#endif // TESTS_H
