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

#include "config.h"
#include "CodeMap.h"
#include "DisplayManager.h"
#include "InputManager.h"
#include "SDLDeviceScreen.h"

GINGA_MB_BEGIN

// Global display manager; initialized in by main().
DisplayManager *_G_DisplayManager = NULL;

set<IInputEventListener *> DisplayManager::iListeners;
pthread_mutex_t DisplayManager::ilMutex;

set<IMotionEventListener *> DisplayManager::mListeners;
pthread_mutex_t DisplayManager::mlMutex;

bool DisplayManager::initMutex = false;

DisplayManager::DisplayManager ()
{
  Thread::mutexInit (&mapMutex);
  Thread::mutexInit (&genMutex);
  Thread::mutexInit (&surMapMutex);
  Thread::mutexInit (&provMapMutex);

  waitingRefreshScreen = false;
  running = false;
  isWaiting = false;

  provIdRefCounter = 1;

  Thread::condInit (&wsSignal, NULL);
  Thread::mutexInit (&wsMutex);
}

DisplayManager::~DisplayManager ()
{
  map<GingaScreenID, SDLDeviceScreen *>::iterator i;

  lockScreenMap ();
  i = screens.begin ();
  while (i != screens.end ())
    {
      delete i->second;
      ++i;
    }
  screens.clear ();
  unlockScreenMap ();
  Thread::mutexDestroy (&mapMutex);

  lock ();
  unlock ();
  Thread::mutexDestroy (&genMutex);

  map<GingaSurfaceID, SDLSurface *>::iterator j;

  Thread::mutexLock (&surMapMutex);
  j = surMap.begin ();
  while (j != surMap.end ())
    {
      delete j->second;
      ++j;
    }
  surMap.clear ();
  Thread::mutexUnlock (&surMapMutex);
  Thread::mutexDestroy (&surMapMutex);

  map<GingaProviderID, IMediaProvider *>::iterator k;

  Thread::mutexLock (&provMapMutex);
  k = provMap.begin ();
  while (k != provMap.end ())
    {
      delete k->second;
      ++k;
    }
  provMap.clear ();
  Thread::mutexUnlock (&provMapMutex);
  Thread::mutexDestroy (&provMapMutex);
}

void
DisplayManager::checkInitMutex ()
{
  if (!initMutex)
    {
      initMutex = true;

      Thread::mutexInit (&ilMutex);
      iListeners.clear ();

      Thread::mutexInit (&mlMutex);
      mListeners.clear ();
    }
}

void
DisplayManager::addIEListenerInstance (IInputEventListener *listener)
{

  checkInitMutex ();
  Thread::mutexLock (&ilMutex);
  iListeners.insert (listener);
  Thread::mutexUnlock (&ilMutex);
}

void
DisplayManager::removeIEListenerInstance (IInputEventListener *listener)
{

  set<IInputEventListener *>::iterator i;

  checkInitMutex ();
  Thread::mutexLock (&ilMutex);
  i = iListeners.find (listener);
  if (i != iListeners.end ())
    {
      iListeners.erase (i);
    }
  Thread::mutexUnlock (&ilMutex);
}

bool
DisplayManager::hasIEListenerInstance (IInputEventListener *listener,
                                           bool removeInstance)
{

  set<IInputEventListener *>::iterator i;
  bool hasListener = false;

  checkInitMutex ();
  Thread::mutexLock (&ilMutex);
  i = iListeners.find (listener);
  if (i != iListeners.end ())
    {
      hasListener = true;

      if (removeInstance)
        {
          iListeners.erase (i);
        }
    }
  Thread::mutexUnlock (&ilMutex);

  return hasListener;
}

void
DisplayManager::addMEListenerInstance (IMotionEventListener *listener)
{

  checkInitMutex ();
  Thread::mutexLock (&mlMutex);
  mListeners.insert (listener);
  Thread::mutexUnlock (&mlMutex);
}

void
DisplayManager::removeMEListenerInstance (
    IMotionEventListener *listener)
{

  set<IMotionEventListener *>::iterator i;

  checkInitMutex ();
  Thread::mutexLock (&mlMutex);
  i = mListeners.find (listener);
  if (i != mListeners.end ())
    {
      mListeners.erase (i);
    }
  Thread::mutexUnlock (&mlMutex);
}

bool
DisplayManager::hasMEListenerInstance (IMotionEventListener *listener,
                                           bool removeInstance)
{

  set<IMotionEventListener *>::iterator i;
  bool hasListener = false;

  checkInitMutex ();
  Thread::mutexLock (&mlMutex);
  i = mListeners.find (listener);
  if (i != mListeners.end ())
    {
      hasListener = true;

      if (removeInstance)
        {
          mListeners.erase (i);
        }
    }
  Thread::mutexUnlock (&mlMutex);

  return hasListener;
}

void
DisplayManager::setBackgroundImage (GingaScreenID screenId, string uri)
{

  SDLDeviceScreen *screen;

  if (getScreen (screenId, &screen))
    {
      screen->setBackgroundImage (uri);
    }
}

int
DisplayManager::getDeviceWidth (GingaScreenID screenId)
{
  SDLDeviceScreen *screen;
  int width = 0;

  if (getScreen (screenId, &screen))
    {
      width = screen->getWidthResolution ();
    }

  return width;
}

int
DisplayManager::getDeviceHeight (GingaScreenID screenId)
{
  SDLDeviceScreen *screen;
  int height = 0;

  if (getScreen (screenId, &screen))
    {
      height = screen->getHeightResolution ();
    }

  return height;
}

void *
DisplayManager::getGfxRoot (GingaScreenID screenId)
{
  SDLDeviceScreen *screen;
  void *gfxRoot = NULL;

  if (getScreen (screenId, &screen))
    {
      gfxRoot = screen->getGfxRoot ();
    }

  return gfxRoot;
}

void
DisplayManager::releaseScreen (GingaScreenID screenId)
{
  SDLDeviceScreen *screen;

  if (getScreen (screenId, &screen))
    {
      removeScreen (screenId);
      screen->releaseScreen ();
      delete screen;
    }
}

void
DisplayManager::releaseMB (GingaScreenID screenId)
{
  SDLDeviceScreen *screen;

  if (getScreen (screenId, &screen))
    {
      screen->releaseMB ();
    }
}

void
DisplayManager::clearWidgetPools (GingaScreenID screenId)
{
  SDLDeviceScreen *screen;

  if (getScreen (screenId, &screen))
    {
      screen->clearWidgetPools ();
    }
}

