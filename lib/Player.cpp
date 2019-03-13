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

// TODO --------------------------------------------------------------------


// Property table.
typedef struct PlayerPropertyInfo
{
  Player::Property code; // property code
  bool init;             // whether it should be initialized
  string defval;         // default value
} PlayerPropertyInfo;

static map<string, PlayerPropertyInfo> player_property_map = {
  { "background", { Player::PROP_BACKGROUND, true, "" } },
  { "balance", { Player::PROP_BALANCE, false, "0.0" } },
  { "bass", { Player::PROP_BASS, false, "0" } },
  { "bottom", { Player::PROP_BOTTOM, false, "0%" } },
  { "bounds", { Player::PROP_BOUNDS, false, "0%,0%,100%,100%" } },
  { "focusIndex", { Player::PROP_FOCUS_INDEX, true, "" } },
  { "fontBgColor", { Player::PROP_FONT_BG_COLOR, true, "" } },
  { "fontColor", { Player::PROP_FONT_COLOR, true, "black" } },
  { "fontFamily", { Player::PROP_FONT_FAMILY, true, "sans" } },
  { "fontSize", { Player::PROP_FONT_SIZE, true, "12" } },
  { "fontStyle", { Player::PROP_FONT_STYLE, true, "" } },
  { "fontVariant", { Player::PROP_FONT_VARIANT, true, "" } },
  { "fontWeight", { Player::PROP_FONT_WEIGHT, true, "" } },
  { "freeze", { Player::PROP_FREEZE, true, "false" } },
  { "freq", { Player::PROP_FREQ, true, "440" } },
  { "height", { Player::PROP_HEIGHT, true, "100%" } },
  { "horzAlign", { Player::PROP_HORZ_ALIGN, true, "left" } },
  { "left", { Player::PROP_LEFT, true, "0" } },
  { "location", { Player::PROP_LOCATION, false, "0,0" } },
  { "mute", { Player::PROP_MUTE, false, "false" } },
  { "right", { Player::PROP_RIGHT, false, "0%" } },
  { "size", { Player::PROP_SIZE, false, "100%,100%" } },
  { "speed", { Player::PROP_SPEED, false, "1" } },
  { "time", { Player::PROP_TIME, false, "indefinite" } },
  { "top", { Player::PROP_TOP, true, "0" } },
  { "transparency", { Player::PROP_TRANSPARENCY, true, "0%" } },
  { "treble", { Player::PROP_TREBLE, false, "0" } },
  { "vertAlign", { Player::PROP_VERT_ALIGN, true, "top" } },
  { "visible", { Player::PROP_VISIBLE, true, "true" } },
  { "volume", { Player::PROP_VOLUME, false, "100%" } },
  { "wave", { Player::PROP_WAVE, true, "sine" } },
  { "width", { Player::PROP_WIDTH, true, "100%" } },
  { "uri", { Player::PROP_URI, true, "" } },
  { "type", { Player::PROP_TYPE, true, "application/x-ginga-timer" } },
};

static map<string, string> player_property_aliases = {
  { "backgroundColor", "background" },
  { "balanceLevel", "balance" },
  { "bassLevel", "bass" },
  { "soundLevel", "volume" },
  { "rate", "speed" },
  { "trebleLevel", "treble" },
};

// Public.

Player::Player (Media *media)
{
  g_return_if_fail (media != NULL);

  _L = media->getDocument ()->getLuaState ();
  _media = media;
  _eos = false;

  _state = SLEEPING;
  _dirty = true;
  _surface = nullptr;
  this->resetProperties ();

  LuaAPI::Player_attachWrapper (_L, this, media);
}

Player::~Player ()
{
  if (_surface != nullptr)
    cairo_surface_destroy (_surface);
  _properties.clear ();

  LuaAPI::Player_detachWrapper (_L, this);
}

Player::State
Player::getState ()
{
  return _state;
}

void
Player::start ()
{
  g_assert (_state != OCCURRING);
  _state = OCCURRING;
  _eos = false;
  this->reload ();
}

void
Player::stop ()
{
  g_assert (_state != SLEEPING);
  _state = SLEEPING;
  this->resetProperties ();
}

void
Player::pause ()
{
  g_assert (_state != PAUSED && _state != SLEEPING);
  _state = PAUSED;
}

void
Player::resume ()
{
  g_assert (_state == PAUSED);
  _state = OCCURRING;
}

string
Player::getProperty (string const &name)
{
  return (_properties.count (name) != 0) ? _properties[name] : "";
}

