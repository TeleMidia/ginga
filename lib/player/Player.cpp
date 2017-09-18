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

#include "ginga-internal.h"
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

#include "mb/Display.h"
using namespace ::ginga::mb;

GINGA_PLAYER_BEGIN


// Public.

/**
 * @brief Creates a player from a mime-type.
 * @param id Object id.
 * @param uri Source URI.
 * @param mime Mime-type of content.
 * @return A new player to exhibit source.
 */
Player *
Player::createPlayer (const string &id, const string &uri,
                      const string &mime)
{
  Player *player = nullptr;

  if (xstrhasprefix (mime, "audio") || xstrhasprefix (mime, "video"))
    {
      player = new VideoPlayer (id, uri);
    }
#if WITH_LIBRSVG && WITH_LIBRSVG
  else if (xstrhasprefix (mime, "image/svg"))
    {
      player = new SvgPlayer (id, uri);
    }
#endif
  else if (xstrhasprefix (mime, "image"))
    {
      player = new ImagePlayer (id, uri);
    }
#if defined WITH_CEF && WITH_CEF
  else if (xstrhasprefix (mime, "text/html"))
    {
      player = new HTMLPlayer (id, uri);
    }
#endif
  else if (mime == "text/plain")
    {
      player = new TextPlayer (id, uri);
    }
  else if (mime == "application/x-ginga-NCLua")
    {
      player = new LuaPlayer (id, uri);
    }
  else
    {
      player = new Player (id, uri);
      if (uri != "")
        {
          WARNING ("unknown mime '%s': creating an empty player",
                   mime.c_str ());
        }
    }

  g_assert_nonnull (player);
  return player;
}

/**
 * @brief Creates player for the given URI.
 */
Player::Player (const string &id, const string &uri)
{
  // Internal data.
  _id = id;
  _uri = uri;
  _state = PL_SLEEPING;
  _time = 0;
  _eos = false;
  _dirty = true;
  _surface = nullptr;

  // Properties
  _debug = true;
  _focusIndex = "";
  _rect = {0, 0, 0, 0};
  _z = 0;
  _zorder = 0;
  _alpha = 255;                 // opaque
  _bgColor = {0, 0, 0, 0};      // none
  _visible = true;
  _duration = GINGA_TIME_NONE;
}

/**
 * Destroys player.
 */
Player::~Player ()
{
  if (_surface != nullptr)
    cairo_surface_destroy (_surface);
  _properties.clear ();
}

/**
 * @brief Gets the id of the associated object.
 */
string
Player::getId ()
{
  return _id;
}

/**
 * @brief Gets player URI.
 */
string
Player::getURI ()
{
  return _uri;
}

/**
 * @brief Gets player state.
 */
Player::PlayerState
Player::getState ()
{
   return _state;
}

/**
 * @brief Gets player playback time.
 */
GingaTime
Player::getTime ()
{
  return _time;
}

/**
 * @brief Increments player playback time.
 */
void
Player::incTime (GingaTime inc)
{
  _time += inc;
}

/**
 * @brief Gets player EOS flag.
 */
bool
Player::getEOS ()
{
  return _eos;
}

/**
 * @brief Sets player EOS flag.
 */
void
Player::setEOS (bool eos)
{
  _eos = eos;
}

/**
 * @brief Starts player.
 */
void
Player::start ()
{
  g_assert (_state != PL_OCCURRING);
  TRACE ("starting %s", _id.c_str ());

  _state = PL_OCCURRING;
  _time = 0;
  _eos = false;
  this->reload ();
  Ginga_Display->registerPlayer (this);
}

/**
 * @brief Stops player.
 */
void
Player::stop ()
{
  g_assert (_state != PL_SLEEPING);
  TRACE ("stopping %s", _id.c_str ());

  _state = PL_SLEEPING;
  _animator.clear ();
  Ginga_Display->unregisterPlayer (this);
}

/**
 * @brief Pauses player.
 */
void
Player::pause ()
{
  g_assert (_state != PL_PAUSED && _state != PL_SLEEPING);
  TRACE ("pausing %s", _id.c_str ());

  _state = PL_PAUSED;
}

/**
 * @brief Resumes player.
 */
