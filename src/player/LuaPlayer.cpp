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

#include "ginga-internal.h"
#include "LuaPlayer.h"

#include "mb/Display.h"
using namespace ::ginga::mb;

GINGA_PRAGMA_DIAG_IGNORE (-Wunused-macros)

GINGA_PLAYER_BEGIN

// Event handling.
#define evt_ncl_send_attribution(nw, action, name, value)\
  ncluaw_send_ncl_event (nw, "attribution", action, name, value)

#define evt_ncl_send_presentation(nw, action, name)\
  ncluaw_send_ncl_event (nw, "presentation", action, name, NULL)

#define evt_ncl_send_selection(nw, action, name)\
  ncluaw_send_ncl_event (nw, "selection", action, name, NULL)

#define evt_key_send ncluaw_send_key_event
#define evt_pointer_send ncluaw_send_pointer_event


// Public methods.

LuaPlayer::LuaPlayer (const string &id, const string &uri)
  : Player (id, uri)
{
  gchar *dir;

  dir = g_path_get_dirname (uri.c_str ());
  g_assert_nonnull (dir);

  if (g_chdir (dir) < 0)
    ERROR ("cannot chdir to '%s': %s", dir, g_strerror (errno));
  g_free (dir);

  _nw = NULL;
  _init_rect = {0, 0, 0, 0};
}

LuaPlayer::~LuaPlayer (void)
{
}

void
LuaPlayer::start (void)
{
  char *errmsg;

  g_assert (_state != PL_OCCURRING);
  g_assert_null (_nw);
  TRACE ("starting");

  _init_rect = _rect;
  _nw = ncluaw_open
    (_uri.c_str (), _init_rect.width, _init_rect.height, &errmsg);
  if (unlikely (_nw == NULL))
    ERROR ("cannot load NCLua file %s: %s", _uri.c_str (), errmsg);

  evt_ncl_send_presentation (_nw, "start", "");
  g_assert (Ginga_Display->registerEventListener (this));

  Player::start ();
}

void
LuaPlayer::stop (void)
{
  g_assert (_state != PL_SLEEPING);
  g_assert_nonnull (_nw);

  evt_ncl_send_presentation (_nw, "stop", "");
  ncluaw_cycle (_nw);
  ncluaw_close (_nw);
  g_assert (Ginga_Display->unregisterEventListener (this));
  _nw = NULL;

  Player::stop ();
}

void G_GNUC_NORETURN
LuaPlayer::pause (void)
{
  g_assert (_state != PL_PAUSED && _state != PL_SLEEPING);
  ERROR_NOT_IMPLEMENTED ("pause action is not supported");
}

void G_GNUC_NORETURN
LuaPlayer::resume (void)
{
  g_assert (_state != PL_PAUSED && _state != PL_SLEEPING);
  ERROR_NOT_IMPLEMENTED ("resume action is not supported");
}

void
LuaPlayer::setProperty (const string &name, const string &value)
{
  if (_nw != nullptr && _state == PL_OCCURRING)
    {
      const char *k = name.c_str ();
      const char *v = value.c_str ();
      evt_ncl_send_attribution (_nw, "start", k, v);
      evt_ncl_send_attribution (_nw, "stop", k, v);
    }
  Player::setProperty (name, value);
}

void
LuaPlayer::handleKeyEvent (string const &key, bool press)
{
  g_assert_nonnull (_nw);
  evt_key_send (_nw, press ? "press" : "release", key.c_str ());
}

void
LuaPlayer::redraw (cairo_t *cr)
{
  g_assert (_state != PL_SLEEPING);
  g_assert_nonnull (_nw);

  ncluaw_cycle (_nw);

  _surface = (cairo_surface_t *) ncluaw_debug_get_surface (_nw);
  g_assert_nonnull (_surface);

  Player::redraw (cr);
  _surface = nullptr;
}

GINGA_PLAYER_END
