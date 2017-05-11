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
  { "attribution", (void *)Player::TYPE_ATTRIBUTION },
  { "presentation", (void *)Player::TYPE_PRESENTATION },
  { "selection", (void *)Player::TYPE_SELECTION },
};

static G_GNUC_UNUSED const evt_map_t evt_map_ncl_action[] = {
  // KEEP THIS SORTED ALPHABETICALLY
  { "abort", (void *)Player::PL_NOTIFY_ABORT },
  { "pause", (void *)Player::PL_NOTIFY_PAUSE },
  { "resume", (void *)Player::PL_NOTIFY_RESUME },
  { "start", (void *)Player::PL_NOTIFY_START },
  { "stop", (void *)Player::PL_NOTIFY_STOP },
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


bool
LuaPlayer::displayJobCallbackWrapper (DisplayJob *job,
                                      SDL_Renderer *renderer,
                                      void *self)
{
  return ((LuaPlayer *) self)->displayJobCallback (job, renderer);
}

bool
LuaPlayer::displayJobCallback (arg_unused (DisplayJob *job),
                               SDL_Renderer *renderer)
{
  SDL_Rect rect;
  SDL_Surface *sfc;

  bool signal = false;

  this->lock ();
  if (this->nw == NULL)
    {
      g_debug ("last cycle");
      this->unlock ();
      return false;             // remove job
    }

  rect = this->window->getRect ();
  g_assert (rect.w > 0 && rect.h > 0);

  ncluaw_cycle (this->nw);

#if SDL_VERSION_ATLEAST(2,0,5)
  sfc = SDL_CreateRGBSurfaceWithFormat (0, rect.w, rect.h, 32,
                                        SDL_PIXELFORMAT_ARGB8888);
#else
  sfc = SDL_CreateRGBSurface (0, rect.w, rect.h, 32,
                              0xff000000,
                              0x00ff0000,
                              0x0000ff00,
                              0x000000ff);
#endif

  g_assert_nonnull (sfc);
  SDLx_LockSurface (sfc);

  ncluaw_paint (nw, (guchar *) sfc->pixels, "ARGB32",

                sfc->w, sfc->h, sfc->pitch);
  SDLx_UnlockSurface (sfc);

  if (unlikely (this->texture == NULL)) // first call
    {
      this->texture = SDL_CreateTextureFromSurface (renderer, sfc);
      g_assert_nonnull (this->texture);
      signal = true;
    }

  SDLx_LockSurface (sfc);
  SDL_UpdateTexture (this->texture, NULL, sfc->pixels, sfc->pitch);
  SDLx_UnlockSurface (sfc);

  if (unlikely (signal))
    {
      g_debug ("first cycle");
      this->unlock ();
      this->condDisplayJobSignal ();
      return true;              // keep job
    }
  else
    {
      this->unlock ();
      return true;              // keep job
    }
}


// Public methods.

LuaPlayer::LuaPlayer (const string &mrl) : Player (mrl)
{
  gchar *dir;

  dir = g_path_get_dirname (mrl.c_str ());
  g_assert_nonnull (dir);

  if (g_chdir (dir) < 0)
    g_error ("%s", g_strerror (errno));
  g_free (dir);

  this->mutexInit ();
  this->condDisplayJobInit ();
  this->nw = NULL;              // created by start ()
  this->hasExecuted = false;
  this->isKeyHandler = false;
  this->scope = "";

  Ginga_Display->registerKeyEventListener(this);
}

LuaPlayer::~LuaPlayer (void)
{
  this->stop ();
  this->condDisplayJobClear ();
  this->mutexClear ();
}

void
LuaPlayer::abort (void)
{
  this->lock ();
  g_debug ("abort scope %s", this->scope.c_str ());
  evt_ncl_send_presentation (this->nw, "abort", this->scope.c_str ());
  this->unlock ();
  this->stop ();
}

void
LuaPlayer::pause (void)
{
  this->lock ();
  g_debug ("pause scope %s", this->scope.c_str ());
  evt_ncl_send_presentation (this->nw, "pause", this->scope.c_str ());
  this->unlock ();
  Player::pause ();
}

bool
LuaPlayer::play (void)
{
  char *errmsg;
  this->lock ();
  g_debug ("play scope %s", this->scope.c_str ());
  if (this->nw != NULL)
    {
      this->unlock ();
      Player::play ();
      return true;
    }

   SDL_Rect rect = this->window->getRect ();
   g_assert (rect.w > 0 && rect.h > 0);

  this->nw = ncluaw_open (this->mrl.c_str (), rect.w, rect.h, &errmsg);

  if (unlikely (this->nw == NULL))
    g_error ("cannot load NCLua file %s: %s", this->mrl.c_str (), errmsg);

//  this->im->addApplicationInputEventListener (this);
  evt_ncl_send_presentation (this->nw, "start", this->scope.c_str ());

  Ginga_Display->addJob (displayJobCallbackWrapper, this);

  g_debug ("waiting for first cycle");
  this->unlock ();
  this->condDisplayJobWait ();

  Player::play ();
  return true;
}

void
LuaPlayer::resume (void)
{
  this->lock ();
  g_debug ("resume scope %s", this->scope.c_str ());
  evt_ncl_send_presentation (this->nw, "resume", this->scope.c_str ());
  this->unlock ();
  Player::resume ();
}

void
LuaPlayer::stop (void)
{
  this->lock ();
  g_debug ("stop scope %s", this->scope.c_str ());

  if (this->nw == NULL)
    goto done;

  evt_ncl_send_presentation (this->nw, "stop", this->scope.c_str ());
  ncluaw_cycle (this->nw);
  ncluaw_close (this->nw);
  this->nw = NULL;
 // this->im->removeApplicationInputEventListener (this);
  this->forcedNaturalEnd = true;
  this->hasExecuted = true;
 done:
  this->unlock ();
  Player::stop ();
}

bool
LuaPlayer::hasPresented (void)
{
  bool hasExecuted;

  this->lock ();
  hasExecuted = this->hasExecuted;
  this->unlock ();

  return hasExecuted;
}

void
LuaPlayer::setCurrentScope (const string &name)
{
  this->lock ();
  this->scope = name;
  this->unlock ();
}

bool
LuaPlayer::setKeyHandler (bool b)
{
  this->lock ();
  this->isKeyHandler = b;
  this->unlock ();
  return b;
}

void
LuaPlayer::setPropertyValue (const string &name, const string &value)
{
  this->lock ();

  // FIXME: Before calling play(), FormatterPlayerAdapter calls
  // setPropertyValue() to initialize the object's properties.  We
  // need to work around this bogus behavior, since it is the play()
  // call that creates the NCLua engine.

  if (this->nw != NULL && this->status == OCCURRING)
    {
      const char *k = name.c_str ();
      const char *v = value.c_str ();
      evt_ncl_send_attribution (this->nw, "start", k, v);
      evt_ncl_send_attribution (this->nw, "stop", k, v);
    }
  Player::setPropertyValue (name, value);
  this->unlock ();
}

void
LuaPlayer::keyInputCallback (SDL_EventType evtType, SDL_Keycode key){

  this->lock ();

  if (this->nw == NULL)
    goto tail;

  if(evtType == SDL_KEYDOWN || evtType == SDL_KEYUP){
     string evt="release";
     if(evtType == SDL_KEYDOWN)
            evt="press";

     evt_key_send (this->nw, evt.c_str(), convertSdl2GingaKey(key).c_str() );
  }

  tail:
  this->unlock ();
}

GINGA_PLAYER_END