GingaScreenID
DisplayManager::createScreen (int argc, char **args)
{
  int i;
  bool externalRenderer = false;
  bool useStdin = false;
  string vSystem = "", vSubSystem = "", vMode = "";
  string vParent = "", vEmbed = "";
  string aSystem = "";
  GingaScreenID newScreen;

  clog << "DisplayManager::createScreen argv[";
  clog << argc << "]" << endl;

  lock ();
  for (i = 0; i < argc; i++)
    {
      if ((strcmp (args[i], "--vsystem") == 0) && ((i + 1) < argc))
        {
          vSystem.assign (args[i + 1]);
        }
      else if ((strcmp (args[i], "--vsubsystem") == 0) && ((i + 1) < argc))
        {

          vSubSystem.assign (args[i + 1]);
        }
      else if ((strcmp (args[i], "--vmode") == 0) && ((i + 1) < argc))
        {
          vMode.assign (args[i + 1]);
        }
      else if ((strcmp (args[i], "--parent") == 0) && ((i + 1) < argc))
        {
          vParent.assign (args[i + 1]);
        }
      else if ((strcmp (args[i], "--embed") == 0
                || strcmp (args[i], "--wid") == 0)
               && ((i + 1) < argc))
        {

          vEmbed.assign (args[i + 1]);
        }
      else if ((strcmp (args[i], "--asystem") == 0) && ((i + 1) < argc))
        {
          aSystem.assign (args[i + 1]);
        }
      else if (strcmp (args[i], "--external-renderer") == 0)
        {
          externalRenderer = true;
        }
      else if (strcmp (args[i], "--poll-stdin") == 0)
        {
          useStdin = true;
        }

      clog << "DisplayManager::createScreen PARSER argv[";
      clog << i << "] = '" << args[i] << "'" << endl;
    }

  newScreen = createScreen (vSystem, vSubSystem, vMode, vParent, vEmbed,
                            aSystem, externalRenderer, useStdin);

  unlock ();

  return newScreen;
}

GingaScreenID
DisplayManager::createScreen (string vSystem, string vSubSystem,
                                  string vMode, string vParent,
                                  string vEmbed, string aSystem,
                                  bool externalRenderer, bool useStdin)
{

  SDLDeviceScreen *screen = NULL;
  UnderlyingWindowID embedWin = NULL;
  int argc = 0;

  short sysType;
  GingaScreenID screenId;
  char *mbArgs[64];

  string params = "";
  string paramsSfx = "";
  string mycmd = "ginga";

  screenId = getNumOfScreens ();
  mbArgs[argc] = (char *)mycmd.c_str ();
  argc++;

  if (vMode != "")
    {
      mbArgs[argc] = (char *)"mode";
      argc++;

      mbArgs[argc] = (char *)vMode.c_str ();
      argc++;
    }

  if (vParent != "")
    {
      mbArgs[argc] = (char *)"parent";
      argc++;

      mbArgs[argc] = (char *)vParent.c_str ();
      argc++;

      clog << "DisplayManager::createScreen parent with ";
      clog << "following data '" << vParent << "'";
      clog << endl;
    }

  if (vEmbed != "")
    {
      embedWin = (void *)strtoul (vEmbed.c_str (), NULL, 10);

      clog << "DisplayManager::createScreen embed src = ";
      clog << vEmbed << "' and dst = '" << embedWin << "'";
      clog << endl;
    }

  if (useStdin)
    {
      mbArgs[argc] = (char *)"poll-stdin";
      argc++;
    }

  screen = new SDLDeviceScreen (argc, mbArgs, screenId, embedWin,
                                externalRenderer);
  g_assert_nonnull (screen);
  addScreen (screenId, screen);

  return screenId;
}

string
DisplayManager::getScreenName (GingaScreenID screenId)
{
  SDLDeviceScreen *screen;
  string screenName = "";

  if (getScreen (screenId, &screen))
    {
      screenName = screen->getScreenName ();
    }

  return screenName;
}

UnderlyingWindowID
DisplayManager::getScreenUnderlyingWindow (GingaScreenID screenId)
{

  SDLDeviceScreen *screen;
  UnderlyingWindowID uWin = NULL;

  if (getScreen (screenId, &screen))
    {
      uWin = screen->getScreenUnderlyingWindow ();
    }

  return uWin;
}

SDLWindow *
DisplayManager::getIWindowFromId (GingaScreenID screenId,
                                      GingaWindowID winId)
{

  SDLDeviceScreen *screen;
  SDLWindow *window = NULL;

  if (getScreen (screenId, &screen))
    {
      window = screen->getIWindowFromId (winId);
    }

  return window;
}

bool
DisplayManager::mergeIds (GingaScreenID screenId, GingaWindowID destId,
                              vector<GingaWindowID> *srcIds)
{

  SDLDeviceScreen *screen;

  if (getScreen (screenId, &screen))
    {
      return screen->mergeIds (destId, srcIds);
    }

  return false;
}

void
DisplayManager::blitScreen (GingaScreenID screenId,
                                SDLSurface *destination)
{

  SDLDeviceScreen *screen;

  if (getScreen (screenId, &screen))
    {
      screen->blitScreen (destination);
    }
}

void
DisplayManager::blitScreen (GingaScreenID screenId, string fileUri)
{

  SDLDeviceScreen *screen;

  if (getScreen (screenId, &screen))
    {
      screen->blitScreen (fileUri);
    }
}

void
DisplayManager::refreshScreen (GingaScreenID screenId)
{
  SDLDeviceScreen *screen;

  if (getScreen (screenId, &screen))
    {
      screen->refreshScreen ();
    }
}

/* interfacing output */

GingaWindowID
DisplayManager::createWindow (GingaScreenID screenId, int x, int y,
                                  int w, int h, float z)
{

  SDLDeviceScreen *screen;
  SDLWindow *window = NULL;

  if (getScreen (screenId, &screen))
    {
      window = screen->createWindow (x, y, w, h, z);
    }
  else
    {
      clog << "DisplayManager::createWindow Warning! ";
      clog << "can't find screen '" << screenId << "'" << endl;
    }

  return window->getId ();
}

