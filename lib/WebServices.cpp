#include <WebServices.h>
#include <Formatter.h>
#include <Document.h>
#include <PlayerRemote.h>
#include <Media.h>
#include <Document.h>
#include <gio/gio.h>
#include <json/json.h>
#include <algorithm>
#include <iostream>

#define WS_ADD_ROUTE(ws, r, f)                                             \
  G_STMT_START                                                             \
  {                                                                        \
    soup_server_add_handler (ws, r, f, this, nullptr);                     \
  }                                                                        \
  G_STMT_END

#define WS_DEFAULT_APPID "1"
#define WS_DEFAULT_DOCID "ncl"
#define WS_DEFAULT_JSON_NOTIFY_EVTS                                        \
  "[\"selection\", \"onlookAt\", \"onlookAway\"]"

bool
PlayerRemoteData::isValid ()
{
  return (!location.empty () && !deviceType.empty ()
          && !supportedFormats.empty () && !recognizedableEvents.empty ());
}

/**
 * @brief Creates a new WebServices.
 * @return New #WebServices.
 */
WebServices::WebServices (Formatter *fmt)
{
  _formatter = fmt;
  _resource_group = nullptr;
  _client = nullptr;
  _server = nullptr;
  _state = WS_STATE_STOPPED;
}

WebServices::~WebServices ()
{
  this->stop ();
  g_object_unref (_resource_group);
  g_object_unref (_client);
  g_object_unref (_server);
}

bool
WebServices::stop ()
{
  _state = WS_STATE_STOPPED;
  // stop ssdp
  gssdp_resource_group_set_available (_resource_group, false);
  // stop http
  soup_server_disconnect (_server);
  return true;
}

static void
cb_null (SoupServer *server, SoupMessage *msg, const char *path,
         GHashTable *query, SoupClientContext *client, gpointer user_data)
{
  soup_message_set_status (msg, SOUP_STATUS_NOT_FOUND);
  soup_message_set_response (msg, "text/plan", SOUP_MEMORY_COPY,
                             "Route not implemented", 0);
}

static void
cb_locaction (SoupServer *server, SoupMessage *msg, const char *path,
              GHashTable *query, SoupClientContext *client,
              gpointer user_data)
{
  char *value;
  WebServices *ws = (WebServices *) user_data;

  soup_message_set_status (msg, SOUP_STATUS_OK);
  soup_message_set_response (msg, "text/plan", SOUP_MEMORY_COPY, nullptr,
                             0);

  // add GingaCC-Server-* headers
  value = g_strdup_printf ("http://%s:%d", ws->host_addr, WS_PORT);
  soup_message_headers_append (msg->response_headers,
                               "GingaCC-Server-BaseURL", value);
  g_free (value);
  value = g_strdup_printf ("https://%s:%d", ws->host_addr, WS_PORT);
  soup_message_headers_append (msg->response_headers,
                               "GingaCC-Server-SecureBaseURL", value);
  g_free (value);
  soup_message_headers_append (
      msg->response_headers, "GingaCC-Server-PairingMethods", "qcode,kex");
}

static void
cb_remoteplayer (SoupServer *server, SoupMessage *msg, const char *path,
                 GHashTable *query, SoupClientContext *client,
                 gpointer user_data)
{
  Json::Value root;
  Json::CharReaderBuilder builder;
  Json::CharReader *reader = builder.newCharReader ();
  string errors;
  WebServices *ws = (WebServices *) user_data;
  bool status = false;

  TRACE_SOUP_REQ_MSG (msg);

  if (!reader->parse (msg->request_body->data,
                      msg->request_body->data + msg->request_body->length,
                      &root, &errors))
    WARNING ("Error parsing request body");

  PlayerRemoteData pdata;
  pdata.location = root["location"].asString ();
  pdata.deviceType = root["deviceType"].asString ();
  for (const auto &item : root["supportedFormats"])
    pdata.supportedFormats.push_back (item.asString ());
  for (const auto &item : root["recognizableEvents"])
    pdata.recognizedableEvents.push_back (item.asString ());
  if (pdata.isValid ())
    status = ws->machMediaThenSetPlayerRemote (pdata);
  soup_message_set_status (msg,
                           status ? SOUP_STATUS_OK : SOUP_STATUS_NOT_FOUND);
}

