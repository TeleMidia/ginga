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
#include "Display.h"
#include "Dashboard.h"

GINGA_MB_BEGIN

// Global display; initialized by Ginga class constructor.
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
 * @brief Creates a new display.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param fullscreen Full-screen mode.
 */
Display::Display (int width, int height, bool fullscreen)
{
  _width = width;
  _height = height;
  _fullscreen = fullscreen;

  _listeners = nullptr;
  _players = nullptr;

  this->registerEventListener (&_dashboard);
}

/**
 * @brief Destroys display.
 */
Display::~Display ()
{
  g_list_free (_listeners);
  g_list_free (_players);
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
  _width = width;
  _height = height;
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
 * @brief Redraws everything.
 */
void
Display::redraw (cairo_t *cr)
{
  GList *l;

  _players = g_list_sort (_players, (GCompareFunc) win_cmp_z);
  l = _players;
  while (l != NULL)             // can be modified while being traversed
    {
      GList *next = l->next;
      Player *pl = (Player *)l->data;
      if (pl == NULL)
        {
          _players = g_list_remove_link (_players, l);
        }
      else
        {
          cairo_save (cr);
          pl->redraw (cr);
          cairo_restore (cr);
        }
      l = next;
    }

  _dashboard.redraw2 (cr);
}

// The gymnastics below is necessary to ensure that the list can be safely
// modified while it is being traversed.
#define NOTIFY_LISTENERS(list, Type, method, ...)                          \
  G_STMT_START                                                             \
  {                                                                        \
    guint n = g_list_length ((list));                                      \
    for (guint i = 0; i < n; i++)                                          \
      {                                                                    \
        Type *obj = (Type *)g_list_nth_data ((list), i);                   \
        if (obj == NULL)                                                   \
          return;                                                          \
        obj->method (__VA_ARGS__);                                         \
      }                                                                    \
  }                                                                        \
  G_STMT_END

/**
 * @brief Post tick event to listeners.
 */
void
Display::notifyTickListeners (GingaTime total, GingaTime diff, int frameno)
{
  NOTIFY_LISTENERS (_listeners, IEventListener, handleTickEvent, total,
                    diff, frameno);
}

/**
 * @brief Post key event to listeners.
 */
void
Display::notifyKeyListeners (const string &key, bool press)
{
  NOTIFY_LISTENERS (_listeners, IEventListener, handleKeyEvent, key, press);
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

  GList *l;

  g_assert_nonnull (list);
  l = *list;
  while (l != NULL)
    {
      GList *next = l->next;
      if (l->data == data)
        {
          *list = g_list_delete_link (*list, l);
          return true;
        }
      l = next;
    }
  WARNING ("object %p not in list %p", data, *list);
  return false;
}

GINGA_MB_END
