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
 * @param uri Source URI.
 * @param mime Mime-type of content.
 * @return A new player to exhibit source.
 */
Player *
Player::createPlayer (const string &uri, const string &mime)
{
  Player *player = nullptr;

  if (xstrhasprefix (mime, "audio") || xstrhasprefix (mime, "video"))
    {
      player = new VideoPlayer (uri);
    }
#if WITH_LIBRSVG && WITH_LIBRSVG
  else if (xstrhasprefix (mime, "image/svg"))
    {
      player = new SvgPlayer (uri);
    }
#endif
  else if (xstrhasprefix (mime, "image"))
    {
      player = new ImagePlayer (uri);
    }
#if defined WITH_CEF && WITH_CEF
  else if (xstrhasprefix (mime, "text/html"))
    {
      player = new HTMLPlayer (uri);
    }
#endif
  else if (mime == "text/plain")
    {
      player = new TextPlayer (uri);
    }
  else if (mime == "application/x-ginga-NCLua")
    {
      player = new LuaPlayer (uri);
    }
  else
    {
      player = new Player (uri);
      WARNING ("unknown mime '%s': creating an empty player",
               mime.c_str ());
    }

  g_assert_nonnull (player);
  return player;
}

/**
 * @brief Creates player for the given URI.
 */
Player::Player (const string &uri)
{
  _rect = {0, 0, 0, 0};
  _z = 0;
  _zorder = 0;
  _alpha = 255;                 // opaque
  _bgColor = {0, 0, 0, 0};      // none
  _focused = false;
  _state = PL_SLEEPING;
  _uri = uri;
  _texture = nullptr;
}

/**
 * Destroys player.
 */
Player::~Player ()
{
  if (_texture != nullptr)
    SDL_DestroyTexture (_texture);
  _properties.clear ();
}

/**
 * @brief Gets player output rectangle.
 */
SDL_Rect
Player::getRect ()
{
  return _rect;
}

/**
 * @brief Sets player output rectangle.
 */
void
Player::setRect (SDL_Rect rect)
{
  _rect = rect;
}

/**
 * @brief Gets player z-index and z-order.
 */
void
Player::getZ (int *z, int *zorder)
{
  set_if_nonnull (z , _z);
  set_if_nonnull (zorder , _zorder);
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
SDL_Color
Player::getBgColor ()
{
  return _bgColor;
}

/**
 * @brief Sets player background color.
 */
void
Player::setBgColor (SDL_Color color)
{
  _bgColor = color;
}

/**
 * @brief Gets player focus.
 */
bool
Player::getFocus (void)
{
  return _focused;
}

/**
 * @brief Sets player focus.
 */
void
Player::setFocus (bool focus)
{
  _focused = focus;
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
 * @brief Gets player URI.
 */
string
Player::getURI ()
{
  return _uri;
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

  TRACE ("setting %p.%s to '%s'", this, name.c_str (), value.c_str ());

  if (value == "")
    goto done;

  if (name == "bounds")
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
    }
  else if (name == "right")
    {
      int width;
      Ginga_Display->getSize (&width, nullptr);
      _rect.x = width - _rect.w
        - ginga_parse_percent (value, _rect.w, 0, G_MAXINT);
    }
  else if (name == "top")
    {
      int height;
      Ginga_Display->getSize (nullptr, &height);
      _rect.y = ginga_parse_percent (value, height, 0, G_MAXINT);
    }
  else if (name == "bottom")
    {
      int height;
      Ginga_Display->getSize (nullptr, &height);
      _rect.y = height - _rect.h
        - ginga_parse_percent (value, _rect.h, 0, G_MAXINT);
    }
  else if (name == "width")
    {
      int width;
      Ginga_Display->getSize (&width, nullptr);
      _rect.w = ginga_parse_percent (value, width, 0, G_MAXINT);
    }
  else if (name == "height")
    {
      int height;
      Ginga_Display->getSize (nullptr, &height);
      _rect.h = ginga_parse_percent (value, height, 0, G_MAXINT);
    }
  else if (name == "background" || name == "backgroundColor")
    {
      _bgColor = ginga_parse_color (value);
    }
  else if (name == "transparency")
    {
      _alpha = (guint8) CLAMP (255 - ginga_parse_pixel (value), 0, 255);
    }
  else if (name == "zIndex")
    {
      this->setZ (xstrtoint (value, 10), _zorder);
    }

 done:
  _properties[name] = value;
  return;

 syntax_error:
  ERROR_SYNTAX ("property '%s': bad value '%s'",
                name.c_str (), value.c_str ());
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
  TRACE ("animating %p.%s from '%s' to '%s' in %" GINGA_TIME_FORMAT,
         this, name.c_str (), from.c_str (), to.c_str (),
         GINGA_TIME_ARGS (dur));
  _animator.schedule (name, from, to, dur);
}

/**
 * @brief Starts player.
 */
void
Player::start ()
{
  TRACE ("starting");
  _state = PL_OCCURRING;
  _eos = false;
  Ginga_Display->registerPlayer (this);
}

/**
 * @brief Stops player.
 */
void
Player::stop ()
{
  TRACE ("stopping");
  _state = PL_SLEEPING;
  _animator.clear ();
  Ginga_Display->unregisterPlayer (this);
}

/**
 * @brief Pauses player.
 */
void G_GNUC_NORETURN
Player::pause ()
{
  ERROR_NOT_IMPLEMENTED ("pause action is not supported");
}

/**
 * @brief Resumes player.
 */
void G_GNUC_NORETURN
Player::resume ()
{
  ERROR_NOT_IMPLEMENTED ("resume action is not supported");
}

/**
 * Redraws player onto renderer.
 */
void
Player::redraw (SDL_Renderer *renderer)
{
  g_assert (_state != PL_SLEEPING);

  _animator.update (&_rect, &_bgColor, &_alpha);

  if (_focused)
    TRACE ("%p focused", this);

  if (_bgColor.a > 0)
    {
      SDLx_SetRenderDrawBlendMode (renderer, SDL_BLENDMODE_BLEND);
      SDLx_SetRenderDrawColor (renderer,
                               _bgColor.r,
                               _bgColor.g,
                               _bgColor.b,
                               _alpha);
      SDLx_RenderFillRect (renderer, &_rect);
    }

  if (_texture != nullptr)
    {
      SDLx_SetTextureBlendMode (_texture, SDL_BLENDMODE_BLEND);
      SDLx_SetTextureAlphaMod (_texture, _alpha);
      SDLx_RenderCopy (renderer, _texture, nullptr, &_rect);
    }

  // if (this->borderWidth < 0)
  //   {
  //     this->borderWidth *= -1;
  //     SDLx_SetRenderDrawBlendMode (renderer, SDL_BLENDMODE_BLEND);
  //     SDLx_SetRenderDrawColor (renderer,
  //                              this->borderColor.r,
  //                              this->borderColor.g,
  //                              this->borderColor.b, 255);
  //     SDLx_RenderDrawRect (renderer, &_rect);
  //   }
}

GINGA_PLAYER_END
