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
#include "Player.h"
#include "player/ImagePlayer.h"
#include "player/LuaPlayer.h"
#include "player/TextPlayer.h"
#include "player/VideoPlayer.h"
#if defined WITH_LIBRSVG && WITH_LIBRSVG
# include "player/SvgPlayer.h"
#endif
#if defined WITH_CEF && WITH_CEF
# include "player/HTMLPlayer.h"
#endif

GINGA_PLAYER_BEGIN

typedef struct PlayerPropertyInfo
{
  Player::PlayerProperty code;  // property code
  bool init;                    // whether it should be initialized
  string defval;                // default value
} PlayerPropertyInfo;

static map<string, PlayerPropertyInfo> player_property_map =
{
 {"background",   {Player::PROP_BACKGROUND,    true,  ""}},
 {"balance",      {Player::PROP_BALANCE,       true,  "0.0"}},
 {"bass",         {Player::PROP_BASS,          false,  "100%"}},
 {"bottom",       {Player::PROP_BOTTOM,        false, "0%"}},
 {"bounds",       {Player::PROP_BOUNDS,        false, "0%,0%,100%,100%"}},
 {"debug",        {Player::PROP_DEBUG,         true,  "false"}},
 {"duration",     {Player::PROP_DURATION,      true,  "indefinite"}},
 {"focusIndex",   {Player::PROP_FOCUS_INDEX,   true,  ""}},
 {"fontBgColor",  {Player::PROP_FONT_BG_COLOR, true,  ""}},
 {"fontColor",    {Player::PROP_FONT_COLOR,    true,  "black"}},
 {"fontFamily",   {Player::PROP_FONT_FAMILY,   true,  "sans"}},
 {"fontSize",     {Player::PROP_FONT_SIZE,     true,  "12"}},
 {"fontStyle",    {Player::PROP_FONT_STYLE,    true,  ""}},
 {"fontVariant",  {Player::PROP_FONT_VARIANT,  true,  ""}},
 {"fontWeight",   {Player::PROP_FONT_WEIGHT,   true,  ""}},
 {"freeze",       {Player::PROP_FREEZE,        true,  "false"}},
 {"height",       {Player::PROP_HEIGHT,        true,  "100%"}},
 {"horzAlign",    {Player::PROP_HORZ_ALIGN,    true,  "left"}},
 {"left",         {Player::PROP_LEFT,          true,  "0"}},
 {"location",     {Player::PROP_LOCATION,      false, "0,0"}},
 {"mute",         {Player::PROP_MUTE,          true,  "false"}},
 {"right",        {Player::PROP_RIGHT,         false, "0%"}},
 {"size",         {Player::PROP_SIZE,          false, "100%,100%"}},
 {"top",          {Player::PROP_TOP,           true,  "0"}},
 {"transparency", {Player::PROP_TRANSPARENCY,  true,  "0%"}},
 {"treble",       {Player::PROP_TREBLE,        false,  "100%"}},
 {"vertAlign",    {Player::PROP_VERT_ALIGN,    true,  "top"}},
 {"visible",      {Player::PROP_VISIBLE,       true,  "true"}},
 {"volume",       {Player::PROP_VOLUME,        true,  "100%"}},
 {"width",        {Player::PROP_WIDTH,         true,  "100%"}},
 {"zIndex",       {Player::PROP_Z_INDEX,       true,  "0"}},
};

static map<string, string> player_property_aliases =
{
 {"backgroundColor", "background"},
 {"balanceLevel",    "balance"},
 {"bassLevel",      "bass"},
 {"explicitDur",     "duration"},
 {"soundLevel",      "volume"},
 {"trebleLevel",    "treble"},
};


// Public.

/**
 * @brief Creates player for the given URI.
 * @param ginga Ginga handle.
 * @param id Media object id.
 * @param uri Content URI.
 */
Player::Player (GingaInternal *ginga, const string &id, const string &uri)
{
  // Internal data.
  g_assert_nonnull (ginga);
  _ginga = ginga;
  _id = id;
  _uri = uri;
  _state = SLEEPING;
  _time = 0;
  _eos = false;
  _dirty = true;
  _animator = new PlayerAnimator (_ginga);
  _surface = nullptr;
  _gltexture = 0;
  this->resetProperties ();
}

/**
 * Destroys player.
 */
Player::~Player ()
{
  delete _animator;
  if (_surface != nullptr)
    cairo_surface_destroy (_surface);
  _properties.clear ();
}

/**
 * @brief Gets player state.
 * @return Player state.
 */
Player::PlayerState
Player::getState ()
{
   return _state;
}

