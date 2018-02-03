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
 * @file   PlayerHTML.h
 * @author Eduardo Cruz Araújo (edcaraujo@gmail.com)
 * @date   April, 2017
 *
 * @brief  Header file of HTML player.
 *
 * This is the header file for the PlayerHTML and others
 * related classes.
 */

#ifndef HTML_PLAYER_H
#define HTML_PLAYER_H

#include "aux-ginga.h"

#include "Player.h"

GINGA_PRAGMA_DIAG_PUSH ()
GINGA_PRAGMA_DIAG_IGNORE (-Wunused - parameter)
// GINGA_PRAGMA_DIAG_IGNORE (-Wunused-const-variable)
#include <cef_app.h>
#include <cef_client.h>
#include <cef_render_handler.h>
#include <cef_life_span_handler.h>
#include <cef_load_handler.h>
#include <wrapper/cef_helpers.h>
GINGA_PRAGMA_DIAG_POP ()

GINGA_NAMESPACE_BEGIN

/**
 * @brief A render handler class of CEF for Ginga.
 *
 * This class is responsible for copying the page load result
 * to a local SDL texture.
 */
class GingaCefHandler : public CefRenderHandler
{
public:
  /**
   * @brief Default constructor for GingaCefHandler.
   *
   * @param width The window width.
   * @param height The window height.
   * @param renderer The renderer.
   * @return Nothing.
   */
  GingaCefHandler (int width, int height, SDL_Renderer *renderer);

  /**
   * @brief Default destructor for GingaCefHandler.
   *
   * @return Nothing.
   */
  virtual ~GingaCefHandler ();

  /**
   * @brief Return the current window width.
   *
   * @return The current window width.
   */
  int getWidth ();

  /**
   * @brief Set the current window width.
   *
   * @return Nothing.
   * @see setHeight(), setRenderer() and getTexture().
   *
   * The window width is used to create the local texture.
   */
  void setWidth (int width);

  /**
   * @brief Return the current window height.
   *
   * @return The current window height.
   */
  int getHeight ();

  /**
   * @brief Set the current window height.
   *
   * @return Nothing.
   * @see setWidth(), setRenderer() and getTexture().
   *
   * The window height is used to create the local texture.
   */
  void setHeight (int height);

  /**
   * @brief Return the current renderer.
   *
   * @return The current renderer.
   */
  SDL_Renderer *getRenderer ();

  /**
   * @brief Set the current renderer.
   *
   * @return Nothing.
   * @see setWidth(), setHeight() and getTexture().
   *
   * This method also create a new instance of the local texture based on
   * the width and height values and the renderer itself.
   */
  void setRenderer (SDL_Renderer *renderer);

  /**
   * @brief Return the local texture.
   *
   * @return The texture.
   */
  SDL_Texture *getTexture ();

  /**
   * @brief Set the view rectangle.
   *
   * @param browser A browser instance.
   * @param rect The view rectangle to be set.
   * @return The rectangle status.
   *
   * "Called to retrieve the view rectangle which is relative to screen
   * c\oordinates. Return **true** if the rectangle was provided."
   * (copying from CEF documentation).
   */
  bool GetViewRect (CefRefPtr<CefBrowser> browser, CefRect &rect) override;

  /**
   * @brief Paint an element.
   *
   * @param browser A browser instance.
   * @param type The element type (view or widget).
   * @param dirtyRects Set of rectangle that need to be repainted.
   * @param buffer The pixel data of whole image.
   * @param width The image width.
   * @param height The image height.
   * @return Nothing.
   *
   * "Called when an element should be painted."
   * (copying from CEF documentation).
   */
  void OnPaint (CefRefPtr<CefBrowser> browser, PaintElementType type,
                const RectList &dirtyRects, const void *buffer, int width,
                int height) override;

private:
  /**
   * @brief Keep the current window width.
   */
  int _width;

  /**
   * @brief Keep the current window height.
   */
  int _height;

  /**
   * @brief Keep the local texture.
   */
  SDL_Texture *_texture;

  /**
   * @brief Keep the current renderer.
   */
  SDL_Renderer *_renderer; //

  /**
   * @brief Provides atomic refcounting implementation.
   *
   * This a MACRO defined by CEF.
   */
  IMPLEMENT_REFCOUNTING (GingaCefHandler)
};

/**
 * @brief A client class of CEF for Ginga.
 *
 * This class is responsible for setting the handlers used by the browser.
 */
