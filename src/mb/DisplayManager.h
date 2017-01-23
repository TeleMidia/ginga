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

#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include "ginga.h"

#include "SDLDisplay.h"
#include "InputManager.h"

#include "SDLWindow.h"
#include "SDLSurface.h"

#include "IContinuousMediaProvider.h"
#include "IFontProvider.h"
#include "IImageProvider.h"

#include "InputManager.h"
#include "SDLInputEvent.h"
#include "SDLEventBuffer.h"

#include "system/Thread.h"
using namespace ::ginga::system;

GINGA_MB_BEGIN

class DisplayManager
{
public:
  DisplayManager ();

protected:
  map<GingaScreenID, SDLDisplay *> screens;

  pthread_mutex_t mapMutex;
  pthread_mutex_t genMutex;

  map<GingaSurfaceID, SDLSurface *> surMap;
  pthread_mutex_t surMapMutex;

  map<GingaProviderID, IMediaProvider *> provMap;
  pthread_mutex_t provMapMutex;

  static set<IInputEventListener *> iListeners;
  static pthread_mutex_t ilMutex;

  static set<IMotionEventListener *> mListeners;
  static pthread_mutex_t mlMutex;

  static bool initMutex;
  bool running;

  bool isWaiting;
  pthread_cond_t wsSignal;
  pthread_mutex_t wsMutex;
  GingaScreenID waitingRefreshScreen;

  static void checkInitMutex ();

public:
  virtual ~DisplayManager ();

  void releaseHandler ();

  static void addIEListenerInstance (IInputEventListener *listener);

  static void removeIEListenerInstance (IInputEventListener *listener);

  static bool hasIEListenerInstance (IInputEventListener *listener,
                                     bool removeInstance = false);

  static void addMEListenerInstance (IMotionEventListener *listener);

  static void removeMEListenerInstance (IMotionEventListener *listener);

  static bool hasMEListenerInstance (IMotionEventListener *listener,
                                     bool removeInstance = false);

  void setBackgroundImage (GingaScreenID screenId, string uri);

  int getDeviceWidth (GingaScreenID screenId);

  int getDeviceHeight (GingaScreenID screenId);

  void *getGfxRoot (GingaScreenID screenId);

  void releaseScreen (GingaScreenID screenId);
  void releaseMB (GingaScreenID screenId);
  void clearWidgetPools (GingaScreenID screenId);

  GingaScreenID createScreen (int argc, char **args);

protected:
  GingaScreenID createScreen (string vMode, string vParent, string vEmbed,
                              bool externalRenderer, bool useStdin);

public:
  string getScreenName (GingaScreenID screenId);
  UnderlyingWindowID getScreenUnderlyingWindow (GingaScreenID screenId);

protected:
  GingaSurfaceID provIdRefCounter;

public:
  IMediaProvider *getIMediaProviderFromId (const GingaProviderID &provId);
  SDLSurface *getISurfaceFromId (const GingaSurfaceID &surfaceId);

  SDLWindow *getIWindowFromId (GingaScreenID screenId, GingaWindowID winId);

  bool mergeIds (GingaScreenID screenId, GingaWindowID destId,
                 vector<GingaWindowID> *srcIds);

  void blitScreen (GingaScreenID screenId, SDLSurface *destination);
  void blitScreen (GingaScreenID screenId, string fileUri);
  void refreshScreen (GingaScreenID screenId);

  // Interfacing output.
  GingaWindowID createWindow (GingaScreenID screenId, int x, int y, int w,
                              int h, double z);

  UnderlyingWindowID createUnderlyingSubWindow (GingaScreenID screenId,
                                                int x, int y, int w, int h,
                                                double z);

  bool hasWindow (GingaScreenID screenId, GingaWindowID window);

  void releaseWindow (GingaScreenID screenId, SDLWindow *window);

  void registerSurface (SDLSurface *);

  GingaSurfaceID createSurface (GingaScreenID screenId);

  GingaSurfaceID createSurface (GingaScreenID screenId, int w, int h);

  GingaSurfaceID createSurfaceFrom (GingaScreenID screenId,
                                    GingaSurfaceID underlyingSurface);

  bool hasSurface (const GingaScreenID &screenId,
                   const GingaSurfaceID &surId);
  bool releaseSurface (GingaScreenID screenId, SDLSurface *surface);

  void lowerWindowToBottom (const GingaScreenID &screenId,
                            const GingaWindowID &winId);

  /* Interfacing content */
  GingaProviderID createContinuousMediaProvider (GingaScreenID screenId,
                                                 const char *mrl,
                                                 bool isRemote);