/**
 * @brief Tests whether player is focused.
 * @return True if successful, or false otherwise.
 */
bool
Player::isFocused ()
{
  return _prop.focusIndex != "" && _prop.focusIndex == _currentFocus;
}

/**
 * @brief Gets player playback time.
 * @return Playback time.
 */
GingaTime
Player::getTime ()
{
  return _time;
}

/**
 * @brief Increments player playback time.
 * @param inc Time increment.
 */
void
Player::incTime (GingaTime inc)
{
  _time += inc;
}

/**
 * @brief Gets player explicit duration.
 * @return Playback duration.
 */
GingaTime
Player::getDuration ()
{
  return _prop.duration;
}

/**
 * @brief Sets player explicit duration.
 * @param duration Playback duration.
 */
void
Player::setDuration (GingaTime duration)
{
  _prop.duration = duration;
}

/**
 * @brief Gets player EOS flag.
 * @return EOS flag value.
 */
bool
Player::getEOS ()
{
  return _eos;
}

/**
 * @brief Sets player EOS flag.
 * @param eos EOS flag value.
 */
void
Player::setEOS (bool eos)
{
  _eos = eos;
}

/**
 * @brief Gets player z-index and z-order.
 * @param z Address of variable to store player's z-index.
 * @param zorder Address of variable to store player's z-order.
 */
void
Player::getZ (int *z, int *zorder)
{
  tryset (z , _prop.z);
  tryset (zorder , _prop.zorder);
}

/**
 * @brief Sets player z-index and z-order.
 * @param z Z-index value.
 * @param zorder Z-order value.
 */
void
Player::setZ (int z, int zorder)
{
  _prop.z = z;
  _prop.zorder = zorder;
}

/**
 * @brief Starts player.
 */
void
Player::start ()
{
  g_assert (_state != OCCURRING);
  TRACE ("%s", _id.c_str ());

  _state = OCCURRING;
  _time = 0;
  _eos = false;
  this->reload ();
  _ginga->registerPlayer (this);
}

/**
 * @brief Stops player.
 */
void
Player::stop ()
{
  g_assert (_state != SLEEPING);
  TRACE ("%s", _id.c_str ());

  _state = SLEEPING;
  _ginga->unregisterPlayer (this);
  this->resetProperties ();
}

/**
 * @brief Pauses player.
 */
void
Player::pause ()
{
  g_assert (_state != PAUSED && _state != SLEEPING);
  TRACE ("%s", _id.c_str ());

  _state = PAUSED;
}

/**
 * @brief Resumes player.
 */
void
Player::resume ()
{
  g_assert (_state == PAUSED);
  TRACE ("%s", _id.c_str ());

  _state = OCCURRING;
}

/**
 * @brief Gets player property.
 * @param name Property name.
 * @return Property value.
 */
string
Player::getProperty (string const &name)
{
  return (_properties.count (name) != 0) ? _properties[name] : "";
}

/**
 * @brief Sets player property.
 * @param name Property name.
 * @param value Property value.
 */
void
Player::setProperty (const string &name, const string &value)
{
  Player::PlayerProperty code;
  bool use_defval;
  string defval;
  string _value;

  use_defval = false;
  _value = value;

  code = Player::getPlayerProperty (name, &defval);
  if (code == Player::PROP_UNKNOWN)
    goto done;

  if (_value == "")
    {
      use_defval = true;
      _value = defval;
    }

  if (unlikely (!this->doSetProperty (code, name, _value)))
    {
      ERROR_SYNTAX ("property '%s': bad value '%s'",
                    name.c_str (), _value.c_str ());
    }

  if (use_defval)               // restore value
    _value = "";

 done:
  TRACE ("%s.%s:='%s'%s",
         _id.c_str (), name.c_str (), _value.c_str (),
         (use_defval) ? (" (default: '" + defval + "')").c_str () : "");
  _properties[name] = _value;
  return;
}

/**
 * @brief Reset all player properties to their default values.
 */
void
Player::resetProperties ()
{
  for (auto it: player_property_map)
    if (it.second.init)
      this->setProperty (it.first, "");
  _properties.clear ();
}

/**
 * @brief Reset the given player properties.
 * @param props Properties to reset.
 */
void
Player::resetProperties (set<string> *props)
{
  for (auto name: *props)
    this->setProperty (name, "");
}

/**
 * @brief Schedules linear animation of property value.
 * @param name Property name.
 * @param from Current value.
 * @param to Target value.
 * @param dur Duration of the animation.
 */
