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

LuaPlayer::LuaPlayer (const string &mrl) : Player (mrl)
{
  gchar *dir;

  dir = g_path_get_dirname (mrl.c_str ());
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
LuaPlayer::pause (void)
{
  TRACE ("pause");
  evt_ncl_send_presentation (_nw, "pause", "");
  Player::pause ();
}

void
LuaPlayer::start (void)
{
  char *errmsg;

  TRACE ("play");
  if (_nw != NULL)
    {
      Player::start ();
      return;
    }

  _init_rect = _rect;
  _nw = ncluaw_open (_uri.c_str (), _init_rect.w, _init_rect.h, &errmsg);
  if (unlikely (_nw == NULL))
    ERROR ("cannot load NCLua file %s: %s", _uri.c_str (), errmsg);

  evt_ncl_send_presentation (_nw, "start", "");
  g_assert (Ginga_Display->registerEventListener (this));

  TRACE ("waiting for first cycle");

  Player::start ();
}

void
LuaPlayer::resume (void)
{
  TRACE ("resume");
  evt_ncl_send_presentation (_nw, "resume", "");
  Player::resume ();
}

void
LuaPlayer::stop (void)
{
  TRACE ("stop");
  evt_ncl_send_presentation (_nw, "stop", "");
  ncluaw_cycle (_nw);
  ncluaw_close (_nw);
  g_assert (Ginga_Display->unregisterEventListener (this));
  _nw = NULL;
  Player::stop ();
}

void
LuaPlayer::setProperty (const string &name, const string &value)
{
  if (_nw != NULL && _state == PL_OCCURRING)
    {
      const char *k = name.c_str ();
      const char *v = value.c_str ();
      evt_ncl_send_attribution (_nw, "start", k, v);
      evt_ncl_send_attribution (_nw, "stop", k, v);
    }
  Player::setProperty (name, value);
}

void
LuaPlayer::handleKeyEvent (SDL_EventType type, SDL_Keycode key)
{
  string typestr;
  string keystr;

  if (_nw == NULL)
    return;

  if (type == SDL_KEYDOWN)
    {
      typestr = "press";
    }
  else if (type == SDL_KEYUP)
    {
      typestr = "release";
    }
  else
    {
      return;
    }

  if (!ginga_key_table_index (key, &keystr))
    return;

  evt_key_send (_nw, typestr.c_str (), keystr.c_str ());
}

void
LuaPlayer::redraw (SDL_Renderer *renderer)
{
  SDL_Surface *sfc;

  if (_nw == NULL)
    {
      TRACE ("last cycle");
      return;                   // nothing to do
    }

  ncluaw_cycle (_nw);

  SDLx_CreateSurfaceARGB32 (_init_rect.w, _init_rect.h, &sfc);
  SDLx_LockSurface (sfc);
  ncluaw_paint (_nw, (guchar *) sfc->pixels, "ARGB32",
                sfc->w, sfc->h, sfc->pitch);
  SDLx_UnlockSurface (sfc);

  if (_texture == nullptr) // first call
    {
      _texture = SDL_CreateTextureFromSurface (renderer, sfc);
      g_assert_nonnull (_texture);
    }

  SDLx_LockSurface (sfc);
  SDL_UpdateTexture (_texture, NULL, sfc->pixels, sfc->pitch);
  SDLx_UnlockSurface (sfc);

  Player::redraw (renderer);
}

GINGA_PLAYER_END
