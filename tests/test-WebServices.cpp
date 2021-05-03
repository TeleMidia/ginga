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
#include "WebServices.h"
#include "PlayerRemote.h"
#include <libsoup/soup.h>

Formatter *fmt;

static void
cb_endloop (SoupSession *session, SoupMessage *msg, gpointer loop)
{
  g_assert (msg->status_code == SOUP_STATUS_OK);
  g_main_loop_quit ((GMainLoop *) loop);
}

static void
cb_test_location_endloop (SoupSession *session, SoupMessage *msg,
                          gpointer loop)
{
  SoupMessageHeadersIter it;
  const gchar *name;
  const gchar *value;
  char *url1;
  char *url2;
  const char *host_addr = fmt->getWebServices ()->host_addr;
  SoupMessageHeaders *hdrs;

  g_assert (msg->status_code == SOUP_STATUS_OK);

  url1 = g_strdup_printf ("http://%s:%d", host_addr, WS_PORT);
  url2 = g_strdup_printf ("https://%s:%d", host_addr, WS_PORT);

  hdrs = msg->response_headers;
  g_assert_cmpstr (
      soup_message_headers_get_one (hdrs, "GingaCC-Server-BaseURL"), ==,
      url1);
  g_assert_cmpstr (
      soup_message_headers_get_one (hdrs, "GingaCC-Server-SecureBaseURL"),
      ==, url2);
  g_assert_cmpstr (
      soup_message_headers_get_one (hdrs, "GingaCC-Server-PairingMethods"),
      ==, "qcode,kex");
  g_free (url1);
  g_free (url2);
  g_main_loop_quit ((GMainLoop *) loop);
}

void
test_ws_get_location (GMainLoop *loop)
{
  SoupMessage *msg;
  SoupSession *session;
  gchar *url;

  session = soup_session_new ();
  g_assert_nonnull (session);
  url = g_strdup_printf ("http://localhost:%d%s", WS_PORT, WS_ROURTE_LOC);
  g_assert_nonnull (url);
  msg = soup_message_new ("GET", url);
  g_assert_nonnull (msg);
  soup_session_queue_message (session, msg, cb_test_location_endloop, loop);

  g_free (url);
  g_object_unref (session);
}

void
test_ws_post_action (const char *node, const char *action, GMainLoop *loop)
{
  guint status;
  SoupMessage *msg;
  SoupSession *session;
  gchar *body, *url;

  session = soup_session_new ();
  g_assert_nonnull (session);
  url = g_strdup_printf ("http://localhost:%d%s1/docId/%s", WS_PORT,
                         WS_ROURTE_APPS, node);
  g_assert_nonnull (url);
  msg = soup_message_new ("POST", url);
  g_assert_nonnull (msg);
  body = g_strdup_printf (REMOTE_PLAYER_JSON_ACT, action, 0);
  g_assert_nonnull (body);
  soup_message_set_request (msg, "application/json", SOUP_MEMORY_COPY, body,
                            strlen (body));
  soup_session_queue_message (session, msg, cb_endloop, loop);

  g_free (url);
  g_free (body);
  g_object_unref (session);
}

int
main (int argc, char **argv)
{
  Document *doc;
  tests_parse_and_start (&fmt, &doc, "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='onLookAtStart'>\n\
    <simpleCondition role='onLookAt'/>\n\
    <simpleAction role='start'/>\n\
   </causalConnector>\n\
   <causalConnector id='onLookAwayStart'>\n\
    <simpleCondition role='onLookAway'/>\n\
    <simpleAction role='start'/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body id='body'>\n\
  <port id='start' component='m1'/>\n\
  <media id='m1'/>\n\
  <media id='m2'/>\n\
  <media id='m3'/>\n\
  <link xconnector='onLookAtStart'>\n\
   <bind role='onLookAt' component='m1'/>\n\
   <bind role='start' component='m2'/>\n\
  </link>\n\
  <link xconnector='onLookAwayStart'>\n\
   <bind role='onLookAway' component='m1'/>\n\
   <bind role='start' component='m3'/>\n\
  </link>\n\
 </body>\n\
</ncl>");
  Formatter::setOptionDebug (fmt, "debug", true);
  Formatter::setOptionWebServices (fmt, "webservices", true);

  GMainLoop *loop;
  loop = g_main_loop_new (NULL, FALSE);

  // test get location
  test_ws_get_location (loop);
  g_main_loop_run (loop);

  // test post actions: lookAt and lookAway
  {
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
    Media *m3 = cast (Media *, doc->getObjectById ("m3"));
    g_assert_nonnull (m3);
    Event *m3_lambda = m3->getLambda ();
    g_assert_nonnull (m3_lambda);

    // when document is started, only the body_lambda is OCCURING
    g_assert_cmpint ((body_lambda)->getState (), ==, Event::OCCURRING);
    g_assert_cmpint ((m1_lambda)->getState (), ==, Event::SLEEPING);
    g_assert_cmpint ((m2_lambda)->getState (), ==, Event::SLEEPING);
    g_assert_cmpint ((m3_lambda)->getState (), ==, Event::SLEEPING);

    // when advance time, m1_lambda is OCCURRING
    (fmt)->sendTick (0, 0, 0);
    g_assert_cmpint ((body_lambda)->getState (), ==, Event::OCCURRING);
    g_assert_cmpint ((m1_lambda)->getState (), ==, Event::OCCURRING);
    g_assert_cmpint ((m2_lambda)->getState (), ==, Event::SLEEPING);
    g_assert_cmpint ((m3_lambda)->getState (), ==, Event::SLEEPING);

    // START is done
    Event *evtOnLookAt = m1->getLookAtEvent ("@lambda");
    g_assert_nonnull (evtOnLookAt);

    // lookAt
    test_ws_post_action ("m1", "lookAt", loop);
    g_main_loop_run (loop);

    // after START, m1_onLooAt m2 OCCURRING
    (fmt)->sendTick (0, 0, 0);
    g_assert_cmpint ((body_lambda)->getState (), ==, Event::OCCURRING);
    g_assert_cmpint ((m1_lambda)->getState (), ==, Event::OCCURRING);
    g_assert_cmpint ((m2_lambda)->getState (), ==, Event::OCCURRING);
    g_assert_cmpint ((m3_lambda)->getState (), ==, Event::SLEEPING);

    // lookAway
    test_ws_post_action ("m1", "lookAway", loop);
    g_main_loop_run (loop);

    // after START, m1_onLooAway m3 are OCCURRING
    (fmt)->sendTick (0, 0, 0);
    g_assert_cmpint ((body_lambda)->getState (), ==, Event::OCCURRING);
    g_assert_cmpint ((m1_lambda)->getState (), ==, Event::OCCURRING);
    g_assert_cmpint ((m2_lambda)->getState (), ==, Event::OCCURRING);
    g_assert_cmpint ((m3_lambda)->getState (), ==, Event::OCCURRING);
  }

  g_main_loop_unref (loop);
  delete fmt;
  exit (EXIT_SUCCESS);
}
