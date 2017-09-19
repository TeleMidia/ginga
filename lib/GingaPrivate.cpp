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

static GingaOptions default_opts = {
  800,                          // width
  600,                          // height
  false,                        // fullscreen
  false,                        // boolean
};

GingaPrivate::GingaPrivate (unused (int argc), unused (char **argv),
                            GingaOptions *opts)
  : Ginga (argc, argv, opts)
{
  _opts = (opts) ? opts : &default_opts;
  _started = false;
  _scheduler = new Scheduler ();
  _display = new ginga::mb::Display (opts->width, opts->height, opts->fullscreen);
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
GingaPrivate::send_key (const string &key, bool press)
{
  g_assert_nonnull (_display);
  _display->notifyKeyListeners (key, press);
}

void
GingaPrivate::send_tick (uint64_t total, uint64_t diff, uint64_t frameno)
{
  g_assert_nonnull (_display);
  _display->notifyTickListeners (total, diff, (int) frameno);
}
