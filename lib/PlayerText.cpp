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
#include "aux-gl.h"
#include "PlayerText.h"

GINGA_NAMESPACE_BEGIN

// Public: Static.

/**
 * @brief Creates surface from text.
 * @param text Text to be rendered.
 * @param family Font family.
 * @param weight Font weight ("normal" or "bold").
 * @param style Font style ("normal" or "italic").
 * @param size Font size
 * @param fg Text color.
 * @param bg Background color.
 * @param rect Dimensions of the resulting surface.
 * @param halign Horizontal alignment
 *        ("left", "center", "right", or "justified").
 * @param valign Vertical alignment ("bottom", "middle", or "top").
 * @param antialias Whether to use antialias.
 * @param ink Variable to store the inked rectangle.
 * @return The resulting surface.
 */
cairo_surface_t *
PlayerText::renderSurface (const string &text, const string &family,
                           const string &weight, const string &style,
                           const string &size, Color fg, Color bg,
                           Rect rect, const string &halign,
                           const string &valign, bool antialias, Rect *ink)
{
  cairo_t *cr;
  cairo_surface_t *sfc; // result

  PangoLayout *layout;
  cairo_font_options_t *opts;
  string font;
  PangoFontDescription *desc;
  double align;
  int height;
  PangoRectangle r;

  g_assert_cmpint (rect.width, >, 0);
  g_assert_cmpint (rect.height, >, 0);

  sfc = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, rect.width,
                                    rect.height);
  g_assert_nonnull (sfc);

  cr = cairo_create (sfc);
  g_assert_nonnull (cr);

  layout = pango_cairo_create_layout (cr);
  g_assert_nonnull (layout);

  opts = cairo_font_options_create ();
  if (!antialias)
    cairo_font_options_set_antialias (opts, CAIRO_ANTIALIAS_NONE);

  pango_cairo_context_set_font_options (pango_layout_get_context (layout),
                                        opts);
  cairo_font_options_destroy (opts);

  pango_layout_set_text (layout, text.c_str (), -1);
  font = xstrbuild ("%s %s %s %s", family.c_str (), weight.c_str (),
                    style.c_str (), size.c_str ());

  desc = pango_font_description_from_string (font.c_str ());
  g_assert_nonnull (desc);

  pango_layout_set_font_description (layout, desc);
  pango_font_description_free (desc);

  if (halign == "" || halign == "left")
    pango_layout_set_alignment (layout, PANGO_ALIGN_LEFT);
  else if (halign == "center")
    pango_layout_set_alignment (layout, PANGO_ALIGN_CENTER);
  else if (halign == "right")
    pango_layout_set_alignment (layout, PANGO_ALIGN_RIGHT);
  else if (halign == "justified")
    pango_layout_set_justify (layout, true);
  else
    ERROR ("bad horizontal alignment: %s", halign.c_str ());

  pango_layout_set_width (layout, rect.width * PANGO_SCALE);
  pango_layout_set_wrap (layout, PANGO_WRAP_WORD);
  pango_layout_get_size (layout, NULL, &height);

  cairo_set_source_rgba (cr, fg.red, fg.green, fg.blue, fg.alpha);
  pango_cairo_update_layout (cr, layout);

  if (valign == "bottom")
    align = rect.height - (height / PANGO_SCALE);
  else if (valign == "middle")
    align = (rect.height / 2) - ((height / PANGO_SCALE) / 2);
  else if (valign == "" || valign == "top")
    align = 0;
  else
    ERROR ("unknown vertical alignment: %s", valign.c_str ());

  pango_layout_get_pixel_extents (layout, &r, nullptr);
  if (ink != nullptr)
    {
      ink->x = r.x;
      ink->y = r.y;
      ink->width = r.width;
      ink->height = r.height;
    }

  if (bg.alpha > 0.)
    {
      cairo_save (cr);
      cairo_set_source_rgba (cr, bg.red, bg.green, bg.blue, bg.alpha);
      r.x -= 2;
      r.y -= 2;
      r.width += 4;
      r.height += 4;
      cairo_rectangle (cr, r.x, r.y + align, r.width, r.height);
      cairo_fill (cr);
      cairo_restore (cr);
    }

  cairo_move_to (cr, 0, align);
  pango_cairo_show_layout (cr, layout);

  g_object_unref (layout);
  cairo_destroy (cr);

  return sfc;
}

// Public.

PlayerText::PlayerText (Formatter *formatter, Media *media)
    : Player (formatter, media)
{
  // Initialize handled properties.
  static set<string> handled = {
    "fontColor",   "bgColor",    "fontFamily", "fontSize",  "fontStyle",
    "fontVariant", "fontWeight", "horzAlign",  "vertAlign",
  };
  this->resetProperties (&handled);
}

PlayerText::~PlayerText ()
{
}

void
PlayerText::reload ()
{
  string text;

  if (unlikely (!xurigetcontents (Player::_prop.uri, text)))
    {
      ERROR ("cannot load text file %s", Player::_prop.uri.c_str ());
    }

  if (_surface != nullptr)
    {
      cairo_surface_destroy (_surface);
      if (_opengl)
        GL::delete_texture (&_gltexture);
    }

  _surface = PlayerText::renderSurface (
      text, _prop.fontFamily, _prop.fontWeight, _prop.fontStyle,
      _prop.fontSize, _prop.fontColor, _prop.fontBgColor,
      Player::_prop.rect, _prop.horzAlign, _prop.vertAlign, true, nullptr);

  g_assert_nonnull (_surface);

  if (_opengl)
    GL::create_texture (&_gltexture,
                        cairo_image_surface_get_width (_surface),
                        cairo_image_surface_get_height (_surface),
                        cairo_image_surface_get_data (_surface));

  Player::reload ();
}

// Protected.

bool
PlayerText::doSetProperty (Property code, unused (const string &name),
                           const string &value)
{
  switch (code)
    {
    case PROP_FONT_COLOR:
      if (unlikely (!ginga::try_parse_color (value, &_prop.fontColor)))
        return false;
      _dirty = true;
      break;
    case PROP_FONT_BG_COLOR:
      if (unlikely (!ginga::try_parse_color (value, &_prop.fontBgColor)))
        return false;
      _dirty = true;
      break;
    case PROP_FONT_FAMILY:
      _prop.fontFamily = value;
      _dirty = true;
      break;
    case PROP_FONT_SIZE:
      _prop.fontSize = value;
      _dirty = true;
      break;
    case PROP_FONT_STYLE:
      if (unlikely (value != "" && value != "normal" && value != "italic"))
        return false;
      _prop.fontStyle = value;
      _dirty = true;
      break;
    case PROP_FONT_VARIANT:
      if (unlikely (value != "" && value != "small-caps"))
        return false;
      _prop.fontVariant = value;
      _dirty = true;
      break;
    case PROP_FONT_WEIGHT:
      if (unlikely (value != "" && value != "normal" && value != "bold"))
        return false;
      _prop.fontWeight = value;
      _dirty = true;
      break;
    case PROP_HORZ_ALIGN:
      if (unlikely (value != "" && value != "left" && value != "right"
                    && value != "center" && value != "justified"))
        {
          return false;
        }
      _prop.horzAlign = value;
      _dirty = true;
      break;
    case PROP_VERT_ALIGN:
      if (unlikely (value != "" && value != "top" && value != "middle"
                    && value != "bottom"))
        {
          return false;
        }
      _prop.vertAlign = value;
      _dirty = true;
      break;
    default:
      return Player::doSetProperty (code, name, value);
    }
  return true;
}

GINGA_NAMESPACE_END
