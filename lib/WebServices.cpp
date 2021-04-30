#include <WebServices.h>
#include <Formatter.h>
#include <Document.h>
#include <Media.h>
#include <Document.h>
#include <gio/gio.h>
#include <json/json.h>
#include <algorithm>
#include <iostream>

#define WS_ROURTE_LOC "/location"
#define WS_ROURTE_RPLAYER "/remote-mediaplayer"
#define WS_ROURTE_APPS "/current-service/apps/"
#define WS_PORT 44642
#define SSDP_UUID "uuid:b16f8e7e-8050-11eb-8036-00155dfe4f40"
#define SSDP_DEVICE "upnp:rootdevice"
#define SSDP_NAME "TeleMidia GingaCCWebServices"
#define SSDP_USN "urn:schemas-sbtvd-org:service:GingaCCWebServices:1"

#define WS_ADD_ROUTE(ws, r, f)                                             \
  G_STMT_START                                                             \
  {                                                                        \
    soup_server_add_handler (ws, r, f, this, nullptr);                     \
  }                                                                        \
  G_STMT_END

#define TRACE_RQ_HEADERS(msg)                                              \
  G_STMT_START                                                             \
  {                                                                        \
    SoupMessageHeadersIter it;                                             \
    const gchar *name;                                                     \
    const gchar *value;                                                    \
    soup_message_headers_iter_init (&it, msg->request_headers);            \
    while (soup_message_headers_iter_next (&it, &name, &value))            \
      TRACE ("req %s: %s", name, value);                                   \
    TRACE ("req body:\n%s\n", name, value, msg->request_body->data);       \
  }                                                                        \
  G_STMT_END

/**
 * @brief Creates a new WebServices.
 * @return New #WebServices.
 */
WebServices::WebServices (Formatter *fmt)
{
  _formatter = fmt;
  _started = false;
}

WebServices::~WebServices ()
{
  g_object_unref (_resource_group);
  g_object_unref (_client);
}

bool
WebServices::machMediaThenSetPlayerRemote (PlayerRemoteData &data)
{
  if (!getCurrentDocument ())
    return false;

  auto mrts = _formatter->getDocument ()->getMediasRemote ();
  if (mrts->empty ())
    return false;

  // match media deviceType then set remotePlayerBaseURL
  bool found = false;
  auto it = find_if (mrts->begin (), mrts->end (), [data] (Media *m) {
    return (m->getProperty ("device") == data.deviceType);
  });
  if (it != mrts->end ())
    {
      (*it)->setProperty ("remotePlayerBaseURL", data.location);
      found = true;
    }

  // match supportedFormats then set remotePlayerBaseURL
  for (auto mime : data.supportedFormats)
    {
      auto it = find_if (mrts->begin (), mrts->end (), [mime] (Media *m) {
        return (m->getProperty ("type") == mime);
      });
      if (it != mrts->end ())
        {
          (*it)->setProperty ("remotePlayerBaseURL", data.location);
          found = true;
        }
    }
  return found;
}

bool
WebServices::isStarted ()
{
  return _started;
}

Document *
WebServices::getCurrentDocument ()
{
  return _formatter->getDocument ();
}

static void
ws_null_callback (SoupServer *server, SoupMessage *msg, const char *path,
                  GHashTable *query, SoupClientContext *client,
                  gpointer user_data)
{
  soup_message_set_status (msg, SOUP_STATUS_NOT_FOUND);
  soup_message_set_response (msg, "text/plan", SOUP_MEMORY_COPY,
                             "Route not implemented", 0);
}

static void
ws_loc_callback (SoupServer *server, SoupMessage *msg, const char *path,
                 GHashTable *query, SoupClientContext *client,
                 gpointer user_data)
{
  char *value;
  WebServices *ws = (WebServices *) user_data;

  soup_message_set_status (msg, SOUP_STATUS_OK);
  soup_message_set_response (msg, "text/plan", SOUP_MEMORY_COPY, NULL, 0);

  // Add GingaCC-WS headers
  value = g_strdup_printf ("http://%s:%d", ws->_host_addr, WS_PORT);
  soup_message_headers_append (msg->response_headers,
                               "GingaCC-Server-BaseURL", value);

  value = g_strdup_printf ("https://%s:%d", ws->_host_addr, WS_PORT);
  soup_message_headers_append (msg->response_headers,
                               "GingaCC-Server-SecureBaseURL", value);
  g_free (value);

  soup_message_headers_append (
      msg->response_headers, "GingaCC-Server-PairingMethods", "qcode,kex");
}

