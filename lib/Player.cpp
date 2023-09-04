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

#include "Player.h"
#include "Media.h"

#include "PlayerImage.h"
#include "PlayerText.h"
#include "PlayerVideo.h"
#include "PlayerSigGen.h"
#include "PlayerRemote.h"

#include "PlayerLua.h"

#include "PlayerSvg.h"

#if defined WITH_CEF && WITH_CEF
#include "PlayerHTML.h"
#endif

namespace ginga {

// Mime-type table.
static map<string, string> mime_table = {
  { "ac3", "audio/ac3" },
  { "avi", "video/x-msvideo" },
  { "avif", "image/avif" },
  { "bmp", "image/bmp" },
  { "bpg", "image/x-bpg" },
  { "class", "application/x-ginga-NCLet" },
  { "css", "text/css" },
  { "gif", "image/gif" },
  { "htm", "text/html" },
  { "html", "text/html" },
  { "heic", "image/heic"},
  { "heif", "image/heic"},
  { "jpeg", "image/jpeg" },
  { "jpg", "image/jpeg" },
  { "lua", "application/x-ginga-NCLua" },
  { "mov", "video/quicktime" },
  { "mp2", "audio/mp2" },
  { "mp3", "audio/mp3" },
  { "mp4", "video/mp4" },
  { "mpa", "audio/mpa" },
  { "mpeg", "video/mpeg" },
  { "mpg", "video/mpeg" },
  { "mpv", "video/mpv" },
  { "ncl360", REMOTE_PLAYER_MIME_NCL360 },
  { "ncl", "application/x-ginga-ncl" },
  { "oga", "audio/ogg" },
  { "ogg", "audio/ogg" },
  { "ogv", "video/ogg" },
  { "opus", "audio/ogg" },
  { "png", "image/png" },
  { "smil", "application/smil" },
  { "spx", "audio/ogg" },
  { "srt", "text/srt" },
  { "ssml", "application/ssml+xml" },
  { "svg", "image/svg+xml" },
  { "svgz", "image/svg+xml" },
  { "ts", "video/mpeg" },
  { "txt", "text/plain" },
  { "wav", "audio/basic" },
  { "webp", "image/x-webp" },
  { "wmv", "video/x-ms-wmv" },
  { "xlet", "application/x-ginga-NCLet" },
  { "xlt", "application/x-ginga-NCLet" },
  { "xml", "text/xml" },
};

static bool
mime_table_index (const string &key, string *result)
{
  map<string, string>::iterator it;
  if ((it = mime_table.find (key)) == mime_table.end ())
    return false;
  tryset (result, it->second);
  return true;
}

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
  { "debug", { Player::PROP_DEBUG, true, "false" } },
  { "duration", { Player::PROP_DURATION, true, "indefinite" } },
  { "focusIndex", { Player::PROP_FOCUS_INDEX, true, "" } },
  { "focusBorderColor", { Player::PROP_FOCUS_BORDER_COLOR, true, "yellow" } },
  { "focusBorderWidth", { Player::PROP_FOCUS_BORDER_WIDTH, true, "4" } },
  { "focusBorderTransparency", { Player::PROP_FOCUS_BORDER_TRANSPARENCY, true, "0%" } },
  { "selBorderColor", { Player::PROP_SEL_BORDER_COLOR, true, "yellow" } },
  { "fontBgColor", { Player::PROP_FONT_BG_COLOR, true, "" } },
  { "fontColor", { Player::PROP_FONT_COLOR, true, "black" } },
  { "fontFamily", { Player::PROP_FONT_FAMILY, true, "sans" } },
  { "fontSize", { Player::PROP_FONT_SIZE, true, "12" } },
  { "fontStyle", { Player::PROP_FONT_STYLE, true, "" } },
  { "fontVariant", { Player::PROP_FONT_VARIANT, true, "" } },
  { "fontWeight", { Player::PROP_FONT_WEIGHT, true, "" } },
  { "freeze", { Player::PROP_FREEZE, true, "false" } },
  { "freq", { Player::PROP_FREQ, true, "440" } },
  { "remotePlayerBaseURL",
    { Player::PROP_REMOTE_PLAYER_BASE_URL, false, "" } },
  { "height", { Player::PROP_HEIGHT, true, "100%" } },
  { "horzAlign", { Player::PROP_HORZ_ALIGN, true, "left" } },
  { "left", { Player::PROP_LEFT, true, "0" } },
  { "location", { Player::PROP_LOCATION, false, "0,0" } },
  { "mute", { Player::PROP_MUTE, false, "false" } },
  { "right", { Player::PROP_RIGHT, false, "0%" } },
  { "size", { Player::PROP_SIZE, false, "100%,100%" } },
  { "speed", { Player::PROP_SPEED, false, "1" } },
  { "currentTime", { Player::PROP_TIME, false, "indefinite" } },
  { "time", { Player::PROP_TIME, false, "indefinite" } },
  { "top", { Player::PROP_TOP, true, "0" } },
  { "transparency", { Player::PROP_TRANSPARENCY, true, "0%" } },
  { "treble", { Player::PROP_TREBLE, false, "0" } },
  { "vertAlign", { Player::PROP_VERT_ALIGN, true, "top" } },
  { "visible", { Player::PROP_VISIBLE, true, "true" } },
  { "volume", { Player::PROP_VOLUME, false, "100%" } },
  { "wave", { Player::PROP_WAVE, true, "sine" } },
  { "width", { Player::PROP_WIDTH, true, "100%" } },
  { "zIndex", { Player::PROP_Z_INDEX, true, "0" } },
  { "zOrder", { Player::PROP_Z_ORDER, true, "0" } },
  { "uri", { Player::PROP_URI, true, "" } },
  { "type", { Player::PROP_TYPE, true, "application/x-ginga-timer" } },
};

