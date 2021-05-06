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
#include <json/json.h>

Formatter *fmt;
char *test_remote_player_location;
int test_remote_player_port;
const char *expectedAction;

static void
cb_test_ok (SoupSession *session, SoupMessage *msg, gpointer loop)
{
  g_assert (msg->status_code == SOUP_STATUS_OK);
  g_main_loop_quit ((GMainLoop *) loop);
  g_object_unref (session);
}

void
remoteplayer_registry (GMainLoop *loop)
{
  SoupMessage *msg;
  SoupSession *session;
  gchar *body, *url;

  session = soup_session_new ();
  g_assert_nonnull (session);
  url = g_strdup_printf ("http://localhost:%d%s", WS_PORT, WS_ROUTE_PLAYER);
  g_assert_nonnull (url);
  msg = soup_message_new (SOUP_METHOD_POST, url);
  g_assert_nonnull (msg);
  body
      = g_strdup_printf (WS_JSON_REMOTE_PLAYER, test_remote_player_location,
                         "ncl360", "[\"" REMOTE_PLAYER_MIME_NCL360 "\"]",
                         "[\"onLookAt\", \"onLookAway\"]");
  g_assert_nonnull (body);
  soup_message_headers_append (msg->request_headers, "Accept",
                               "application/json");
  soup_message_set_request (msg, "application/json", SOUP_MEMORY_COPY, body,
                            strlen (body));
  soup_session_queue_message (session, msg, cb_test_ok, loop);

  g_free (body);
  g_free (url);
}

static void
cb_remoteplayer_action (SoupServer *server, SoupMessage *msg,
                        const char *path, GHashTable *query,
                        SoupClientContext *client, gpointer loop)
{
  Json::Value root;
  Json::CharReaderBuilder builder;
  Json::CharReader *reader = builder.newCharReader ();
  string errors;

  TRACE_SOUP_REQ_MSG (msg);
  if (!reader->parse (msg->request_body->data,
                      msg->request_body->data + msg->request_body->length,
                      &root, &errors))
    ERROR ("Error parsing request body");

  g_assert (root["action"].asString () == expectedAction);
  g_assert (root["delay"].asString () == "0");
  soup_message_set_status (msg, SOUP_STATUS_OK);

  g_main_loop_quit ((GMainLoop *) loop);
}

void
setup_remoteplayer (const char *nodeId, GMainLoop *loop)
{
  GError *error = NULL;
  SoupServer *_server;
  int ret = 0;
  char *route;

  _server = soup_server_new (SOUP_SERVER_SERVER_HEADER, "remoteplayer", nullptr);
  g_assert_nonnull (_server);
  // set server to handle both /location and other routes in WS_PORT
  ret = soup_server_listen_all (_server, test_remote_player_port,
                                SoupServerListenOptions (0), &error);
  if (!ret)
    ERROR ("could not start remoteplayer listen: %s\n", error->message);

  route = g_strdup_printf ("%s%s", REMOTE_PLAYER_ROUTE_NODES, nodeId);
  soup_server_add_handler (_server, route, cb_remoteplayer_action, loop,
                           nullptr);

  g_free (route);
}