class GingaCefClient : public CefClient,
                       public CefLifeSpanHandler,
                       public CefLoadHandler
{
public:
  /**
   * @brief Default constructor for GingaCefClient.
   *
   * @param handler A render handler.
   * @return Nothing.
   */
  GingaCefClient (CefRefPtr<CefRenderHandler> handler);

  /**
   * @brief Default destructor for GingaCefClient.
   *
   * @return Nothing.
   */
  virtual ~GingaCefClient ();

  /**
   * @brief Return the handler for browser life span events.
   *
   * @return The handler for browser life span events.
   */
  virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler () override;

  /**
   * @brief Return the handler for browser load status events.
   *
   * @return The handler for browser load status events.
   */
  virtual CefRefPtr<CefLoadHandler> GetLoadHandler () override;

  /**
   * @brief Return the handler for off-screen rendering events.
   *
   * @return The handler for off-screen rendering events.
   */
  virtual CefRefPtr<CefRenderHandler> GetRenderHandler () override;

private:
  /**
   * @brief Keep the current render handler.
   */
  CefRefPtr<CefRenderHandler> _handler;

  /**
   * @brief Provides atomic refcounting implementation.
   *
   * This a MACRO defined by CEF.
   */
  IMPLEMENT_REFCOUNTING (GingaCefClient)
};

/**
 * @brief A player class for HTML.
 *
 * This class is responsible to present HTML pages. The player use the
 * Chromium Embedded Framework (CEF) together with SDL to render
 * the result in the screen.
 */
class PlayerHTML : public Player, public IEventListener
{
public:
  /**
   * @brief Default constructor for PlayerHTML.
   *
   * @param location An URI for the HTML file.
   * @return Nothing.
   */
  explicit PlayerHTML (const string &location);

  /**
   * @brief Default destructor for PlayerHTML.
   *
   * @return Nothing.
   */
  virtual ~PlayerHTML ();

  /**
   * @brief Start the presentation.
   *
   * @return Nothing.
   * @see displayJobCallbackWrapper() and displayJobCallback().
   *
   * This method adds a new **display job** to start the presentation.
   */
  virtual bool play () override;

  /**
   * @brief Stop the presentation.
   *
   * @return Nothing.
   * @see displayJobCallbackWrapper() and displayJobCallback().
   *
   * This method stop the presentation by finishing the **display job** and
   * cleaning the screen.
   */
  virtual void stop () override;

  /**
   * @brief Process keyboard input events.
   *
   * @param type A SDL event type (SDL_KEYDOWN or SDL_KEYUP).
   * @param key A SDL key code.
   * @return Nothing.
   * @see getPlayerKey().
   *
   * This method translate the key code captured by SDL in the event to the
   * CEF code style and forward the events to the active browser.
   */
  virtual void handleKeyEvent (SDL_EventType type,
                               SDL_Keycode key) override;

  virtual void handleTickEvent (Time, Time, int) override{};

private:
  /**
   * @brief Defines mutex methods.
   *
   * This MACRO is defined in src/ginga.h.
   */
  GINGA_MUTEX_DEFN ()

  /**
   * @brief Defines the job actions to be executed by the renderer.
   *
   * @param job The job data.
   * @param renderer The renderer.
   * @param self The object instance.
   * @return **false** if the job could be removed from the execution list
   * and
   *         **true** otherwise.
   * @see displayJobCallback() and play().
   *
   * Actually, this is just a wrapper to displayJobCallback().
   */
  static bool displayJobCallbackWrapper (DisplayJob *job,
                                         SDL_Renderer *renderer,
                                         void *self);

  /**
   * @brief Defines the job actions to be executed by the renderer.
   *
   * @param job The job data.
   * @param renderer The renderer.
   * @return **false** if the job could be removed from the execution list
   * and
   *         **true** otherwise.
   * @see displayJobCallbackWrapper(), play() and stop().
   *
   * This method create a new browser and load the page to be render in the
   * screen. In case the browser already exist, just process the browser
   * events and update the screen. Return **false** only when stop() is
   * called.
   */
  bool displayJobCallback (DisplayJob *job, SDL_Renderer *renderer);

  /**
   * @brief Process mouse input events.
   *
   * @param type A SDL event type (SDL_MOUSEBUTTONDOWN or SDL_MOUSEBUTTONUP)
   * @param x The X coordinate, relative to window.
   * @param y The Y coordinate, relative to window.
   * @return Nothing.
   *
   * This method forward the events captured by SDL to the active browser.
   */
  void mouseInputCallback (SDL_EventType type, int x, int y);

  /**
   * @brief Translate a SDL key code to a CEF key code.
   *
   * @param key A SDL key code.
   * @return A CEF key code.
   */
  int getPlayerKey (SDL_Keycode key);

  /**
   * @brief Keep the active handler instance, needed to create a new
   *   client instance.
   */
  CefRefPtr<GingaCefHandler> _handler;

  /**
   * @brief Keep the active client instance, needed to create a new
   *   browser instance.
   */
  CefRefPtr<GingaCefClient> _client;

  /**
   * @brief Keep the active browser instance.
   */
  CefRefPtr<CefBrowser> _browser;

  /**
   * @brief Keep execution player status.
   */
  bool _isPlaying;
};

GINGA_NAMESPACE_END

#endif /* HTML_PLAYER_H */