void
Player::resume ()
{
  g_assert (_state == PL_PAUSED);
  TRACE ("resuming %s", _id.c_str ());

  _state = PL_OCCURRING;
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
  TRACE ("animating %s.%s from '%s' to '%s' in %" GINGA_TIME_FORMAT,
         _id.c_str (), name.c_str (), from.c_str (), to.c_str (),
         GINGA_TIME_ARGS (dur));
  _animator.schedule (name, from, to, dur);
}


// Public: Properties.

/**
 * @brief Gets player property.
 */
string
Player::getProperty (string const &name)
{
  return (_properties.count (name) != 0) ? _properties[name] : "";
}

/**
 * @brief Sets player property.
 */
void
Player::setProperty (const string &name, const string &value)
{
  vector<string> params;

  TRACE ("setting %s.%s to '%s'", _id.c_str (),
         name.c_str (), value.c_str ());

  if (value == "")
    goto done;

  if (name == "debug")
    {
      _debug = ginga_parse_bool (value);
    }
  else if (name == "focusIndex")
    {
      _focusIndex = value;
    }
  else if (name == "bounds")
    {
      vector<string> v;

      if (unlikely (!_ginga_parse_list (value, ',', 4, 4, &v)))
        goto syntax_error;

      this->setProperty ("left", v[0]);
      this->setProperty ("top", v[1]);
      this->setProperty ("width", v[2]);
      this->setProperty ("height", v[3]);
    }
  else if (name == "location")
    {
      vector<string> v;

      if (unlikely (!_ginga_parse_list (value, ',', 2, 2, &v)))
        goto syntax_error;

      this->setProperty ("left", v[0]);
      this->setProperty ("top", v[1]);
    }
  else if (name == "size")
    {
      vector<string> v;

      if (unlikely (!_ginga_parse_list (value, ',', 2, 2, &v)))
        goto syntax_error;

      this->setProperty ("width", v[0]);
      this->setProperty ("height", v[1]);
    }
  else if (name == "left")
    {
      int width;
      Ginga_Display->getSize (&width, nullptr);
      _rect.x = ginga_parse_percent (value, width, 0, G_MAXINT);
      _dirty = true;
    }
  else if (name == "right")
    {
      int width;
      Ginga_Display->getSize (&width, nullptr);
      _rect.x = width - _rect.width
        - ginga_parse_percent (value, _rect.width, 0, G_MAXINT);
      _dirty = true;
    }
  else if (name == "top")
    {
      int height;
      Ginga_Display->getSize (nullptr, &height);
      _rect.y = ginga_parse_percent (value, height, 0, G_MAXINT);
      _dirty = true;
    }
  else if (name == "bottom")
    {
      int height;
      Ginga_Display->getSize (nullptr, &height);
      _rect.y = height - _rect.height
        - ginga_parse_percent (value, _rect.height, 0, G_MAXINT);
      _dirty = true;
    }
  else if (name == "width")
    {
      int width;
      Ginga_Display->getSize (&width, nullptr);
      _rect.width = ginga_parse_percent (value, width, 0, G_MAXINT);
      _dirty = true;
    }
  else if (name == "height")
    {
      int height;
      Ginga_Display->getSize (nullptr, &height);
      _rect.height = ginga_parse_percent (value, height, 0, G_MAXINT);
      _dirty = true;
    }
  else if (name == "zIndex")
    {
      this->setZ (xstrtoint (value, 10), _zorder);
    }
  else if (name == "transparency")
    {
      _alpha = (guint8) CLAMP (255 - ginga_parse_pixel (value), 0, 255);
    }
  else if (name == "background" || name == "backgroundColor")
    {
      _bgColor = ginga_parse_color (value);
    }
  else if (name == "visible")
    {
      _visible = ginga_parse_bool (value);
    }
  else if (name == "explicitDur" || name == "duration")
    {
      if (value == "indefinite")
        _duration = GINGA_TIME_NONE;
      else
        _duration = ginga_parse_time (value);
    }

 done:
  _properties[name] = value;
  return;

 syntax_error:
  ERROR_SYNTAX ("property '%s': bad value '%s'",
                name.c_str (), value.c_str ());
}