int
main (int argc, char **argv)
{
  Document *doc;
  GMainLoop *loop = g_main_loop_new (NULL, FALSE);

  tests_parse_and_start (&fmt, &doc, "\
<ncl>\n\
 <head>\n\
  <connectorBase>\n\
   <causalConnector id='onBeginStart'>\n\
    <simpleCondition role='onBegin'/>\n\
    <simpleAction role='start'/>\n\
   </causalConnector>\n\
   <causalConnector id='onEndStart'>\n\
    <simpleCondition role='onEnd'/>\n\
    <simpleAction role='start'/>\n\
   </causalConnector>\n\
  </connectorBase>\n\
 </head>\n\
 <body id='body'>\n\
  <port id='entry' component='hidden'/>\n\
  <media id='hidden'/>\n\
  <media id='m1' type='application/x-ncl360'/>\n\
  <media id='m2'/>\n\
  <media id='m3'/>\n\
  <link xconnector='onBeginStart'>\n\
   <bind role='onBegin' component='m1'/>\n\
   <bind role='start' component='m2'/>\n\
  </link>\n\
  <link xconnector='onEndStart'>\n\
   <bind role='onEnd' component='m1'/>\n\
   <bind role='start' component='m3'/>\n\
  </link>\n\
 </body>\n\
</ncl>");
  Formatter::setOptionDebug (fmt, "debug", true);
  Formatter::setOptionWebServices (fmt, "webservices", true);

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

  // test mime
  g_assert (m1->getProperty ("type") == REMOTE_PLAYER_MIME_NCL360);

  test_remote_player_port = WS_PORT + 10; // different port then WS_PORT
  test_remote_player_location
      = g_strdup_printf ("http://%s:%d",
                         fmt->getWebServices ()->host_addr, // ws addres
                         test_remote_player_port);

  // test post remoteplayer
  remoteplayer_registry (loop);
  g_main_loop_run (loop);
  g_assert (m1->getProperty ("remotePlayerBaseURL")
            == test_remote_player_location);

  // test post actions: lookAt and lookAway
  {
    // when document is started, only the body_lambda is OCCURING
    g_assert_cmpint ((body_lambda)->getState (), ==, Event::OCCURRING);
    g_assert_cmpint ((m1_lambda)->getState (), ==, Event::SLEEPING);
    g_assert_cmpint ((m2_lambda)->getState (), ==, Event::SLEEPING);
    g_assert_cmpint ((m3_lambda)->getState (), ==, Event::SLEEPING);

    // when advance time, m1_lambda is OCCURRING
    (fmt)->sendTick (0, 0, 0);
    g_assert_cmpint ((body_lambda)->getState (), ==, Event::OCCURRING);
    g_assert_cmpint ((m1_lambda)->getState (), ==, Event::SLEEPING);
    g_assert_cmpint ((m2_lambda)->getState (), ==, Event::SLEEPING);
    g_assert_cmpint ((m3_lambda)->getState (), ==, Event::SLEEPING);

    // setup remoteplayer
    setup_remoteplayer ("m1", loop);

    // perform remoteplayer start
    expectedAction = "start";
    doc->evalAction (m1->getLambda (), Event::Transition::START);
    (fmt)->sendTick (0, 0, 0);
    g_main_loop_run (loop);

    // after START m1, m2 is OCCURRING
    (fmt)->sendTick (0, 0, 0);
    g_assert_cmpint ((body_lambda)->getState (), ==, Event::OCCURRING);
    g_assert_cmpint ((m1_lambda)->getState (), ==, Event::OCCURRING);
    g_assert_cmpint ((m2_lambda)->getState (), ==, Event::OCCURRING);
    g_assert_cmpint ((m3_lambda)->getState (), ==, Event::SLEEPING);

    // perform remoteplayer stop
    expectedAction = "stop";
    doc->evalAction (m1->getLambda (), Event::Transition::STOP);
    (fmt)->sendTick (0, 0, 0);
    g_main_loop_run (loop);

    // after STOP m1, m3 is OCCURRING
    (fmt)->sendTick (0, 0, 0);
    g_assert_cmpint ((body_lambda)->getState (), ==, Event::OCCURRING);
    g_assert_cmpint ((m1_lambda)->getState (), ==, Event::SLEEPING);
    g_assert_cmpint ((m2_lambda)->getState (), ==, Event::OCCURRING);
    g_assert_cmpint ((m3_lambda)->getState (), ==, Event::OCCURRING);
  }

  g_main_loop_unref (loop);
  delete fmt;
  g_free (test_remote_player_location);
  exit (EXIT_SUCCESS);
}