UnderlyingWindowID
DisplayManager::createUnderlyingSubWindow (GingaScreenID screenId,
                                               int x, int y, int w, int h,
                                               float z)
{

  SDLDeviceScreen *screen;
  UnderlyingWindowID window = NULL;

  if (getScreen (screenId, &screen))
    {
      window = screen->createUnderlyingSubWindow (x, y, w, h, z);
    }
  else
    {
      clog << "DisplayManager::createWindow Warning! ";
      clog << "can't find screen '" << screenId << "'" << endl;
    }

  return window;
}

bool
DisplayManager::hasWindow (GingaScreenID screenId, GingaWindowID winId)
{

  SDLWindow *window;
  SDLDeviceScreen *screen;
  bool hasWin = false;

  window = getIWindowFromId (screenId, winId);
  if (getScreen (screenId, &screen))
    {
      hasWin = screen->hasWindow (window);
    }

  return hasWin;
}

void
DisplayManager::releaseWindow (GingaScreenID screenId, SDLWindow *win)
{

  SDLDeviceScreen *screen;

  if (getScreen (screenId, &screen))
    {
      screen->releaseWindow (win);
    }
}

void
DisplayManager::registerSurface (SDLSurface *surface)
{
  if (surface != NULL)
    {
      Thread::mutexLock (&surMapMutex);
      surMap[surface->getId ()] = surface;
      Thread::mutexUnlock (&surMapMutex);
    }
}

GingaSurfaceID
DisplayManager::createSurface (GingaScreenID screenId)
{
  SDLDeviceScreen *screen;
  SDLSurface *surface = NULL;
  GingaSurfaceID surId = 0;

  if (getScreen (screenId, &screen))
    {
      surface = screen->createSurface ();
      surId = surface->getId ();

      Thread::mutexLock (&surMapMutex);
      surMap[surId] = surface;
      Thread::mutexUnlock (&surMapMutex);
    }

  return surId;
}

GingaSurfaceID
DisplayManager::createSurface (GingaScreenID screenId, int w, int h)
{

  SDLDeviceScreen *screen;
  SDLSurface *surface = NULL;
  GingaSurfaceID surId = 0;

  if (getScreen (screenId, &screen))
    {
      surface = screen->createSurface (w, h);
      surId = surface->getId ();

      Thread::mutexLock (&surMapMutex);
      surMap[surId] = surface;
      Thread::mutexUnlock (&surMapMutex);
    }

  return surId;
}

GingaSurfaceID
DisplayManager::createSurfaceFrom (GingaScreenID screenId,
                                       GingaSurfaceID underlyingSurface)
{

  SDLDeviceScreen *screen;
  SDLSurface *surface = NULL;
  GingaSurfaceID surId = 0;

  if (getScreen (screenId, &screen))
    {
      surface = screen->createSurfaceFrom (
          (void *)getISurfaceFromId (underlyingSurface));
      surId = surface->getId ();

      Thread::mutexLock (&surMapMutex);
      surMap[surId] = surface;
      Thread::mutexUnlock (&surMapMutex);
    }

  return surId;
}

bool
DisplayManager::hasSurface (const GingaScreenID &screenId,
                                const GingaSurfaceID &surId)
{

  SDLSurface *surface = NULL;
  SDLDeviceScreen *screen = NULL;
  bool hasSur = false;

  surface = getISurfaceFromId (surId);
  if (surface != NULL)
    {
      if (getScreen (screenId, &screen))
        {
          hasSur = screen->hasSurface (surface);
        }
    }

  return hasSur;
}

bool
DisplayManager::releaseSurface (GingaScreenID screenId, SDLSurface *sur)
{

  SDLDeviceScreen *screen;

  if (getScreen (screenId, &screen))
    {
      return screen->releaseSurface (sur);
    }

  return false;
}

/* interfacing content */

GingaProviderID
DisplayManager::createContinuousMediaProvider (GingaScreenID screenId,
                                                   const char *mrl,
                                                   bool isRemote)
{

  SDLDeviceScreen *screen;
  IContinuousMediaProvider *provider = NULL;
  GingaProviderID providerId = 0;

  if (getScreen (screenId, &screen))
    {
      provider = screen->createContinuousMediaProvider (mrl, isRemote);

      provider->setId (provIdRefCounter++);

      Thread::mutexLock (&provMapMutex);
      provMap[provider->getId ()] = provider;
      Thread::mutexUnlock (&provMapMutex);

      providerId = provider->getId ();
    }

  return providerId;
}

void
DisplayManager::releaseContinuousMediaProvider (
    GingaScreenID screenId, GingaProviderID providerId)
{

  SDLDeviceScreen *screen;
  IContinuousMediaProvider *provider = NULL;
  IMediaProvider *iProvider = getIMediaProviderFromId (providerId);

  assert (iProvider != NULL);
  assert (iProvider->getType () == IMediaProvider::VideoProvider
          || iProvider->getType () == IMediaProvider::AudioProvider);

  provider = (IContinuousMediaProvider *)iProvider;

  if (getScreen (screenId, &screen))
    {
      Thread::mutexLock (&provMapMutex);
      provMap.erase (providerId);
      Thread::mutexUnlock (&provMapMutex);

      screen->releaseContinuousMediaProvider (provider);
    }
}

GingaProviderID
DisplayManager::createFontProvider (GingaScreenID screenId,
                                        const char *mrl, int fontSize)
{

  SDLDeviceScreen *screen;
  IFontProvider *provider = NULL;
  GingaProviderID providerId = 0;

  if (getScreen (screenId, &screen))
    {
      provider = screen->createFontProvider (mrl, fontSize);

      provider->setId (provIdRefCounter++);

      Thread::mutexLock (&provMapMutex);
      provMap[provider->getId ()] = provider;
      Thread::mutexUnlock (&provMapMutex);

      providerId = provider->getId ();
    }

  return providerId;
}

void
DisplayManager::releaseFontProvider (GingaScreenID screenId,
                                         GingaProviderID providerId)
{

  SDLDeviceScreen *screen;
  IFontProvider *provider = NULL;
  IMediaProvider *iProvider = getIMediaProviderFromId (providerId);

  assert (iProvider != NULL);
  assert (iProvider->getType () == IMediaProvider::FontProvider);

  provider = (IFontProvider *)iProvider;
  if (provider != NULL && getScreen (screenId, &screen))
    {
      Thread::mutexLock (&provMapMutex);
      provMap.erase (providerId);
      Thread::mutexUnlock (&provMapMutex);

      screen->releaseFontProvider (provider);
    }
}

