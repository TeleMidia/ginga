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
  _uri = "";
  _rect.x = 0;
  _rect.y = 0;
  _rect.width = 0;
  _rect.height = 0;
  _eos = false;

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

string
Player::getURI ()
{
  return _uri;
}

void
Player::setURI (const string &uri)
{
  if (_uri == uri)
    return;                     // nothing to do

  _uri = uri;
  _flag.uri = true;
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
  if (_rect.x == x
      && _rect.y == y
      && _rect.width == width
      && _rect.height == height)
    {
      return;                   // nothing to do
    }
  _rect.x = x;
  _rect.y = y;
  _rect.width = width;
  _rect.height = height;
  _flag.rect = true;
}

bool
Player::getEOS ()
{
  return _eos;
}

void
Player::setEOS (bool eos)
{
  if (_eos == eos)
    return;                     // nothing to do

  _eos = eos;
  _flag.eos = true;
}

void
Player::start ()
{
  g_assert (_state != Player::PLAYING);
  _state = Player::PLAYING;
  if (_state != PAUSED)
    _eos = false;
  this->reload ();
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
  //this->resetProperties ();
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
  { "fontBgColor", { Player::PROP_FONT_BG_COLOR, true, "" } },
  { "fontColor", { Player::PROP_FONT_COLOR, true, "black" } },
  { "fontFamily", { Player::PROP_FONT_FAMILY, true, "sans" } },
  { "fontSize", { Player::PROP_FONT_SIZE, true, "12" } },
  { "fontStyle", { Player::PROP_FONT_STYLE, true, "" } },
  { "fontVariant", { Player::PROP_FONT_VARIANT, true, "" } },
  { "fontWeight", { Player::PROP_FONT_WEIGHT, true, "" } },
  { "freeze", { Player::PROP_FREEZE, true, "false" } },
  { "freq", { Player::PROP_FREQ, true, "440" } },
  { "horzAlign", { Player::PROP_HORZ_ALIGN, true, "left" } },
  { "mute", { Player::PROP_MUTE, false, "false" } },
  { "speed", { Player::PROP_SPEED, false, "1" } },
  { "time", { Player::PROP_TIME, false, "indefinite" } },
  { "transparency", { Player::PROP_TRANSPARENCY, true, "0%" } },
  { "treble", { Player::PROP_TREBLE, false, "0" } },
  { "vertAlign", { Player::PROP_VERT_ALIGN, true, "top" } },
  { "visible", { Player::PROP_VISIBLE, true, "true" } },
  { "volume", { Player::PROP_VOLUME, false, "100%" } },
  { "wave", { Player::PROP_WAVE, true, "sine" } },
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
  if (_state == Player::STOPPED)
    {
      return;                   // nothing to do
    }

  if (!_prop.visible || !(_rect.width > 0 && _rect.height > 0))
    {
      return;                   // nothing to do
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
      cairo_rectangle (cr, _rect.x, _rect.y, _rect.width,
                       _rect.height);
      cairo_fill (cr);
      cairo_restore (cr);
    }

  if (_surface != nullptr)
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
      cairo_paint_with_alpha (cr, _prop.alpha / 255.);
      cairo_restore (cr);
    }

  if (_media->isFocused ())
    {
      cairo_save (cr);
      cairo_set_source_rgba (cr, 1., 1., 0., 1.);
      cairo_set_antialias (cr, CAIRO_ANTIALIAS_NONE);
      cairo_rectangle (cr, _rect.x, _rect.y, _rect.width,
                       _rect.height);
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
    default:
      {
        break;
      }
    }
  return true;
}

GINGA_NAMESPACE_END
