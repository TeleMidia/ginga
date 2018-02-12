/* Copyright (C) 2006-2018 PUC-Rio/Laboratorio TeleMidia

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
along with Ginga.  If not, see <https://www.gnu.org/licenses/>.  */

#include "aux-ginga.h"
#include "PlayerSvg.h"

#if defined WITH_LIBRSVG && WITH_LIBRSVG
#include <librsvg/rsvg.h>
#endif

GINGA_NAMESPACE_BEGIN

PlayerSvg::PlayerSvg (Formatter *formatter, Media *media)
    : Player (formatter, media)
{
}

PlayerSvg::~PlayerSvg ()
{
}

void
PlayerSvg::reload ()
{
  RsvgHandle *svg;
  RsvgDimensionData dim;
  GError *err = NULL;

  double scale;
  int width;
  int height;

  cairo_surface_t *sfc;
  cairo_t *cr;

  g_assert (_state != SLEEPING);

  GFile *file = g_file_new_for_uri (_prop.uri.c_str ());
  svg = rsvg_handle_new_from_gfile_sync (file, RSVG_HANDLE_FLAGS_NONE, NULL,
                                         &err);
  if (unlikely (svg == NULL))
    ERROR ("cannot load SVG file %s: %s", _prop.uri.c_str (), err->message);
  g_object_unref (file);

  g_assert_cmpint (_prop.rect.width, >, 0);
  g_assert_cmpint (_prop.rect.height, >, 0);
  rsvg_handle_get_dimensions (svg, &dim);

  scale = (dim.width > dim.height)
              ? (double) _prop.rect.width / dim.width
              : (double) _prop.rect.height / dim.height;

  width = (int) (floor (dim.width * scale) + 1);
  height = (int) (floor (dim.height * scale) + 1);

  sfc = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, width, height);
  g_assert_nonnull (sfc);

  cr = cairo_create (sfc);
  g_assert_nonnull (cr);
  cairo_scale (cr, scale, scale);
  rsvg_handle_render_cairo (svg, cr);
  cairo_destroy (cr);

  if (_surface != nullptr)
    cairo_surface_destroy (_surface);

  _surface = sfc;

  Player::reload ();
}

GINGA_NAMESPACE_END