GingaProviderID
DisplayManager::createImageProvider (GingaScreenID screenId,
                                         const char *mrl)
{

  SDLDeviceScreen *screen;
  IImageProvider *provider = NULL;
  GingaProviderID providerId = 0;

  if (getScreen (screenId, &screen))
    {
      provider = screen->createImageProvider (mrl);

      provider->setId (provIdRefCounter++);

      Thread::mutexLock (&provMapMutex);
      provMap[provider->getId ()] = provider;
      Thread::mutexUnlock (&provMapMutex);

      providerId = provider->getId ();
    }

  return providerId;
}

void
DisplayManager::releaseImageProvider (GingaScreenID screenId,
                                          GingaProviderID providerId)
{

  SDLDeviceScreen *screen;
  IImageProvider *provider = NULL;
  IMediaProvider *iProvider = getIMediaProviderFromId (providerId);

  assert (iProvider != NULL);
  assert (iProvider->getType () == IMediaProvider::ImageProvider);

  provider = (IImageProvider *)provider;
  if (getScreen (screenId, &screen))
    {
      Thread::mutexLock (&provMapMutex);
      provMap.erase (providerId);
      Thread::mutexUnlock (&provMapMutex);

      screen->releaseImageProvider (provider);
    }
}

GingaSurfaceID
DisplayManager::createRenderedSurfaceFromImageFile (
    GingaScreenID screenId, const char *mrl)
{

  SDLDeviceScreen *screen;
  SDLSurface *uSur = NULL;
  GingaSurfaceID surId = 0;

  if (getScreen (screenId, &screen))
    {
      uSur = screen->createRenderedSurfaceFromImageFile (mrl);
      surId = uSur->getId ();

      Thread::mutexLock (&surMapMutex);
      surMap[surId] = uSur;
      Thread::mutexUnlock (&surMapMutex);
    }

  return surId;
}

/* interfacing input */
InputManager *
DisplayManager::getInputManager (GingaScreenID screenId)
{
  SDLDeviceScreen *screen;
  InputManager *iManager = NULL;

  if (getScreen (screenId, &screen))
    {
      iManager = (InputManager *)screen->getInputManager ();
    }

  return iManager;
}

SDLEventBuffer *
DisplayManager::createEventBuffer (GingaScreenID screenId)
{
  SDLDeviceScreen *screen;
  SDLEventBuffer *buffer = NULL;

  if (getScreen (screenId, &screen))
    {
      buffer = screen->createEventBuffer ();
    }

  return buffer;
}

SDLInputEvent *
DisplayManager::createInputEvent (GingaScreenID screenId, void *event,
                                      const int symbol)
{

  SDLDeviceScreen *screen;
  SDLInputEvent *iEvent = NULL;

  if (getScreen (screenId, &screen))
    {
      iEvent = screen->createInputEvent (event, symbol);
    }

  return iEvent;
}

SDLInputEvent *
DisplayManager::createApplicationEvent (GingaScreenID screenId,
                                            int type, void *data)
{

  SDLDeviceScreen *screen;
  SDLInputEvent *iEvent = NULL;

  if (getScreen (screenId, &screen))
    {
      iEvent = screen->createApplicationEvent (type, data);
    }

  return iEvent;
}

int
DisplayManager::fromMBToGinga (GingaScreenID screenId, int keyCode)
{
  SDLDeviceScreen *screen;
  int translated = CodeMap::KEY_NULL;

  if (getScreen (screenId, &screen))
    {
      translated = screen->fromMBToGinga (keyCode);
    }

  return translated;
}

int
DisplayManager::fromGingaToMB (GingaScreenID screenId, int keyCode)
{
  SDLDeviceScreen *screen;
  int translated = CodeMap::KEY_NULL;

  if (getScreen (screenId, &screen))
    {
      translated = screen->fromGingaToMB (keyCode);
    }

  return translated;
}

/* Methods created to isolate gingacc-mb */
void
DisplayManager::addWindowCaps (const GingaScreenID &screenId,
                                   const GingaWindowID &winId, int caps)
{
  SDLWindow *win = NULL;
  win = getIWindowFromId (screenId, winId);
  if (win != NULL)
    win->addCaps (caps);
}

void
DisplayManager::setWindowCaps (const GingaScreenID &screenId,
                                   const GingaWindowID &winId, int caps)
{
  SDLWindow *win = NULL;
  win = getIWindowFromId (screenId, winId);
  if (win != NULL)
    win->setCaps (caps);
}
int
DisplayManager::getWindowCap (const GingaScreenID &screenId,
                                  const GingaWindowID &winId,
                                  const string &capName)
{
  int cap = 0;
  SDLWindow *win = NULL;
  win = getIWindowFromId (screenId, winId);
  if (win != NULL)
    cap = win->getCap (capName);

  return cap;
}

void
DisplayManager::drawWindow (const GingaScreenID &screenId,
                                const GingaWindowID &winId)
{
  SDLWindow *win = NULL;
  win = getIWindowFromId (screenId, winId);
  if (win != NULL)
    win->draw ();
}

void
DisplayManager::setWindowBounds (const GingaScreenID &screenId,
                                     const GingaWindowID &winId, int x,
                                     int y, int w, int h)
{
  SDLWindow *win = NULL;
  win = getIWindowFromId (screenId, winId);
  if (win != NULL)
    win->setBounds (x, y, w, h);
}

void
DisplayManager::showWindow (const GingaScreenID &screenId,
                                const GingaWindowID &winId)
{
  SDLWindow *win = NULL;
  win = getIWindowFromId (screenId, winId);
  if (win != NULL)
    win->show ();
}

void
DisplayManager::hideWindow (const GingaScreenID &screenId,
                                const GingaWindowID &winId)
{
  SDLWindow *win = NULL;
  win = getIWindowFromId (screenId, winId);
  if (win != NULL)
    win->hide ();
}

void
DisplayManager::raiseWindowToTop (const GingaScreenID &screenId,
                                      const GingaWindowID &winId)
{
  SDLWindow *win = NULL;
  win = getIWindowFromId (screenId, winId);
  if (win != NULL)
    win->raiseToTop ();
}

void
DisplayManager::renderWindowFrom (const GingaScreenID &screenId,
                                      const GingaWindowID &winId,
                                      const GingaSurfaceID &surId)
{
  SDLWindow *win = NULL;
  SDLSurface *surface = NULL;

  surface = getISurfaceFromId (surId);
  if (surface != NULL)
    {
      win = getIWindowFromId (screenId, winId);
      if (win != NULL)
        {
          win->renderFrom (surface);
        }
    }
}

