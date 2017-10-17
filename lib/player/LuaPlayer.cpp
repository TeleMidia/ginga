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

#include "aux-ginga.h"
#include "aux-gl.h"
#include "LuaPlayer.h"

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


// Public.

LuaPlayer::LuaPlayer (GingaInternal *ginga, const string &id,
                      const string &uri)
  : Player (ginga, id, uri)
{
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

  g_assert (_state != OCCURRING);
  g_assert_null (_nw);
  TRACE ("starting");

  this->pwdSave (_uri);
  _init_rect = _prop.rect;
  _nw = ncluaw_open
    (_uri.c_str (), _init_rect.width, _init_rect.height, &errmsg);
  if (unlikely (_nw == nullptr))
    ERROR ("%s", errmsg);
  this->pwdRestore ();

  evt_ncl_send_presentation (_nw, "start", "");
  Player::start ();
}

void
LuaPlayer::stop (void)
{
  g_assert (_state != SLEEPING);
  g_assert_nonnull (_nw);
  TRACE ("stopping");

  evt_ncl_send_presentation (_nw, "stop", "");

  this->pwdSave ();
  ncluaw_cycle (_nw);
  this->pwdRestore ();

  ncluaw_close (_nw);
  _nw = nullptr;

  if (_opengl && _gltexture != 0)
    GL::delete_texture (&_gltexture);

  Player::stop ();
}

void G_GNUC_NORETURN
LuaPlayer::pause (void)
{
  g_assert (_state != PAUSED && _state != SLEEPING);
  TRACE ("pausing");
  ERROR_NOT_IMPLEMENTED ("pause action is not supported");
}

void G_GNUC_NORETURN
LuaPlayer::resume (void)
{
  g_assert (_state != PAUSED && _state != SLEEPING);
  TRACE ("resuming");
  ERROR_NOT_IMPLEMENTED ("resume action is not supported");
}

void
LuaPlayer::sendKeyEvent (string const &key, bool press)
{
  g_assert_nonnull (_nw);
  evt_key_send (_nw, press ? "press" : "release", key.c_str ());
}

void
LuaPlayer::redraw (cairo_t *cr)
{
  cairo_surface_t *sfc;

  g_assert (_state != SLEEPING);
  g_assert_nonnull (_nw);

  this->pwdSave ();
  ncluaw_cycle (_nw);
  this->pwdRestore ();

  sfc = (cairo_surface_t *) ncluaw_debug_get_surface (_nw);
  g_assert_nonnull (sfc);

  if (_opengl)
    {
      if (_gltexture == 0)
        GL::create_texture (&_gltexture,
                            cairo_image_surface_get_width (sfc),
                            cairo_image_surface_get_height (sfc),
                            cairo_image_surface_get_data (sfc));
      else
        GL::update_subtexture (_gltexture, 0, 0,
                               cairo_image_surface_get_width (sfc),
                               cairo_image_surface_get_height (sfc),
                               cairo_image_surface_get_data (sfc));
    }
  else
    {
      _surface = sfc;
    }

  Player::redraw (cr);
  if (!_opengl)
    _surface = nullptr;
}


// Protected.

bool
LuaPlayer::doSetProperty (PlayerProperty code, const string &name,
                          const string &value)
{
  if (_nw != nullptr && _state == OCCURRING)
    {
      const char *k = name.c_str ();
      const char *v = value.c_str ();
      evt_ncl_send_attribution (_nw, "start", k, v);
      evt_ncl_send_attribution (_nw, "stop", k, v);
    }
  return Player::doSetProperty (code, name, value);
}


// Private.

static void
do_chdir (string dir)
{
  if (g_chdir (dir.c_str ()) < 0)
    ERROR ("cannot chdir to '%s': %s", dir.c_str (), g_strerror (errno));
}

void
LuaPlayer::pwdSave (const string &path)
{
  gchar *cwd;
  gchar *dir;

  cwd = g_get_current_dir ();
  g_assert_nonnull (cwd);

  dir = g_path_get_dirname (path.c_str ());
  g_assert_nonnull (dir);

  _saved_pwd = string (cwd);
  _pwd = string (dir);
  do_chdir (_pwd);

  g_free (cwd);
  g_free (dir);
}

void
LuaPlayer::pwdSave ()
{
  do_chdir (_pwd);
}

void
LuaPlayer::pwdRestore ()
{
  do_chdir (_saved_pwd);
}

GINGA_PLAYER_END