static void
ws_remoteplayer_callback (SoupServer *server, SoupMessage *msg,
                          const char *path, GHashTable *query,
                          SoupClientContext *client, gpointer user_data)
{
  Json::Value root;
  Json::CharReaderBuilder builder;
  Json::CharReader *reader = builder.newCharReader ();
  string errors;
  WebServices *ws = (WebServices *) user_data;
  bool status;

  TRACE_RQ_HEADERS (msg);

  if (!reader->parse (msg->request_body->data,
                      msg->request_body->data + msg->request_body->length,
                      &root, &errors))
    WARNING ("Error parsing request body");

  PlayerRemoteData pdata;
  pdata.location = root["location"].asString ();
  pdata.deviceType = root["deviceType"].asString ();
  list<string> supportedFormats;
  for (const auto &item : root["supportedFormats"])
    supportedFormats.push_back (item.asString ());
  list<string> recognizableEvents;
  for (const auto &item : root["recognizableEvents"])
    recognizableEvents.push_back (item.asString ());
  status = ws->machMediaThenSetPlayerRemote (pdata);
  soup_message_set_status (msg,
                           status ? SOUP_STATUS_OK : SOUP_STATUS_NOT_FOUND);
}

static void
ws_apps_callback (SoupServer *server, SoupMessage *msg, const char *path,
                  GHashTable *query, SoupClientContext *client,
                  gpointer user_data)
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
  const char *target = path + strlen (WS_ROURTE_APPS);
  gchar **params = g_strsplit (target, "/", 3);
  const char *appId = params[0];
  const char *docId = params[1];
  const char *nodeId = params[2];

  if (!ws->getCurrentDocument () || !strlen (appId) || !strlen (docId)
      || !strlen (nodeId))
    goto fail;

  TRACE ("request %s: app=%s, docId=%s nodeId=%s", WS_ROURTE_APPS, appId,
         docId, nodeId);
  TRACE_RQ_HEADERS (msg);

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

  node = ws->getCurrentDocument ()->getObjectById (nodeId);
  g_assert_nonnull (node);

  if (xstrcasecmp (action, "select") == 0)
    {
      evt = node->getSelectionEvent (value);
      ws->getCurrentDocument ()->evalAction (evt, Event::START);
      ws->getCurrentDocument ()->evalAction (evt, Event::STOP);
    }
  else if (xstrcasecmp (action, "lookAt") == 0)
    {
      evt = node->getLookAtEvent ("@lambda");
      ws->getCurrentDocument ()->evalAction (evt, Event::START);
    }
  else if (xstrcasecmp (action, "lookAway") == 0)
    {
      evt = node->getLookAtEvent ("@lambda");
      ws->getCurrentDocument ()->evalAction (evt, Event::STOP);
    }
  else
    {
      goto fail;
    }

  soup_message_set_status (msg, SOUP_STATUS_OK);

fail:
  g_strfreev (params);
  soup_message_set_status (msg, SOUP_STATUS_NOT_FOUND);
}

bool
WebServices::start ()
{
  GError *error = NULL;
  int ret = 0;
  char *location;

  // create ssdpclient
  _client = gssdp_client_new (NULL, &error);
  if (error)
    {
      g_printerr ("Error creating the GSSDP client: %s\n", error->message);
      g_error_free (error);
      return false;
    }
  _resource_group = gssdp_resource_group_new (_client);
  g_assert (_resource_group);
  _host_addr = gssdp_client_get_host_ip (_client);

  // ssdp avaliable
  location = g_strdup_printf ("http://%s:%d/location", _host_addr, WS_PORT);
  gssdp_resource_group_add_resource_simple (_resource_group, SSDP_USN,
                                            SSDP_USN, location);
  g_free (location);
  gssdp_resource_group_set_available (_resource_group, TRUE);
  g_assert (gssdp_resource_group_get_available (_resource_group));

  // create server
  _ws = soup_server_new (SOUP_SERVER_SERVER_HEADER, SSDP_NAME, nullptr);
  g_assert_nonnull (_ws);
  // set server to handle both /location and other routes in WS_PORT
  ret = soup_server_listen_all (_ws, WS_PORT, SoupServerListenOptions (0),
                                &error);
  if (!ret)
    {
      g_printerr ("could not start webservices listen: %s\n",
                  error->message);
      goto fail;
    }

  WS_ADD_ROUTE (_ws, WS_ROURTE_LOC, ws_loc_callback);
  WS_ADD_ROUTE (_ws, WS_ROURTE_RPLAYER, ws_remoteplayer_callback);
  WS_ADD_ROUTE (_ws, WS_ROURTE_APPS, ws_apps_callback);
  WS_ADD_ROUTE (_ws, nullptr, ws_null_callback);

  _started = true;
  return true;
fail:
  g_error_free (error);
  return false;
}
