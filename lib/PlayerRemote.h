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

#ifndef MEDIA_REMOTE_H
#define MEDIA_REMOTE_H

#include "Player.h"
#include <libsoup/soup.h>

GINGA_NAMESPACE_BEGIN

class WebServices;

#define REMOTE_PLAYER_JSON_ACT                                             \
  "{\
     \"action\": \"%s\",\
     \"delay\": \"%d\"\
   }"

#define REMOTE_PLAYER_JSON_ACT_WITH_PROPS                                  \
  "{\
     \"action\": \"%s\",\
     \"delay\": \"%d\",\
     \"properties\": \
     {\
        \"bounds\": {\"top\": \"%s\",\"left\": \"%s\",\"width\": \"%s\",\"height\": \"%s\"},\
        \"zIndex\" : \"%s\",\
        \"transparency\" : \"%s\"\
      }\
   }"

#define REMOTE_PLAYER_JSON_MEDIA                                           \
  "{\
     \"appId\": \"%s\",\
     \"documentId\": \"%s\",\
     \"sceneNode\": \"%s\",\
     \"type\": \"%s\",\
     \"notifyEvents\": %s\
   }"

#define REMOTE_PLAYER_ROUTE_NODES "/scene/nodes/"

#define REMOTE_PLAYER_MIME_NCL360 "application/x-ncl360"

class PlayerRemote : public Player
{
public:
  explicit PlayerRemote (Formatter *, Media *);
  ~PlayerRemote ();
  void start ();
  void startPreparation ();
  void stop ();
  void pause ();
  void resume ();
  void reload ();
  void redraw (cairo_t *);

  static bool usesPlayerRemote (Media *);

protected:
  bool doSetProperty (Property, const string &, const string &);
  void sendAction (const char *);
  char *_url;
  bool _sessionStarted;
  WebServices *_ws;
  SoupSession *_session;
};

GINGA_NAMESPACE_END

#endif // MEDIA_REMOTE_H
