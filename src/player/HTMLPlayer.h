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

#ifndef HTML_PLAYER_H
#define HTML_PLAYER_H

#include "ginga.h"

#include "Player.h"

#include "mb/IKeyInputEventListener.h"
#include "mb/Display.h"
using namespace ginga::mb;

#include <cef_app.h>
#include <cef_client.h>
#include <cef_render_handler.h>
#include <cef_life_span_handler.h>
#include <cef_load_handler.h>
#include <wrapper/cef_helpers.h>

GINGA_PLAYER_BEGIN

class GingaRenderHandler : public CefRenderHandler
{
public:
    GingaRenderHandler(int width, int height, SDL_Renderer* renderer);
    virtual ~GingaRenderHandler();
   
    int getWidth();
    void setWidth(int width);

    int getHeight();
    void setHeight(int height);

    SDL_Renderer* getRenderer();
    void setRenderer(SDL_Renderer* renderer);

    SDL_Texture* getTexture();

    bool GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect);
    void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects, const void * buffer, int w, int h);

private:
    int _width;
    int _height;

    SDL_Texture* _texture;
    SDL_Renderer* _renderer;
    
    IMPLEMENT_REFCOUNTING(GingaRenderHandler);
};

class GingaClient :
    public CefClient,
    public CefLifeSpanHandler,
    public CefLoadHandler
{
public:
    GingaClient(CefRefPtr<CefRenderHandler> handler);
    virtual ~GingaClient();

    virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler();
    virtual CefRefPtr<CefLoadHandler> GetLoadHandler();
    virtual CefRefPtr<CefRenderHandler> GetRenderHandler();

private:
    CefRefPtr<CefRenderHandler> _handler;

    IMPLEMENT_REFCOUNTING(GingaClient);
};

class HTMLPlayer : public Player, IKeyInputEventListener
{
public:
  HTMLPlayer (const string &location);
  virtual ~HTMLPlayer ();

  virtual bool play ();
  virtual void stop ();

private:
  GINGA_MUTEX_DEFN ();
  GINGA_COND_DEFN (DisplayJob);

  static bool displayJobCallbackWrapper (DisplayJob *, SDL_Renderer *, void *);
  bool displayJobCallback (DisplayJob *, SDL_Renderer *);

  void keyInputCallback (SDL_EventType evtType, SDL_Keycode key);

  int getPlayerKey(SDL_Keycode key);

  CefRefPtr<GingaRenderHandler> _handler;
  CefRefPtr<GingaClient> _client;
  CefRefPtr<CefBrowser> _browser;

  bool _isPlaying;
};

GINGA_PLAYER_END

#endif /* HTML_PLAYER_H */
