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

/**
 * @file   PlayerHTML.cpp
 * @author Eduardo Cruz Araújo (edcaraujo@gmail.com)
 * @date   April, 2017
 *
 * @brief  Source file of HTML player.
 *
 * This is the header file for the PlayerHTML and others
 * related classes.
 */

#include "PlayerHTML.h"

GINGA_NAMESPACE_BEGIN

PlayerHTML::PlayerHTML (const string &location) : Player (location)
{
  _handler = nullptr;
  _client = nullptr;
  _browser = nullptr;

  _isPlaying = false;

  g_assert (Ginga_Display->registerEventListener (this));
}

PlayerHTML::~PlayerHTML ()
{
  // TODO
}

bool
PlayerHTML::play ()
{
  _isPlaying = true;

  Ginga_Display->addJob (displayJobCallbackWrapper, this);

  return Player::play ();
}

void
PlayerHTML::stop ()
{
  _isPlaying = false;

  _browser->GetHost ()->CloseBrowser (true);
  _browser = nullptr;

  _client = nullptr;
  _handler = nullptr;

  texture = NULL;
}

bool
PlayerHTML::displayJobCallbackWrapper (DisplayJob *job,
                                       SDL_Renderer *renderer, void *self)
{
  return ((PlayerHTML *) self)->displayJobCallback (job, renderer);
}

bool
PlayerHTML::displayJobCallback (unused (DisplayJob *job),
                                SDL_Renderer *renderer)
{
  if (!_isPlaying)
    return false;

  if (!_browser)
    {
      string URI = "";
      CefWindowInfo info;
      CefBrowserSettings settings;

      info.SetAsWindowless (0L, false);
      _handler = new GingaCefHandler (window->getRect ().w,
                                      window->getRect ().h, renderer);
      _client = new GingaCefClient (_handler);

      URI = (mrl.find ("://") != string::npos) ? mrl : "file://" + mrl;
      _browser = CefBrowserHost::CreateBrowserSync (
          info, _client.get (), URI.c_str (), settings, nullptr);
      texture = _handler->getTexture ();
    }

  CefDoMessageLoopWork ();
  return true; // continue
}

void
PlayerHTML::handleKeyEvent (SDL_EventType evtType, SDL_Keycode key)
{
  CefKeyEvent event;

  if (!_browser)
    return;

  if (evtType != SDL_KEYDOWN && evtType != SDL_KEYUP)
    return;

  event.modifiers = 0;
  event.native_key_code = key;
  event.windows_key_code = getPlayerKey (key);
  event.unmodified_character = (char) event.windows_key_code;
  event.character = event.unmodified_character;
  event.type
      = (evtType == SDL_KEYDOWN ? KEYEVENT_RAWKEYDOWN : KEYEVENT_KEYUP);

  _browser->GetHost ()->SendKeyEvent (event);

  if (evtType == SDL_KEYDOWN)
    {
      event.type = KEYEVENT_CHAR;
      _browser->GetHost ()->SendKeyEvent (event);
    }
}

void
PlayerHTML::mouseInputCallback (SDL_EventType evtType, int x, int y)
{
  CefMouseEvent event;

  if (!_browser)
    return;

  if (evtType != SDL_MOUSEBUTTONUP && evtType != SDL_MOUSEBUTTONDOWN)
    return;

  event.x = x;
  event.y = y;
  _browser->GetHost ()->SendMouseClickEvent (
      event, MBT_LEFT, (evtType == SDL_MOUSEBUTTONUP ? true : false), 1);
}