void
Player::setProperty (const string &name, const string &value)
{
  Player::Property code;
  bool use_defval;
  string defval;
  string _value;

  use_defval = false;
  _value = value;

  code = Player::getPlayerProperty (name, &defval);
  if (code == Player::PROP_UNKNOWN)
    {
      TRACE ("unknown property: %s", name.c_str ());
      goto done;
    }

  if (_value == "")
    {
      use_defval = true;
      _value = defval;
    }

  if (unlikely (!this->doSetProperty (code, name, _value)))
    {
      ERROR ("property '%s': bad value '%s'", name.c_str (),
             _value.c_str ());
    }

  if (use_defval) // restore value
    _value = "";

done:
  _properties[name] = _value;
  return;
}

void
Player::resetProperties ()
{
  for (auto it : player_property_map)
    if (it.second.init)
      this->setProperty (it.first, "");
  _properties.clear ();
}

void
Player::resetProperties (set<string> *props)
{
  for (auto name : *props)
    this->setProperty (name, "");
}

void
Player::reload ()
{
  _dirty = false;
}

void
Player::redraw (cairo_t *cr)
{
  if (_state == SLEEPING)
    return;                     // nothing to do

  if (!_prop.visible || !(_prop.rect.width > 0 && _prop.rect.height > 0))
    {
      return; // nothing to do
    }

  if (_dirty)
    {
      this->reload ();
    }

  if (_prop.bgColor.alpha > 0)
    {
      cairo_save (cr);
      cairo_set_source_rgba (cr, _prop.bgColor.red, _prop.bgColor.green,
                             _prop.bgColor.blue, _prop.alpha / 255.);
      cairo_rectangle (cr, _prop.rect.x, _prop.rect.y, _prop.rect.width,
                       _prop.rect.height);
      cairo_fill (cr);
      cairo_restore (cr);
    }

  if (_surface != nullptr)
    {
      double sx, sy;
      sx = (double) _prop.rect.width
        / cairo_image_surface_get_width (_surface);
      sy = (double) _prop.rect.height
        / cairo_image_surface_get_height (_surface);
      cairo_save (cr);
      cairo_translate (cr, _prop.rect.x, _prop.rect.y);
      cairo_scale (cr, sx, sy);
      cairo_set_source_surface (cr, _surface, 0., 0.);
      cairo_paint_with_alpha (cr, _prop.alpha / 255.);
      cairo_restore (cr);
    }

  if (_media->isFocused ())
    {
      cairo_save (cr);
      cairo_set_source_rgba (cr, 1., 1., 0., 1.);
      cairo_set_antialias (cr, CAIRO_ANTIALIAS_NONE);
      cairo_rectangle (cr, _prop.rect.x, _prop.rect.y, _prop.rect.width,
                       _prop.rect.height);
      cairo_stroke (cr);
      cairo_restore (cr);
    }

  cairo_save (cr);
  cairo_set_operator (cr, CAIRO_OPERATOR_CLEAR);
  cairo_set_source_rgba (cr, 1., 1., 1., 1.);

  for (auto it = _crop.begin (); it != _crop.end (); ++it)
    {
      bool fdot = false;
      if (it == _crop.begin ())
        fdot = true;

      int x = *it;
      advance (it, 1);
      int y = *it;

      if (fdot)
        cairo_move_to (cr, x, y);
      else
        cairo_line_to (cr, x, y);
    }

  cairo_close_path (cr);
  cairo_stroke_preserve (cr);
  cairo_fill (cr);
  cairo_set_operator (cr, CAIRO_OPERATOR_OVER);
  cairo_restore (cr);
}

void
Player::sendKeyEvent (unused (const string &key), unused (bool press))
{
}

Player::Property
Player::getPlayerProperty (const string &name, string *defval)
{
  map<string, PlayerPropertyInfo>::iterator it;
  PlayerPropertyInfo *info;
  string _name = name;

  if ((it = player_property_map.find (_name)) == player_property_map.end ())
    {
      map<string, string>::iterator italias;
      if ((italias = player_property_aliases.find (_name))
          == player_property_aliases.end ())
        {
          tryset (defval, "");
          return PROP_UNKNOWN;
        }
      _name = italias->second;
      it = player_property_map.find (_name);
      g_assert (it != player_property_map.end ());
    }
  info = &it->second;
  tryset (defval, info->defval);
  return info->code;
}

