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
#include "TextPlayer.h"

#include "mb/Display.h"
#include "mb/SDLWindow.h"

using namespace ::ginga::mb;

GINGA_PLAYER_BEGIN


// Public.

TextPlayer::TextPlayer (const string &uri) : Player (uri)
{
  _fontColor = {0, 0, 0, 255};  // black
  _fontFamily = "helvetica";
  _fontSize = "18px";
  _fontStyle = "";
  _fontVariant = "";
  _fontWeight = "";
  _textAlign = "left";
  _verticalAlign = "top";
}

void
TextPlayer::setProperty (const string &name, const string &value)
{
  Player::setProperty (name, value);

  if (name == "fontColor")
    {
      if (value != "" && !_ginga_parse_color (value, &_fontColor))
        goto syntax_error;
    }
  else if(name == "fontFamily")
    {
      _fontFamily = value;
    }
  else if (name == "fontSize")
    {
      _fontSize = value;
    }
  else if(name == "fontStyle")
    {
      if (value != "" && value != "normal" && value != "italic")
        {
          goto syntax_error;
        }
      _fontStyle = value;
    }
  else if(name == "fontVariant")
    {
      if (value != "" && value != "small-caps")
        {
          goto syntax_error;
        }
      _fontVariant = value;
    }
  else if (name == "fontWeight")
    {
      if (value != "" && value != "normal" && value != "bold")
        {
          goto syntax_error;
        }
      _fontWeight = value;
    }
  else if (name == "textAlign")
    {
      if (value != "" && value != "left" && value != "right"
          && value != "center" && value != "justify")
        {
          goto syntax_error;
        }
      _textAlign = value;
    }
  else if (name == "verticalAlign")
    {
      if (value != "" && value != "top"
          && value != "middle" && value != "bottom")
        {
          goto syntax_error;
        }
      _verticalAlign = value;
    }

  if (_state == PL_OCCURRING)
    this->dirty = true;
  return;

 syntax_error:
  ERROR_SYNTAX ("property '%s': bad value '%s'",
                name.c_str (), value.c_str ());
}

void
TextPlayer::redraw (SDL_Renderer *renderer)
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
TextPlayer::reload (SDL_Renderer *renderer)
{
  const char *path;
  gchar *contents = NULL;
  GError *err = NULL;

  SDL_Surface *sfc;
  cairo_t *cr;
  cairo_surface_t *sfc_cr;

  PangoLayout *layout;
  string font;
  PangoFontDescription *desc;

  double align;
  int height;

  path = _uri.c_str ();
  if (unlikely (!g_file_get_contents (path, &contents, NULL, &err)))
    {
      g_assert_nonnull (err);
      ERROR ("cannot load text file %s: %s", path, err->message);
      g_error_free (err);
    }
  g_assert_nonnull (contents);

  SDLx_CreateSurfaceARGB32 (_rect.w, _rect.h, &sfc);
  SDLx_LockSurface (sfc);
  sfc_cr = cairo_image_surface_create_for_data
    ((guchar *) sfc->pixels, CAIRO_FORMAT_ARGB32,
     sfc->w, sfc->h, sfc->pitch);

  cr = cairo_create (sfc_cr);
  g_assert_nonnull (cr);

  layout = pango_cairo_create_layout (cr);
  g_assert_nonnull (layout);

  pango_layout_set_text (layout, contents, -1);
  g_free (contents);

  font = xstrbuild ("%s %s %s %s",
                    _fontFamily.c_str (),
                    _fontWeight.c_str (),
                    _fontStyle.c_str (),
                    _fontSize.c_str ());

  desc = pango_font_description_from_string (font.c_str ());
  g_assert_nonnull (desc);

  pango_layout_set_font_description (layout, desc);
  pango_font_description_free (desc);

  if(_textAlign == "left")
    pango_layout_set_alignment (layout, PANGO_ALIGN_LEFT);
  else if(_textAlign == "center")
    pango_layout_set_alignment (layout, PANGO_ALIGN_CENTER);
  else if(_textAlign == "right")
    pango_layout_set_alignment (layout, PANGO_ALIGN_RIGHT);
  else
    pango_layout_set_justify (layout, true);

  pango_layout_set_width (layout, _rect.w * PANGO_SCALE);
  pango_layout_set_wrap (layout, PANGO_WRAP_WORD);
  pango_layout_get_size (layout, NULL, &height);

  cairo_set_source_rgba (cr, _fontColor.r/255,
                         _fontColor.g/255,
                         _fontColor.b/255,
                         _fontColor.a/255);

  pango_cairo_update_layout (cr, layout);

  if (_verticalAlign == "bottom")
    align = _rect.h - (height / PANGO_SCALE);
  else if(_verticalAlign == "middle")
    align = (_rect.h / 2) - ((height / PANGO_SCALE) / 2);
  else
    align = 0;

  cairo_move_to (cr, 0, align);
  pango_cairo_show_layout (cr, layout);

  g_object_unref (layout);
  cairo_destroy (cr);
  cairo_surface_destroy (sfc_cr);

  _texture = SDL_CreateTextureFromSurface (renderer, sfc);
  g_assert_nonnull (_texture);

  SDLx_UnlockSurface (sfc);
  SDL_FreeSurface(sfc);
}

GINGA_PLAYER_END
