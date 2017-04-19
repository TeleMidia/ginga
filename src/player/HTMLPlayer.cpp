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

#include "HTMLPlayer.h"

GINGA_PLAYER_BEGIN

HTMLPlayer::HTMLPlayer (const string &location) 
  : Player (location)
{
  _handler = nullptr;
  _client = nullptr;
  _browser = nullptr;

  _isPlaying = false;
}

HTMLPlayer::~HTMLPlayer ()
{
  // TODO
}

bool HTMLPlayer::play ()
{
  _isPlaying = true;

  Ginga_Display->addJob (displayJobCallbackWrapper, this);

  return Player::play ();
}

void HTMLPlayer::stop ()
{
  _isPlaying = false;

  _handler = nullptr;
  _client = nullptr;
  _browser = nullptr;
}

bool HTMLPlayer::displayJobCallbackWrapper(DisplayJob* job, SDL_Renderer* renderer,  void* self)
{
  return ((HTMLPlayer*) self)->displayJobCallback(job, renderer);  
}

bool HTMLPlayer::displayJobCallback (DisplayJob *job, SDL_Renderer *renderer)
{
  if (_isPlaying)
  {
    if (!_handler)
    {
      CefWindowInfo info;
      info.SetAsWindowless(0L, false);

      CefBrowserSettings settings;

      _handler = new GingaRenderHandler(window->getRect().w, window->getRect().h, renderer);
      _client = new GingaClient(_handler);

      string URI = "";
      
      if (!mrl.empty())
      {
        if (mrl.find("://") != string::npos)
        {
          URI = mrl;
        }
        else
        {
          URI = "file://"+mrl;
        }
      }
      
      _browser = CefBrowserHost::CreateBrowserSync(info, _client.get(), URI.c_str(), settings, nullptr);

      texture = _handler->getTexture();
    }

    CefDoMessageLoopWork(); 

    return true; 
  }

  return false;
}

GingaRenderHandler::GingaRenderHandler(int width, int height, SDL_Renderer* renderer)
{
  setWidth(width);
  setHeight(height);
  setRenderer(renderer);
}

GingaRenderHandler::~GingaRenderHandler()
{
  // TODO
}

int GingaRenderHandler::getWidth()
{
  return _width;
}

void GingaRenderHandler::setWidth(int width)
{
  _width = width;
}

int GingaRenderHandler::getHeight()
{
  return _height;
}

void GingaRenderHandler::setHeight(int height)
{
  _height = height;
}

SDL_Renderer* GingaRenderHandler::getRenderer()
{
  return _renderer;
}

void GingaRenderHandler::setRenderer(SDL_Renderer* renderer)
{
  _renderer = renderer;

  _texture = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_UNKNOWN, SDL_TEXTUREACCESS_STREAMING, _width, _height);
}

SDL_Texture* GingaRenderHandler::getTexture()
{
  return _texture;
}

bool GingaRenderHandler::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect)
{
  rect = CefRect(0, 0, _width, _height);

  return true;
}

void GingaRenderHandler::OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects, const void * buffer, int w, int h)
{
  if (_texture)
  {
    unsigned char* data = NULL;
    int pitch = 0;

    SDL_LockTexture(_texture, 0, (void **)&data, &pitch);
    memcpy(data, buffer, w * h * 4);
    SDL_UnlockTexture(_texture);
  }
}

GingaClient::GingaClient(CefRefPtr<CefRenderHandler> handler)
{
  _handler = handler;
}

GingaClient::~GingaClient()
{
  // TODO
}

CefRefPtr<CefLifeSpanHandler> GingaClient::GetLifeSpanHandler()
{
  return this;
}

CefRefPtr<CefLoadHandler> GingaClient::GetLoadHandler()
{
  return this;
}

CefRefPtr<CefRenderHandler> GingaClient::GetRenderHandler()
{
  return _handler;
}

GINGA_PLAYER_END