void
DisplayManager::setWindowBgColor (const GingaScreenID &screenId,
                                      const GingaWindowID &winId, int r,
                                      int g, int b, int alpha)
{
  SDLWindow *win = NULL;
  win = getIWindowFromId (screenId, winId);
  if (win != NULL)
    win->setBgColor (r, g, b, alpha);
}

void
DisplayManager::setWindowBorder (const GingaScreenID &screenId,
                                     const GingaWindowID &winId, int r,
                                     int g, int b, int alpha, int width)
{
  SDLWindow *win = NULL;
  win = getIWindowFromId (screenId, winId);
  if (win != NULL)
    win->setBorder (r, g, b, alpha, width);
}

void
DisplayManager::setWindowCurrentTransparency (
    const GingaScreenID &screenId, const GingaWindowID &winId,
    int transparency)
{
  SDLWindow *win = NULL;
  win = getIWindowFromId (screenId, winId);
  if (win != NULL)
    win->setCurrentTransparency (transparency);
}

void
DisplayManager::setWindowColorKey (const GingaScreenID &screenId,
                                       const GingaWindowID &winId, int r,
                                       int g, int b)
{
  SDLWindow *win = NULL;
  win = getIWindowFromId (screenId, winId);
  if (win != NULL)
    win->setColorKey (r, g, b);
}

void
DisplayManager::setWindowX (const GingaScreenID &screenId,
                                const GingaWindowID &winId, int x)
{
  SDLWindow *win = NULL;
  win = getIWindowFromId (screenId, winId);
  if (win != NULL)
    win->setX (x);
}

void
DisplayManager::setWindowY (const GingaScreenID &screenId,
                                const GingaWindowID &winId, int y)
{
  SDLWindow *win = NULL;
  win = getIWindowFromId (screenId, winId);
  if (win != NULL)
    win->setY (y);
}

void
DisplayManager::setWindowW (const GingaScreenID &screenId,
                                const GingaWindowID &winId, int w)
{
  SDLWindow *win = NULL;
  win = getIWindowFromId (screenId, winId);
  if (win != NULL)
    win->setW (w);
}

void
DisplayManager::setWindowH (const GingaScreenID &screenId,
                                const GingaWindowID &winId, int h)
{
  SDLWindow *win = NULL;
  win = getIWindowFromId (screenId, winId);
  if (win != NULL)
    win->setH (h);
}

void
DisplayManager::setWindowZ (const GingaScreenID &screenId,
                                const GingaWindowID &winId, float z)
{
  SDLWindow *win = NULL;
  win = getIWindowFromId (screenId, winId);
  if (win != NULL)
    win->setZ (z);
}

void
DisplayManager::disposeWindow (const GingaScreenID &screenId,
                                   const GingaWindowID &winId)
{
  SDLWindow *win = NULL;
  win = getIWindowFromId (screenId, winId);
  if (win != NULL)
    delete win;
}

void
DisplayManager::setGhostWindow (const GingaScreenID &screenId,
                                    const GingaWindowID &winId, bool ghost)
{
  SDLWindow *win = NULL;
  win = getIWindowFromId (screenId, winId);
  if (win != NULL)
    win->setGhostWindow (ghost);
}

void
DisplayManager::validateWindow (const GingaScreenID &screenId,
                                    const GingaWindowID &winId)
{
  SDLWindow *win = NULL;
  win = getIWindowFromId (screenId, winId);
  if (win != NULL)
    win->validate ();
}

int
DisplayManager::getWindowX (const GingaScreenID &screenId,
                                const GingaWindowID &winId)
{
  int reply = 0;
  SDLWindow *win = NULL;
  win = getIWindowFromId (screenId, winId);
  if (win != NULL)
    reply = win->getX ();

  return reply;
}

int
DisplayManager::getWindowY (const GingaScreenID &screenId,
                                const GingaWindowID &winId)
{
  int reply = 0;
  SDLWindow *win = NULL;
  win = getIWindowFromId (screenId, winId);
  if (win != NULL)
    reply = win->getY ();

  return reply;
}

int
DisplayManager::getWindowW (const GingaScreenID &screenId,
                                const GingaWindowID &winId)
{
  int reply = 0;
  SDLWindow *win = NULL;
  win = getIWindowFromId (screenId, winId);
  if (win != NULL)
    reply = win->getW ();

  return reply;
}

int
DisplayManager::getWindowH (const GingaScreenID &screenId,
                                const GingaWindowID &winId)
{
  int reply = 0;
  SDLWindow *win = NULL;
  win = getIWindowFromId (screenId, winId);
  if (win != NULL)
    reply = win->getH ();

  return reply;
}

float
DisplayManager::getWindowZ (const GingaScreenID &screenId,
                                const GingaWindowID &winId)
{
  float reply = 0.0;
  SDLWindow *win = NULL;
  win = getIWindowFromId (screenId, winId);
  if (win != NULL)
    reply = win->getZ ();

  return reply;
}

int
DisplayManager::getWindowTransparencyValue (
    const GingaScreenID &screenId, const GingaWindowID &winId)
{
  int reply = 0;
  SDLWindow *win = NULL;
  win = getIWindowFromId (screenId, winId);
  if (win != NULL)
    reply = win->getTransparencyValue ();

  return reply;
}

void
DisplayManager::resizeWindow (const GingaScreenID &screenId,
                                  const GingaWindowID &winId, int width,
                                  int height)
{
  SDLWindow *win = NULL;
  win = getIWindowFromId (screenId, winId);
  if (win != NULL)
    win->resize (width, height);
}

string
DisplayManager::getWindowDumpFileUri (const GingaScreenID &screenId,
                                          const GingaWindowID &winId,
                                          int quality, int dumpW, int dumpH)
{
  string reply = "";
  SDLWindow *win = NULL;
  win = getIWindowFromId (screenId, winId);
  if (win != NULL)
    reply = win->getDumpFileUri (quality, dumpW, dumpH);

  return reply;
}

void
DisplayManager::clearWindowContent (const GingaScreenID &screenId,
                                        const GingaWindowID &winId)
{
  SDLWindow *win;
  win = getIWindowFromId (screenId, winId);
  if (win != NULL)
    win->clearContent ();
}

void
DisplayManager::revertWindowContent (const GingaScreenID &screenId,
                                         const GingaWindowID &winId)
{
  SDLWindow *win;
  win = getIWindowFromId (screenId, winId);
  if (win != NULL)
    win->revertContent ();
}

