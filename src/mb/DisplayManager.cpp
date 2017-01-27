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
#include "CodeMap.h"
#include "DisplayManager.h"
#include "InputManager.h"
#include "SDLDisplay.h"

GINGA_MB_BEGIN

// Global display manager; initialized by main().
DisplayManager *_Ginga_Display_M = NULL;

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

  running = false;
  isWaiting = false;

  provIdRefCounter = 1;

  Thread::condInit (&wsSignal, NULL);
  Thread::mutexInit (&wsMutex);
}

DisplayManager::~DisplayManager ()
{
  Thread::mutexDestroy (&mapMutex);

  lock ();
  unlock ();
  Thread::mutexDestroy (&genMutex);

  map<SDLSurface*, SDLSurface *>::iterator j;

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

  map<IMediaProvider*, IMediaProvider *>::iterator k;

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

/* interfacing output */

SDLWindow*
DisplayManager::createWindow (int x, int y, int w, int h, double z)
{
  return Ginga_Display->createWindow (x, y, w, h, z);
}

bool
DisplayManager::hasWindow (SDLWindow* winId)
{
  return Ginga_Display->hasWindow (winId);
}

void
DisplayManager::releaseWindow (SDLWindow *win)
{
  Ginga_Display->releaseWindow (win);
}

SDLSurface*
DisplayManager::createSurface ()
{
  return Ginga_Display->createSurface ();
}

SDLSurface*
DisplayManager::createSurface (int w, int h)
{
  return Ginga_Display->createSurface (w, h);
}

SDLSurface*
DisplayManager::createSurfaceFrom (SDLSurface* underlyingSurface)
{
  return Ginga_Display->createSurfaceFrom (underlyingSurface);
}

bool
DisplayManager::hasSurface (SDLSurface *surId)
{
  return Ginga_Display->hasSurface (surId);
}

bool
DisplayManager::releaseSurface (SDLSurface *sur)
{
  return Ginga_Display->releaseSurface (sur);
}

/* interfacing content */

IMediaProvider*
DisplayManager::createContinuousMediaProvider (const char *mrl,
                                               bool isRemote)
{
  return Ginga_Display->createContinuousMediaProvider (mrl, isRemote);
}

void
DisplayManager::releaseContinuousMediaProvider (IMediaProvider* providerId)
{
  Ginga_Display->releaseContinuousMediaProvider ((IContinuousMediaProvider *)providerId);
}

IMediaProvider*
DisplayManager::createFontProvider (const char *mrl, int fontSize)
{
  return Ginga_Display->createFontProvider (mrl, fontSize);
}

void
DisplayManager::releaseFontProvider (IMediaProvider* providerId)
{
  Ginga_Display->releaseFontProvider ((IFontProvider *)providerId);
}

IMediaProvider*
DisplayManager::createImageProvider (const char *mrl)
{
  return Ginga_Display->createImageProvider (mrl);
}

void
DisplayManager::releaseImageProvider (IMediaProvider* providerId)
{
  Ginga_Display->releaseImageProvider ((IImageProvider *)providerId);
}

SDLSurface*
DisplayManager::createRenderedSurfaceFromImageFile (const char *mrl)
{
  return Ginga_Display->createRenderedSurfaceFromImageFile (mrl);
}

/* interfacing input */
InputManager *
DisplayManager::getInputManager ()
{
  return (InputManager *)Ginga_Display->getInputManager ();
}

SDLEventBuffer *
DisplayManager::createEventBuffer ()
{
  return Ginga_Display->createEventBuffer ();
}

SDLInputEvent *
DisplayManager::createInputEvent (void *event, const int symbol)
{
  return Ginga_Display->createInputEvent (event, symbol);
}

SDLInputEvent *
DisplayManager::createApplicationEvent (int type, void *data)
{
  return Ginga_Display->createApplicationEvent (type, data);
}

int
DisplayManager::fromMBToGinga (int keyCode)
{
  return Ginga_Display->fromMBToGinga (keyCode);
}

int
DisplayManager::fromGingaToMB (int keyCode)
{
  return Ginga_Display->fromGingaToMB (keyCode);
}

/* Methods created to isolate gingacc-mb */
void
DisplayManager::addWindowCaps (SDLWindow* winId, int caps)
{
    winId->addCaps (caps);
}

void
DisplayManager::setWindowCaps (SDLWindow* winId, int caps)
{
  winId->setCaps (caps);
}
int
DisplayManager::getWindowCap (SDLWindow* winId,
                                  const string &capName)
{
  return winId->getCap (capName);
}

void
DisplayManager::drawWindow (SDLWindow* winId)
{
  winId->draw ();
}

void
DisplayManager::setWindowBounds (SDLWindow* winId, int x,
                                 int y, int w, int h)
{
    return winId->setBounds (x, y, w, h);
}

void
DisplayManager::showWindow (SDLWindow* winId)
{
    winId->show ();
}

void
DisplayManager::hideWindow (SDLWindow* winId)
{
  winId->hide ();
}

void
DisplayManager::raiseWindowToTop (SDLWindow* winId)
{
    winId->raiseToTop ();
}

void
DisplayManager::renderWindowFrom (SDLWindow* winId,
                                  SDLSurface *surId)
{
  winId->renderFrom (surId);
}

void
DisplayManager::setWindowBgColor (SDLWindow* winId, guint8 r,
                                  guint8 g, guint8 b, guint8 alpha)
{
  winId->setBgColor (r, g, b, alpha);
}

void
DisplayManager::setWindowBorder (SDLWindow* winId, guint8 r,
                                 guint8 g, guint8 b, guint8 alpha, int width)
{
  winId->setBorder (r, g, b, alpha, width);
}

void
DisplayManager::setWindowCurrentTransparency (SDLWindow* winId,
                                              guint8 transparency)
{
  winId->setCurrentTransparency (transparency);
}

void
DisplayManager::setWindowColorKey (SDLWindow* winId, guint8 r,
                                   guint8 g, guint8 b)
{
  winId->setColorKey (r, g, b);
}

void
DisplayManager::setWindowX (SDLWindow* winId, int x)
{
  winId->setX (x);
}

void
DisplayManager::setWindowY (SDLWindow* winId, int y)
{
  winId->setY (y);
}

void
DisplayManager::setWindowW (SDLWindow* winId, int w)
{
  winId->setW (w);
}

void
DisplayManager::setWindowH (SDLWindow* winId, int h)
{
  winId->setH (h);
}

void
DisplayManager::setWindowZ (SDLWindow* winId, double z)
{
  winId->setZ (z);
}

void
DisplayManager::disposeWindow (SDLWindow* winId)
{
  delete winId;
}

void
DisplayManager::setGhostWindow (SDLWindow* winId, bool ghost)
{
  winId->setGhostWindow (ghost);
}

void
DisplayManager::validateWindow (SDLWindow* winId)
{
  winId->validate ();
}

int
DisplayManager::getWindowX (SDLWindow* winId)
{
  return winId->getX ();
}

int
DisplayManager::getWindowY (SDLWindow* winId)
{
  return winId->getY ();
}

int
DisplayManager::getWindowW (SDLWindow* winId)
{
  return winId->getW ();
}

int
DisplayManager::getWindowH (SDLWindow* winId)
{
  return winId->getH ();
}

double
DisplayManager::getWindowZ (SDLWindow* winId)
{
  return winId->getZ ();
}

guint8
DisplayManager::getWindowTransparencyValue (SDLWindow* winId)
{
  return winId->getTransparencyValue ();
}

void
DisplayManager::resizeWindow (SDLWindow* winId, int width,
                              int height)
{
  winId->resize (width, height);
}

string
DisplayManager::getWindowDumpFileUri (SDLWindow* winId,
                                      int quality, int dumpW, int dumpH)
{
  return winId->getDumpFileUri (quality, dumpW, dumpH);
}

void
DisplayManager::clearWindowContent (SDLWindow* winId)
{
  winId->clearContent ();
}

void
DisplayManager::revertWindowContent (SDLWindow* winId)
{
  winId->revertContent ();
}

void
DisplayManager::deleteWindow (SDLWindow* winId)
{
  delete winId;
}

void
DisplayManager::moveWindowTo (SDLWindow* winId, int x, int y)
{
  winId->moveTo (x, y);
}

void
DisplayManager::lowerWindowToBottom (SDLWindow* winId)
{
  winId->lowerToBottom ();
}

void
DisplayManager::setWindowMirrorSrc (SDLWindow* winId,
                                    SDLWindow* mirrorSrc)
{
  winId->setMirrorSrc (mirrorSrc);
}

void *
DisplayManager::getSurfaceContent (SDLSurface *surId)
{
  return surId->getSurfaceContent ();
}

SDLWindow*
DisplayManager::getSurfaceParentWindow (SDLSurface *surId)
{
  return (SDLWindow *) surId->getParentWindow ();
}

void
DisplayManager::deleteSurface (SDLSurface *surId)
{
  delete surId;
}

bool
DisplayManager::setSurfaceParentWindow (SDLSurface *surId,
                                            SDLWindow* winId)
{
  return surId->setParentWindow (winId);
}

void
DisplayManager::clearSurfaceContent (SDLSurface *surId)
{
  surId->clearContent ();
}

void
DisplayManager::getSurfaceSize (SDLSurface *surId, int *width,
                                    int *height)
{
  surId->getSize (width, height);
}

void
DisplayManager::addSurfaceCaps (SDLSurface *surId,
                                    const int caps)
{
  surId->addCaps (caps);
}

void
DisplayManager::setSurfaceCaps (SDLSurface *surId,
                                    const int caps)
{
  surId->setCaps (caps);
}

int
DisplayManager::getSurfaceCap (SDLSurface *surId,
                                   const string &cap)
{
  return surId->getCap (cap);
}

int
DisplayManager::getSurfaceCaps (SDLSurface *surId)
{
  return surId->getCaps ();
}

void
DisplayManager::setSurfaceBgColor (SDLSurface *surId, guint8 r,
                                       guint8 g, guint8 b, guint8 alpha)
{
  surId->setBgColor (r, g, b, alpha);
}


void
DisplayManager::setColor (SDLSurface *surId, guint8 r, guint8 g,
                              guint8 b, guint8 alpha)
{
  surId->setColor (r, g, b, alpha);
}

void
DisplayManager::setExternalHandler (SDLSurface *surId,
                                        bool extHandler)
{
  surId->setExternalHandler (extHandler);
}

void
DisplayManager::blitSurface (SDLSurface *surId, int x, int y,
                                 SDLSurface* src, int srcX, int srcY,
                                 int srcW, int srcH)
{
  surId->blit (x, y, src, srcX, srcY, srcW, srcH);
}

void
DisplayManager::flipSurface (SDLSurface *surId)
{
  surId->flip ();
}

void
DisplayManager::setSurfaceContent (SDLSurface *surId,
                                       void *surface)
{
  surId->setSurfaceContent (surface);
}

Color *
DisplayManager::getSurfaceColor (SDLSurface *surId)
{
  return surId->getColor ();
}

bool
DisplayManager::hasSurfaceExternalHandler (SDLSurface *surId)
{
  return surId->hasExternalHandler ();
}

void
DisplayManager::setSurfaceColor (SDLSurface *surId, guint8 r,
                                     guint8 g, guint8 b, guint8 alpha)
{
  return surId->setColor (r, g, b, alpha);
}

void
DisplayManager::setProviderSoundLevel (IMediaProvider*provId,
                                           double level)
{
  ((IContinuousMediaProvider *)provId)->setSoundLevel (level);
}

void
DisplayManager::getProviderOriginalResolution (
    IMediaProvider*provId, int *width, int *height)
{

  ((IContinuousMediaProvider *)provId)->getOriginalResolution (width, height);
}

double
DisplayManager::getProviderTotalMediaTime (
    IMediaProvider*provId)
{
  return ((IContinuousMediaProvider *)provId)->getTotalMediaTime ();
}

double
DisplayManager::getProviderSoundLevel (IMediaProvider*provId)
{
  return ((IContinuousMediaProvider *)provId)->getSoundLevel ();
}

int64_t
DisplayManager::getProviderVPts (IMediaProvider*provId)
{
  return ((IContinuousMediaProvider *)provId)->getVPts ();
}

void
DisplayManager::setProviderMediaTime (IMediaProvider*provId,
                                          double pos)
{
  ((IContinuousMediaProvider *)provId)->setMediaTime (pos);
}

double
DisplayManager::getProviderMediaTime (IMediaProvider*provId)
{

  return ((IContinuousMediaProvider *)provId)->getMediaTime ();
}

void
DisplayManager::pauseProvider (IMediaProvider*provId)
{
  ((IContinuousMediaProvider *)provId)->pause ();
}

void
DisplayManager::stopProvider (IMediaProvider*provId)
{
  ((IContinuousMediaProvider *)provId)->stop ();
}

void
DisplayManager::setProviderAVPid (IMediaProvider*provId,
                                      int aPid, int vPid)
{
  ((IContinuousMediaProvider *)provId)->setAVPid (aPid, vPid);
}

void
DisplayManager::resumeProvider (IMediaProvider*provId,
                                    SDLSurface* surface)
{
  ((IContinuousMediaProvider *)provId)->resume (surface);
}

void
DisplayManager::feedProviderBuffers (IMediaProvider*provId)
{
  ((IContinuousMediaProvider *)provId)->feedBuffers ();
}

bool
DisplayManager::checkProviderVideoResizeEvent (
    IMediaProvider*provId, SDLSurface *frame)
{
  return ((IContinuousMediaProvider *)provId)->checkVideoResizeEvent (frame);
}

int
DisplayManager::getProviderStringWidth (IMediaProvider*provId,
                                            const char *text,
                                            int textLength)
{
  return ((IFontProvider *)provId)->getStringWidth (text, textLength);
}

void
DisplayManager::playProviderOver (IMediaProvider*provId,
                                      SDLSurface *surface)
{
  provId->playOver (surface);
}

void
DisplayManager::playProviderOver (IMediaProvider*provId,
                                      SDLSurface *surface,
                                      const char *text, int x, int y,
                                      short align)
{
  ((IFontProvider*)provId)->playOver (surface, text, x, y, align);
}

int
DisplayManager::getProviderHeight (IMediaProvider*provId)
{
  return ((IFontProvider *)provId)->getHeight ();
}

/* private functions */


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
