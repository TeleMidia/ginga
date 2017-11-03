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
#include "GingaInternal.h"

#include "formatter/Scheduler.h"
using namespace ::ginga::formatter;

#include "ncl/ParserXercesC.h"
#include "ncl/ParserLibXML.h"
using namespace ::ginga;

#include "player/TextPlayer.h"
using namespace ::ginga::player;

// Option defaults.
static GingaOptions opts_defaults =
{
  800,                          // width
  600,                          // height
  false,                        // debug
  false,                        // experimental
  false,                        // opengl
  "",                           // background ("" == none)
};

// Option data.
typedef struct GingaOptionData
{
  GType type;                   // option type
  int offset;                   // offset in GingaOption struct
  void *func;                   // update function
} OptionTab;

#define OPTS_ENTRY(name,type,func)                              \
  {G_STRINGIFY (name),                                          \
      {(type), offsetof (GingaOptions, name),                   \
         pointerof (G_PASTE (GingaInternal::setOption, func))}}

// Option table.
static map<string, GingaOptionData> opts_table =
{
 OPTS_ENTRY (background,   G_TYPE_STRING,  Background),
 OPTS_ENTRY (debug,        G_TYPE_BOOLEAN, Debug),
 OPTS_ENTRY (experimental, G_TYPE_BOOLEAN, Experimental),
 OPTS_ENTRY (height,       G_TYPE_INT,     Size),
 OPTS_ENTRY (opengl,       G_TYPE_BOOLEAN, OpenGL),
 OPTS_ENTRY (width,        G_TYPE_INT,     Size),
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
static gint G_GNUC_UNUSED
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
 * @brief Get current state.
 * @return Current state.
 */
GingaState
GingaInternal::getState ()
{
  return _state;
}

/**
 * @brief Starts NCL from file.
 * @param file Path to NCL file.
 * @param errmsg Address of a variable to store an error message.
 * @return True if successfully, or false otherwise.
 */
bool
GingaInternal::start (const string &file, string *errmsg)
{
  NclDocument *doc;
  int w, h;

  if (_state != GINGA_STATE_STOPPED)
    return false;               // nothing to do

  // Parse document.
  w = _opts.width;
  h = _opts.height;
  if (!_opts.experimental)
    doc = ParserXercesC::parse (file, w, h, errmsg);
  else
    doc = ParserLibXML::parseFile (file, w, h, errmsg);
  if (unlikely (doc == nullptr))
    return false;

  // Create scheduler.
  _scheduler = new Scheduler (this);
  _ncl_file = file;
  _eos = false;
  _last_tick_total = 0;
  _last_tick_diff = 0;
  _last_tick_frameno = 0;

  // Run document.
  TRACE ("%s", file.c_str ());
  if (unlikely (!_scheduler->run (doc)))
    {
      delete _scheduler;
      return false;
    }

  _state = GINGA_STATE_PLAYING;
  return true;
}

/**
 * @brief Stops NCL.
 */
bool
GingaInternal::stop ()
{
  if (_state == GINGA_STATE_STOPPED)
    return false;               // nothing to do

  delete _scheduler;
  _scheduler = nullptr;
  _state = GINGA_STATE_STOPPED;
  return true;
}

/**
 * @brief Resize current surface.
 * @param width New width (in pixels).
 * @param height New height (in pixels).
 */
void
GingaInternal::resize (int width, int height)
{
  g_assert (width > 0 && height > 0);
  _opts.width = width;
  _opts.height = height;
  _scheduler->resize (width, height);
}

/**
 * @brief Draw current surface onto cairo context.
 * @param cr Target cairo context.
 */
void
GingaInternal::redraw (cairo_t *cr)
{
  g_assert_nonnull (_scheduler);
  _scheduler->redraw (cr);
}

// Stop formatter if EOS has been seen.
#define _GINGA_CHECK_EOS(ginga)                                 \
  G_STMT_START                                                  \
  {                                                             \
    if ((ginga)->getEOS ())                                     \
      {                                                         \
        g_assert ((ginga)->_state == GINGA_STATE_PLAYING);      \
        (ginga)->setEOS (false);                                \
        g_assert ((ginga)->stop ());                            \
        g_assert ((ginga)->_state == GINGA_STATE_STOPPED);      \
      }                                                         \
  }                                                             \
  G_STMT_END

/**
 * @brief Sends key event.
 * @param key Key name.
 * @param press True if press or false if release.
 * @return True if successful, or false otherwise.
 */
bool
GingaInternal::sendKeyEvent (const string &key, bool press)
{
  _GINGA_CHECK_EOS (this);
  if (_state != GINGA_STATE_PLAYING)
    return false;               // nothing to do

  _scheduler->sendKeyEvent (key, press);
  return true;
}

/**
 * @brief Sends tick event.
 * @param total Time passed since start (in microseconds).
 * @param diff Time passed since last tick (in microseconds).
 * @param frame Current frame number.
 * @return True if successful, or false otherwise.
 */
bool
GingaInternal::sendTickEvent (uint64_t total, uint64_t diff, uint64_t frame)
{
  _GINGA_CHECK_EOS (this);
  if (_state != GINGA_STATE_PLAYING)
    return false;               // nothing to do

  _last_tick_total = total;
  _last_tick_diff = diff;
  _last_tick_frameno = frame;

  _scheduler->sendTickEvent (total, diff, frame);
  return true;
}

/**
 * @brief Gets current options.
 * @return The current options.
 */
const GingaOptions *
GingaInternal::getOptions ()
{
  return &_opts;
}

#define OPT_ERR_UNKNOWN(name)\
  ERROR ("unknown GingaOption '%s'", (name))
#define OPT_ERR_BAD_TYPE(name, typename)\
  ERROR ("GingaOption '%s' is of type '%s'", (name), (typename))

#define OPT_GETSET_DEFN(Name, Type, GType)                              \
  Type                                                                  \
  GingaInternal::getOption##Name (const string &name)                   \
  {                                                                     \
    GingaOptionData *opt;                                               \
    if (unlikely (!opts_table_index (name, &opt)))                      \
      OPT_ERR_UNKNOWN (name.c_str ());                                  \
    if (unlikely (opt->type != (GType)))                                \
      OPT_ERR_BAD_TYPE (name.c_str (), G_STRINGIFY (Type));             \
    return *((Type *)(((ptrdiff_t) &_opts) + opt->offset));             \
  }                                                                     \
  void                                                                  \
  GingaInternal::setOption##Name (const string &name, Type value)       \
  {                                                                     \
    GingaOptionData *opt;                                               \
    if (unlikely (!opts_table_index (name, &opt)))                      \
      OPT_ERR_UNKNOWN (name.c_str ());                                  \
    if (unlikely (opt->type != (GType)))                                \
      OPT_ERR_BAD_TYPE (name.c_str (), G_STRINGIFY (Type));             \
    *((Type *)(((ptrdiff_t) &_opts) + opt->offset)) = value;            \
    if (opt->func)                                                      \
      {                                                                 \
        ((void (*) (GingaInternal *, const string &, Type)) opt->func)  \
          (this, name, value);                                          \
      }                                                                 \
  }

OPT_GETSET_DEFN (Bool, bool, G_TYPE_BOOLEAN)
OPT_GETSET_DEFN (Int, int, G_TYPE_INT)
OPT_GETSET_DEFN (String, string, G_TYPE_STRING)


// Internal API.

/**
 * @brief Creates a new instance.
 */
GingaInternal::GingaInternal (unused (int argc), unused (char **argv),
                              GingaOptions *opts) : Ginga (argc, argv, opts)
{
  const char *s;

  _state = GINGA_STATE_STOPPED;
  _opts = (opts) ? *opts : opts_defaults;
  _scheduler = nullptr;

  _ncl_file = "";
  _eos = false;
  _last_tick_total = 0;
  _last_tick_diff = 0;
  _last_tick_frameno = 0;
  _saved_G_MESSAGES_DEBUG = (s = g_getenv ("G_MESSAGES_DEBUG"))
    ? string (s) : "";

  setOptionBackground (this, "background", _opts.background);
  setOptionDebug (this, "debug", _opts.debug);
  setOptionExperimental (this, "experimental", _opts.experimental);
  setOptionOpenGL (this, "opengl", _opts.opengl);

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
GingaInternal::~GingaInternal ()
{
  if (_state != GINGA_STATE_STOPPED)
    this->stop ();
#if defined WITH_CEF && WITH_CEF
  CefShutdown ();
#endif
}

/**
 * @brief Gets associated scheduler.
 * @return The associated scheduler.
 */
Scheduler *
GingaInternal::getScheduler ()
{
  return _scheduler;
}

/**
 * @brief Gets EOS flag value.
 * @return EOS flag value.
 */
bool
GingaInternal::getEOS ()
{
  return _eos;
}

/**
 * @brief Sets EOS flag.
 * @param eos Flag value.
 */
void
GingaInternal::setEOS (bool eos)
{
  _eos = eos;
}

/**
 * @brief Updates debug option.
 */
void
GingaInternal::setOptionDebug (GingaInternal *self, const string &name,
                               bool value)
{
  g_assert (name == "debug");
  if (value)
    {
      const char *curr = g_getenv ("G_MESSAGES_DEBUG");
      if (curr != nullptr)
        self->_saved_G_MESSAGES_DEBUG = string (curr);
      g_assert (g_setenv ("G_MESSAGES_DEBUG", "all", true));
    }
  else
    {
      g_assert (g_setenv ("G_MESSAGES_DEBUG",
                          self->_saved_G_MESSAGES_DEBUG.c_str (), true));
    }
  TRACE ("%s:=%s", name.c_str (), strbool (value));
}

/**
 * @brief Updates experimental option.
 */
void
GingaInternal::setOptionExperimental (unused (GingaInternal *self),
                                      const string &name, bool value)
{
  g_assert (name == "experimental");
  TRACE ("%s:=%s", name.c_str (), strbool (value));
}

/**
 * @brief Initializes OpenGL option.
 */
void
GingaInternal::setOptionOpenGL (unused (GingaInternal *self),
                                const string &name, bool value)
{
  static int n = 0;
  g_assert (name == "opengl");
  if (unlikely (n++ > 0))
    ERROR ("Cannot change to 'opengl' on-the-fly");
#if !(defined WITH_OPENGL && WITH_OPENGL)
  if (unlikely (value))
    ERROR ("Not compiled with OpenGL support");
#endif
  TRACE ("%s:=%s", name.c_str (), strbool (value));
}

/**
 * @brief Updates size option.
 */
void
GingaInternal::setOptionSize (GingaInternal *self, const string &name,
                              int value)
{
  const GingaOptions *opts;
  g_assert (name == "width" || name == "height");
  opts = self->getOptions ();
  self->resize (opts->width, opts->height);
  TRACE ("%s:=%d", name.c_str (), value);
}

/**
 * @brief Updates background option.
 */
void
GingaInternal::setOptionBackground (GingaInternal *self, const string &name,
                                    string value)
{
  g_assert (name == "background");
  if (value == "")
    self->_background = {0.,0.,0.,0.};
  else
    self->_background = ginga_parse_color (value);
  TRACE ("%s:='%s'", name.c_str (), value.c_str ());
}


// Private methods.

bool
GingaInternal::add (GList **list, gpointer data)
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
GingaInternal::remove (GList **list, gpointer data)
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
