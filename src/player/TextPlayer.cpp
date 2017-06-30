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

using namespace ::ginga::mb;

GINGA_PLAYER_BEGIN


// Public.

/**
 * @brief Creates a texture from text.
 * @param renderer SDL renderer.
 * @param text Text to be rendered.
 * @param family Font family.
 * @param width Font weight ("normal" or "bold").
 * @param style Font style ("normal" or "italic").
 * @param size Font size
 * @param color Text color.
 * @param rect Dimensions of the resulting texture.
 * @param haling Horizontal alignment
 *        ("left", "center", "right", or "justified").
 * @param valign Vertical alignment ("bottom", "middle", or "top").
 * @return The resulting texture.
 */
SDL_Texture *
TextPlayer::renderTexture (SDL_Renderer *renderer, const string &text,
                           const string &family, const string &weight,
                           const string &style, const string &size,
                           SDL_Color color, SDL_Rect rect,
                           const string &halign, const string &valign)
{
  SDL_Surface *sfc;
  cairo_t *cr;
  cairo_surface_t *sfc_cr;

  PangoLayout *layout;
  string font;
  PangoFontDescription *desc;

  double align;
  int height;

  SDL_Texture *texture;         // result

  SDLx_CreateSurfaceARGB32 (rect.w, rect.h, &sfc);
  SDLx_LockSurface (sfc);
  sfc_cr = cairo_image_surface_create_for_data
    ((guchar *) sfc->pixels, CAIRO_FORMAT_ARGB32,
     sfc->w, sfc->h, sfc->pitch);

  cr = cairo_create (sfc_cr);
  g_assert_nonnull (cr);

  layout = pango_cairo_create_layout (cr);
  g_assert_nonnull (layout);

  pango_layout_set_text (layout, text.c_str (), -1);
  font = xstrbuild ("%s %s %s %s",
                    family.c_str (),
                    weight.c_str (),
                    style.c_str (),
                    size.c_str ());

  desc = pango_font_description_from_string (font.c_str ());
  g_assert_nonnull (desc);

  pango_layout_set_font_description (layout, desc);
  pango_font_description_free (desc);

  if(halign == "left")
    pango_layout_set_alignment (layout, PANGO_ALIGN_LEFT);
  else if(halign == "center")
    pango_layout_set_alignment (layout, PANGO_ALIGN_CENTER);
  else if(halign == "right")
    pango_layout_set_alignment (layout, PANGO_ALIGN_RIGHT);
  else if (halign == "justified")
    pango_layout_set_justify (layout, true);
  else
    ERROR ("bad horizontal alignment: %s", halign.c_str ());

  pango_layout_set_width (layout, rect.w * PANGO_SCALE);
  pango_layout_set_wrap (layout, PANGO_WRAP_WORD);
  pango_layout_get_size (layout, NULL, &height);

  cairo_set_source_rgba (cr, color.r/255, color.g/255, color.b/255,
                         color.a/255);
  pango_cairo_update_layout (cr, layout);

  if (valign == "bottom")
    align = rect.h - (height / PANGO_SCALE);
  else if (valign == "middle")
    align = (rect.h / 2) - ((height / PANGO_SCALE) / 2);
  else if (valign == "top")
    align = 0;
  else
    ERROR ("unknown vertical alignment: %s", valign.c_str ());

  cairo_move_to (cr, 0, align);
  pango_cairo_show_layout (cr, layout);

  g_object_unref (layout);
  cairo_destroy (cr);
  cairo_surface_destroy (sfc_cr);

  texture = SDL_CreateTextureFromSurface (renderer, sfc);
  g_assert_nonnull (texture);

  SDLx_UnlockSurface (sfc);
  SDL_FreeSurface(sfc);

  return texture;
}

TextPlayer::TextPlayer (const string &uri) : Player (uri)
{
  _fontColor = {0, 0, 0, 255};  // black
  _fontFamily = "helvetica";
  _fontSize = "18px";
  _fontStyle = "";
  _fontVariant = "";
  _fontWeight = "";
  _horzAlign = "left";
  _vertAlign = "top";
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
  else if (name == "horzAlign")
    {
      if (value != "" && value != "left" && value != "right"
          && value != "center" && value != "justified")
        {
          goto syntax_error;
        }
      _horzAlign = value;
    }
  else if (name == "vertAlign")
    {
      if (value != "" && value != "top"
          && value != "middle" && value != "bottom")
        {
          goto syntax_error;
        }
      _vertAlign = value;
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
  string text;

  path = _uri.c_str ();
  if (unlikely (!g_file_get_contents (path, &contents, NULL, &err)))
    {
      g_assert_nonnull (err);
      ERROR ("cannot load text file %s: %s", path, err->message);
      g_error_free (err);
    }
  g_assert_nonnull (contents);

  text = string (contents);
  g_free (contents);

  _texture = TextPlayer::renderTexture
    (renderer, text, _fontFamily, _fontWeight, _fontStyle, _fontSize,
     _fontColor, _rect, _horzAlign, _vertAlign);
  g_assert_nonnull (_texture);
}

GINGA_PLAYER_END