bool
Player::doSetProperty (Property code, unused (const string &name),
                       const string &value)
{
  switch (code)
    {
    case PROP_BOUNDS:
      {
        list<string> lst;
        if (unlikely (!ginga::try_parse_list (value, ',', 4, 4, &lst)))
          return false;
        auto it = lst.begin ();
        _media->setPropertyString ("left", *it++);
        _media->setPropertyString ("top", *it++);
        _media->setPropertyString ("width", *it++);
        _media->setPropertyString ("height", *it++);
        g_assert (it == lst.end ());
        break;
      }
    case PROP_FOCUS_INDEX:
      {
        _prop.focusIndex = value;
        break;
      }
    case PROP_LOCATION:
      {
        list<string> lst;
        if (unlikely (!ginga::try_parse_list (value, ',', 2, 2, &lst)))
          return false;
        auto it = lst.begin ();
        _media->setPropertyString ("left", *it++);
        _media->setPropertyString ("top", *it++);
        g_assert (it == lst.end ());
        break;
      }
    case PROP_SIZE:
      {
        list<string> lst;
        if (unlikely (!ginga::try_parse_list (value, ',', 2, 2, &lst)))
          return false;
        auto it = lst.begin ();
        _media->setPropertyString ("width", *it++);
        _media->setPropertyString ("height", *it++);
        g_assert (it == lst.end ());
        break;
      }
    case PROP_LEFT:
      {
        lua_Integer width;
        g_assert (_media->getDocument ()->getSettings ()
                  ->getPropertyInteger ("width", &width));
        _prop.rect.x = ginga::parse_percent (value, (int) width, 0, G_MAXINT);
        _dirty = true;
        break;
      }
    case PROP_RIGHT:
      {
        lua_Integer width;
        g_assert (_media->getDocument ()->getSettings ()
                  ->getPropertyInteger ("width", &width));
        _prop.rect.x = (int) width - _prop.rect.width
          - ginga::parse_percent (value, (int) width, 0, G_MAXINT);
        _dirty = true;
        break;
      }
    case PROP_TOP:
      {
        lua_Integer height;
        g_assert (_media->getDocument ()->getSettings ()
                  ->getPropertyInteger ("height", &height));
        _prop.rect.y = ginga::parse_percent
          (value, (int) height, 0, G_MAXINT);
        _dirty = true;
        break;
      }
    case PROP_BOTTOM:
      {
        lua_Integer height;
        g_assert (_media->getDocument ()->getSettings ()
                  ->getPropertyInteger ("height", &height));
        _prop.rect.y = (int) height - _prop.rect.height
                       - ginga::parse_percent (value, _prop.rect.height, 0,
                                               G_MAXINT);
        _dirty = true;
        break;
      }
    case PROP_WIDTH:
      {
        lua_Integer width;
        g_assert (_media->getDocument ()->getSettings ()
                  ->getPropertyInteger ("width", &width));

        _prop.rect.width = ginga::parse_percent
          (value, (int) width, 0, G_MAXINT);
        _dirty = true;

        string right;
        if (_media->getPropertyString ("right", &right))
          _media->setPropertyString ("right", right);
        break;
      }
    case PROP_HEIGHT:
      {
        lua_Integer height;
        g_assert (_media->getDocument ()->getSettings ()
                  ->getPropertyInteger ("height", &height));
        _prop.rect.height
          = ginga::parse_percent (value, (int) height, 0, G_MAXINT);
        _dirty = true;

        string bottom;
        if (_media->getPropertyString ("bottom", &bottom))
          _media->setPropertyString ("bottom", bottom);

        break;
      }
    case PROP_Z_INDEX:
      {
        _prop.zindex = xstrtoint (value, 10);
        break;
      }
    case PROP_Z_ORDER:
      {
        _prop.zorder = xstrtoint (value, 10);
        break;
      }
    case PROP_TRANSPARENCY:
      {
        _prop.alpha
            = (guint8) CLAMP (255 - ginga::parse_pixel (value), 0, 255);
        break;
      }
    case PROP_BACKGROUND:
      {
        if (value == "")
          _prop.bgColor = { 0, 0, 0, 0 };
        else
          _prop.bgColor = ginga::parse_color (value);
        break;
      }
    case PROP_VISIBLE:
      {
        _prop.visible = ginga::parse_bool (value);
        break;
      }
    case PROP_URI:
      {
        _prop.uri = value;
        _dirty = true;
        break;
      }
    case PROP_TYPE:
      {
        _prop.type = value;
        break;
      }
    default:
      {
        break;
      }
    }
  return true;
}

GINGA_NAMESPACE_END
