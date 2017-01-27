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