void
DisplayManager::deleteWindow (const GingaScreenID &screenId,
                                  const GingaWindowID &winId)
{
  SDLWindow *win;
  win = getIWindowFromId (screenId, winId);
  if (win != NULL)
    delete win;
}

void
DisplayManager::moveWindowTo (const GingaScreenID &screenId,
                                  const GingaWindowID &winId, int x, int y)
{
  SDLWindow *win;
  win = getIWindowFromId (screenId, winId);
  if (win != NULL)
    win->moveTo (x, y);
}

void
DisplayManager::lowerWindowToBottom (const GingaScreenID &screenId,
                                         const GingaWindowID &winId)
{
  SDLWindow *win;
  win = getIWindowFromId (screenId, winId);
  if (win != NULL)
    win->lowerToBottom ();
}

void
DisplayManager::setWindowMirrorSrc (const GingaScreenID &screenId,
                                        const GingaWindowID &winId,
                                        const GingaWindowID &mirrorSrc)
{
  SDLWindow *win;
  SDLWindow *mirrorSrcWin;

  win = getIWindowFromId (screenId, winId);
  mirrorSrcWin = getIWindowFromId (screenId, mirrorSrc);

  if (win != NULL && mirrorSrcWin != NULL)
    win->setMirrorSrc (mirrorSrcWin);
}

void *
DisplayManager::getSurfaceContent (const GingaSurfaceID &surId)
{
  SDLSurface *surface = NULL;
  void *surfaceContent = NULL;

  surface = getISurfaceFromId (surId);
  if (surface != NULL)
    surfaceContent = surface->getSurfaceContent ();

  return surfaceContent;
}

GingaWindowID
DisplayManager::getSurfaceParentWindow (const GingaSurfaceID &surId)
{
  SDLSurface *surface = NULL;
  GingaWindowID parentWindow = 0;

  surface = getISurfaceFromId (surId);
  if (surface != NULL)
    {
      SDLWindow *win = (SDLWindow *)surface->getParentWindow ();
      if (win != NULL)
        parentWindow = win->getId ();
    }

  return parentWindow;
}

void
DisplayManager::deleteSurface (const GingaSurfaceID &surId)
{
  SDLDeviceScreen *screen = NULL;
  SDLSurface *surface = NULL;

  surface = getISurfaceFromId (surId);
  if (surface)
    {
      Thread::mutexLock (&surMapMutex);
      surMap.erase (surId);
      Thread::mutexUnlock (&surMapMutex);

      delete surface;
    }
}

bool
DisplayManager::setSurfaceParentWindow (const GingaScreenID &screenId,
                                            const GingaSurfaceID &surId,
                                            const GingaWindowID &winId)
{
  bool reply = false;
  SDLSurface *surface = NULL;
  SDLWindow *window = NULL;

  surface = getISurfaceFromId (surId);
  if (surface != NULL)
    {
      window = getIWindowFromId (screenId, winId);
      if (window != NULL)
        reply = surface->setParentWindow (window);
    }

  return reply;
}

void
DisplayManager::clearSurfaceContent (const GingaSurfaceID &surId)
{
  SDLSurface *surface = NULL;

  surface = getISurfaceFromId (surId);
  if (surface != NULL)
    surface->clearContent ();
}

void
DisplayManager::getSurfaceSize (const GingaSurfaceID &surId, int *width,
                                    int *height)
{
  SDLSurface *surface = NULL;
  surface = getISurfaceFromId (surId);
  if (surface != NULL)
    surface->getSize (width, height);
  else
    {
      *width = 0;
      *height = 0;
    }
}

void
DisplayManager::addSurfaceCaps (const GingaSurfaceID &surId,
                                    const int caps)
{
  SDLSurface *surface = NULL;

  surface = getISurfaceFromId (surId);
  if (surface != NULL)
    surface->addCaps (caps);
}

void
DisplayManager::setSurfaceCaps (const GingaSurfaceID &surId,
                                    const int caps)
{
  SDLSurface *surface = NULL;

  surface = getISurfaceFromId (surId);
  if (surface != NULL)
    surface->setCaps (caps);
}

int
DisplayManager::getSurfaceCap (const GingaSurfaceID &surId,
                                   const string &cap)
{
  SDLSurface *surface = NULL;
  int value = 0;

  surface = getISurfaceFromId (surId);
  if (surface != NULL)
    value = surface->getCap (cap);

  return value;
}

int
DisplayManager::getSurfaceCaps (const GingaSurfaceID &surId)
{
  SDLSurface *surface = NULL;
  int value = 0;

  surface = getISurfaceFromId (surId);
  if (surface != NULL)
    value = surface->getCaps ();

  return value;
}

void
DisplayManager::setSurfaceBgColor (const GingaSurfaceID &surId, int r,
                                       int g, int b, int alpha)
{
  SDLSurface *surface = NULL;

  surface = getISurfaceFromId (surId);
  if (surface != NULL)
    surface->setBgColor (r, g, b, alpha);
}

void
DisplayManager::setSurfaceFont (const GingaSurfaceID &surId,
                                    GingaSurfaceID font)
{
  SDLSurface *surface = NULL;
  IFontProvider *fontProvider = NULL;
  IMediaProvider *iProvider = getIMediaProviderFromId (font);

  if (iProvider->getType () == IMediaProvider::FontProvider)
    fontProvider = (IFontProvider *)iProvider;
  else
    return;

  surface = getISurfaceFromId (surId);
  if (surface != NULL)
    {
      surface->setSurfaceFont (fontProvider);
    }
}

void
DisplayManager::setColor (const GingaSurfaceID &surId, int r, int g,
                              int b, int alpha)
{
  SDLSurface *surface = NULL;

  surface = getISurfaceFromId (surId);
  if (surface != NULL)
    surface->setColor (r, g, b, alpha);
}

void
DisplayManager::setExternalHandler (const GingaSurfaceID &surId,
                                        bool extHandler)
{
  SDLSurface *surface = NULL;

  surface = getISurfaceFromId (surId);
  if (surface != NULL)
    surface->setExternalHandler (extHandler);
}

void
DisplayManager::blitSurface (const GingaSurfaceID &surId, int x, int y,
                                 GingaSurfaceID src, int srcX, int srcY,
                                 int srcW, int srcH)
{
  SDLSurface *surface = NULL;
  SDLSurface *surfaceSrc = NULL;

  surface = getISurfaceFromId (surId);
  surfaceSrc = getISurfaceFromId (src);
  if (surface != NULL)
    {
      surface->blit (x, y, surfaceSrc, srcX, srcY, srcW, srcH);
    }
}

