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
  _sessionStarted = false;
  _url = nullptr;
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
      _url = g_strdup_printf ("http://%s/scene/nodes/%s", value.c_str (),
                              _media->getId ().c_str ());
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
ws_action_callback (SoupSession *session, SoupMessage *msg,
                    gpointer user_data)
{
  string url;
  auto player = (PlayerRemote *) user_data;
  url = player->getProperty ("remotePlayerBaseURL");
  if (!msg->status_code == SOUP_STATUS_OK)
    WARNING ("Failed to perform request %s", url.c_str ());
  TRACE_SOUP_REQ_MSG (msg);
}

void
PlayerRemote::sendAction (const string &action)
{
  guint status;
  SoupMessage *msg;
  char *body;

  g_assert_nonnull (_url);
  if (!_sessionStarted)
    {
      _session
          = soup_session_new_with_options (SOUP_SESSION_ADD_FEATURE_BY_TYPE,
                                           SOUP_TYPE_CONTENT_SNIFFER, NULL);
      _sessionStarted = true;
    }
  msg = soup_message_new ("POST", _url);
  body = g_strdup_printf (REMOTE_PLAYER_JSON_ACT, action.c_str (), 0);
  soup_message_set_request (msg, "application/json", SOUP_MEMORY_COPY, body,
                            strlen (body));
  soup_session_queue_message (_session, msg, ws_action_callback, this);

  g_free (msg);
  g_free (body);
}

void
PlayerRemote::start ()
{
  sendAction ("start");
}

void
PlayerRemote::startPreparation ()
{
  sendAction ("prepare");
}

void
PlayerRemote::stop ()
{
  sendAction ("stop");
}

void
PlayerRemote::pause ()
{
  sendAction ("pause");
}

void
PlayerRemote::resume ()
{
  sendAction ("resume");
}

GINGA_NAMESPACE_END