void
Player::schedulePropertyAnimation (const string &name, const string &from,
                                   const string &to, GingaTime dur)
{
  TRACE ("%s.%s from '%s' to '%s' in %" GINGA_TIME_FORMAT,
         _id.c_str (), name.c_str (), from.c_str (), to.c_str (),
         GINGA_TIME_ARGS (dur));
  _animator->schedule (name, from, to, dur);
}

/**
 * @brief Reloads player texture.
 */
void
Player::reload (void)
{
  TRACE ("%s", _id.c_str ());
  _dirty = false;
}

/**
 * @brief Redraws player.
 */
void
Player::redraw (cairo_t *cr)
{
  g_assert (_state != SLEEPING);
  _animator->update (&_prop.rect, &_prop.bgColor, &_prop.alpha);

  if (!_prop.visible || !(_prop.rect.width > 0 && _prop.rect.height > 0))
    return;                     // nothing to do

  if (_dirty)
    {
      this->reload ();
    }

  if (_prop.bgColor.alpha > 0)
    {
      cairo_save (cr);
      cairo_set_source_rgba (cr,
                             _prop.bgColor.red,
                             _prop.bgColor.green,
                             _prop.bgColor.blue,
                             _prop.alpha / 255.);
      cairo_rectangle (cr,
                       _prop.rect.x,
                       _prop.rect.y,
                       _prop.rect.width,
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

  if (this->isFocused ())
    {
      cairo_save (cr);
      cairo_set_source_rgba (cr, 1., 1., 0., 1.);
      cairo_set_antialias (cr, CAIRO_ANTIALIAS_NONE);
      cairo_rectangle (cr,
                       _prop.rect.x,
                       _prop.rect.y,
                       _prop.rect.width,
                       _prop.rect.height);
      cairo_stroke (cr);
      cairo_restore (cr);
    }

  if (_prop.debug || _ginga->getOptionBool ("debug"))
    this->redrawDebuggingInfo (cr);
}

/**
 * @brief Redraws player using OpenGL.
 */
void
Player::redrawGL ()
{
#if !(defined WITH_OPENGL && WITH_OPENGL)
  WARNING_NOT_IMPLEMENTED ("not compiled with OpenGL support");
#else
  static int i = 0;

  i += 1;
  g_assert (_state != SLEEPING);
  _animator->update (&_prop.rect, &_prop.bgColor, &_prop.alpha);

  if (_prop.bgColor.alpha > 0)
    {
      gl_draw_quad (_prop.rect.x, _prop.rect.y,
                    _prop.rect.width, _prop.rect.height,
                    // Color
                    (GLfloat) _prop.bgColor.red,
                    (GLfloat) _prop.bgColor.green,
                    (GLfloat) _prop.bgColor.blue,
                    (GLfloat)(_prop.alpha / 255.));
    }

  if (_gltexture)
    {
      gl_draw_quad (_prop.rect.x, _prop.rect.y,
                    _prop.rect.width, _prop.rect.height,
                    _gltexture, (GLfloat)(_prop.alpha / 255.));
    }

#endif
}



// Public: Static.

/**
 * @brief Current focus index value.
 */
string Player::_currentFocus = "";

/**
 * @brief Gets current focus index.
 * @return Current focus index.
 */
string
Player::getCurrentFocus ()
{
  return _currentFocus;
}

/**
 * @brief Sets current focus index.
 * @param index Focus index.
 */
void
Player::setCurrentFocus (const string &index)
{
  TRACE ("from '%s' to '%s'", _currentFocus.c_str (), index.c_str ());
  _currentFocus = index;
}

/**
 * @brief Gets the property code of property.
 * @param name Property name.
 * @param defval Address of variable to store property default value.
 * @return Property code.
 */
Player::PlayerProperty
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

/**
 * @brief Creates a player from a mime-type.
 * @param ginga Ginga handle.
 * @param id Object id.
 * @param uri Source URI.
 * @param mime Mime-type of content.
 * @return New player.
 */
Player *
Player::createPlayer (GingaInternal *ginga, const string &id,
                      const string &uri, const string &mime)
{
  Player *player = nullptr;
  g_assert_nonnull (ginga);

  if (xstrhasprefix (mime, "audio") || xstrhasprefix (mime, "video"))
    {
      player = new VideoPlayer (ginga, id, uri);
    }
#if WITH_LIBRSVG && WITH_LIBRSVG
  else if (xstrhasprefix (mime, "image/svg"))
    {
      player = new SvgPlayer (ginga, id, uri);
    }
#endif
  else if (xstrhasprefix (mime, "image"))
    {
      player = new ImagePlayer (ginga, id, uri);
    }
#if defined WITH_CEF && WITH_CEF
  else if (xstrhasprefix (mime, "text/html"))
    {
      player = new HTMLPlayer (ginga, id, uri);
    }
#endif
  else if (mime == "text/plain")
    {
      player = new TextPlayer (ginga, id, uri);
    }
  else if (mime == "application/x-ginga-NCLua")
    {
      player = new LuaPlayer (ginga, id, uri);
    }
  else
    {
      player = new Player (ginga, id, uri);
      if (uri != "")
        {
          WARNING ("unknown mime '%s': creating an empty player",
                   mime.c_str ());
        }
    }

  g_assert_nonnull (player);
  return player;
}


// Protected.

bool
Player::doSetProperty (PlayerProperty code, unused (const string &name),
                       const string &value)
{
  switch (code)
    {
    case PROP_DEBUG:
      _prop.debug = ginga_parse_bool (value);
      break;
    case PROP_FOCUS_INDEX:
      _prop.focusIndex = value;
      break;
    case PROP_BOUNDS:
      {
        vector<string> v;
        if (unlikely (!_ginga_parse_list (value, ',', 4, 4, &v)))
          return false;
        this->setProperty ("left", v[0]);
        this->setProperty ("top", v[1]);
        this->setProperty ("width", v[2]);
        this->setProperty ("height", v[3]);
        break;
      }
    case PROP_LOCATION:
      {
        vector<string> v;
        if (unlikely (!_ginga_parse_list (value, ',', 2, 2, &v)))
          return false;
        this->setProperty ("left", v[0]);
        this->setProperty ("top", v[1]);
        break;
      }
    case PROP_SIZE:
      {
        vector<string> v;
        if (unlikely (!_ginga_parse_list (value, ',', 2, 2, &v)))
          return false;
        this->setProperty ("width", v[0]);
        this->setProperty ("height", v[1]);
        break;
      }
    case PROP_LEFT:
      {
        int width = _ginga->getOptionInt ("width");
        _prop.rect.x = ginga_parse_percent (value, width, 0, G_MAXINT);
        _dirty = true;
        break;
      }
    case PROP_RIGHT:
      {
        int width = _ginga->getOptionInt ("width");
        _prop.rect.x = width - _prop.rect.width
          - ginga_parse_percent (value, _prop.rect.width, 0, G_MAXINT);
        _dirty = true;
        break;
      }
    case PROP_TOP:
      {
        int height = _ginga->getOptionInt ("height");
        _prop.rect.y = ginga_parse_percent (value, height, 0, G_MAXINT);
        _dirty = true;
        break;
      }
    case PROP_BOTTOM:
      {
        int height = _ginga->getOptionInt ("height");
        _prop.rect.y = height - _prop.rect.height
          - ginga_parse_percent (value, _prop.rect.height, 0, G_MAXINT);
        _dirty = true;
        break;
      }
    case PROP_WIDTH:
      {
        int width = _ginga->getOptionInt ("width");
        _prop.rect.width = ginga_parse_percent (value, width, 0, G_MAXINT);
        _dirty = true;
        break;
      }
    case PROP_HEIGHT:
      {
        int height = _ginga->getOptionInt ("height");
        _prop.rect.height = ginga_parse_percent
          (value, height, 0, G_MAXINT);
        _dirty = true;
        break;
      }
    case PROP_Z_INDEX:
      this->setZ (xstrtoint (value, 10), _prop.zorder);
      break;
    case PROP_TRANSPARENCY:
      _prop.alpha = (guint8) CLAMP
        (255 - ginga_parse_pixel (value), 0, 255);
      break;
    case PROP_BACKGROUND:
      if (value == "")
        _prop.bgColor = {0, 0, 0, 0};
      else
        _prop.bgColor = ginga_parse_color (value);
      break;
    case PROP_VISIBLE:
      _prop.visible = ginga_parse_bool (value);
      break;
    case PROP_DURATION:
      {
        if (value == "indefinite")
          _prop.duration = GINGA_TIME_NONE;
        else
          _prop.duration = ginga_parse_time (value);
        break;
      }
    default:
      break;
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
  str = xstrbuild ("%s:%.1fs\n%dx%d:(%d,%d):%d",
                   id.c_str (),
                   ((double) GINGA_TIME_AS_MSECONDS (_time)) / 1000.,
                   _prop.rect.width, _prop.rect.height,
                   _prop.rect.x, _prop.rect.y, _prop.z);

  debug = TextPlayer::renderSurface
    (str, "monospace", "", "", "7", {1.,0,0,1.}, {0,0,0,.75},
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

GINGA_PLAYER_END
