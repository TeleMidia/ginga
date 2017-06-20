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
#include "Display.h"
#include "Dashboard.h"

#include "SDLWindow.h"

GINGA_MB_BEGIN

// Global display; initialized by main().
Display *_Ginga_Display = NULL;

// Compares the z-index and z-order of two players.
static gint
win_cmp_z (Player *p1, Player *p2)
{
  int z1, zo1, z2, zo2;

  g_assert_nonnull (p1);
  g_assert_nonnull (p2);

  p1->getZ (&z1, &zo1);
  p2->getZ (&z2, &zo2);

  if (z1 < z2)
    return -1;
  if (z1 > z2)
    return 1;
  if (zo1 < zo2)
    return -1;
  if (zo1 > zo2)
    return 1;
  return 0;
}


// Public methods.

/**
 * @brief Creates a new display with the given parameters.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param fullscreen Full-screen mode.
 * @param fps Target FPS rate.
 */
Display::Display (int width, int height, double fps, bool fullscreen)
{
  guint flags;

  _width = width;
  _height = height;
  _fps = fps;
  _fullscreen = fullscreen;
  _quit = false;

  _listeners = NULL;
  _players = NULL;
  _textures = NULL;

  g_assert (!SDL_WasInit (0));
  if (unlikely (SDL_Init (0) != 0))
    CRITICAL ("cannot initialize SDL: %s", SDL_GetError ());

#if SDL_VERSION_ATLEAST (2,0,4)
  SDL_SetHint (SDL_HINT_NO_SIGNAL_HANDLERS, "1");
#endif
  SDL_SetHint (SDL_HINT_RENDER_SCALE_QUALITY, "1");

  flags = SDL_WINDOW_SHOWN;
  if (_fullscreen)
    flags |= SDL_WINDOW_FULLSCREEN;

  _dashboard = new Dashboard ();
  _screen = SDL_CreateWindow ("ginga", 0, 0, width, height, flags);
  g_assert_nonnull (_screen);
  _renderer = SDL_CreateRenderer (_screen, -1,
                                       SDL_RENDERER_PRESENTVSYNC);
  g_assert_nonnull (_renderer);
}

/**
 * @brief Destroys display.
 */
Display::~Display ()
{
  this->quit ();

  g_list_free (_listeners);
  g_list_free (_players);
  g_assert (g_list_length (_textures) == 0);

  SDL_DestroyRenderer (_renderer);
  SDL_DestroyWindow (_screen);
  delete _dashboard;
}

/**
 * @brief Gets the target FPS rate.
 * @return Target FPS rate.
 */
double
Display::getFPS ()
{
  return _fps;
}

/**
 * @brief Sets the target FPS rate.
 * @param fps Target FPS rate.
 */
void
Display::setFPS (double fps)
{
  _fps = MAX (fps, 0);
}

/**
 * @brief Gets full-screen mode.
 * @return True if display is in full-screen.
 */
bool
Display::getFullscreen ()
{
  return _fullscreen;
}

/**
 * @brief Sets full-screen mode.
 * @param fullscreen Full-screen mode.
 */
void
Display::setFullscreen (bool fullscreen)
{
  int status;
  guint flags;

  flags = (fullscreen) ? SDL_WINDOW_FULLSCREEN : 0;
  status = SDL_SetWindowFullscreen (_screen, flags);
  if (unlikely (status != 0))
    {
      WARNING ("cannot set full-screen mode to %s: %s",
               (fullscreen) ? "true" : "false", SDL_GetError ());
      return;
    }
  _fullscreen = fullscreen;
}

/**
 * @brief Gets display size.
 * @param width Variable to store width.
 * @param height Variable to store height.
 */
void
Display::getSize (int *width, int *height)
{
  set_if_nonnull (width, _width);
  set_if_nonnull (height, _height);
}

/**
 * @brief Sets display size.
 * @param width Width in pixels.
 * @param height Height in pixels.
 */
void
Display::setSize (int width, int height)
{
  g_assert (width > 0 && height > 0);
  SDL_SetWindowSize (_screen, width, height);
  _width = width;
  _height = height;
}

/**
 * @brief Quits render loop.
 */
void
Display::quit ()
{
  _quit = true;
}

/**
 * @brief Tests whether display has quitted.
 * @return True if display has quitted.
 */
bool
Display::hasQuitted ()
{
  return _quit;
}

/**
 * @brief Adds listener to display listener list.
 * @param obj Event listener.
 */
bool
Display::registerEventListener (IEventListener *obj)
{
  g_assert_nonnull (obj);
  return this->add (&_listeners, obj);
}

