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

static GingaOptions opts_defaults = {
  800,                          // width
  600,                          // height
  false,                        // debug
};

typedef struct GingaOptionData
{
  GType type;
  int offset;
  void *func;
} OptionTab;

#define OPTS_ENTRY(name,type,func)                              \
  {G_STRINGIFY (name),                                          \
      {(type), offsetof (GingaOptions, name),                   \
         pointerof (G_PASTE (GingaPrivate::setOption, func))}}

static map<string, GingaOptionData> opts_table =
{
 OPTS_ENTRY (debug,  G_TYPE_BOOLEAN, Debug),
 OPTS_ENTRY (height, G_TYPE_INT, Size),
 OPTS_ENTRY (width,  G_TYPE_INT, Size),
};

static bool
opts_table_index (const string &key, GingaOptionData **result)
{
  map<string, GingaOptionData>::iterator it;
  if ((it = opts_table.find (key)) == opts_table.end ())
    return false;
  tryset (result, &it->second);
  return true;
}


// Public methods.

GingaPrivate::GingaPrivate (unused (int argc), unused (char **argv),
                            GingaOptions *opts)
  : Ginga (argc, argv, opts)
{
  _opts = (opts) ? opts : &opts_defaults;
  _started = false;
  _scheduler = new Scheduler ();
  _display = new ginga::mb::Display (_opts->width, _opts->height);
  _Ginga_Display = _display;

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

GingaPrivate::~GingaPrivate ()
{
  delete _display;
  delete _scheduler;
#if defined WITH_CEF && WITH_CEF
  CefShutdown ();
#endif
}

void
GingaPrivate::start (const string &file)
{
  if (_started)
    return;                     // nothing to do
  g_assert_nonnull (_scheduler);
  _scheduler->startDocument (file);
  _started = true;
}

void G_GNUC_NORETURN
GingaPrivate::stop ()
{
  ERROR_NOT_IMPLEMENTED ("stop is not supported");
}

void
GingaPrivate::resize (int width, int height)
{
  g_assert_nonnull (_display);
  _display->setSize (width, height);
}

void
GingaPrivate::redraw (cairo_t *cr)
{
  g_assert_nonnull (_display);
  _display->redraw (cr);
}

void
GingaPrivate::sendKeyEvent (const string &key, bool press)
{
  g_assert_nonnull (_display);
  _display->notifyKeyListeners (key, press);
}

void
GingaPrivate::sendTickEvent (uint64_t total, uint64_t diff,
                             uint64_t frameno)
{
  g_assert_nonnull (_display);
  _display->notifyTickListeners (total, diff, (int) frameno);
}


// Option handling.

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

void
GingaPrivate::setOptionDebug (GingaPrivate *self,
                              const string &name, bool value)
{
  (void) self;
  TRACE ("toggle debug %s=%d", name.c_str (), value);
}

void
GingaPrivate::setOptionSize (GingaPrivate *self,
                             const string &name, unused (int value))
{
  GingaOptions opts;
  g_assert (name == "width" || name == "height");
  opts = self->getOptions ();
  TRACE ("resizing %dx%d", opts.width, opts.height);
  self->resize (opts.width, opts.height);
}
