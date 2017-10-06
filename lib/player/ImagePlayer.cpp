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
#include "ImagePlayer.h"

GINGA_PLAYER_BEGIN

// Creates a new surface by loading the image file at path PATH.  Stores the
// resulting surface into *DUP and return CAIRO_STATUS_SUCCESS if
// successful, or an error status otherwise.

static cairo_status_t
cairox_surface_create_from_file (const char *path, cairo_surface_t **dup)
{
  cairo_surface_t *sfc;         // result
  GdkPixbuf *pixbuf;
  GError *error = NULL;
  cairo_t *cr;
  int w, h;

  g_assert_nonnull (dup);
  pixbuf = gdk_pixbuf_new_from_file (path, &error);
  if (unlikely (pixbuf == NULL))
    {
      cairo_status_t status = (error->domain == G_FILE_ERROR)
        ? CAIRO_STATUS_FILE_NOT_FOUND : CAIRO_STATUS_READ_ERROR;
      g_error_free (error);
      return status;
    }

  w = gdk_pixbuf_get_width (pixbuf);
  h = gdk_pixbuf_get_height (pixbuf);
  sfc = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, w, h);
  g_assert_nonnull (sfc);
  if (unlikely (cairo_surface_status (sfc) != CAIRO_STATUS_SUCCESS))
    return cairo_surface_status (sfc);

  cr = cairo_create (sfc);
  g_assert_nonnull (cr);
  if (unlikely (cairo_status (cr) != CAIRO_STATUS_SUCCESS))
    return cairo_status (cr);

  gdk_cairo_set_source_pixbuf (cr, pixbuf, 0, 0);
  cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
  cairo_paint (cr);

  cairo_destroy (cr);
  g_object_unref (pixbuf);

  *dup = sfc;
  return CAIRO_STATUS_SUCCESS;
}


// Public.

void
ImagePlayer::reload ()
{
  cairo_status_t status;

  if (_surface != nullptr)
    {
      cairo_surface_destroy (_surface);
#if defined WITH_OPENGL && WITH_OPENGL
      gl_delete_texture (&_gltexture);
#endif
    }

  status = cairox_surface_create_from_file (_uri.c_str (), &_surface);
  if (unlikely (status != CAIRO_STATUS_SUCCESS))
    {
      ERROR ("cannot load image file %s: %s",
             _uri.c_str (), cairo_status_to_string (status));
    }
  g_assert_nonnull (_surface);

#if defined WITH_OPENGL && WITH_OPENGL
  gl_create_texture (&_gltexture,
                     cairo_image_surface_get_width (_surface),
                     cairo_image_surface_get_height (_surface),
                     cairo_image_surface_get_data (_surface));
#endif

  Player::reload ();
}

GINGA_PLAYER_END
