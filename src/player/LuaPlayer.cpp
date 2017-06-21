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

GINGA_PRAGMA_DIAG_IGNORE (-Wunused-macros)

GINGA_PLAYER_BEGIN

// Event handling.

typedef struct
{
  const char *key;
  void *value;
} evt_map_t;

static G_GNUC_UNUSED const evt_map_t evt_map_ncl_type[] = {
  // KEEP THIS SORTED ALPHABETICALLY
  { "attribution", (void *) Player::PL_TYPE_ATTRIBUTION },
  { "presentation", (void *) Player::PL_TYPE_PRESENTATION },
  { "selection", (void *) Player::PL_TYPE_SELECTION },
};

static G_GNUC_UNUSED const evt_map_t evt_map_ncl_action[] = {
  // KEEP THIS SORTED ALPHABETICALLY
  { "abort", (void *) Player::PL_NOTIFY_ABORT },
  { "pause", (void *) Player::PL_NOTIFY_PAUSE },
  { "resume", (void *) Player::PL_NOTIFY_RESUME },
  { "start", (void *) Player::PL_NOTIFY_START },
  { "stop", (void *) Player::PL_NOTIFY_STOP },
};

static int
evt_map_compare (const void *e1, const void *e2)
{
     return g_strcmp0 (deconst (evt_map_t *, e1)->key,
                       deconst (evt_map_t *, e2)->key);
}

static G_GNUC_UNUSED const evt_map_t *
_evt_map_get (const evt_map_t map[], size_t size, const char *key)
{
  evt_map_t e = { key, NULL };
  return (const evt_map_t *)bsearch (&e, map, size, sizeof (evt_map_t),
                                     evt_map_compare);
}

#define evt_map_get(map, key)\
  ((ptrdiff_t)((_evt_map_get (map, nelementsof (map), key))->value))

#define evt_ncl_get_type(type) evt_map_get (evt_map_ncl_type, type)

#define evt_ncl_get_action(act) evt_map_get (evt_map_ncl_action, act)

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
  _isKeyHandler = false;
  _scope = "";
}

LuaPlayer::~LuaPlayer (void)
{
  this->stop ();
}

void
LuaPlayer::abort (void)
{
  TRACE ("abort scope %s", this->_scope.c_str ());
  evt_ncl_send_presentation (this->_nw, "abort", this->_scope.c_str ());
  this->stop ();
}

void
LuaPlayer::pause (void)
{
  TRACE ("pause scope %s", this->_scope.c_str ());
  evt_ncl_send_presentation (this->_nw, "pause", this->_scope.c_str ());
  Player::pause ();
}

bool
LuaPlayer::play (void)
{
  char *errmsg;

  TRACE ("play scope %s", this->_scope.c_str ());
  if (this->_nw != NULL)
    {
      Player::play ();
      return true;
    }

  _init_rect = _rect;
  _nw = ncluaw_open (this->mrl.c_str (), _init_rect.w, _init_rect.h, &errmsg);
  if (unlikely (this->_nw == NULL))
    ERROR ("cannot load NCLua file %s: %s", this->mrl.c_str (), errmsg);

  evt_ncl_send_presentation (this->_nw, "start", this->_scope.c_str ());
  g_assert (Ginga_Display->registerEventListener (this));

  TRACE ("waiting for first cycle");

  Player::play ();
  return true;
}

void
LuaPlayer::resume (void)
{
  TRACE ("resume scope %s", this->_scope.c_str ());
  evt_ncl_send_presentation (this->_nw, "resume", this->_scope.c_str ());
  Player::resume ();
}

void
LuaPlayer::stop (void)
{
  TRACE ("stop scope %s", this->_scope.c_str ());

  if (this->_nw == NULL)
    goto done;

  evt_ncl_send_presentation (this->_nw, "stop", this->_scope.c_str ());
  ncluaw_cycle (this->_nw);
  ncluaw_close (this->_nw);
  g_assert (Ginga_Display->unregisterEventListener (this));
  this->_nw = NULL;
  this->forcedNaturalEnd = true;
 done:
  Player::stop ();
}

void
LuaPlayer::setCurrentScope (const string &name)
{
  this->_scope = name;
}

bool
LuaPlayer::setKeyHandler (bool b)
{
  this->_isKeyHandler = b;
  return b;
}

void
LuaPlayer::setProperty (const string &name, const string &value)
{
  if (this->_nw != NULL && this->status == PL_OCCURRING)
    {
      const char *k = name.c_str ();
      const char *v = value.c_str ();
      evt_ncl_send_attribution (this->_nw, "start", k, v);
      evt_ncl_send_attribution (this->_nw, "stop", k, v);
    }
  Player::setProperty (name, value);
}

void
LuaPlayer::handleKeyEvent (SDL_EventType type, SDL_Keycode key)
{
  string typestr;
  string keystr;

  if (this->_nw == NULL)
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

  evt_key_send (this->_nw, typestr.c_str (), keystr.c_str ());
}

void
LuaPlayer::redraw (SDL_Renderer *renderer)
{
  SDL_Surface *sfc;

  if (this->_nw == NULL)
    {
      TRACE ("last cycle");
      return;                   // nothing to do
    }

  ncluaw_cycle (this->_nw);

  SDLx_CreateSurfaceARGB32 (_init_rect.w, _init_rect.h, &sfc);
  SDLx_LockSurface (sfc);
  ncluaw_paint (_nw, (guchar *) sfc->pixels, "ARGB32",
                sfc->w, sfc->h, sfc->pitch);
  SDLx_UnlockSurface (sfc);

  if (this->texture == NULL)    // first call
    {
      this->texture = SDL_CreateTextureFromSurface (renderer, sfc);
      g_assert_nonnull (this->texture);
    }

  SDLx_LockSurface (sfc);
  SDL_UpdateTexture (this->texture, NULL, sfc->pixels, sfc->pitch);
  SDLx_UnlockSurface (sfc);

  Player::redraw (renderer);
}

GINGA_PLAYER_END
