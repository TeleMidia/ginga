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
  _url = nullptr;
}

/**
 * @brief Evaluates if Media uses PlayerRemote
 */
bool
PlayerRemote::usesPlayerRemote (Media *media)
{
  string mime = media->getProperty ("type");
  if (mime == REMOTE_PLAYER_MIME_NCL360)
    return true;
  string device = media->getProperty ("device");
  if (!device.empty ())
    return true;
  return false;
}

bool
PlayerRemote::doSetProperty (Property code, const string &name,
                             const string &value)
{
  switch (code)
    {
    case PROP_REMOTE_PLAYER_BASE_URL:
      if (_url != nullptr)
        g_free (_url);
      _url = g_strdup_printf ("%s" REMOTE_PLAYER_ROUTE_NODES "%s",
                              value.c_str (), _media->getId ().c_str ());
      break;
    default:
      return Player::doSetProperty (code, name, value);
    }
  return true;
}

PlayerRemote::~PlayerRemote ()
{
  g_free (_url);
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
PlayerRemote::sendAction (const char *body)
{
  guint status;
  SoupMessage *msg;

  g_assert_nonnull (_url);
  if (!_session)
    _session = soup_session_new ();
  msg = soup_message_new (SOUP_METHOD_POST, _url);
  soup_message_set_request (msg, "application/json", SOUP_MEMORY_COPY, body,
                            strlen (body));
  soup_session_queue_message (_session, msg, cb_action, this);
}

void
PlayerRemote::start ()
{
  string zOrder = getProperty ("zOrder");
  string props = xstrbuild (
      REMOTE_PLAYER_JSON_ACT_WITH_PROPS, "start", 0,
      getProperty ("top").c_str (), getProperty ("left").c_str (),
      getProperty ("width").c_str (), getProperty ("height").c_str (),
      getProperty ("zOrder").c_str (), "0%");
  sendAction (props.c_str ());
  Player::start ();
}

void
PlayerRemote::startPreparation ()
{
  string body = xstrbuild (REMOTE_PLAYER_JSON_ACT, "prepare", 0);
  sendAction (body.c_str ());
  Player::startPreparation ();
}

void
PlayerRemote::stop ()
{
  string body = xstrbuild (REMOTE_PLAYER_JSON_ACT, "stop", 0);
  sendAction (body.c_str ());
  Player::stop ();
}

void
PlayerRemote::pause ()
{
  string body = xstrbuild (REMOTE_PLAYER_JSON_ACT, "pause", 0);
  sendAction (body.c_str ());
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
PlayerRemote::reload ()
{
}

void
PlayerRemote::redraw (cairo_t *)
{
}

GINGA_NAMESPACE_END