  void releaseContinuousMediaProvider (GingaScreenID screenId,
                                       GingaProviderID provider);

  GingaProviderID createFontProvider (GingaScreenID screenId,
                                      const char *mrl, int fontSize);

  void releaseFontProvider (GingaScreenID screenId,
                            GingaProviderID provider);

  GingaProviderID createImageProvider (GingaScreenID screenId,
                                       const char *mrl);

  void releaseImageProvider (GingaScreenID screenId,
                             GingaProviderID provider);

  GingaSurfaceID createRenderedSurfaceFromImageFile (GingaScreenID screenId,
                                                     const char *mrl);

public:
  InputManager *getInputManager (GingaScreenID screenId);
  SDLEventBuffer *createEventBuffer (GingaScreenID screenId);
  SDLInputEvent *createInputEvent (GingaScreenID screenId, void *event,
                                   const int symbol);

  SDLInputEvent *createApplicationEvent (GingaScreenID screenId, int type,
                                         void *data);

  int fromMBToGinga (GingaScreenID screenId, int keyCode);
  int fromGingaToMB (GingaScreenID screenId, int keyCode);

  // windows
  void addWindowCaps (const GingaScreenID &screenId,
                      const GingaWindowID &winId, int caps);
  void setWindowCaps (const GingaScreenID &screenId,
                      const GingaWindowID &winId, int caps);
  int getWindowCap (const GingaScreenID &screenId,
                    const GingaWindowID &winId, const string &capName);

  void drawWindow (const GingaScreenID &screenId,
                   const GingaWindowID &winId);
  void setWindowBounds (const GingaScreenID &screenId,
                        const GingaWindowID &winId, int x, int y, int w,
                        int h);
  void showWindow (const GingaScreenID &screenId,
                   const GingaWindowID &winId);
  void hideWindow (const GingaScreenID &screenId,
                   const GingaWindowID &winId);
  void raiseWindowToTop (const GingaScreenID &screenId,
                         const GingaWindowID &winId);
  void renderWindowFrom (const GingaScreenID &screenId,
                         const GingaWindowID &winId,
                         const GingaSurfaceID &surId);

  void setWindowBgColor (const GingaScreenID &screenId,
                         const GingaWindowID &winId, guint8 r, guint8 g,
                         guint8 b, guint8 alpha);

  void setWindowBorder (const GingaScreenID &screenId,
                        const GingaWindowID &winId, guint8 r, guint8 g,
                        guint8 b, guint8 alpha, int width);

  void setWindowCurrentTransparency (const GingaScreenID &screenId,
                                     const GingaWindowID &winId,
                                     guint8 transparency);
  void setWindowColorKey (const GingaScreenID &screenId,
                          const GingaWindowID &winId, guint8 r, guint8 g,
                          guint8 b);

  void setWindowX (const GingaScreenID &screenId,
                   const GingaWindowID &winId, int x);

  void setWindowY (const GingaScreenID &screenId,
                   const GingaWindowID &winId, int y);

  void setWindowW (const GingaScreenID &screenId,
                   const GingaWindowID &winId, int w);

  void setWindowH (const GingaScreenID &screenId,
                   const GingaWindowID &winId, int h);

  void setWindowZ (const GingaScreenID &screenId,
                   const GingaWindowID &winId, double z);

  void disposeWindow (const GingaScreenID &screenId,
                      const GingaWindowID &winId);

  void setGhostWindow (const GingaScreenID &screenId,
                       const GingaWindowID &winId, bool ghost);

  void validateWindow (const GingaScreenID &screenId,
                       const GingaWindowID &winId);