void
DisplayManager::flipSurface (const GingaSurfaceID &surId)
{
  SDLSurface *surface = NULL;

  surface = getISurfaceFromId (surId);
  if (surface != NULL)
    surface->flip ();
}

void
DisplayManager::setSurfaceContent (const GingaSurfaceID &surId,
                                       void *surface)
{
  SDLSurface *sur = NULL;

  sur = getISurfaceFromId (surId);
  if (sur != NULL)
    sur->setSurfaceContent (surface);
}

Color *
DisplayManager::getSurfaceColor (const GingaSurfaceID &surId)
{
  SDLSurface *surface = NULL;
  Color *color = NULL;

  surface = getISurfaceFromId (surId);
  if (surface != NULL)
    color = surface->getColor ();

  return color;
}

bool
DisplayManager::hasSurfaceExternalHandler (const GingaSurfaceID &surId)
{
  SDLSurface *surface = NULL;
  bool reply = false;

  surface = getISurfaceFromId (surId);
  if (surface != NULL)
    reply = surface->hasExternalHandler ();

  return reply;
}

void
DisplayManager::setSurfaceColor (const GingaSurfaceID &surId, int r,
                                     int g, int b, int alpha)
{
  SDLSurface *surface = NULL;

  surface = getISurfaceFromId (surId);
  if (surface != NULL)
    surface->setColor (r, g, b, alpha);
}

SDLSurface *
DisplayManager::getISurfaceFromId (const GingaSurfaceID &surId)
{
  map<GingaSurfaceID, SDLSurface *>::iterator i;
  SDLSurface *iSur = NULL;

  Thread::mutexLock (&surMapMutex);
  i = surMap.find (surId);
  if (i != surMap.end ())
    {
      iSur = i->second;
    }
  Thread::mutexUnlock (&surMapMutex);

  return iSur;
}

void
DisplayManager::setProviderSoundLevel (const GingaProviderID &provId,
                                           float level)
{
  IContinuousMediaProvider *provider = NULL;
  IMediaProvider *iProvider = getIMediaProviderFromId (provId);

  if (iProvider != NULL
      && (iProvider->getType () == IMediaProvider::AudioProvider
          || iProvider->getType () == IMediaProvider::VideoProvider))
    {
      provider = (IContinuousMediaProvider *)iProvider;
      provider->setSoundLevel (level);
    }
}

void
DisplayManager::getProviderOriginalResolution (
    const GingaProviderID &provId, int *width, int *height)
{
  IContinuousMediaProvider *provider = NULL;
  IMediaProvider *iProvider = getIMediaProviderFromId (provId);

  if (iProvider != NULL
      && (iProvider->getType () == IMediaProvider::AudioProvider
          || iProvider->getType () == IMediaProvider::VideoProvider))
    {
      provider = (IContinuousMediaProvider *)iProvider;
      provider->getOriginalResolution (width, height);
    }
}

double
DisplayManager::getProviderTotalMediaTime (
    const GingaProviderID &provId)
{
  double totalTime = 0.0;
  IContinuousMediaProvider *provider = NULL;
  IMediaProvider *iProvider = getIMediaProviderFromId (provId);

  if (iProvider != NULL
      && (iProvider->getType () == IMediaProvider::AudioProvider
          || iProvider->getType () == IMediaProvider::VideoProvider))
    {
      provider = (IContinuousMediaProvider *)iProvider;
      totalTime = provider->getTotalMediaTime ();
    }

  return totalTime;
}

double
DisplayManager::getProviderSoundLevel (const GingaProviderID &provId)
{
  double soundLevel = 0.0;
  IContinuousMediaProvider *provider = NULL;
  IMediaProvider *iProvider = getIMediaProviderFromId (provId);

  if (iProvider != NULL
      && (iProvider->getType () == IMediaProvider::AudioProvider
          || iProvider->getType () == IMediaProvider::VideoProvider))
    {
      provider = (IContinuousMediaProvider *)iProvider;
      soundLevel = provider->getSoundLevel ();
    }

  return soundLevel;
}

int64_t
DisplayManager::getProviderVPts (const GingaProviderID &provId)
{
  int64_t vpts = 0.0;
  IContinuousMediaProvider *provider = NULL;
  IMediaProvider *iProvider = getIMediaProviderFromId (provId);

  if (iProvider != NULL
      && (iProvider->getType () == IMediaProvider::AudioProvider
          || iProvider->getType () == IMediaProvider::VideoProvider))
    {
      provider = (IContinuousMediaProvider *)iProvider;
      vpts = provider->getVPts ();
    }

  return vpts;
}

void
DisplayManager::setProviderMediaTime (const GingaProviderID &provId,
                                          double pos)
{
  IContinuousMediaProvider *provider = NULL;
  IMediaProvider *iProvider = getIMediaProviderFromId (provId);

  if (iProvider != NULL
      && (iProvider->getType () == IMediaProvider::AudioProvider
          || iProvider->getType () == IMediaProvider::VideoProvider))
    {
      provider = (IContinuousMediaProvider *)iProvider;
      provider->setMediaTime (pos);
    }
}

double
DisplayManager::getProviderMediaTime (const GingaProviderID &provId)
{
  double totalTime = 0.0;
  IContinuousMediaProvider *provider = NULL;
  IMediaProvider *iProvider = getIMediaProviderFromId (provId);

  if (iProvider != NULL
      && (iProvider->getType () == IMediaProvider::AudioProvider
          || iProvider->getType () == IMediaProvider::VideoProvider))
    {
      provider = (IContinuousMediaProvider *)iProvider;
      totalTime = provider->getMediaTime ();
    }

  return totalTime;
}

void
DisplayManager::pauseProvider (const GingaProviderID &provId)
{
  IContinuousMediaProvider *provider = NULL;
  IMediaProvider *iProvider = getIMediaProviderFromId (provId);

  if (iProvider != NULL
      && (iProvider->getType () == IMediaProvider::AudioProvider
          || iProvider->getType () == IMediaProvider::VideoProvider))
    {
      provider = (IContinuousMediaProvider *)iProvider;
      provider->pause ();
    }
}

