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
#include "SvgPlayer.h"

#include "mb/Display.h"
#include "mb/SDLWindow.h"
using namespace ::ginga::mb;

GINGA_PLAYER_BEGIN


// Public.

void
SvgPlayer::setProperty (const string &name, const string &value)
{
  Player::setProperty (name, value);
  if (status == PL_OCCURRING)
    this->dirty = true;
}

void
SvgPlayer::redraw (SDL_Renderer *renderer)
{
  if (this->dirty)
    {
      this->reload (renderer);
      this->dirty = false;
    }
  Player::redraw (renderer);
}


// Private.

void
SvgPlayer::reload (SDL_Renderer *renderer)
{
  RsvgHandle* svg;
  RsvgDimensionData dim;
  GError *err = NULL;

  double scale;
  int width;
  int height;

  SDL_Surface *sfc;

  cairo_surface_t *cr_sfc;
  cairo_t *cr;

  svg = rsvg_handle_new_from_file(this->mrl.c_str (), &err);
  if (unlikely (svg == NULL))
    ERROR ("cannot load SVG file %s: %s", this->mrl.c_str (), err->message);

  g_assert (_rect.w > 0 && _rect.h > 0);
  rsvg_handle_get_dimensions (svg, &dim);

  scale = (dim.width > dim.height)
    ? (double) _rect.w / dim.width
    : (double) _rect.h / dim.height;

  width = (int)(floor (dim.width * scale) + 1);
  height = (int)(floor (dim.height * scale) + 1);

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
  cr_sfc = cairo_image_surface_create_for_data ((guchar*) sfc->pixels,
                                                CAIRO_FORMAT_ARGB32,
                                                sfc->w, sfc->h, sfc->pitch);
  g_assert_nonnull (cr_sfc);
  cr = cairo_create (cr_sfc);
  g_assert_nonnull (cr);
  cairo_scale (cr, scale, scale);
  rsvg_handle_render_cairo (svg, cr);
  SDLx_UnlockSurface (sfc);

  cairo_destroy (cr);
  cairo_surface_destroy (cr_sfc);

  if (this->texture != NULL)
    Ginga_Display->destroyTexture (this->texture);

  this->texture = SDL_CreateTextureFromSurface (renderer, sfc);
  g_assert_nonnull (this->texture);

  SDL_FreeSurface (sfc);
}

GINGA_PLAYER_END
