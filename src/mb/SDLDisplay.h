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

#ifndef SDLDEVICESCREEN_H_
#define SDLDEVICESCREEN_H_

#include "ginga.h"

#include "IContinuousMediaProvider.h"
#include "IDiscreteMediaProvider.h"
#include "IFontProvider.h"
#include "InputManager.h"
#include "SDLEventBuffer.h"
#include "SDLInputEvent.h"
#include "SDLSurface.h"
#include "SDLWindow.h"

GINGA_MB_BEGIN

typedef struct
{
  IMediaProvider *iDec;
  SDL_Surface *uSur;
  SDL_Texture *uTex;
} ReleaseContainer;

class SDLDisplay
{
  // -----------------------------------------------------------------------
private:
  GMutex mutex;                 // sync access to display

  int width;                    // display width in pixels
  int height;                   // display height in pixels
  bool fullscreen;              // true if full-screen mode is on

  SDL_Renderer *renderer;       // display renderer
  SDL_Window *screen;           // display screen
  InputManager *im;             // display input manager (FIXME)

  bool _quit;                   // true if render thread should quit
  GThread *render_thread;       // render thread handle

  GList *windows;               // list of windows to be redrawn
  GList *providers;             // list of providers to be redrawn

  void lock (void);
  void unlock (void);
  void add (GList **, gpointer);
  void remove (GList **, gpointer);

public:
  bool hasQuitted ();
  void quit ();
  void redraw ();

  // -----------------------------------------------------------------------

public:
  /* SDL Underlying Window Tasks*/
  static const short SUW_SHOW = 0;
  static const short SUW_HIDE = 1;
  static const short SUW_RAISETOTOP = 2;
  static const short SUW_LOWERTOBOTTOM = 3;

  static const short SDS_FPS = 35;
  static const int uSleepTime = (int)(1000000 / SDS_FPS);

private:
  static bool hasRenderer;
  static bool mutexInit;
  static map<SDLDisplay *, short> sdlScreens;

  set<SDLWindow *> windowPool;
  set<SDLSurface *> surfacePool;
  set<IContinuousMediaProvider *> cmpPool;
  set<IDiscreteMediaProvider *> dmpPool;



  bool waitingCreator;
  pthread_mutex_t condMutex;
  pthread_cond_t cond;

  static map<int, int> gingaToSDLCodeMap;
  static map<int, int> sdlToGingaCodeMap;
  static map<string, int> sdlStrToSdlCode;

  static set<SDL_Surface *> uSurPool;
  static set<SDL_Texture *> uTexPool;
  static vector<ReleaseContainer *> releaseList;
  static map<int, map<double, set<SDLWindow *> *> *> renderMap;
  static set<IContinuousMediaProvider *> cmpRenderList;

  static pthread_mutex_t sdlMutex; // mutex for SDL structures
  static pthread_mutex_t sieMutex; // mutex for SDL input event Map
  static pthread_mutex_t renMutex; // mutex for C++ STL SDL Render Map
  static pthread_mutex_t scrMutex; // mutex for C++ STL SDL Screens
  static pthread_mutex_t recMutex; // mutex for C++ STL release structures
  static pthread_mutex_t winMutex; // mutex for C++ STL Window
  static pthread_mutex_t surMutex; // mutex for C++ STL Surface
  static pthread_mutex_t proMutex; // mutex for C++ STL Providers
  static pthread_mutex_t
      cstMutex; // mutex for the others C++ STL structures

public:
  SDLDisplay (int width, int height, bool fullscreen);
  virtual ~SDLDisplay ();

private:
  static void checkMutexInit ();

public:
  static void lockSDL ();
  static void unlockSDL ();

  static void updateRenderMap (SDLWindow *window,
                               double oldZIndex, double newZIndex);

  void releaseScreen ();

  void releaseMB ();

  void clearWidgetPools ();

  string getScreenName ();

private:
  void setEmbedFromParent (string parentCoords);

public:
  int getWidthResolution ();
  void setWidthResolution (int wRes);
  int getHeightResolution ();
  void setHeightResolution (int hRes);