/**
 * @brief Tests whether player is focused.
 * @return True if successful, or false otherwise.
 */
bool
Player::isFocused ()
{
  return _focusIndex != "" && _focusIndex == _currentFocus;
}

/**
 * @brief Gets player output rectangle.
 */
GingaRect
Player::getRect ()
{
  return _rect;
}

/**
 * @brief Sets player output rectangle.
 */
void
Player::setRect (GingaRect rect)
{
  _rect = rect;
}

/**
 * @brief Gets player z-index and z-order.
 */
void
Player::getZ (int *z, int *zorder)
{
  tryset (z , _z);
  tryset (zorder , _zorder);
}

/**
 * @brief Sets player z-index and z-order.
 */
void
Player::setZ (int z, int zorder)
{
  _z = z;
  _zorder = zorder;
}

/**
 * @brief Gets player alpha.
 */
double
Player::getAlpha ()
{
  return _alpha / 255.;
}

/**
 * @brief Sets player alpha.
 */
void
Player::setAlpha (double alpha)
{
  _alpha = (guint8) CLAMP (lround (alpha * 255.), 0, 255);
}

/**
 * @brief Gets player background color.
 */
GingaColor
Player::getBgColor ()
{
  return _bgColor;
}

/**
 * @brief Sets player background color.
 */
void
Player::setBgColor (GingaColor color)
{
  _bgColor = color;
}

/**
 * @brief Gets player visible flag.
 */
bool
Player::getVisible ()
{
  return _visible;
}

/**
 * @brief Sets player visible flag.
 */
void
Player::setVisible (bool visible)
{
  _visible = visible;
}

/**
 * @brief Gets player explicit duration.
 */
GingaTime
Player::getDuration ()
{
  return _duration;
}

/**
 * @brief Sets player explicit duration.
 */
void
Player::setDuration (GingaTime duration)
{
  _duration = duration;
}


// Public: Callbacks.

/**
 * @brief Reloads player texture.
 */
void
Player::reload (void)
{
  TRACE ("reloading %s", _id.c_str ());
  _dirty = false;
}

/**
 * @brief Redraws player.
 */
void
Player::redraw (cairo_t *cr)
{
  g_assert (_state != PL_SLEEPING);
  _animator.update (&_rect, &_bgColor, &_alpha);

  if (!_visible || !(_rect.width > 0 && _rect.height > 0))
    return;                     // nothing to do

  if (_dirty)
    {
      this->reload ();
    }

  if (_bgColor.alpha > 0)
    {
      cairo_save (cr);
      cairo_set_source_rgba
        (cr, _bgColor.red, _bgColor.green, _bgColor.blue, _alpha / 255.);
      cairo_rectangle (cr, _rect.x, _rect.y, _rect.width, _rect.height);
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
      cairo_paint_with_alpha (cr, _alpha / 255.);
      cairo_restore (cr);
    }

  if (this->isFocused ())
    {
      cairo_save (cr);
      cairo_set_source_rgba (cr, 1., 1., 0., 1.);
      cairo_set_antialias (cr, CAIRO_ANTIALIAS_NONE);
      cairo_rectangle (cr, _rect.x, _rect.y, _rect.width, _rect.height);
      cairo_stroke (cr);
      cairo_restore (cr);
    }

  if (_debug)
    this->redrawDebuggingInfo (cr);
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
  TRACE ("setting current focus to '%s'", index.c_str ());
  _currentFocus = index;
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
                   _rect.width, _rect.height, _rect.x, _rect.y, _z);

  debug = TextPlayer::renderSurface
    (str, "monospace", "", "", "7", {1.,0,0,1.}, {0,0,0,.75},
     _rect, "center", "middle", true, nullptr);
  g_assert_nonnull (debug);

  sx = (double) _rect.width / cairo_image_surface_get_width (debug);
  sy = (double) _rect.height / cairo_image_surface_get_height (debug);

  cairo_save (cr);
  cairo_translate (cr, _rect.x, _rect.y);
  cairo_scale (cr, sx, sy);
  cairo_set_source_surface (cr, debug, 0., 0.);
  cairo_paint (cr);
  cairo_restore (cr);

  cairo_surface_destroy (debug);
}

GINGA_PLAYER_END
