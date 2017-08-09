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
#include "ginga-internal.h"

#include "formatter/Scheduler.h"
using namespace ::ginga::formatter;

#include "mb/Display.h"
using namespace ::ginga::mb;

#define SCHEDULER(me) ((Scheduler *)(me)->_scheduler)
#define DISPLAY(me)   ((Display *)(me)->_display)

Ginga::Ginga (arg_unused (int argc), arg_unused (char **argv),
              int width, int height, bool fullscreen)
{
  started = false;
  _scheduler = new Scheduler ();
  _display = new ginga::mb::Display (width, height, fullscreen);
  _Ginga_Display = DISPLAY (this);

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

Ginga::~Ginga ()
{
  delete DISPLAY (this);
  delete SCHEDULER (this);

#if defined WITH_CEF && WITH_CEF
  CefShutdown ();
#endif
}

void
Ginga::start (const string &file)
{
  if (started)
    return;                     // nothing to do

  SCHEDULER (this)->startDocument (file);
  started = true;
}

void
Ginga::stop ()
{
  ERROR_NOT_IMPLEMENTED ("stop is not supported");
}

void
Ginga::redraw (cairo_t *cr)
{
  DISPLAY (this)->redraw (cr);
}

void
Ginga::send_key (const string &key, bool press)
{
  DISPLAY (this)->notifyKeyListeners (key, press);
}

void
Ginga::send_tick (guint64 total, guint64 diff, guint64 frameno)
{
  DISPLAY (this)->notifyTickListeners (total, diff, (int) frameno);
}