static map<string, string> player_property_aliases = {
  { "backgroundColor", "background" },
  { "balanceLevel", "balance" },
  { "bassLevel", "bass" },
  { "explicitDur", "duration" },
  { "soundLevel", "volume" },
  { "rate", "speed" },
  { "trebleLevel", "treble" },
};

// Public.

Player::Player (Formatter *formatter, Media *media)
{
  g_assert_nonnull (formatter);
  _formatter = formatter;

  g_assert_nonnull (media);
  _media = media;
  _id = media->getId ();

  _state = SLEEPING;
  _time = 0;
  _eos = false;
  _dirty = true;
  _animator = new PlayerAnimator (_formatter, &_time);
  _surface = nullptr;
  _opengl = _formatter->getOptionBool ("opengl");
  _gltexture = 0;
  this->resetProperties ();
}

Player::~Player ()
{
  delete _animator;
  if (_surface != nullptr)
    cairo_surface_destroy (_surface);
  if (_gltexture)
    GL::delete_texture (&_gltexture);
  _properties.clear ();
}

Player::State
Player::getState ()
{
  return _state;
}

void
Player::getZ (int *zindex, int *zorder)
{
  tryset (zindex, _prop.zindex);
  tryset (zorder, _prop.zorder);
}

bool
Player::isFocused ()
{
  return _prop.focusIndex != "" && _prop.focusIndex == _currentFocus;
}

Time
Player::getTime ()
{
  return _time;
}

void
Player::incTime (Time inc)
{
  _time += inc;
}

Time
Player::getDuration ()
{
  return _prop.duration;
}

