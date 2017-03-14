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

#include "IContinuousMediaProvider.h"
#include "IDiscreteMediaProvider.h"
//#include "IFontProvider.h" deleted
#include "InputManager.h"
#include "InputEvent.h"
#include "SDLSurface.h"
#include "SDLWindow.h"

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
  bool fullscreen;              // true if full-screen mode is on
  guint32 frameTime;            //frame time rate

  SDL_Window *screen;           // display screen
  GRecMutex renderer_mutex;     // sync access to renderer
  SDL_Renderer *renderer;       // display renderer
  InputManager *im;             // display input manager (FIXME)

  bool _quit;                   // true if render thread should quit
  

  GList *jobs;                  // list of jobs to be executed by renderer
  GList *textures;              // list of textures to be destructed
  GList *windows;               // list of windows to be redrawn
  GList *providers;             // list of providers to be redrawn

  gpointer add (GList **, gpointer);
  gpointer remove (GList **, gpointer);
  gboolean find (GList *, gconstpointer);

public:
  Display (int, int, bool, gdouble);
  ~Display ();

  void getSize (int *, int *);
  void setSize (int, int);
  bool getFullscreen ();
  void setFullscreen (bool);

  void quit ();
  bool hasQuitted ();

  DisplayJob *addJob (DisplayJobCallback, void *);
  bool removeJob (DisplayJob *);
  void destroyTexture (SDL_Texture *);

  SDLWindow *createWindow (int, int, int, int, int);
  bool hasWindow (const SDLWindow *);
  void destroyWindow (SDLWindow *);

  IContinuousMediaProvider *createContinuousMediaProvider (const string&);
  void destroyContinuousMediaProvider (IContinuousMediaProvider *);
  
  void renderLoop (void);

  // Let the clutter begin -------------------------------------------------

private:
  static bool mutexInitialized;

  set<SDLSurface *> surfacePool;
  set<IContinuousMediaProvider *> cmpPool;
  set<IDiscreteMediaProvider *> dmpPool;

 

  static map<Key::KeyCode, int> gingaToSDLCodeMap;
  static map<int, Key::KeyCode> sdlToGingaCodeMap;
  static map<string, int> sdlStrToSdlCode;

  static set<SDL_Surface *> uSurPool;
  static set<SDL_Texture *> uTexPool;
  static map<int, map<double, set<SDLWindow *> *> *> renderMap;
  static set<IContinuousMediaProvider *> cmpRenderList;

private:
  static void checkMutexInit ();

public:
  static void lockSDL ();
  static void unlockSDL ();

  /* interfacing output */
public:
  SDLSurface *createSurface ();
  SDLSurface *createSurface (int w, int h);
  SDLSurface *createSurfaceFrom (void *underlyingSurface);
  bool hasSurface (SDLSurface *sur);
  bool releaseSurface (SDLSurface *sur);

  /* interfacing content */
  SDLSurface *createRenderedSurfaceFromImageFile (const char *mrl);

public:
  /* interfacing input */

  InputManager *getInputManager ();
  InputEvent *createInputEvent (void *event, const int symbol);
  InputEvent *createApplicationEvent (int type, void *data);

  Key::KeyCode fromMBToGinga (int keyCode);
  int fromGingaToMB (Key::KeyCode keyCode);

  /* SDL MB internal use*/
private:
  /* input */
  static int convertEventCodeStrToInt (const string &strEvent);
  static void initCodeMaps ();
  static bool checkEventFocus (Display *s);

public:
  static SDL_Window *getUnderlyingWindow (SDLWindow* winId);

public:
  /* CAUTION: call this method only from main SDL thread */
  static SDL_Texture *createTextureFromSurface (SDL_Renderer *renderer,
                                                SDL_Surface *surface);

private:
  static SDL_Texture *createTexture (SDL_Renderer *renderer, int w, int h);

public:
  static bool hasTexture (SDL_Texture *uTex);
  static void releaseTexture (SDL_Texture *uTex);

  static void addUnderlyingSurface (SDL_Surface *uSur);
  static SDL_Surface *createUnderlyingSurface (int width, int height);

  static SDL_Surface *
  createUnderlyingSurfaceFromTexture (SDL_Texture *texture);

  static bool hasUnderlyingSurface (SDL_Surface *uSur);

private:
  static void releaseUnderlyingSurface (SDL_Surface *uSur);
};

// Global display.
extern Display *_Ginga_Display;
#define Ginga_Display\
  GINGA_ASSERT_GLOBAL_NONNULL (_Ginga_Display, ::ginga::mb::Display *)

GINGA_MB_END

#endif /* DISPLAY_H */
