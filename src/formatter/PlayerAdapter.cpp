/* Copyright (C) 2006-2017 PUC-Rio/Laboratorio TeleMidia

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
along with Ginga.  If not, see <http://www.gnu.org/licenses/>.  */

#include "ginga.h"
#include "PlayerAdapter.h"

#include "Scheduler.h"
#include "NclLinkTransitionTriggerCondition.h"

#include "mb/Display.h"
using namespace ::ginga::mb;

#include "player/Player.h"
#include "player/ImagePlayer.h"
#include "player/LuaPlayer.h"
#include "player/VideoPlayer.h"
#if defined WITH_LIBRSVG && WITH_LIBRSVG
# include "player/SvgPlayer.h"
#endif
#if defined WITH_PANGO && WITH_PANGO
# include "player/TextPlayer.h"
#endif
#if defined WITH_CEF && WITH_CEF
# include "player/HTMLPlayer.h"
#endif
using namespace ::ginga::player;

GINGA_FORMATTER_BEGIN


// Public.

PlayerAdapter::PlayerAdapter (string uri, string mimetype)
{
  const char *mime = mimetype.c_str ();
  _object = nullptr;
  _player = nullptr;

  if (g_str_has_prefix (mime, "audio")
      || g_str_has_prefix (mime, "video"))
    {
      _player = new VideoPlayer (uri);
    }
#if WITH_LIBRSVG && WITH_LIBRSVG
  else if (g_str_has_prefix (mime, "image/svg"))
    {
      _player = new SvgPlayer (uri);
    }
#endif
  else if (g_str_has_prefix (mime, "image"))
    {
      _player = new ImagePlayer (uri);
    }
#if defined WITH_CEF &&  WITH_CEF
  else if (g_str_has_prefix (mime, "text/html"))
    {
      _player = new HTMLPlayer (uri);
    }
#endif
#if defined WITH_PANGO && WITH_PANGO
  else if (streq (mime, "text/plain"))
    {
      _player = new TextPlayer (uri);
    }
#endif
  else if (streq (mime, "application/x-ginga-NCLua"))
    {
      _player = new LuaPlayer (uri);
    }
  else
    {
      _player = new Player (uri);
      WARNING ("unknown mime-type '%s': creating empty player", mime);
    }
  g_assert_nonnull (_player);
}

PlayerAdapter::~PlayerAdapter ()
{
  if (_player)
    {
      _player->stop ();
      delete _player;
    }
}

Player *
PlayerAdapter::getPlayer ()
{
  return _player;
}

bool
PlayerAdapter::start ()
{
  g_assert (_player->play ());
  TRACE ("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
  g_assert (Ginga_Display->registerEventListener (this));
  return true;
}

bool
PlayerAdapter::stop ()
{
  if (_player == nullptr)
    return true;
  _player->stop ();
  delete _player;
  _player = nullptr;
  g_assert (Ginga_Display->unregisterEventListener (this));
  return true;
}

bool
PlayerAdapter::pause ()
{
  g_assert_nonnull (_player);
  _player->pause ();
  return true;
}

bool
PlayerAdapter::resume ()
{
  g_assert_nonnull (_player);
  _player->resume ();
  return true;
}

bool
PlayerAdapter::abort ()
{
  return stop ();
}

void
PlayerAdapter::setProperty (AttributionEvent *event,
                            const string &value)
{
  string name;
  g_assert_nonnull (_player);
  g_assert_nonnull (event);
  name = event->getAnchor ()->getName ();
  _player->setProperty (name, value);
}

void
PlayerAdapter::setProperty (const string &name,
                            const string &value)
{
  g_assert_nonnull (_player);
  _player->setProperty (name, value);
}


// TODO: Remove this.
string
PlayerAdapter::getProperty (AttributionEvent *event)
{
  PropertyAnchor *anchor;
  string name;
  string value;

  g_assert_nonnull (_player);
  g_assert_nonnull (event);

  anchor = event->getAnchor ();
  g_assert_nonnull (anchor);

  name = anchor->getName ();
  value = _player->getProperty (name);

  TRACE ("getting property with name='%s', value='%s'",
         name.c_str (), value.c_str ());

  return value;
}

void
PlayerAdapter::setRect (SDL_Rect rect)
{
  _player->setRect (rect);
}

SDL_Rect
PlayerAdapter::getRect ()
{
  return _player->getRect ();
}

void
PlayerAdapter::setZ (int z, int zorder)
{
  _player->setZ (z, zorder);
}

void
PlayerAdapter::getZ (int *z, int *zorder)
{
  _player->getZ (z, zorder);
}

void
PlayerAdapter::handleTickEvent (arg_unused (GingaTime total),
                                GingaTime diff,
                                arg_unused (int frame))
{
  EventTransition *next;
  NclEvent *evt;
  GingaTime waited;
  GingaTime now;

  if (unlikely (_object == nullptr || _player == nullptr))
    return;

  if (_player->getMediaStatus() != Player::PL_OCCURRING)
    return;

  // Update player time.
  _player->incMediaTime (diff);

  next = _object->getNextTransition ();
  if (next == nullptr)
    return;

  waited = next->getTime ();
  now = _player->getMediaTime ();

  // TRACE ("now=%" GINGA_TIME_FORMAT " waited=%" GINGA_TIME_FORMAT,
  //        GINGA_TIME_ARGS (now), GINGA_TIME_ARGS (waited));

  if (now < waited)
    return;

  evt = dynamic_cast <NclEvent *> (next->getEvent ());
  g_assert_nonnull (evt);

  TRACE ("anchor '%s' timed out at %" GINGA_TIME_FORMAT
         ", updating transition table",
         evt->getId ().c_str(), GINGA_TIME_ARGS (now));

  _object->updateTransitionTable (now, _player);
};

void
PlayerAdapter::handleKeyEvent (SDL_EventType evtType,
                               SDL_Keycode key)
{
  if (unlikely (_object == nullptr || _player == nullptr))
    return;

  if (evtType == SDL_KEYDOWN)
    return;

  GingaTime time = _player->getMediaTime ();
  _object->selectionEvent (key, time);
}

GINGA_FORMATTER_END