void
Player::setDuration (Time duration)
{
  _prop.duration = duration;
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
  g_assert (_state != OCCURRING);
  _state = OCCURRING;
  _time = 0;
  _eos = false;
  this->reload ();
  _animator->scheduleTransition ("start", &_prop.rect, &_prop.bgColor,
                                 &_prop.alpha, &_crop);
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

  if (name == "transIn" || name == "transOut")
    _animator->setTransitionProperties (name, value);

  use_defval = false;
  _value = value;

  code = Player::getPlayerProperty (name, &defval);

  // NCLua media should perform the doSetProperty to trigger registred funcs
  if (code == Player::PROP_UNKNOWN
      && this->getProperty ("type") != "application/x-ginga-NCLua")
    goto done;

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
Player::schedulePropertyAnimation (const string &name, const string &from,
                                   const string &to, Time dur)
{
  _animator->schedule (name, from, to, dur);
}

void
Player::reload ()
{
  _dirty = false;
}

void
Player::redraw (cairo_t *cr)
{
  g_assert (_state != SLEEPING);
  _animator->update (&_prop.rect, &_prop.bgColor, &_prop.alpha, &_crop);

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
      if (_opengl)
        {
          GL::draw_quad (_prop.rect.x, _prop.rect.y, _prop.rect.width,
                         _prop.rect.height, (GLfloat) _prop.bgColor.red,
                         (GLfloat) _prop.bgColor.green,
                         (GLfloat) _prop.bgColor.blue,
                         (GLfloat) (_prop.alpha / 255.));
        }
      else
        {
          cairo_save (cr);
          cairo_set_source_rgba (cr, _prop.bgColor.red, _prop.bgColor.green,
                                 _prop.bgColor.blue, _prop.alpha / 255.);

          cairo_rectangle (cr, _prop.rect.x, _prop.rect.y, _prop.rect.width,
                           _prop.rect.height);
          cairo_fill (cr);
          cairo_restore (cr);
        }
    }

  if (_opengl)
    {
      if (_gltexture)
        {
          GL::draw_quad (_prop.rect.x, _prop.rect.y, _prop.rect.width,
                         _prop.rect.height, _gltexture,
                         (GLfloat) (_prop.alpha / 255.));
        }
    }
  else
    {
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
    }

  if (this->isFocused ())
    {
      if (_opengl)
        {
          // TODO.
        }
      else
        {
          cairo_save (cr);
          cairo_set_source_rgba (cr, _prop.focusBorderColor.red, _prop.focusBorderColor.green, _prop.focusBorderColor.blue, _prop.focusBorderTransparency / 255.);
          cairo_set_antialias (cr, CAIRO_ANTIALIAS_NONE);
          cairo_rectangle (cr, _prop.rect.x, _prop.rect.y, _prop.rect.width,
                           _prop.rect.height);
          cairo_stroke (cr);
          cairo_restore (cr);
        }
    }

  if (_opengl)
    {
      // TODO.
    }
  else
    {
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

  if (_prop.debug || _formatter->getOptionBool ("debug"))
    {
      this->redrawDebuggingInfo (cr);
    }
}

void Player::sendKeyEvent (unused (const string &key), unused (bool press))
{
}

// Public: Static.

// Current focus index value.
string Player::_currentFocus = "";

string
Player::getCurrentFocus ()
{
  return _currentFocus;
}

void
Player::setCurrentFocus (const string &index)
{
  _currentFocus = index;
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

bool Player::getMimeForURI (const string &uri, string *result)
{
  string::size_type index, len;
  index = uri.find_last_of (".");
  if (index != std::string::npos)
    {
      index++;
      len = uri.length ();
      if (index < len)
        {
          string extension = uri.substr (index, (len - index));
          if (extension != "")
            return mime_table_index (extension, result);
        }
    }
  return false;
}

Player *
Player::createPlayer (Formatter *formatter, Media *media, const string &uri,
                      const string &type)
{
  Player *player;
  string mime;

  g_assert_nonnull (formatter);
  player = nullptr;
  mime = type;

  if (mime == "" && uri != "")
    {
      getMimeForURI (uri, &mime);
    }

  if (mime == "")
    mime = "application/x-ginga-timer";

  if (mime == "application/x-ginga-ncl")
    {
      ERROR_NOT_IMPLEMENTED ("NCL as Media object is not supported");
    }
  else if (xstrhasprefix (mime, "audio") || xstrhasprefix (mime, "video"))
    {
      player = new PlayerVideo (formatter, media);
    }
  else if (mime == "application/x-ginga-siggen")
    {
      player = new PlayerSigGen (formatter, media);
    }
  else if (xstrhasprefix (mime, "image"))
    {
      player = new PlayerImage (formatter, media);
    }
  else if (mime == "text/plain")
    {
      player = new PlayerText (formatter, media);
    }
  // if has WS setted a remotePlayerBaseURL
  else if (PlayerRemote::usesPlayerRemote (media))
    {
      player = new PlayerRemote (formatter, media);
      WARNING ("Create a PlayerRemote for Media '%s'",
               media->getId ().c_str ());
    }
#if defined WITH_CEF && WITH_CEF
  else if (xstrhasprefix (mime, "text/html"))
    {
      player = new PlayerHTML (formatter, media);
    }
#endif // WITH_CEF
  else if (xstrhasprefix (mime, "image/svg"))
    {
      player = new PlayerSvg (formatter, media);
    }
  else if (mime == "application/x-ginga-NCLua")
    {
      player = new PlayerLua (formatter, media);
    }
  else
    {
      player = new Player (formatter, media);
      if (unlikely (mime != "application/x-ginga-timer" && uri != ""))
        {
          WARNING ("unknown mime '%s': creating an empty player",
                   mime.c_str ());
          if (!media->getProperty ("uri").empty ())
            ERROR ("media from \"application/x-ginga-timer\" type should "
                   "not have src");
        }
    }

  g_assert_nonnull (player);
  media->setProperty ("type", mime);
  return player;
}

// Protected.

bool
Player::doSetProperty (Property code, unused (const string &name),
                       const string &value)
{
  switch (code)
    {
    case PROP_DEBUG:
      {
        _prop.debug = ginga::parse_bool (value);
        break;
      }
    case PROP_BOUNDS:
      {
        list<string> lst;
        if (unlikely (!ginga::try_parse_list (value, ',', 4, 4, &lst)))
          return false;
        auto it = lst.begin ();
        _media->setProperty ("left", *it++);
        _media->setProperty ("top", *it++);
        _media->setProperty ("width", *it++);
        _media->setProperty ("height", *it++);
        g_assert (it == lst.end ());
        break;
      }
    case PROP_FOCUS_INDEX:
      {
        _prop.focusIndex = value;
        break;
      }
    case PROP_FOCUS_BORDER_COLOR:
      {
        _prop.focusBorderColor = ginga::parse_color (value);
        break;
      }
    case PROP_FOCUS_BORDER_WIDTH:
      {
        _prop.focusBorderWidth = xstrtoint (value, 10);
        break;
      }
    case PROP_FOCUS_BORDER_TRANSPARENCY:
      {
        _prop.focusBorderTransparency = (guint8) CLAMP (255 - ginga::parse_pixel (value), 0, 255);
        break;
      }
    case PROP_SEL_BORDER_COLOR:
      {
        _prop.selBorderColor = ginga::parse_color (value);
        break;
      }
    case PROP_LOCATION:
      {
        list<string> lst;
        if (unlikely (!ginga::try_parse_list (value, ',', 2, 2, &lst)))
          return false;
        auto it = lst.begin ();
        _media->setProperty ("left", *it++);
        _media->setProperty ("top", *it++);
        g_assert (it == lst.end ());
        break;
      }
    case PROP_SIZE:
      {
        list<string> lst;
        if (unlikely (!ginga::try_parse_list (value, ',', 2, 2, &lst)))
          return false;
        auto it = lst.begin ();
        _media->setProperty ("width", *it++);
        _media->setProperty ("height", *it++);
        g_assert (it == lst.end ());
        break;
      }
    case PROP_LEFT:
      {
        int width = _formatter->getOptionInt ("width");
        _prop.rect.x = ginga::parse_percent (value, width, 0, G_MAXINT);
        _dirty = true;
        break;
      }
    case PROP_RIGHT:
      {
        int width = _formatter->getOptionInt ("width");
        _prop.rect.x = width - _prop.rect.width
                       - ginga::parse_percent (value, width, 0, G_MAXINT);
        _dirty = true;
        break;
      }
    case PROP_TOP:
      {
        int height = _formatter->getOptionInt ("height");
        _prop.rect.y = ginga::parse_percent (value, height, 0, G_MAXINT);
        _dirty = true;
        break;
      }
    case PROP_BOTTOM:
      {
        int height = _formatter->getOptionInt ("height");
        _prop.rect.y = height - _prop.rect.height
                       - ginga::parse_percent (value, _prop.rect.height, 0,
                                               G_MAXINT);
        _dirty = true;
        break;
      }
    case PROP_WIDTH:
      {
        int width = _formatter->getOptionInt ("width");
        _prop.rect.width = ginga::parse_percent (value, width, 0, G_MAXINT);
        _dirty = true;

        string right = _media->getProperty ("right");
        if (right != "")
          _media->setProperty ("right", right);
        break;
      }
    case PROP_HEIGHT:
      {
        int height = _formatter->getOptionInt ("height");
        _prop.rect.height
            = ginga::parse_percent (value, height, 0, G_MAXINT);
        _dirty = true;

        string bottom = _media->getProperty ("bottom");
        if (bottom != "")
          _media->setProperty ("bottom", bottom);

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
    case PROP_DURATION:
      {
        if (value == "indefinite")
          _prop.duration = GINGA_TIME_NONE;
        else
          _prop.duration = ginga::parse_time (value);
        break;
      }
    case PROP_URI:
      {
        _prop.uri = value;
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

// Private.

void
Player::redrawDebuggingInfo (cairo_t *cr)
{
  cairo_surface_t *debug;
  string id;
  string str;
  double sx, sy;

  id = _id;
  if (id.find ("/") != std::string::npos)
    {
      id = xpathdirname (id);
      if (id.find ("/") != std::string::npos)
        id = xpathbasename (id);
    }

  // Draw info.
  str = xstrbuild ("%s:%.1fs\n%dx%d:(%d,%d):%d", id.c_str (),
                   ((double) GINGA_TIME_AS_MSECONDS (_time)) / 1000.,
                   _prop.rect.width, _prop.rect.height, _prop.rect.x,
                   _prop.rect.y, _prop.zindex);

  debug = PlayerText::renderSurface (
      str, "monospace", "", "", "7", { 1., 0, 0, 1. }, { 0, 0, 0, .75 },
      _prop.rect, "center", "middle", true, nullptr);
  g_assert_nonnull (debug);

  sx = (double) _prop.rect.width / cairo_image_surface_get_width (debug);
  sy = (double) _prop.rect.height / cairo_image_surface_get_height (debug);

  cairo_save (cr);
  cairo_translate (cr, _prop.rect.x, _prop.rect.y);
  cairo_scale (cr, sx, sy);
  cairo_set_source_surface (cr, debug, 0., 0.);
  cairo_paint (cr);
  cairo_restore (cr);

  cairo_surface_destroy (debug);
}

}
