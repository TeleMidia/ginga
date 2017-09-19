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
#include "GingaPrivate.h"

#include "formatter/Scheduler.h"
using namespace ::ginga::formatter;

#include "player/TextPlayer.h"
using namespace ::ginga::player;

// Option defaults.
static GingaOptions opts_defaults = {
  800,                          // width
  600,                          // height
  false,                        // debug
};

// Option data.
typedef struct GingaOptionData
{
  GType type;                   // type
  int offset;                   // offset in GingaOption struct
  void *func;                   // update function
} OptionTab;

#define OPTS_ENTRY(name,type,func)                              \
  {G_STRINGIFY (name),                                          \
      {(type), offsetof (GingaOptions, name),                   \
         pointerof (G_PASTE (GingaPrivate::setOption, func))}}

// Option table.
static map<string, GingaOptionData> opts_table =
{
 OPTS_ENTRY (debug,  G_TYPE_BOOLEAN, Debug),
 OPTS_ENTRY (height, G_TYPE_INT, Size),
 OPTS_ENTRY (width,  G_TYPE_INT, Size),
};

// Indexes option table.
static bool
opts_table_index (const string &key, GingaOptionData **result)
{
  map<string, GingaOptionData>::iterator it;
  if ((it = opts_table.find (key)) == opts_table.end ())
    return false;
  tryset (result, &it->second);
  return true;
}

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


// External API.

/**
 * @brief Starts NCL from file.
 * @param file Path to NCL file.
 */
void
GingaPrivate::start (const string &file)
{
  if (_started)
    return;                     // nothing to do
  g_assert_nonnull (_scheduler);
  _scheduler->startDocument (file);
  _ncl_file = file;
  _started = true;
}

/**
 * @brief Stops NCL.
 */
void G_GNUC_NORETURN
GingaPrivate::stop ()
{
  ERROR_NOT_IMPLEMENTED ("stop is not supported");
}

/**
 * @brief Resize current surface.
 * @param width New width (in pixels).
 * @param height New height (in pixels).
 */
void
GingaPrivate::resize (int width, int height)
{
  g_assert (width > 0 && height > 0);
  _opts->width = width;
  _opts->height = height;
  for (GList *l = _players; l != nullptr; l = l->next)
    {
      Player *pl = (Player *) l->data;
      g_assert_nonnull (pl);
      pl->setProperty ("top", pl->getProperty ("top"));
      pl->setProperty ("left", pl->getProperty ("left"));
      pl->setProperty ("bottom", pl->getProperty ("bottom"));
      pl->setProperty ("right", pl->getProperty ("right"));
      pl->setProperty ("width", pl->getProperty ("width"));
      pl->setProperty ("height", pl->getProperty ("height"));
    }
}

/**
 * @brief Draw current surface onto cairo context.
 * @param cr Target cairo context.
 */