/**
 * @brief Removes listener from display listener list.
 */
bool
Display::unregisterEventListener (IEventListener *obj)
{
  g_assert_nonnull (obj);
  return this->remove (&_listeners, obj);
}

/**
 * @brief Adds player to display player list.
 * @param player Player.
 */
void
Display::registerPlayer (Player *player)
{
  g_assert_nonnull (player);
  g_assert (this->add (&_players, player));
}

/**
 * @brief Removes player from display listener list.
 */
void
Display::unregisterPlayer (Player *player)
{
  g_assert_nonnull (player);
  g_assert (this->remove (&_players, player));
}

/**
 * @brief Schedules the destruction of texture.
 * @param texture Texture.
 */
void
Display::destroyTexture (SDL_Texture *texture)
{
  g_assert_nonnull (texture);
  g_assert (this->add (&_textures, texture));
}

/**
 * @brief Enters render loop.
 */
void
Display::renderLoop ()
{
  GingaTime epoch = ginga_gettime ();
  GingaTime last = epoch;
  int frameno = 1;

  // Render loop.
  while (!this->hasQuitted())
    {
      SDL_Event evt;
      GList *l;

      GingaTime framedur = (GingaTime)(1 * GINGA_SECOND / _fps);
      GingaTime now = ginga_gettime ();
      GingaTime elapsed = now - last;

      if (_fps > 0 && elapsed < framedur)
        g_usleep ((framedur - elapsed) / 1000);

      now = ginga_gettime ();
      elapsed = now - last;
      last = now;
      this->notifyTickListeners (now - epoch, elapsed, frameno++);

      // Handle input.
      while (SDL_PollEvent (&evt))
        {
          SDL_EventType type = (SDL_EventType) evt.type;
          switch (type)
            {
            case SDL_KEYDOWN:
            case SDL_KEYUP:
              if (evt.key.keysym.sym == SDLK_ESCAPE)
                this->quit ();
              this->notifyKeyListeners (type, evt.key.keysym.sym);
              break;
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
              // TODO
              break;
            case SDL_QUIT:
              this->quit ();
              break;
            default:
              break;
            }
        }

      // Draw players.
      SDL_SetRenderDrawColor (_renderer, 255, 0, 255, 255);
      SDL_RenderClear (_renderer);

      _players = g_list_sort (_players, (GCompareFunc) win_cmp_z);
      l =  _players;
      while (l != NULL)         // can be modified while being traversed
        {
          GList *next = l->next;
          Player *pl = (Player *) l->data;
          if (pl == NULL)
            _players = g_list_remove_link (_players, l);
          else
            pl->redraw (_renderer);
          l = next;
        }

      // Draw dashboard.
      _dashboard->redraw (_renderer, now - epoch,
                          ceil ((double)(1 * GINGA_SECOND / elapsed)),
                          frameno);

      SDL_RenderPresent (_renderer);

      // Destroy dead textures.
      g_list_free_full (_textures, (GDestroyNotify) SDL_DestroyTexture);
      _textures = NULL;
    }

  SDL_Quit ();
}


// Private methods.

bool
Display::add (GList **list, gpointer data)
{
  bool found;

  g_assert_nonnull (list);
  if (unlikely (found = g_list_find (*list, data)))
    {
      WARNING ("object %p already in list %p", data, *list);
      goto done;
    }
  *list = g_list_append (*list, data);
 done:
  return !found;
}

bool
Display::remove (GList **list, gpointer data)
{
  GList *elt;

  g_assert_nonnull (list);
  elt = g_list_find (*list, data);
  if (unlikely (elt == NULL))
    {
      WARNING ("object %p not in list %p", data, *list);
      goto done;
    }
  *list = g_list_remove_link (*list, elt);
 done:
  return elt != NULL;
}

bool
Display::find (GList *list, gconstpointer data)
{
  return g_list_find (list, data) != NULL;
}

void
Display::notifyTickListeners (GingaTime total, GingaTime diff, int frameno)
{
  GList *l = _listeners;
  while (l != NULL)
    {
      GList *next = l->next;
      IEventListener *obj = (IEventListener *) l->data;
      g_assert_nonnull (obj);
      obj->handleTickEvent (total, diff, frameno);
      l = next;
    }
}

void
Display::notifyKeyListeners (SDL_EventType type, SDL_Keycode key)
{
  GList *l = _listeners;
  while (l != NULL)
    {
      GList *next = l->next;
      IEventListener *obj = (IEventListener *) l->data;
      g_assert_nonnull (obj);
      obj->handleKeyEvent (type, key);
      l = next;
    }
}

GINGA_MB_END
