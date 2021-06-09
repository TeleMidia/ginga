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

#include "PlayerRemote.h"
#include "aux-ginga.h"
#include "aux-gl.h"
#include "WebServices.h"
#include "Media.h"
#include "Formatter.h"
#include <libsoup/soup.h>

GINGA_NAMESPACE_BEGIN

/**
 * @brief Creates PlayerRemote
 * @param fmt Formatter
 * @param media Media Element
 */

PlayerRemote::PlayerRemote (Formatter *fmt, Media *media)
    : Player (fmt, media)
{
  _session = nullptr;
}

/**
 * @brief Evaluates if Media uses PlayerRemote
 */
bool
PlayerRemote::usesPlayerRemote (Media *media)
{
  string mime = media->getProperty ("type");
  string uri = media->getProperty ("uri");
  if (mime.empty ())
    getMimeForURI (uri, &mime);
  if (mime == REMOTE_PLAYER_MIME_NCL360)
    return true;
  string device = media->getProperty ("device");
  if (!device.empty ())
    return true;
  return false;
}

PlayerRemote::~PlayerRemote ()
{
  g_object_unref (_session);
}

static void
cb_action (SoupSession *session, SoupMessage *msg, gpointer user_data)
{
  string url;
  auto player = (PlayerRemote *) user_data;
  if (!msg->status_code == SOUP_STATUS_OK)
    WARNING ("Failed to perform request to %s",
             player->getProperty ("remotePlayerBaseURL").c_str ());
}

void
PlayerRemote::sendAction (const string &body, const string &label = "")
{
  guint status;
  SoupMessage *msg;

  if (!_session)
    _session = soup_session_new ();

  string url = xstrbuild ("%s" REMOTE_PLAYER_ROUTE_NODES "%s",
                          getProperty ("remotePlayerBaseURL").c_str (),
                          (label == "") ? _media->getId ().c_str ()
                                        : label.c_str ());

  msg = soup_message_new (SOUP_METHOD_POST, url.c_str ());
  soup_message_set_request (msg, "application/json", SOUP_MEMORY_COPY,
                            body.c_str (), strlen (body.c_str ()));
  soup_session_queue_message (_session, msg, cb_action, this);
}

void
PlayerRemote::start ()
{
  string zOrder = getProperty ("zOrder");
  string body_props = xstrbuild (
      REMOTE_PLAYER_JSON_ACT_WITH_PROPS, "start", 0,
      getProperty ("top").c_str (), getProperty ("left").c_str (),
      getProperty ("width").c_str (), getProperty ("height").c_str (),
      getProperty ("zOrder").c_str (), "0%");
  sendAction (body_props);
  Player::start ();
}

void
PlayerRemote::startPreparation ()
{
  string body = xstrbuild (REMOTE_PLAYER_JSON_ACT, "prepare", 0);
  sendAction (body);
  Player::startPreparation ();
}

void
PlayerRemote::stop ()
{
  string body = xstrbuild (REMOTE_PLAYER_JSON_ACT, "stop", 0);
  sendAction (body);
  Player::stop ();
}

void
PlayerRemote::pause ()
{
  string body = xstrbuild (REMOTE_PLAYER_JSON_ACT, "pause", 0);
  sendAction (body);
  Player::pause ();
}

void
PlayerRemote::resume ()
{
  string body = xstrbuild (REMOTE_PLAYER_JSON_ACT, "resume", 0);
  sendAction (body.c_str ());
  Player::resume ();
}

void
PlayerRemote::sendPresentationEvent (const string &action,
                                     const string &label)
{
  string body = xstrbuild (REMOTE_PLAYER_JSON_ACT, action.c_str (), 0);
  sendAction (body, label);
}

void
PlayerRemote::reload ()
{
}

void
PlayerRemote::redraw (cairo_t *)
{
}

GINGA_NAMESPACE_END