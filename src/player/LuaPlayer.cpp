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

#include "mb/InputManager.h"
#include "mb/Display.h"
#include "util/functions.h"
using namespace ::ginga::util;

#include "LuaPlayer.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wconversion)
GINGA_PRAGMA_DIAG_IGNORE (-Wunused-macros)

GINGA_PLAYER_BEGIN

#define debug(...)  g_debug ("nclua: " __VA_ARGS__)
#define nelementsof G_N_ELEMENTS

#define MUTEX_INIT(m)   g_rec_mutex_init (m)
#define MUTEX_FINI(m)   g_rec_mutex_clear (m)
#define MUTEX_LOCK(m)   g_rec_mutex_lock (m)
#define MUTEX_UNLOCK(m) g_rec_mutex_unlock (m)

// Event handling.

typedef struct
{
  const char *key;
  void *value;
} evt_map_t;

static const evt_map_t evt_map_ncl_type[] = {
  // KEEP THIS SORTED ALPHABETICALLY
  { "attribution", (void *)Player::TYPE_ATTRIBUTION },
  { "presentation", (void *)Player::TYPE_PRESENTATION },
  { "selection", (void *)Player::TYPE_SELECTION },
};

static const evt_map_t evt_map_ncl_action[] = {
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

static const evt_map_t *
_evt_map_get (const evt_map_t map[], size_t size, const char *key)
{
  evt_map_t e = { key, NULL };
  return (const evt_map_t *)bsearch (&e, map, size, sizeof (evt_map_t),
                                     evt_map_compare);
}

#define evt_map_get(map, key)                                           \
  ((ptrdiff_t)((_evt_map_get (map, nelementsof (map), key))->value))

#define evt_ncl_get_type(type) evt_map_get (evt_map_ncl_type, type)

#define evt_ncl_get_action(act) evt_map_get (evt_map_ncl_action, act)

#define evt_ncl_send_attribution(nw, action, name, value)\
  ncluaw_send_ncl_event (nw, "attribution", action, name, value)

#define evt_ncl_send_presentation(nw, action, name)                        \
  ncluaw_send_ncl_event (nw, "presentation", action, name, NULL)

#define evt_ncl_send_selection(nw, action, name)                           \
  ncluaw_send_ncl_event (nw, "selection", action, name, NULL)

#define evt_key_send ncluaw_send_key_event
#define evt_pointer_send ncluaw_send_pointer_event

// Update thread.

// List of active players.
list<LuaPlayer *> *LuaPlayer::nw_update_list = NULL;

// Synchronize access to nw_update_list.
GRecMutex LuaPlayer::nw_update_mutex;

// Id of the update thread.
GThread *LuaPlayer::nw_update_thread;

// Time delay (in milliseconds) between updates.
#define NW_UPDATE_DELAY 10

// Traverses the list of active players cycling their NCLua states.  This
// thread is created when the first NCLua state is allocated, and it is
// destroyed whenever the last NCLua state is destroyed.

void *
LuaPlayer::nw_update_thread_fn (arg_unused (void *data))
{
  debug ("starting update thread");
  while (true)
    {
      g_usleep ((NW_UPDATE_DELAY)*1000);

      MUTEX_LOCK (&nw_update_mutex);

      if (nw_update_list == NULL) // end of cycle process
        {
          MUTEX_UNLOCK (&nw_update_mutex);
          debug ("exiting update thread");
          return NULL;
        }

      list<LuaPlayer *>::iterator i;
      list<LuaPlayer *> lst = *nw_update_list;

      for (i = lst.begin (); i != lst.end (); i++)
        {
          LuaPlayer *player;
          SDLSurface *wrapper;
          ncluaw_t *nw;
          ncluaw_event_t *evt;

          player = *i;
          player->lock ();

          nw = player->nw;
          ncluaw_cycle (nw);

          wrapper = player->getSurface ();
          if (wrapper != NULL)
            {
              SDLWindow *window;
              SDL_Texture *texture;
//              guchar *pixels;
#if 0
              int pitch;
#endif
              int width;
              int height;

              window = wrapper->getParentWindow ();
              g_assert_nonnull (window);

              texture = window->getTexture ();
              g_assert_nonnull (texture);
              SDLx_QueryTexture (texture, NULL, NULL, &width, &height);

#if 0
              Ginga_Display->lockRenderer ();
              SDLx_LockTexture (texture, NULL, (void **) &pixels, &pitch);
              ncluaw_paint (nw, pixels, "ARGB32", width, height, pitch);
              SDLx_UnlockTexture (texture);
              Ginga_Display->unlockRenderer ();
#endif
            }

          while ((evt = ncluaw_receive (nw)) != NULL)
            {
              const char *type;
              const char *action;
              const char *name;
              const char *value;

              int type_value;
              int action_value;

              // Currently, we're only handling NCL events.
              if (evt->cls != NCLUAW_EVENT_NCL)
                {
                  goto done;
                }

              type = evt->u.ncl.type;
              action = evt->u.ncl.action;
              name = evt->u.ncl.name;
              value = evt->u.ncl.value;

              g_assert_nonnull (type);
              g_assert_nonnull (action);
              g_assert_nonnull (name);

              type_value = evt_ncl_get_type (type);
              action_value = evt_ncl_get_action (action);

              switch (type_value)
                {
                case Player::TYPE_ATTRIBUTION:
                  player->notifyPlayerListeners (action_value,
                                                 string (name), type_value,
                                                 string (value));
                  break;

                case Player::TYPE_PRESENTATION:
                  if (streq (name, "")
                      && action_value == Player::PL_NOTIFY_STOP)
                    {
                      // FIXME (The "Stop" Mess - Part I): If the
                      // NCLua script posted a "stop" event, we have
                      // to destroy the NCLua state immediately.
                      // Otherwise, notifyPlayerListeners() will
                      // cause the event to be sent back to the
                      // NCLua engine.
                      LuaPlayer::nw_update_remove (player);
                      player->doStop ();
                    }
                  player->notifyPlayerListeners (action_value,
                                                 string (name));
                  break;

                case Player::TYPE_SELECTION:
                  break;

                default:
                  g_assert_not_reached ();
                }

            done:
              ncluaw_event_free (evt);

              // Check if NCLua state was destroyed by doStop().
              if (player->nw == NULL)
                {
                  break;
                }
            }

          player->unlock ();
        }

      MUTEX_UNLOCK (&nw_update_mutex);
    }

  return NULL;
}

// Inserts player PLAYER into update list.
// If this is the first player, creates both the list and the update thread.

void
LuaPlayer::nw_update_insert (LuaPlayer *player)
{
  if (nw_update_list == NULL)
    {
      MUTEX_INIT (&nw_update_mutex);
      MUTEX_LOCK (&nw_update_mutex);
      nw_update_list = new list<LuaPlayer *> ();
      nw_update_thread = g_thread_new ("nclua", nw_update_thread_fn, NULL);
      g_assert_nonnull (nw_update_thread);
    }
  else
    {
      MUTEX_LOCK (&nw_update_mutex);
    }

  nw_update_list->push_back (player);
  MUTEX_UNLOCK (&nw_update_mutex);
}

// Removes player PLAYER from update list.
// If this is the last player, destroys both the list and the update thread.

void
LuaPlayer::nw_update_remove (LuaPlayer *player)
{
  MUTEX_LOCK (&nw_update_mutex);

  g_assert_nonnull (nw_update_list);
  nw_update_list->remove (player);

  // FIXME (The "Stop" Mess - Part II): If a "stop" was posted by the
  // NCLua script, this function will be called in the update thread.  At
  // this point, the player mutex is locked and the NCLua state is about
  // to be destroyed.  To avoid corruption, we postpone the destruction
  // of nw_update_list until the player's destructor is called.

  if (nw_update_list->empty () && g_thread_self () != nw_update_thread)
    {
      delete nw_update_list;
      nw_update_list = NULL;    // signal end of cycle process
      MUTEX_UNLOCK (&nw_update_mutex);
      g_assert_null (g_thread_join (nw_update_thread));
      MUTEX_FINI (&nw_update_mutex);
    }
  else
    {
      MUTEX_UNLOCK (&nw_update_mutex);
    }
}

// Private (or almost private) methods -- these should not call lock/unlock.

// Locks player.

void
LuaPlayer::lock (void)
{
  MUTEX_LOCK (&this->mutex);
}

// Unlocks player.

void
LuaPlayer::unlock (void)
{
  MUTEX_UNLOCK (&this->mutex);
}

// Creates and loads the associated NCLua engine.
// Returns true if successful, otherwise returns false.

bool
LuaPlayer::doPlay (void)
{
  SDLSurface *surface;
  SDLWindow *window;
  SDL_Renderer *renderer;
  SDL_Texture *texture;
  char *errmsg = NULL;
  int width = 0;
  int height = 0;

  g_assert_null (this->nw);

  surface = this->getSurface ();
  g_assert_nonnull (surface);
  surface->getSize (&width, &height);

  // Create the NCLua state.
  this->nw = ncluaw_open (this->mrl.c_str (), width, height, &errmsg);
  if (unlikely (this->nw == NULL))
    g_error ("cannot load NCLua file %s: %s", this->mrl.c_str (),
             errmsg);

  renderer = NULL;
#if 0
  renderer = Ginga_Display->getLockedRenderer ();
#endif
  texture = SDL_CreateTexture (renderer, SDL_PIXELFORMAT_ARGB8888,
                               SDL_TEXTUREACCESS_STREAMING, width, height);
  g_assert_nonnull (texture);
#if 0
  Ginga_Display->unlockRenderer ();
#endif

  window = surface->getParentWindow ();
  g_assert_nonnull (window);
  window->setTexture (texture);

  this->im->addApplicationInputEventListener (this);

  return true;
}

// Destroys the associated NCLua engine and stops the player.

void
LuaPlayer::doStop (void)
{
  if (this->nw != NULL)
    {
      ncluaw_close (this->nw);
    }
  this->nw = NULL;
  this->im->removeApplicationInputEventListener (this);
  this->forcedNaturalEnd = true;
  this->hasExecuted = true;
}

// Constructor and destructor.

LuaPlayer::LuaPlayer (const string &mrl) : Player (mrl)
{
#ifdef _MSC_VER
  static int putenv = 0;

  if (!putenv)
    {
      string env;
      string dir;

      dir = SystemCompat::getGingaBinPath ();
      env = dir + "\\.lua;" + dir + "\\?\\init.lua;;";
      _putenv_s ("LUA_PATH", env.c_str ());

      env = dir + "\\?.dll;;";
      _putenv_s ("LUA_CPATH", env.c_str ());
      putenv = 1;
    }
#endif
  debug ("mrl='%s'", mrl.c_str ());

  // FIXME: This is *WRONG*: the chdir() call changes the working
  // directory of the whole process.

  string cwd = SystemCompat::getPath (mrl);
  if (g_chdir (cwd.c_str ()) < 0)
    g_warning ("%s", g_strerror (errno));

  this->im = Ginga_Display->getInputManager ();
  this->nw = NULL; // created by start()
  MUTEX_INIT (&this->mutex);
  this->hasExecuted = false;
  this->isKeyHandler = false;
  this->scope = "";
}

LuaPlayer::~LuaPlayer (void)
{
  this->lock ();

  if (nw_update_list != NULL && nw_update_list->empty ())
    {
      // FIXME (The "Stop" Mess - Part III): If this->nw is NULL but
      // nw_update_list is non-NULL, then the '"Stop" Mess' is in
      // course.  Thus, we must call nw_update_remove to eventually
      // destroy the nw_update_list -- which, at this point, should be
      // empty.

      LuaPlayer::nw_update_remove (this);
    }

  this->doStop ();

  this->unlock ();
  MUTEX_FINI (&this->mutex);
}

// Inherited from Player.

void
LuaPlayer::abort (void)
{
  this->lock ();
  debug ("scope='%s'", this->scope.c_str ());

  evt_ncl_send_presentation (this->nw, "abort", this->scope.c_str ());
  this->stop ();

  this->unlock ();
}

void
LuaPlayer::pause (void)
{
  this->lock ();
  debug ("scope='%s'", this->scope.c_str ());

  evt_ncl_send_presentation (this->nw, "pause", this->scope.c_str ());
  Player::pause ();

  this->unlock ();
}

bool
LuaPlayer::play (void)
{
  bool status;

  this->lock ();
  debug ("scope='%s'", this->scope.c_str ());

  status = true;
  if (this->nw == NULL)
    {
      if (!this->doPlay ())
        {
          status = false;
          goto error;
        }

      nw_update_insert (this);
    }

  evt_ncl_send_presentation (this->nw, "start", this->scope.c_str ());
  Player::play ();

error:
  this->unlock ();
  return status;
}

void
LuaPlayer::resume (void)
{
  this->lock ();
  debug ("scope='%s'", this->scope.c_str ());

  evt_ncl_send_presentation (this->nw, "resume", this->scope.c_str ());
  Player::resume ();

  this->unlock ();
}

void
LuaPlayer::stop (void)
{
  this->lock ();
  debug ("scope='%s'", this->scope.c_str ());

  if (this->nw != NULL)
    {
      nw_update_remove (this);
      evt_ncl_send_presentation (this->nw, "stop", this->scope.c_str ());
      ncluaw_cycle (this->nw);
      this->doStop ();
    }

  Player::stop ();

  this->unlock ();
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
  debug ("isKeyHandler=%s", b ? "true" : "false");

  this->isKeyHandler = b;

  this->unlock ();
  return b;
}

void
LuaPlayer::setPropertyValue (const string &name, const string &value)
{
  this->lock ();
  debug ("name='%s', value='%s'", name.c_str (), value.c_str ());

  // FIXME: Before calling play(), FormatterPlayerAdapter calls
  // setPropertyValue() to initialize the object's properties.  We
  // need to work around this bogus behavior, since it is the play()
  // call that creates the NCLua engine.

  if (this->nw != NULL && this->status == PLAY)
    {
      const char *k = name.c_str ();
      const char *v = value.c_str ();
      evt_ncl_send_attribution (this->nw, "start", k, v);
      evt_ncl_send_attribution (this->nw, "stop", k, v);
    }

  Player::setPropertyValue (name, value);

  this->unlock ();
}

// Inherited from IInputEventListener.

bool
LuaPlayer::userEventReceived (InputEvent *evt)
{
  this->lock ();

  if (this->nw == NULL)
    {
      goto tail;
    }

  if (evt->isKeyType () && this->isKeyHandler)
    {
      string key;
      int press;

      key = (Key::getName (evt->getKeyCode ()));
      press = evt->isPressedType ();
      debug ("key='%s', type='%s'", key.c_str (),
             press ? "press" : "release");

      evt_key_send (this->nw, press ? "press" : "release", key.c_str ());
    }

tail:
  this->unlock ();
  return true;
}

GINGA_PLAYER_END
