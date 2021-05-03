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

GINGA_NAMESPACE_BEGIN
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
typedef struct
{
  string location;
  string deviceType;
  list<string> supportedFormats;
  list<string> recognizedableEvents;
} PlayerRemoteData;


#define WS_ROURTE_LOC "/location"
#define WS_ROURTE_RPLAYER "/remote-mediaplayer"
#define WS_ROURTE_APPS "/current-service/apps/"
#define WS_PORT 44642
#define SSDP_UUID "uuid:b16f8e7e-8050-11eb-8036-00155dfe4f40"
#define SSDP_DEVICE "upnp:rootdevice"
#define SSDP_NAME "TeleMidia GingaCCWebServices"
#define SSDP_USN "urn:schemas-sbtvd-org:service:GingaCCWebServices:1"

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

private:
  Formatter *_formatter;
  WebServicesState _state;
  map<Media *, PlayerRemoteData> _playerMap;
  GSSDPClient *_client;
  GSSDPResourceGroup *_resource_group;
  SoupServer *_ws;
};

GINGA_NAMESPACE_END

#endif // WebServices_H
