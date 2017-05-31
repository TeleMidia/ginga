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
#include "DisplayDebug.h"

#include "Display.h"

GINGA_MB_BEGIN

DisplayDebug::DisplayDebug ()
{
  this->texture = NULL;
  this->font = pango_font_description_from_string ("fixed 12pt");
  g_assert_nonnull (this->font);
}

DisplayDebug::~DisplayDebug()
{
  SDL_DestroyTexture (this->texture);
  pango_font_description_free (this->font);
}

void
DisplayDebug::redraw (SDL_Renderer *renderer, GingaTime total, double fps,
                      int frameno)
{
  char *text;
  int width;
  int height;
  int text_width;
  int text_height;

  SDL_Surface *sfc;
  cairo_surface_t *cr_sfc;

  cairo_t *cr;
  PangoLayout *layout;

  text = g_strdup_printf ("#%d  %" GINGA_TIME_FORMAT "  %.1ffps",
                          frameno, GINGA_TIME_ARGS (total), fps);
  g_assert_nonnull (text);

  Ginga_Display->getSize (&width, &height);
#if SDL_VERSION_ATLEAST(2,0,5)
  sfc = SDL_CreateRGBSurfaceWithFormat (0, width, height, 32,
                                        SDL_PIXELFORMAT_ARGB8888);
#else
  sfc = SDL_CreateRGBSurface (0, width, height, 32,
                              0xff000000,
                              0x00ff0000,
                              0x0000ff00,
                              0x000000ff);
#endif
  g_assert_nonnull (sfc);

  SDLx_LockSurface (sfc);

  cr_sfc = cairo_image_surface_create_for_data
    ((guchar*) sfc->pixels, CAIRO_FORMAT_ARGB32,
     sfc->w, sfc->h, sfc->pitch);
  g_assert_nonnull (cr_sfc);

  cr = cairo_create (cr_sfc);
  g_assert_nonnull (cr);
  cairo_surface_destroy (cr_sfc);

  layout = pango_cairo_create_layout (cr);
  g_assert_nonnull (layout);

  pango_layout_get_pixel_size (layout, &text_width, &text_height);
  pango_layout_set_text (layout, text, -1);
  g_free (text);

  pango_layout_set_font_description (layout, this->font);
  pango_layout_set_alignment (layout, PANGO_ALIGN_CENTER);
  pango_layout_set_width (layout, width * PANGO_SCALE);
  pango_layout_set_wrap (layout, PANGO_WRAP_WORD);

  cairo_set_source_rgba (cr, 1., 0., 0., .5);
  cairo_rectangle (cr, 0, height - text_height,
                   width, height - text_height);
  cairo_fill (cr);

  cairo_set_source_rgba (cr, 1., 1., 1., 1);
  cairo_move_to (cr, 0., height - text_height);
  pango_cairo_update_layout (cr, layout);
  pango_cairo_show_layout (cr, layout);

  g_object_unref (layout);
  cairo_destroy (cr);

  SDL_DestroyTexture (this->texture);
  this->texture = SDL_CreateTextureFromSurface (renderer, sfc);
  g_assert_nonnull (this->texture);

  SDLx_UnlockSurface (sfc);
  SDL_FreeSurface(sfc);

  SDL_RenderCopy (renderer, this->texture, NULL, NULL);
}

GINGA_MB_END
