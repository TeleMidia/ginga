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
#include "Dashboard.h"
#include "Display.h"

#include "player/TextPlayer.h"
using namespace ::ginga::player;

GINGA_MB_BEGIN

Dashboard::Dashboard ()
{
}

Dashboard::~Dashboard()
{
}

void
Dashboard::redraw2 (cairo_t *cr)
{
  static GingaColor fg = {1., 1., 1., 1.};
  static GingaColor bg = {0, 0, 0, 0};
  static GingaRect rect = {0, 0, 0, 0};

  string info;
  cairo_surface_t *debug;
  GingaRect ink;

  info = xstrbuild ("#%d %" GINGA_TIME_FORMAT "  %.1ffps",
                    _frameno, GINGA_TIME_ARGS (_total),
                    1 * GINGA_SECOND / (double) _diff);

  Ginga_Display->getSize (&rect.width, &rect.height);
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

void
Dashboard::handleTickEvent (GingaTime total, GingaTime diff, int frameno)
{
  _total = total;
  _diff = diff;
  _frameno = frameno;
}

GINGA_MB_END