  int getWindowX (const GingaScreenID &screenId,
                  const GingaWindowID &winId);
  int getWindowY (const GingaScreenID &screenId,
                  const GingaWindowID &winId);
  int getWindowW (const GingaScreenID &screenId,
                  const GingaWindowID &winId);
  int getWindowH (const GingaScreenID &screenId,
                  const GingaWindowID &winId);
  double getWindowZ (const GingaScreenID &screenId,
                     const GingaWindowID &winId);
  guint8 getWindowTransparencyValue (const GingaScreenID &screenId,
                                     const GingaWindowID &winId);
  void resizeWindow (const GingaScreenID &screenId,
                     const GingaWindowID &winId, int width, int height);
  string getWindowDumpFileUri (const GingaScreenID &screenId,
                               const GingaWindowID &winId, int quality,
                               int dumpW, int dumpH);
  void clearWindowContent (const GingaScreenID &screenId,
                           const GingaWindowID &winId);
  void revertWindowContent (const GingaScreenID &screenId,
                            const GingaWindowID &winId);
  void deleteWindow (const GingaScreenID &screenId,
                     const GingaWindowID &winId);
  void moveWindowTo (const GingaScreenID &screenId,
                     const GingaWindowID &winId, int x, int y);
  void setWindowMirrorSrc (const GingaScreenID &screenId,
                           const GingaWindowID &winId,
                           const GingaWindowID &mirrorSrc);
  // surfaces
  void *getSurfaceContent (const GingaSurfaceID &surId);
  GingaWindowID getSurfaceParentWindow (const GingaSurfaceID &surId);
  void deleteSurface (const GingaSurfaceID &surId);
  bool setSurfaceParentWindow (const GingaScreenID &screenId,
                               const GingaSurfaceID &surId,
                               const GingaWindowID &winId);
  void clearSurfaceContent (const GingaSurfaceID &surId);
  void getSurfaceSize (const GingaSurfaceID &surId, int *width,
                       int *height);
  void addSurfaceCaps (const GingaSurfaceID &surId, const int caps);
  void setSurfaceCaps (const GingaSurfaceID &surId, const int caps);
  int getSurfaceCap (const GingaSurfaceID &surId, const string &cap);
  int getSurfaceCaps (const GingaSurfaceID &surId);
  void setSurfaceBgColor (const GingaSurfaceID &surId, guint8 r, guint8 g,
                          guint8 b, guint8 alpha);
  void setSurfaceFont (const GingaSurfaceID &surId, GingaSurfaceID font);
  void setColor (const GingaSurfaceID &surId, guint8 r, guint8 g, guint8 b,
                 guint8 alpha);
  void setExternalHandler (const GingaSurfaceID &surId, bool extHandler);
  void blitSurface (const GingaSurfaceID &surId, int x, int y,
                    GingaSurfaceID src = 0, int srcX = -1, int srcY = -1,
                    int srcW = -1, int srcH = -1);
  void flipSurface (const GingaSurfaceID &surId);
  void setSurfaceContent (const GingaSurfaceID &surId, void *surface);
  Color *getSurfaceColor (const GingaSurfaceID &surId);
  bool hasSurfaceExternalHandler (const GingaSurfaceID &surId);
  void setSurfaceColor (const GingaSurfaceID &surId, guint8 r, guint8 g,
                        guint8 b, guint8 alpha);
  // providers
  void setProviderSoundLevel (const GingaProviderID &provId, double level);
  void getProviderOriginalResolution (const GingaProviderID &provId,
                                      int *width, int *height);
  double getProviderTotalMediaTime (const GingaProviderID &provId);
  double getProviderSoundLevel (const GingaProviderID &provId);
  int64_t getProviderVPts (const GingaProviderID &provId);
  void setProviderMediaTime (const GingaProviderID &provId, double pos);
  double getProviderMediaTime (const GingaProviderID &provId);
  void pauseProvider (const GingaProviderID &provId);
  void stopProvider (const GingaProviderID &provId);
  void resumeProvider (const GingaProviderID &provId,
                       GingaSurfaceID surface);
  void setProviderAVPid (const GingaProviderID &provId, int aPid, int vPid);
  void feedProviderBuffers (const GingaProviderID &provId);
  bool checkProviderVideoResizeEvent (const GingaProviderID &provId,
                                      const GingaSurfaceID &frame);
  int getProviderStringWidth (const GingaProviderID &provId,
                              const char *text, int textLength = 0);
  void playProviderOver (const GingaProviderID &provId,
                         const GingaSurfaceID &surface);
  void playProviderOver (const GingaProviderID &provId,
                         const GingaSurfaceID &surface, const char *text,
                         int x, int y, short align);
  int getProviderHeight (const GingaProviderID &provId);

protected:
  void addScreen (GingaScreenID screenId, SDLDisplay *screen);
  short getNumOfScreens ();
  bool getScreen (GingaScreenID screenId, SDLDisplay **screen);
  bool removeScreen (GingaScreenID screenId);
  void lockScreenMap ();
  void unlockScreenMap ();
  void lock ();
  void unlock ();
};

// Global screen manager.
extern DisplayManager *_Ginga_Display;
#define Ginga_Display                                                      \
  GINGA_ASSERT_GLOBAL_NONNULL (_Ginga_Display, DisplayManager *)

GINGA_MB_END

#endif /* DISPLAY_MANAGER_H*/
