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

#ifndef DISPLAY_H
#define DISPLAY_H

#include "ginga.h"

#include "IEventListener.h"

#include "SDLWindow.h"
#include "IKeyInputEventListener.h"
#include "IMouseEventListener.h"

#include "Dashboard.h"

#include "player/Player.h"
using namespace ::ginga::player;

#include "formatter/NclExecutionObject.h"
using namespace ::ginga::formatter;

GINGA_MB_BEGIN

// Type used for renderer job data.
typedef struct _DisplayJob DisplayJob;

// Type used for renderer job callbacks.
// Return true to keep job on the display job list.
typedef bool (*DisplayJobCallback) (DisplayJob *, SDL_Renderer *, void *);

class Display
{
private:
  GINGA_MUTEX_DEFN ();
  int width;                    // display width in pixels
  int height;                   // display height in pixels
  double fps;                   // target frame-rate
  bool fullscreen;              // full-screen mode
  bool _quit;                   // true if render thread should quit
  Dashboard *dashboard;         // control panel

  GList *jobs;                  // list of jobs to be executed by renderer
  GList *listeners;             // list of listeners to be notified
  GList *players;               // list of players to be ticked
  GList *textures;              // list of textures to be destructed

  SDL_Window *screen;           // display screen
  SDL_Renderer *renderer;       // display renderer

  set<IKeyInputEventListener*> keyEventListeners; // key event listeners
  set<IMouseEventListener*> mouseEventListeners;  // mouse event listeners
  set<NclExecutionObject*> timeAnchorListeners;   // time anchor listeners

  bool add (GList **, gpointer);
  bool remove (GList **, gpointer);
  bool find (GList *, gconstpointer);

  void notifyListeners (GingaTime, GingaTime, int);

public:
  Display (int, int, double, bool);
  ~Display ();

  double getFPS ();
  void setFPS (double);
  bool getFullscreen ();
  void setFullscreen (bool);
  void getSize (int *, int *);
  void setSize (int, int);

  void quit ();
  bool hasQuitted ();

  DisplayJob *addJob (DisplayJobCallback, void *);
  bool removeJob (DisplayJob *);

  bool registerEventListener (IEventListener *);
  bool unregisterEventListener (IEventListener *);

  void destroyTexture (SDL_Texture *);
  void renderLoop (void);

  // -----------------------------------------------------------------------


  // Players.
  void registerPlayer (Player *);
  void unregisterPlayer (Player *);

  // key event listeners
  void registerKeyEventListener(IKeyInputEventListener*);
  void unregisterKeyEventListener(IKeyInputEventListener*);
  void postKeyInputEventListener(SDL_Keycode); // gambi used by formatterFocusManager listener

  // mouse event listeners
  void registerMouseEventListener(IMouseEventListener*);
  void unregisterMouseEventListener(IMouseEventListener*);

  // time anchors listeners
  void registerTimeAnchorListener(NclExecutionObject*);
  void unregisterTimeAnchorListener(NclExecutionObject*);

private:
  void notifyKeyEventListeners(SDL_EventType, SDL_Keycode);
  void notifyMouseEventListeners(SDL_EventType);
  void notifyTimeAnchorListeners();

};

// Global display.
extern Display *_Ginga_Display;
#define Ginga_Display\
  GINGA_ASSERT_GLOBAL_NONNULL (_Ginga_Display, ::ginga::mb::Display *)

GINGA_MB_END

#endif /* DISPLAY_H */
