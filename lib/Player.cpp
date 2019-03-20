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
#include "Player.h"
#include "LuaAPI.h"

#include "Document.h"
#include "Media.h"
#include "MediaSettings.h"
#include "PlayerText.h"

GINGA_NAMESPACE_BEGIN

Player::Player (Media *media)
{
  g_return_if_fail (media != NULL);

  _L = media->getDocument ()->getLuaState ();
  _media = media;
  _state = Player::STOPPED;
  _surface = NULL;

  _uri = "";
  _rect.x = 0;
  _rect.y = 0;
  _rect.width = 0;
  _rect.height = 0;
  _eos = false;
  _alpha = 1.;
  _bgColor.red = 0.;
  _bgColor.green = 0.;
  _bgColor.blue = 0.;
  _bgColor.alpha = 0.;

  LuaAPI::Player_attachWrapper (_L, this, media);
}

Player::~Player ()
{
  if (_surface != NULL)
    cairo_surface_destroy (_surface);

  LuaAPI::Player_detachWrapper (_L, this);
}

Player::State
Player::getState ()
{
  return _state;
}

string
Player::getURI ()
{
  return _uri;
}

void
Player::setURI (const string &uri)
{
  _uri = uri;
}

void
Player::getRect (int *x, int *y, int *width, int *height)
{
  if (x != NULL)
    *x = _rect.x;
  if (y != NULL)
    *y = _rect.y;
  if (width != NULL)
    *width = _rect.width;
  if (height != NULL)
    *height = _rect.height;
}

void
Player::setRect (int x, int y, int width, int height)
{
  _rect.x = x;
  _rect.y = y;
  _rect.width = CLAMP (width, 0, G_MAXINT);
  _rect.height = CLAMP (height, 0, G_MAXINT);
}

bool
Player::getEOS ()
{
  return _eos;
}

void
Player::setEOS (bool eos)
{
  _eos = eos;
}

void
Player::start ()
{
  g_assert (_state != Player::PLAYING);
  _state = Player::PLAYING;
}

void
Player::pause ()
{
  g_assert (_state != PAUSED && _state != Player::STOPPED);
  _state = PAUSED;
}

void
Player::stop ()
{
  g_assert (_state != Player::STOPPED);
  _state = Player::STOPPED;
}

void
Player::draw (cairo_t *cr)
{
  if (_state == Player::STOPPED)
    return;                     // nothing to do

  if (_rect.width <= 0 || _rect.height <= 0)
    return;                     // nothing to do

  if (_bgColor.alpha > 0)
    {
      cairo_save (cr);
      cairo_set_source_rgba
        (cr, _bgColor.red, _bgColor.green, _bgColor.blue, _alpha);
      cairo_rectangle (cr, _rect.x, _rect.y, _rect.width, _rect.height);
      cairo_fill (cr);
      cairo_restore (cr);
    }

  if (_surface != NULL)
    {
      double sx, sy;
      sx = (double) _rect.width
        / cairo_image_surface_get_width (_surface);
      sy = (double) _rect.height
        / cairo_image_surface_get_height (_surface);
      cairo_save (cr);
      cairo_translate (cr, _rect.x, _rect.y);
      cairo_scale (cr, sx, sy);
      cairo_set_source_surface (cr, _surface, 0., 0.);
      cairo_paint_with_alpha (cr, _alpha);
      cairo_restore (cr);
    }

  // if (_media->isFocused ())
  //   {
  //     cairo_save (cr);
  //     cairo_set_source_rgba (cr, 1., 1., 0., 1.);
  //     cairo_set_antialias (cr, CAIRO_ANTIALIAS_NONE);
  //     cairo_rectangle (cr, _rect.x, _rect.y, _rect.width,
  //                      _rect.height);
  //     cairo_stroke (cr);
  //     cairo_restore (cr);
  //   }

  cairo_save (cr);
  cairo_set_operator (cr, CAIRO_OPERATOR_CLEAR);
  cairo_set_source_rgba (cr, 1., 1., 1., 1.);

  cairo_close_path (cr);
  cairo_stroke_preserve (cr);
  cairo_fill (cr);
  cairo_set_operator (cr, CAIRO_OPERATOR_OVER);
  cairo_restore (cr);
}

GINGA_NAMESPACE_END