void
DisplayManager::stopProvider (const GingaProviderID &provId)
{
  IContinuousMediaProvider *provider = NULL;
  IMediaProvider *iProvider = getIMediaProviderFromId (provId);

  if (iProvider != NULL
      && (iProvider->getType () == IMediaProvider::AudioProvider
          || iProvider->getType () == IMediaProvider::VideoProvider))
    {
      provider = (IContinuousMediaProvider *)iProvider;
      provider->stop ();
    }
}

void
DisplayManager::setProviderAVPid (const GingaProviderID &provId,
                                      int aPid, int vPid)
{
  IContinuousMediaProvider *provider = NULL;
  IMediaProvider *iProvider = getIMediaProviderFromId (provId);

  if (iProvider != NULL
      && (iProvider->getType () == IMediaProvider::AudioProvider
          || iProvider->getType () == IMediaProvider::VideoProvider))
    {
      provider = (IContinuousMediaProvider *)iProvider;
      provider->setAVPid (aPid, vPid);
    }
}

void
DisplayManager::resumeProvider (const GingaProviderID &provId,
                                    GingaSurfaceID surface)
{

  IContinuousMediaProvider *provider = NULL;
  IMediaProvider *iProvider = getIMediaProviderFromId (provId);

  if (iProvider != NULL
      && (iProvider->getType () == IMediaProvider::AudioProvider
          || iProvider->getType () == IMediaProvider::VideoProvider))
    {
      provider = (IContinuousMediaProvider *)iProvider;
      provider->resume (surface);
    }
}

void
DisplayManager::feedProviderBuffers (const GingaProviderID &provId)
{
  IContinuousMediaProvider *provider = NULL;
  IMediaProvider *iProvider = getIMediaProviderFromId (provId);

  if (iProvider != NULL
      && (iProvider->getType () == IMediaProvider::AudioProvider
          || iProvider->getType () == IMediaProvider::VideoProvider))
    {
      provider = (IContinuousMediaProvider *)iProvider;
      provider->feedBuffers ();
    }
}

bool
DisplayManager::checkProviderVideoResizeEvent (
    const GingaProviderID &provId, const GingaSurfaceID &frame)
{
  bool resized = false;
  IContinuousMediaProvider *provider = NULL;
  IMediaProvider *iProvider = getIMediaProviderFromId (provId);

  if (iProvider != NULL
      && (iProvider->getType () == IMediaProvider::AudioProvider
          || iProvider->getType () == IMediaProvider::VideoProvider))
    {
      provider = (IContinuousMediaProvider *)iProvider;
      resized = provider->checkVideoResizeEvent (frame);
    }

  return resized;
}

int
DisplayManager::getProviderStringWidth (const GingaProviderID &provId,
                                            const char *text,
                                            int textLength)
{
  int width = 0;
  IFontProvider *provider = NULL;
  IMediaProvider *iProvider = getIMediaProviderFromId (provId);

  if (iProvider != NULL
      && iProvider->getType () == IMediaProvider::FontProvider)
    {
      provider = (IFontProvider *)iProvider;
      width = provider->getStringWidth (text, textLength);
    }

  return width;
}

void
DisplayManager::playProviderOver (const GingaProviderID &provId,
                                      const GingaSurfaceID &surface)
{
  IMediaProvider *iProvider = getIMediaProviderFromId (provId);

  if (iProvider != NULL)
    {
      iProvider->playOver (surface);
    }
}

void
DisplayManager::playProviderOver (const GingaProviderID &provId,
                                      const GingaSurfaceID &surface,
                                      const char *text, int x, int y,
                                      short align)
{
  IFontProvider *provider = NULL;
  IMediaProvider *iProvider = getIMediaProviderFromId (provId);

  if (iProvider != NULL
      && iProvider->getType () == IMediaProvider::FontProvider)
    {
      provider = (IFontProvider *)iProvider;
      provider->playOver (surface, text, x, y, align);
    }
}

int
DisplayManager::getProviderHeight (const GingaProviderID &provId)
{
  int height = 0;
  IFontProvider *provider = NULL;
  IMediaProvider *iProvider = getIMediaProviderFromId (provId);

  if (iProvider != NULL
      && iProvider->getType () == IMediaProvider::FontProvider)
    {
      provider = (IFontProvider *)iProvider;
      height = provider->getHeight ();
    }

  return height;
}

IMediaProvider *
DisplayManager::getIMediaProviderFromId (const GingaProviderID &provId)
{
  map<GingaProviderID, IMediaProvider *>::iterator i;
  IMediaProvider *iProv = NULL;

  Thread::mutexLock (&provMapMutex);
  i = provMap.find (provId);
  if (i != provMap.end ())
    {
      iProv = i->second;
    }
  Thread::mutexUnlock (&provMapMutex);

  return iProv;
}

/* private functions */

void
DisplayManager::addScreen (GingaScreenID screenId,
                               SDLDeviceScreen *screen)
{

  lockScreenMap ();
  if (screen != NULL)
    {
      screens[screenId] = screen;
    }
  else
    {
      clog << "DisplayManager::addScreen Warning! Trying to add ";
      clog << "a NULL screen" << endl;
    }
  unlockScreenMap ();
}

short
DisplayManager::getNumOfScreens ()
{
  short numOfScreens;

  lockScreenMap ();
  numOfScreens = screens.size ();
  unlockScreenMap ();

  return numOfScreens;
}

bool
DisplayManager::getScreen (GingaScreenID screenId,
                               SDLDeviceScreen **screen)
{

  bool hasScreen = false;
  map<GingaScreenID, SDLDeviceScreen *>::iterator i;

  lockScreenMap ();
  i = screens.find (screenId);
  if (i != screens.end ())
    {
      hasScreen = true;
      *screen = i->second;
    }
  unlockScreenMap ();

  return hasScreen;
}

bool
DisplayManager::removeScreen (GingaScreenID screenId)
{
  bool hasScreen = false;
  map<GingaScreenID, SDLDeviceScreen *>::iterator i;

  lockScreenMap ();
  i = screens.find (screenId);
  if (i != screens.end ())
    {
      screens.erase (i);
    }
  unlockScreenMap ();

  return hasScreen;
}

void
DisplayManager::lockScreenMap ()
{
  Thread::mutexLock (&mapMutex);
}

void
DisplayManager::unlockScreenMap ()
{
  Thread::mutexUnlock (&mapMutex);
}

void
DisplayManager::lock ()
{
  Thread::mutexLock (&genMutex);
}

void
DisplayManager::unlock ()
{
  Thread::mutexUnlock (&genMutex);
}

GINGA_MB_END