static void
cb_apps (SoupServer *server, SoupMessage *msg, const char *path,
         GHashTable *query, SoupClientContext *client, gpointer user_data)
{
  Json::Value root;
  Json::CharReaderBuilder builder;
  Json::CharReader *reader = builder.newCharReader ();
  string errors;
  WebServices *ws = (WebServices *) user_data;
  bool status = false;
  string action, interface, value;
  Object *node;
  Event *evt;
  Document *doc;
  const char *target = path + strlen (WS_ROUTE_APPS);
  gchar **params = g_strsplit (target, "/", 3);

  doc = ws->getFormatter ()->getDocument ();
  g_assert_nonnull (doc);
  const char *appId = params[0]; // ignored
  const char *docId = params[1]; // ignored
  const char *nodeId = params[2];
  if (!strlen (appId) || !strlen (docId) || !strlen (nodeId))
    goto fail;

  TRACE ("request %s: app=%s, docId=%s nodeId=%s", WS_ROUTE_APPS, appId,
         docId, nodeId);
  TRACE_SOUP_REQ_MSG (msg);

  if (!reader->parse (msg->request_body->data,
                      msg->request_body->data + msg->request_body->length,
                      &root, &errors))
    {
      WARNING ("Error parsing request body");
      goto fail;
    }
  action = root["action"].asString ();
  interface = root["interface"].asString ();
  value = root["value"].asString ();

  node = doc->getObjectById (nodeId);
  g_strfreev (params);

  if (!node)
    goto fail;

  if (action == "select")
    {
      evt = node->getSelectionEvent (value);
      doc->evalAction (evt, Event::START);
      doc->evalAction (evt, Event::STOP);
    }
  else if (action == "lookAt")
    {
      evt = node->getLookAtEvent ("@lambda");
      doc->evalAction (evt, Event::START);
    }
  else if (action == "lookAway")
    {
      evt = node->getLookAtEvent ("@lambda");
      doc->evalAction (evt, Event::STOP);
    }
  else
    goto fail;

  soup_message_set_status (msg, SOUP_STATUS_OK);
  return;

fail:
  soup_message_set_status (msg, SOUP_STATUS_NOT_FOUND);
}

bool
WebServices::start ()
{
  GError *error = nullptr;
  int ret = 0;
  char *location;

  // create ssdpclient
  _client = gssdp_client_new (nullptr, &error);
  if (error)
    {
      g_printerr ("Error creating the GSSDP client: %s\n", error->message);
      g_error_free (error);
      return false;
    }
  gssdp_client_set_server_id (_client, WS_NAME);
  _resource_group = gssdp_resource_group_new (_client);
  g_assert (_resource_group);
  host_addr = gssdp_client_get_host_ip (_client);

  // ssdp avaliable
  location = g_strdup_printf ("http://%s:%d/location", host_addr, WS_PORT);
  gssdp_resource_group_add_resource_simple (
      _resource_group, SSDP_ST, SSDP_UUID "::" SSDP_ST, location);
  gssdp_resource_group_set_available (_resource_group, true);
  g_assert (gssdp_resource_group_get_available (_resource_group));
  g_assert (gssdp_client_get_active (_client));

  // create server
  _server = soup_server_new (SOUP_SERVER_SERVER_HEADER, WS_NAME, nullptr);
  g_assert_nonnull (_server);
  // set server to handle both /location and other routes in WS_PORT
  ret = soup_server_listen_all (_server, WS_PORT,
                                SoupServerListenOptions (0), &error);
  if (!ret)
    {
      g_printerr ("Fails start WebServices: %s\n", error->message);
      g_assert_not_reached ();
    }

  WS_ADD_ROUTE (_server, WS_ROUTE_LOC, cb_locaction);
  WS_ADD_ROUTE (_server, WS_ROUTE_PLAYER, cb_remoteplayer);
  WS_ADD_ROUTE (_server, WS_ROUTE_APPS, cb_apps);
  WS_ADD_ROUTE (_server, nullptr, cb_null);

  _state = WS_STATE_STARTED;

  g_free (location);
  return true;
fail:
  g_error_free (error);
  return false;
}

bool
WebServices::machMediaThenSetPlayerRemote (PlayerRemoteData &data)
{
  if (!_formatter->getDocument ())
    return false;

  auto mrts = _formatter->getDocument ()->getMediasRemote ();
  if (mrts->empty ())
    return false;

  for (auto m : *mrts)
    {
      // match media deviceType then set remotePlayerBaseURL
      if (m->getProperty ("device") == data.deviceType)
        m->setProperty ("remotePlayerBaseURL", data.location);

      // match supportedFormats then set remotePlayerBaseURL
      for (auto mime : data.supportedFormats)
        if (m->getProperty ("type") == mime)
          m->setProperty ("remotePlayerBaseURL", data.location);

      if (m->getProperty ("remotePlayerBaseURL") != "")
        {
          SoupSession *session;
          SoupMessage *msg;
          char *body;

          session = soup_session_new ();
          msg = soup_message_new ("POST", data.location.c_str ());
          body
              = g_strdup_printf (REMOTE_PLAYER_JSON_MEDIA, // json format
                                 WS_DEFAULT_APPID,         // appId
                                 WS_DEFAULT_DOCID,         // documentID
                                 m->getId ().c_str (),     // sceneNode
                                 m->getProperty ("type").c_str (), // type
                                 WS_DEFAULT_JSON_NOTIFY_EVTS // notifyEvents
              );
          soup_message_set_request (msg, "application/json",
                                    SOUP_MEMORY_COPY, body, strlen (body));
          soup_session_queue_message (session, msg, nullptr, this);

          g_free (body);
          g_object_unref (session);
        }
    }
  return true;
}

WebServicesState
WebServices::getState ()
{
  return _state;
}

Formatter *
WebServices::getFormatter ()
{
  return _formatter;
}