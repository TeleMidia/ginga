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

GINGA_NAMESPACE_BEGIN
class Formatter;
class Document;
class Media;

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

/**
 * @brief WebSercices.
 */

class WebServices
{
public:
  explicit WebServices (Formatter *);
  ~WebServices ();
  bool start ();
  bool isStarted ();
  const char *_host_addr;
  bool machMediaThenSetPlayerRemote (PlayerRemoteData &);
  Document *getCurrentDocument ();

private:
  Formatter *_formatter;
  bool _started;
  map<Media *, PlayerRemoteData> _playerMap;
  GSSDPClient *_client;
  GSSDPResourceGroup *_resource_group;
  SoupServer *_ws;
};

GINGA_NAMESPACE_END

#endif // WebServices_H