void
GingaPrivate::redraw (cairo_t *cr)
{
  GList *l;

  _players = g_list_sort (_players, (GCompareFunc) win_cmp_z);
  l = _players;
  while (l != NULL)             // can be modified while being traversed
    {
      GList *next = l->next;
      Player *pl = (Player *) l->data;
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

  if (_opts->debug)
    {
      static GingaColor fg = {1., 1., 1., 1.};
      static GingaColor bg = {0, 0, 0, 0};
      static GingaRect rect = {0, 0, 0, 0};

      string info;
      cairo_surface_t *debug;
      GingaRect ink;

      info = xstrbuild ("%s: #%lu %" GINGA_TIME_FORMAT " %.1ffps",
                        _ncl_file.c_str (),
                        _last_tick_frameno,
                        GINGA_TIME_ARGS (_last_tick_total),
                        1 * GINGA_SECOND / (double) _last_tick_diff);


      rect.width = _opts->width;
      rect.height = _opts->height;
      debug = TextPlayer::renderSurface
        (info, "monospace", "", "bold", "9", fg, bg,
         rect, "center", "", true, &ink);
      ink = {0, 0, rect.width, ink.height - ink.y + 4};

      cairo_save (cr);
      cairo_set_source_rgba (cr, 1., 0., 0., .5);
      cairo_rectangle (cr, 0, 0, ink.width, ink.height);
      cairo_fill (cr);
      cairo_set_source_surface (cr, debug, 0, 0);
      cairo_paint (cr);
      cairo_restore (cr);
      cairo_surface_destroy (debug);
    }
}

// This gymnastics is necessary to ensure that the list can be safely
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
 * @brief Sends key event.
 * @param key Key name.
 * @param press True if press, False if release.
 */
void
GingaPrivate::sendKeyEvent (const string &key, bool press)
{
  NOTIFY_LISTENERS (_listeners, IGingaPrivateEventListener,
                    handleKeyEvent, key, press);
}

/**
 * @brief Sends tick event.
 * @param total Time passed since start (in microseconds).
 * @param diff Time passed since last tick (in microseconds).
 * @param frameno Current frame number.
 */
void
GingaPrivate::sendTickEvent (uint64_t total, uint64_t diff,
                             uint64_t frameno)
{
  _last_tick_total = total;
  _last_tick_diff = diff;
  _last_tick_frameno = frameno;
  NOTIFY_LISTENERS (_listeners, IGingaPrivateEventListener,
                    handleTickEvent, total, diff, (int) frameno);
}

/**
 * @brief Gets current options.
 * @return A copy of the current options.
 */
GingaOptions
GingaPrivate::getOptions ()
{
  return *_opts;
}

#define GET_SET_OPTION_DEFN(Name, Type, GType, Msg)                     \
  Type                                                                  \
  GingaPrivate::getOption##Name (const string &name)                    \
  {                                                                     \
    GingaOptionData *opt;                                               \
    if (unlikely (!opts_table_index (name, &opt)))                      \
      {                                                                 \
        ERROR ("unknown option '%s'", name.c_str ());                   \
      }                                                                 \
    if (unlikely (opt->type != (GType)))                                \
      {                                                                 \
        ERROR ("option '%s' is not %s", name.c_str (), Msg);            \
      }                                                                 \
    return *((Type *)(((ptrdiff_t) _opts) + opt->offset));              \
  }                                                                     \
  void                                                                  \
  GingaPrivate::setOption##Name (const string &name, Type value)        \
  {                                                                     \
    GingaOptionData *opt;                                               \
    if (unlikely (!opts_table_index (name, &opt)))                      \
      {                                                                 \
        ERROR ("unknown option '%s'", name.c_str ());                   \
      }                                                                 \
    if (unlikely (opt->type != (GType)))                                \
      {                                                                 \
        ERROR ("option '%s' is not %s", name.c_str (), Msg);            \
      }                                                                 \
    *((Type *)(((ptrdiff_t) _opts) + opt->offset)) = value;             \
    if (opt->func)                                                      \
      {                                                                 \
        ((void (*) (GingaPrivate *, const string &, Type)) opt->func)   \
          (this, name, value);                                          \
      }                                                                 \
  }

GET_SET_OPTION_DEFN (Bool, bool, G_TYPE_BOOLEAN, "a boolean")
GET_SET_OPTION_DEFN (Int, int, G_TYPE_INT, "an int")


// Internal API.

/**
 * @brief Creates a new instance.
 */
GingaPrivate::GingaPrivate (unused (int argc), unused (char **argv),
                            GingaOptions *opts)
  : Ginga (argc, argv, opts)
{
  _opts = (opts) ? opts : &opts_defaults;
  _scheduler = new Scheduler (this);
  _listeners = nullptr;
  _players = nullptr;

  _started = false;
  _ncl_file = "";
  _last_tick_total = 0;
  _last_tick_diff = 0;
  _last_tick_frameno = 0;

#if defined WITH_CEF && WITH_CEF
  CefMainArgs args (argc, argv);
  CefSettings settings;
  int pstatus = CefExecuteProcess (args, nullptr, nullptr);
  if (pstatus >= 0)
    return pstatus;
  if (unlikely (!CefInitialize (args, settings, nullptr, nullptr)))
    exit (EXIT_FAILURE);
#endif
}

/**
 * @brief Destroys instance.
 */
GingaPrivate::~GingaPrivate ()
{
  delete _scheduler;
  g_list_free (_listeners);
  g_list_free (_players);
#if defined WITH_CEF && WITH_CEF
  CefShutdown ();
#endif
}

/**
 * @brief Adds event listener.
 * @param obj Event listener.
 */
bool
GingaPrivate::registerEventListener (IGingaPrivateEventListener *obj)
{
  g_assert_nonnull (obj);
  return this->add (&_listeners, obj);
}

/**
 * @brief Removes event listener.
 */
bool
GingaPrivate::unregisterEventListener (IGingaPrivateEventListener *obj)
{
  g_assert_nonnull (obj);
  return this->remove (&_listeners, obj);
}

/**
 * @brief Adds handled player.
 * @param player Player.
 */
void
GingaPrivate::registerPlayer (Player *player)
{
  g_assert_nonnull (player);
  g_assert (this->add (&_players, player));
}

/**
 * @brief Removes handled player.
 */
void
GingaPrivate::unregisterPlayer (Player *player)
{
  g_assert_nonnull (player);
  g_assert (this->remove (&_players, player));
}

/**
 * @brief Updates debug option.
 */
void
GingaPrivate::setOptionDebug (unused (GingaPrivate *self),
                              const string &name, bool value)
{
  TRACE ("setting option %s to %s", name.c_str (), strbool (value));
}

/**
 * @brief Updates size option.
 */
void
GingaPrivate::setOptionSize (GingaPrivate *self,
                             const string &name,
                             int value)
{
  GingaOptions opts;
  g_assert (name == "width" || name == "height");
  opts = self->getOptions ();
  self->resize (opts.width, opts.height);
  TRACE ("setting option %s to %d", name.c_str (), value);
}


// Private methods.

bool
GingaPrivate::add (GList **list, gpointer data)
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
GingaPrivate::remove (GList **list, gpointer data)
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