int
PlayerHTML::getPlayerKey (SDL_Keycode key)
{
  int result = 0;

  bool WITH_SHIFT = false;
  bool WITH_CAPSLOCK = false;

  bool UPPERCASE
      = (WITH_CAPSLOCK && !WITH_SHIFT) || (WITH_SHIFT && !WITH_CAPSLOCK);

  switch (key)
    {
    case SDLK_RETURN:
      result = 0x0D;
      break;
    case SDLK_ESCAPE:
      result = 0x1B;
      break;
    case SDLK_BACKSPACE:
      result = 0x08;
      break;
    case SDLK_TAB:
      result = 0x09;
      break;
    case SDLK_SPACE:
      result = 0x20;
      break;

    case SDLK_LEFT:
      result = 0x25;
      break;
    case SDLK_UP:
      result = 0x26;
      break;
    case SDLK_RIGHT:
      result = 0x27;
      break;
    case SDLK_DOWN:
      result = 0x28;
      break;

    case SDLK_0:
      result = 0x30;
      break;
    case SDLK_1:
      result = 0x31;
      break;
    case SDLK_2:
      result = 0x32;
      break;
    case SDLK_3:
      result = 0x33;
      break;
    case SDLK_4:
      result = 0x34;
      break;
    case SDLK_5:
      result = 0x35;
      break;
    case SDLK_6:
      result = 0x36;
      break;
    case SDLK_7:
      result = 0x37;
      break;
    case SDLK_8:
      result = 0x38;
      break;
    case SDLK_9:
      result = 0x39;
      break;

    case SDLK_a:
      result = UPPERCASE ? 65 : 97;
      break;
    case SDLK_b:
      result = UPPERCASE ? 66 : 98;
      break;
    case SDLK_c:
      result = UPPERCASE ? 67 : 99;
      break;
    case SDLK_d:
      result = UPPERCASE ? 68 : 100;
      break;
    case SDLK_e:
      result = UPPERCASE ? 69 : 101;
      break;
    case SDLK_f:
      result = UPPERCASE ? 70 : 102;
      break;
    case SDLK_g:
      result = UPPERCASE ? 71 : 103;
      break;
    case SDLK_h:
      result = UPPERCASE ? 72 : 104;
      break;
    case SDLK_i:
      result = UPPERCASE ? 73 : 105;
      break;
    case SDLK_j:
      result = UPPERCASE ? 74 : 106;
      break;
    case SDLK_k:
      result = UPPERCASE ? 75 : 107;
      break;
    case SDLK_l:
      result = UPPERCASE ? 76 : 108;
      break;
    case SDLK_m:
      result = UPPERCASE ? 77 : 109;
      break;
    case SDLK_n:
      result = UPPERCASE ? 78 : 110;
      break;
    case SDLK_o:
      result = UPPERCASE ? 79 : 111;
      break;
    case SDLK_p:
      result = UPPERCASE ? 80 : 112;
      break;
    case SDLK_q:
      result = UPPERCASE ? 81 : 113;
      break;
    case SDLK_r:
      result = UPPERCASE ? 82 : 114;
      break;
    case SDLK_s:
      result = UPPERCASE ? 83 : 115;
      break;
    case SDLK_t:
      result = UPPERCASE ? 84 : 116;
      break;
    case SDLK_u:
      result = UPPERCASE ? 85 : 117;
      break;
    case SDLK_v:
      result = UPPERCASE ? 86 : 118;
      break;
    case SDLK_w:
      result = UPPERCASE ? 87 : 119;
      break;
    case SDLK_x:
      result = UPPERCASE ? 88 : 120;
      break;
    case SDLK_y:
      result = UPPERCASE ? 89 : 121;
      break;
    case SDLK_z:
      result = UPPERCASE ? 90 : 122;
      break;
    default:
      result = 0;
    }

  return result;
}

GingaCefHandler::GingaCefHandler (int width, int height,
                                  SDL_Renderer *renderer)
{
  setWidth (width);
  setHeight (height);
  setRenderer (renderer);
}

GingaCefHandler::~GingaCefHandler ()
{
  // TODO
}

int
GingaCefHandler::getWidth ()
{
  return _width;
}

void
GingaCefHandler::setWidth (int width)
{
  _width = width;
}

int
GingaCefHandler::getHeight ()
{
  return _height;
}

void
GingaCefHandler::setHeight (int height)
{
  _height = height;
}

SDL_Renderer *
GingaCefHandler::getRenderer ()
{
  return _renderer;
}

void
GingaCefHandler::setRenderer (SDL_Renderer *renderer)
{
  _renderer = renderer;

  _texture
      = SDL_CreateTexture (_renderer, SDL_PIXELFORMAT_UNKNOWN,
                           SDL_TEXTUREACCESS_STREAMING, _width, _height);
}

SDL_Texture *
GingaCefHandler::getTexture ()
{
  return _texture;
}

bool
GingaCefHandler::GetViewRect (unused (CefRefPtr<CefBrowser> browser),
                              CefRect &rect)
{
  rect = CefRect (0, 0, _width, _height);
  return true;
}

void
GingaCefHandler::OnPaint (unused (CefRefPtr<CefBrowser> browser),
                          unused (PaintElementType type),
                          unused (const RectList &dirtyRects),
                          const void *buffer, int w, int h)
{
  unsigned char *data = NULL;
  int pitch = 0;

  if (!_texture)
    return;

  SDL_LockTexture (_texture, 0, (void **) &data, &pitch);
  memcpy (data, buffer, w * h * 4); // FIXME
  SDL_UnlockTexture (_texture);
}

GingaCefClient::GingaCefClient (CefRefPtr<CefRenderHandler> handler)
{
  _handler = handler;
}

GingaCefClient::~GingaCefClient ()
{
}

CefRefPtr<CefLifeSpanHandler>
GingaCefClient::GetLifeSpanHandler ()
{
  return this;
}

CefRefPtr<CefLoadHandler>
GingaCefClient::GetLoadHandler ()
{
  return this;
}

CefRefPtr<CefRenderHandler>
GingaCefClient::GetRenderHandler ()
{
  return _handler;
}

GINGA_NAMESPACE_END