  bool mergeIds (SDLWindow* destId, vector<SDLWindow *> *srcIds);
  void blitScreen (SDLSurface *destination);
  void blitScreen (string fileUri);

private:
  void blitScreen (SDL_Surface *destination);

public:
  void refreshScreen ();

  /* interfacing output */

  SDLWindow *createWindow (int x, int y, int w, int h, double z);

  SDLWindow* createUnderlyingSubWindow (int x, int y, int w, int h,
                                                double z);

private:
  SDLWindow* createUnderlyingSubWindow (SDLWindow* parent,
                                                string spec, int x, int y,
                                                int w, int h, double z);

public:
  SDLWindow* getScreenUnderlyingWindow ();

  bool hasWindow (SDLWindow *win);
  void releaseWindow (SDLWindow *win);

  SDLSurface *createSurface ();
  SDLSurface *createSurface (int w, int h);
  SDLSurface *createSurfaceFrom (void *underlyingSurface);
  bool hasSurface (SDLSurface *sur);
  bool releaseSurface (SDLSurface *sur);

  /* interfacing content */

  IContinuousMediaProvider *createContinuousMediaProvider (const char *mrl,
                                                           bool isRemote);

  void releaseContinuousMediaProvider (IContinuousMediaProvider *provider);

  IFontProvider *createFontProvider (const char *mrl, int fontSize);

  void releaseFontProvider (IFontProvider *provider);

  SDLSurface *createRenderedSurfaceFromImageFile (const char *mrl);

  static void addCMPToRendererList (IContinuousMediaProvider *cmp);
  static void removeCMPToRendererList (IContinuousMediaProvider *cmp);

  static void createReleaseContainer (SDL_Surface *uSur, SDL_Texture *uTex,
                                      IMediaProvider *iDec);

public:
  static void refreshRC (SDLDisplay *screen);
  static void refreshCMP (SDLDisplay *screen);
  static void refreshWin (SDLDisplay *screen);

private:
  static void initScreen (SDLDisplay *screen);
  static void clearScreen (SDLDisplay *screen);
  static void releaseScreen (SDLDisplay *screen);

  static void releaseAll ();

  static void initCMP (SDLDisplay *screen,
                       IContinuousMediaProvider *cmp);

  static bool blitFromWindow (SDLWindow *iWin, SDL_Surface *dest);

public:
  /* interfacing input */

  InputManager *getInputManager ();

  SDLEventBuffer *createEventBuffer ();

  SDLInputEvent *createInputEvent (void *event, const int symbol);
  SDLInputEvent *createApplicationEvent (int type, void *data);

  int fromMBToGinga (int keyCode);
  int fromGingaToMB (int keyCode);

  /* interfacing underlying multimedia system */

  void *getGfxRoot ();

  /* SDL MB internal use*/
private:
  /* input */
  static int convertEventCodeStrToInt (string strEvent);
  static void initCodeMaps ();
  static bool checkEventFocus (SDLDisplay *s);

public:
  /* output */
  static void renderMapInsertWindow (SDLWindow *iWin, double z);
  static void renderMapRemoveWindow (SDLWindow *iWin, double z);

private:
  static void removeFromWindowList (vector<SDLWindow *> *windows,
                                    SDLWindow *win);

public:
  static SDL_Window *getUnderlyingWindow (SDLWindow* winId);

private:
  static bool drawSDLWindow (SDL_Renderer *renderer, SDL_Texture *texture,
                             SDLWindow *iWin);

  static void insertWindowFromRenderList (SDLWindow *win,
                                          vector<SDLWindow *> *windows);

  static void removeWindowFromRenderList (SDLWindow *win,
                                          vector<SDLWindow *> *windows);

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

// Global screen manager.
extern SDLDisplay *_Ginga_Display;
#define Ginga_Display                                           \
  GINGA_ASSERT_GLOBAL_NONNULL (_Ginga_Display, SDLDisplay *)

GINGA_MB_END

#endif /*SDLDEVICESCREEN_H_*/
