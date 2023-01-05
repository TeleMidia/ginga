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

#ifndef WEBSERVICES_H
#define WEBSERVICES_H

#include "aux-ginga.h"
#include <libgssdp/gssdp.h>
#include <libsoup/soup.h>
#include "Event.h"

namespace ginga {
class Formatter;
class Document;
class Media;

/**
 * @brief WevServices states.
 */
typedef enum
{
  WS_STATE_STARTED,
  WS_STATE_STOPPED,
} WebServicesState;

/**
 * @brief PlayerRemoteData.
 */
class PlayerRemoteData
{
public:
  PlayerRemoteData (){};
  bool isValid ();
  string location;
  string deviceType;
  list<string> supportedFormats;
  list<string> recognizedableEvents;
};

#define SSDP_ST "urn:schemas-sbtvd-org:service:GingaCCWebServices:1"
#define SSDP_UUID "uuid:1234abcd-12ab-12ab-12ab-1234567abc13"
#define WS_NAME "TeleMidia GingaCC-WebServices"
#define WS_ROUTE_LOC "/location"
#define WS_ROUTE_PLAYER "/remote-mediaplayer"
#define WS_ROUTE_APPS "/current-service/apps/"
#define WS_PORT_DEFAULT 44642
#define WS_JSON_REMOTE_PLAYER                                              \
  "{\
     \"location\" : \"%s\", \
     \"deviceType\" : \"%s\", \
     \"supportedFormats\" : %s, \
     \"recognizableEvents\" : %s, \
   }"

#define WS_JSON_ACT                                                        \
  "{\
     \"action\": \"%s\",\
   }"
#define WS_JSON_ACT_WITH_INTERFACE                                         \
  "{\
     \"action\": \"%s\",\
     \"interface\": \"%s\",\
   }"
#define WS_JSON_ACT_WITH_INTERFACE_VALUE                                   \
  "{\
     \"action\": \"%s\",\
     \"interface\": \"%s\",\
     \"value\": \"%s\"\
   }"

#define TRACE_SOUP_REQ_MSG(msg)                                            \
  G_STMT_START                                                             \
  {                                                                        \
    SoupMessageHeadersIter it;                                             \
    const gchar *name;                                                     \
    const gchar *value;                                                    \
    soup_message_headers_iter_init (&it, msg->request_headers);            \
    while (soup_message_headers_iter_next (&it, &name, &value))            \
      TRACE ("request header %s: %s", name, value);                        \
    TRACE ("request body:\n%s\n", msg->request_body->data);                \
  }                                                                        \
  G_STMT_END

/**
 * @brief WebSercices.
 */

class WebServices
{
public:
  explicit WebServices (Formatter *);
  ~WebServices ();
  bool start ();
  bool stop ();
  WebServicesState getState ();
  bool machMediaThenSetPlayerRemote (PlayerRemoteData &);
  Formatter *getFormatter ();
  const char *host_addr;
  guint host_port;

private:
  Formatter *_formatter;
  WebServicesState _state;
  map<Media *, PlayerRemoteData> _playerMap;
  GSSDPClient *_client;
  GSSDPResourceGroup *_resource_group;
  SoupServer *_server;
};

}

#endif // WebServices_H
