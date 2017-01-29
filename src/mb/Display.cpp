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
#include "Display.h"

#include "InputManager.h"
#include "SDLAudioProvider.h"
#include "SDLEventBuffer.h"
#include "SDLFontProvider.h"
#include "SDLInputEvent.h"
#include "SDLSurface.h"
#include "SDLVideoProvider.h"
#include "SDLWindow.h"

GINGA_MB_BEGIN

// Global display; initialized by main().
Display *_Ginga_Display = NULL;

bool Display::mutexInit = false;
map<int, int> Display::gingaToSDLCodeMap;
map<int, int> Display::sdlToGingaCodeMap;
map<string, int> Display::sdlStrToSdlCode;
set<SDL_Texture *> Display::uTexPool;
set<SDL_Surface *> Display::uSurPool;
vector<ReleaseContainer *> Display::releaseList;
map<int, map<double, set<SDLWindow *> *> *>
    Display::renderMap;
set<IContinuousMediaProvider *> Display::cmpRenderList;

pthread_mutex_t Display::sdlMutex;
pthread_mutex_t Display::sieMutex;
pthread_mutex_t Display::renMutex;
pthread_mutex_t Display::scrMutex;
pthread_mutex_t Display::recMutex;
pthread_mutex_t Display::winMutex;
pthread_mutex_t Display::surMutex;
pthread_mutex_t Display::proMutex;
pthread_mutex_t Display::cstMutex;

// -------------------------------------------------------------------------

// Compare the z-index of two windows.
static gint
cmp_win_z (gconstpointer p1, gconstpointer p2)
{
  SDLWindow *w1 = deconst (SDLWindow *, p1);
  SDLWindow *w2 = deconst (SDLWindow *, p2);
  double z1 = w1->getZ ();
  double z2 = w2->getZ ();
  return (z1 < z2) ? -1 : (z1 > z2) ? 1 : 0;
}

// Render thread.
static gpointer
render_thread_func (gpointer data)
{
  Display *display;

  display = (Display *) data;
  g_assert_nonnull (display);

  while (!display->hasQuitted())
    {
      SDL_Event evt;
      while (SDL_PollEvent (&evt)) // handle input
        {
          switch (evt.type)
            {
            case SDL_KEYDOWN:
            case SDL_KEYUP:
              if (evt.key.keysym.sym != SDLK_ESCAPE)
                break;
              // fall-through
            case SDL_QUIT:
              display->quit ();
              goto tail;
            default:
              break;
            }

          InputManager *im = display->getInputManager ();
          SDLEventBuffer *buf = im->getEventBuffer ();
          buf->feed (evt, false, false);
        }

      Display::refreshCMP (display);
      display->redraw ();       // redraw providers and windows
    tail:
      ;
    }

  SDL_Quit ();
  exit (EXIT_SUCCESS);          // FIXME: Die gracefully
  return display;
}


// Private methods.

void
Display::lock (void)
{
  g_mutex_lock (&this->mutex);
}

void
Display::unlock (void)
{
  g_mutex_unlock (&this->mutex);
}

void
Display::add (GList **list, gpointer data)
{
  this->lock ();
  g_assert_null (g_list_find (*list, data));
  *list = g_list_append (*list, data);
  this->unlock ();
}

void
Display::remove (GList **list, gpointer data)
{
  GList *elt;

  this->lock ();
  elt = g_list_find (*list, data);
  g_assert_nonnull (elt);
  *list = g_list_remove_link (*list, elt);
  this->unlock ();
}


// Public methods.

/**
 * Returns true if display render thread has quitted.
 */
bool
Display::hasQuitted ()
{
  bool quit;

  this->lock ();
  quit = this->_quit;
  this->unlock ();

  return quit;
}

/**
 * Quits display render thread.
 */
void
Display::quit ()
{
  this->lock ();
  this->_quit = true;
  this->unlock ();
}

/**
 * Redraws the registered providers and windows.
 * The z-index order of windows determines their redraw order.
 */
void
Display::redraw ()
{
  GList *l;

  this->lock ();
  this->windows = g_list_sort (this->windows, cmp_win_z);
  for (l = this->windows; l != NULL; l = l->next)
    {
      SDL_Texture *texture;
      SDLWindow *win;

      win = (SDLWindow *) l->data;
      if (!win->isVisible () || win->isGhostWindow ())
        continue;               // nothing to do

      texture = win->getTexture (this->renderer);
      drawSDLWindow (this->renderer, texture, win);
      win->rendered ();
    }

  SDL_RenderPresent (this->renderer);
  this->unlock ();
}

/**
 * Creates and returns a new display with the given dimensions.
 * If FULLSCREEN is true, enable full-screen mode.
 */
Display::Display (int width, int height, bool fullscreen)
{
  guint flags;
  int status;

  g_mutex_init (&this->mutex);

  this->width = width;
  this->height = height;
  this->fullscreen = fullscreen;

  this->im = NULL;
  this->renderer = NULL;
  this->screen = NULL;

  this->_quit = false;
  this->render_thread = NULL;

  this->windows = NULL;
  this->providers = NULL;

  g_assert (!SDL_WasInit (0));
  status = SDL_Init (0);
  if (unlikely (status != 0))
    g_critical ("cannot initialize SDL: %s", SDL_GetError ());

  flags = SDL_WINDOW_SHOWN;
  if (this->fullscreen)
    flags |= SDL_WINDOW_FULLSCREEN;

  status = SDL_CreateWindowAndRenderer (this->width, this->height, flags,
                                        &this->screen, &this->renderer);
  g_assert (status == 0);
  g_assert_nonnull (this->screen);
  g_assert_nonnull (this->renderer);

  this->im = new InputManager ();
  g_assert_nonnull (this->im);
  this->im->setAxisBoundaries (this->width, this->height, 0);

  checkMutexInit ();            // FIXME
  initCodeMaps ();              // FIXME

  this->render_thread = g_thread_new ("render", render_thread_func, this);
  g_assert_nonnull (this->render_thread);
}

// END SANITY --------------------------------------------------------------

Display::~Display ()
{
  map<Display *, short>::iterator i;
  map<int, map<double, set<SDLWindow *> *> *>::iterator j;
  map<double, set<SDLWindow *> *>::iterator k;

  waitingCreator = false;
  Thread::mutexDestroy (&condMutex);
  Thread::condDestroy (&cond);

  Thread::mutexLock (&renMutex);
  j = renderMap.find (0);
  if (j != renderMap.end ())
    {
      k = j->second->begin ();
      while (k != j->second->end ())
        {
          delete k->second;
          ++k;
        }
      delete j->second;
      renderMap.erase (j);
    }
  Thread::mutexUnlock (&renMutex);

  if (im != NULL)
    {
      delete im;
      im = NULL;
    }
}

void
Display::checkMutexInit ()
{
  if (!mutexInit)
    {
      mutexInit = true;

      Thread::mutexInit (&sdlMutex, true);
      Thread::mutexInit (&sieMutex, true);
      Thread::mutexInit (&renMutex, true);
      Thread::mutexInit (&scrMutex, true);
      Thread::mutexInit (&recMutex, true);
      Thread::mutexInit (&winMutex, true);
      Thread::mutexInit (&surMutex, true);
      Thread::mutexInit (&proMutex, true);
      Thread::mutexInit (&cstMutex, true);
    }
}

void
Display::lockSDL ()
{
  checkMutexInit ();

  Thread::mutexLock (&sdlMutex);
}

void
Display::unlockSDL ()
{
  checkMutexInit ();

  Thread::mutexUnlock (&sdlMutex);
}

void
Display::updateRenderMap (SDLWindow *window,
                             double oldZIndex, double newZIndex)
{
  checkMutexInit ();
  renderMapRemoveWindow (window, oldZIndex);
  renderMapInsertWindow (window, newZIndex);
}

int
Display::getWidthResolution ()
{
  return this->width;
}

int
Display::getHeightResolution ()
{
  return this->height;
}

bool
Display::mergeIds (SDLWindow* destId,
                           vector<SDLWindow*> *srcIds)
{
  vector<SDLWindow*>::iterator i;
  SDLWindow *destWin;
  SDL_Surface *destSur;
  bool merged = false;
  int w, h;

  lockSDL ();

  Thread::mutexLock (&winMutex);

  destWin = destId;
  w = destWin->getW ();
  h = destWin->getH ();

  destSur = createUnderlyingSurface (w, h);

  for (i = srcIds->begin (); i != srcIds->end (); i++)
    if (blitFromWindow (*i, destSur))
      merged = true;

  Thread::mutexUnlock (&winMutex);
  unlockSDL ();

  destWin->setRenderedSurface (destSur);
  return merged;
}

void
Display::blitScreen (SDLSurface *destination)
{
  SDL_Surface *dest;

  lockSDL ();
  dest = (SDL_Surface *)(destination->getContent ());
  if (dest == NULL)
    {
      dest = createUnderlyingSurface (this->width, this->height);
      destination->setContent (dest);
    }

  blitScreen (dest);

  unlockSDL ();
}

void
Display::blitScreen (string fileUri)
{
  SDL_Surface *dest;

  lockSDL ();
  dest = createUnderlyingSurface (this->width, this->height);
  blitScreen (dest);

  if (SDL_SaveBMP_RW (dest, SDL_RWFromFile (fileUri.c_str (), "wb"), 1) < 0)
    {
      clog << "Display::blitScreen SDL error: '";
      clog << SDL_GetError () << "'" << endl;
    }
  unlockSDL ();
}

void
Display::blitScreen (SDL_Surface *dest)
{
  map<int, map<double, set<SDLWindow *> *> *>::iterator i;
  map<double, set<SDLWindow *> *>::iterator j;
  set<SDLWindow *>::iterator k;

  Thread::mutexLock (&renMutex);
  i = renderMap.find (0);
  if (i != renderMap.end ())
    {
      j = i->second->begin ();
      while (j != i->second->end ())
        {
          k = j->second->begin ();
          while (k != j->second->end ())
            {
              blitFromWindow ((*k), dest);
              ++k;
            }
          ++j;
        }
    }
  Thread::mutexUnlock (&renMutex);
}

/* interfacing output */

SDLWindow *
Display::createWindow (int x, int y, int w, int h, double z)
{
  SDLWindow *iWin;

  Thread::mutexLock (&winMutex);

  iWin = new SDLWindow (0, x, y, w, h, z);

  windowPool.insert (iWin);
  this->add (&this->windows, iWin);

  renderMapInsertWindow (iWin, z);

  Thread::mutexUnlock (&winMutex);

  return iWin;
}

bool
Display::hasWindow (SDLWindow *win)
{
  set<SDLWindow *>::iterator i;
  bool hasWin = false;

  Thread::mutexLock (&winMutex);

  i = windowPool.find (win);
  if (i != windowPool.end ())
    {
      hasWin = true;
    }

  Thread::mutexUnlock (&winMutex);

  return hasWin;
}

void
Display::releaseWindow (SDLWindow *win)
{
  set<SDLWindow *>::iterator i;
  map<SDLWindow*, SDLWindow *>::iterator j;
  SDLWindow *iWin;
  SDL_Texture *uTex = NULL;
  bool uTexOwn;

  Thread::mutexLock (&winMutex);
  i = windowPool.find (win);
  if (i != windowPool.end ())
    {
      iWin = (SDLWindow *)(*i);

      renderMapRemoveWindow (iWin, iWin->getZ ());

      windowPool.erase (i);
      this->remove (&this->windows, win);

      uTex = iWin->getTexture (NULL);
      uTexOwn = iWin->isTextureOwner (uTex);

      iWin->clearContent ();
      iWin->setTexture (NULL);

      Thread::mutexUnlock (&winMutex);

      if (uTexOwn)
        {
          //createReleaseContainer (NULL, uTex, NULL);
        }
    }
  else
    {
      Thread::mutexUnlock (&winMutex);
    }
}

SDLSurface *
Display::createSurface ()
{
  return createSurfaceFrom (NULL);
}

SDLSurface *
Display::createSurface (int w, int h)
{
  SDLSurface *iSur = NULL;
  SDL_Surface *uSur = NULL;

  lockSDL ();

  uSur = createUnderlyingSurface (w, h);

  iSur = new SDLSurface (uSur);

  unlockSDL ();

  Thread::mutexLock (&surMutex);
  surfacePool.insert (iSur);
  Thread::mutexUnlock (&surMutex);

  return iSur;
}

SDLSurface *
Display::createSurfaceFrom (void *uSur)
{
  SDLSurface *iSur = NULL;

  lockSDL ();
  if (uSur != NULL)
    {
      iSur = new SDLSurface (uSur);
    }
  else
    {
      iSur = new SDLSurface ();
    }
  unlockSDL ();

  Thread::mutexLock (&surMutex);
  surfacePool.insert (iSur);
  Thread::mutexUnlock (&surMutex);

  return iSur;
}

bool
Display::hasSurface (SDLSurface *s)
{
  set<SDLSurface *>::iterator i;
  bool hasSur = false;

  Thread::mutexLock (&surMutex);
  i = surfacePool.find (s);
  if (i != surfacePool.end ())
    {
      hasSur = true;
    }
  Thread::mutexUnlock (&surMutex);

  return hasSur;
}

bool
Display::releaseSurface (SDLSurface *s)
{
  set<SDLSurface *>::iterator i;
  bool released = false;

  Thread::mutexLock (&surMutex);
  i = surfacePool.find (s);
  if (i != surfacePool.end ())
    {
      surfacePool.erase (i);
      released = true;
    }
  Thread::mutexUnlock (&surMutex);

  return released;
}

/* interfacing content */
IContinuousMediaProvider *
Display::createContinuousMediaProvider (const char *mrl,
                                                arg_unused (bool isRemote))
{
  IContinuousMediaProvider *provider;
  string strSym;

  lockSDL ();
  strSym = "SDLVideoProvider";
  provider = new SDLVideoProvider (mrl);
  unlockSDL ();

  Thread::mutexLock (&proMutex);
  cmpPool.insert (provider);
  Thread::mutexUnlock (&proMutex);

  return provider;
}

void
Display::releaseContinuousMediaProvider (
    IContinuousMediaProvider *provider)
{
  set<IContinuousMediaProvider *>::iterator i;
  IContinuousMediaProvider *cmp;

  Thread::mutexLock (&proMutex);
  i = cmpPool.find (provider);
  if (i != cmpPool.end ())
    {
      cmp = (*i);
      cmpPool.erase (i);
      cmp->stop ();

      Thread::mutexUnlock (&proMutex);
      //createReleaseContainer (NULL, NULL, cmp);
    }
  else
    {
      Thread::mutexUnlock (&proMutex);
    }
}

IFontProvider *
Display::createFontProvider (const char *mrl, int fontSize)
{
  IFontProvider *provider = NULL;

  lockSDL ();
  provider = new SDLFontProvider (mrl, fontSize);
  unlockSDL ();

  Thread::mutexLock (&proMutex);
  dmpPool.insert (provider);
  Thread::mutexUnlock (&proMutex);

  return provider;
}

void
Display::releaseFontProvider (IFontProvider *provider)
{
  set<IDiscreteMediaProvider *>::iterator i;
  //IDiscreteMediaProvider *dmp;

  Thread::mutexLock (&proMutex);
  i = dmpPool.find (provider);
  if (i != dmpPool.end ())
    {
      //dmp = (*i);
      dmpPool.erase (i);

      Thread::mutexUnlock (&proMutex);
      //createReleaseContainer (NULL, NULL, dmp);
    }
  else
    {
      Thread::mutexUnlock (&proMutex);
    }
}

SDLSurface *
Display::createRenderedSurfaceFromImageFile (const char *mrl)
{
  SDL_Surface *sfc;
  SDLSurface *surface;
  SDLWindow *window;

  surface = Ginga_Display->createSurfaceFrom (NULL);
  g_assert_nonnull (surface);

  sfc = IMG_Load (mrl);
  if (unlikely (sfc == NULL))
    g_error ("cannot load image file %s: %s", mrl, IMG_GetError ());

  g_assert_nonnull (surface);
  surface->setContent (sfc);
  Display::addUnderlyingSurface (sfc);

  window = surface->getParentWindow ();
  g_assert_null (window);

  return surface;
}

void
Display::addCMPToRendererList (IContinuousMediaProvider *cmp)
{
  checkMutexInit ();

  Thread::mutexLock (&proMutex);
  if (cmp->getHasVisual ())
    {
      cmpRenderList.insert (cmp);
    }
  Thread::mutexUnlock (&proMutex);
}

void
Display::removeCMPToRendererList (IContinuousMediaProvider *cmp)
{
  set<IContinuousMediaProvider *>::iterator i;

  checkMutexInit ();

  Thread::mutexLock (&proMutex);
  i = cmpRenderList.find (cmp);
  if (i != cmpRenderList.end ())
    {
      cmpRenderList.erase (i);
    }
  Thread::mutexUnlock (&proMutex);
}

void
Display::refreshRC (Display *s)
{
  vector<ReleaseContainer *>::iterator i;
  ReleaseContainer *rc;
  IMediaProvider *dec;
  SDL_Surface *sur;
  SDL_Texture *tex;
  IContinuousMediaProvider *cmp;
  IDiscreteMediaProvider *dmp;
  string strSym = "";

  set<IDiscreteMediaProvider *>::iterator j;

  Thread::mutexLock (&recMutex);

  if (s->releaseList.empty ())
    {
      Thread::mutexUnlock (&recMutex);
      return;
    }

  i = s->releaseList.begin ();
  while (i != s->releaseList.end ())
    {
      rc = (*i);

      dec = rc->iDec;
      sur = rc->uSur;
      tex = rc->uTex;

      delete rc;

      s->releaseList.erase (i);
      Thread::mutexUnlock (&recMutex);

      if (sur != NULL)
        {
          releaseUnderlyingSurface (sur);
        }

      if (tex != NULL)
        {
          releaseTexture (tex);
        }

      if (dec != NULL)
        {
          strSym = "";

          cmp = dynamic_cast<IContinuousMediaProvider *> (dec);

          if (cmp != NULL)
            {
              strSym = cmp->getLoadSymbol ();
              delete cmp;
            }
          else
            {
              dmp = dynamic_cast<IDiscreteMediaProvider *> (dec);

              if (dmp != NULL)
                {
                  Thread::mutexLock (&proMutex);
                  j = s->dmpPool.find (dmp);
                  if (j != s->dmpPool.end ())
                    {
                      s->dmpPool.erase (j);
                    }
                  Thread::mutexUnlock (&proMutex);

                  strSym = dmp->getLoadSymbol ();
                  delete dmp;
                }
            }
        }

      Thread::mutexLock (&recMutex);
      i = s->releaseList.begin ();
    }

  s->releaseList.clear ();
  Thread::mutexUnlock (&recMutex);
}

void
Display::refreshCMP (Display *s)
{
  set<IContinuousMediaProvider *>::iterator i;

  Thread::mutexLock (&proMutex);
  for (i = cmpRenderList.begin (); i != cmpRenderList.end (); i++)
    {
      IContinuousMediaProvider *p = *i;
      if (p->getProviderContent () == NULL)
        {
          SDL_Texture *texture;
          int w, h;

          p->getOriginalResolution (&w, &h);
          texture = createTexture (s->renderer, w, h);
          g_assert_nonnull (texture);
          p->setProviderContent (texture);
        }
      p->refreshDR (NULL);
    }
  Thread::mutexUnlock (&proMutex);
}

void
Display::refreshWin (Display *s)
{
  SDL_Texture *uTex;
  SDLWindow *dstWin;
  SDLWindow *mirrorSrc;

  map<int, map<double, set<SDLWindow *> *> *>::iterator i;
  map<double, set<SDLWindow *> *>::iterator j;
  set<SDLWindow *>::iterator k;

  Thread::mutexLock (&renMutex);
  if (s->renderer != NULL && !renderMap.empty ())
    {
      lockSDL ();
      SDL_RenderClear (s->renderer);
      unlockSDL ();

      i = renderMap.find (0);
      if (i != renderMap.end ())
        {
          j = i->second->begin ();
          while (j != i->second->end ())
            {
              k = j->second->begin ();
              while (k != j->second->end ())
                {
                  dstWin = (SDLWindow *)(*k);

                  if (dstWin->isVisible () && !dstWin->isGhostWindow ())
                    {
                      mirrorSrc = (SDLWindow *)dstWin->getMirrorSrc ();
                      if (mirrorSrc != NULL)
                        {
                          while (mirrorSrc->getMirrorSrc () != NULL)
                            {
                              mirrorSrc
                                  = (SDLWindow *)mirrorSrc->getMirrorSrc ();
                            }
                          uTex = mirrorSrc->getTexture (s->renderer);
                        }
                      else
                        {
                          uTex = dstWin->getTexture (s->renderer);
                        }

                      drawSDLWindow (s->renderer, uTex, dstWin);
                      dstWin->rendered ();
                    }

                  ++k;
                }
              ++j;
            }
        }

      lockSDL ();
      SDL_RenderPresent (s->renderer);
      unlockSDL ();
    }
  Thread::mutexUnlock (&renMutex);
}

void
Display::initScreen (Display *s)
{
  lockSDL ();

  SDL_VideoInit (NULL);

  guint flags = SDL_WINDOW_SHOWN;
  if (s->fullscreen)
    flags |= SDL_WINDOW_FULLSCREEN;

  s->screen = NULL;
  s->renderer = NULL;
  int status = SDL_CreateWindowAndRenderer (s->width, s->height, flags,
                                            &s->screen, &s->renderer);
  g_assert (status == 0);
  g_assert_nonnull (s->screen);
  g_assert_nonnull (s->renderer);

  initCodeMaps ();

  s->im = new InputManager ();

  if (s->im != NULL)
    s->im->setAxisBoundaries (s->width, s->height, 0);

  unlockSDL ();
}

void
Display::clearScreen (Display *s)
{
  SDLWindow *iWin;
  SDLSurface *iSur;
  IContinuousMediaProvider *iCmp;
  IDiscreteMediaProvider *iDmp;

  set<SDLWindow *>::iterator i;
  set<SDLSurface *>::iterator j;
  set<IContinuousMediaProvider *>::iterator k;
  set<IDiscreteMediaProvider *>::iterator l;

  Thread::mutexLock (&winMutex);

  // Releasing remaining Window objects in Window Pool
  if (!s->windowPool.empty ())
    {
      i = s->windowPool.begin ();
      while (i != s->windowPool.end ())
        {
          iWin = (*i);
          if (iWin != NULL)
            {
              delete iWin;
            }
          ++i;
        }
      s->windowPool.clear ();
    }
  Thread::mutexUnlock (&winMutex);

  Thread::mutexLock (&surMutex);
  // Releasing remaining Surface objects in Surface Pool
  if (!s->surfacePool.empty ())
    {
      j = s->surfacePool.begin ();
      while (j != s->surfacePool.end ())
        {
          iSur = (*j);
          if (iSur != NULL)
            {
              delete iSur;
            }
          ++j;
        }
      s->surfacePool.clear ();
    }

  Thread::mutexUnlock (&surMutex);

  Thread::mutexLock (&proMutex);

  // Releasing remaining CMP objects in CMP Pool
  if (!s->cmpPool.empty ())
    {
      k = s->cmpPool.begin ();
      while (k != s->cmpPool.end ())
        {
          iCmp = (*k);

          if (iCmp != NULL)
            {
              iCmp->stop ();
              delete iCmp;
            }
          ++k;
        }
      s->cmpPool.clear ();
    }

  // Releasing remaining DMP objects in DMP Pool
  if (!s->dmpPool.empty ())
    {
      l = s->dmpPool.begin ();
      while (l != s->dmpPool.end ())
        {
          iDmp = *l;

          if (iDmp != NULL)
            {
              delete iDmp;
            }
          ++l;
        }
      s->dmpPool.clear ();
    }

  Thread::mutexUnlock (&proMutex);
}

void
Display::releaseScreen (Display *s)
{
  lockSDL ();

  clearScreen (s);

  if (s->screen != NULL)
    {
      SDL_HideWindow (s->screen);
    }

  if (s->renderer != NULL)
    {
      SDL_DestroyRenderer (s->renderer);
      s->renderer = NULL;
    }

  if (s->screen != NULL)
    {
      SDL_DestroyWindow (s->screen);
      s->screen = NULL;
    }

  unlockSDL ();
}

bool
Display::blitFromWindow (SDLWindow *iWin, SDL_Surface *dest)
{
  SDL_Surface *tmpSur;
  SDL_Texture *tmpTex;
  SDL_Rect rect;

  bool blitted = false;
  bool freeSurface = false;

  lockSDL ();
  iWin->lock ();
  tmpSur = (SDL_Surface *)(iWin->getContent ());

  if (tmpSur == NULL)
    {
      tmpTex = ((SDLWindow *)iWin)->getTexture (NULL);
      if (hasTexture (tmpTex))
        {
          tmpSur = createUnderlyingSurfaceFromTexture (tmpTex);
          freeSurface = true;
        }
    }

  if (tmpSur != NULL)
    {
      rect.x = iWin->getX ();
      rect.y = iWin->getY ();
      rect.w = iWin->getW ();
      rect.h = iWin->getH ();

      if (SDL_UpperBlitScaled (tmpSur, NULL, dest, &rect) < 0)
        {
          SDL_Surface *tmpSur2;

          clog << "Display::blitFromWindow SDL error: '";
          clog << SDL_GetError () << "'! Trying to convert source surface";
          clog << endl;

          tmpSur2 = SDL_ConvertSurface (tmpSur, dest->format, 0);

          if (tmpSur2 != NULL)
            {
              if (SDL_UpperBlitScaled (tmpSur2, NULL, dest, &rect) < 0)
                {
                  clog << "Display::blitFromWindow ";
                  clog << "BLIT from converted surface SDL error: '";
                  clog << SDL_GetError () << "'";
                  clog << endl;
                }
              else
                {
                  blitted = true;
                }
              //createReleaseContainer (tmpSur2, NULL, NULL);
            }
          else
            {
              clog << "Display::blitFromWindow convert surface";
              clog << " SDL error: '";
              clog << SDL_GetError () << "'" << endl;
            }
        }
      else
        {
          blitted = true;
        }
    }

  if (freeSurface)
    {
      freeSurface = false;
      releaseUnderlyingSurface (tmpSur);
    }

  iWin->unlock ();
  unlockSDL ();

  return blitted;
}

/* interfacing input */

InputManager *
Display::getInputManager ()
{
  /*
   * im == NULL is an initial state. So pthread_cond_t
   * is not necessary here.
   */
  while (im == NULL)
    {
      g_usleep (1000000 / SDS_FPS);
    }
  return im;
}

SDLEventBuffer *
Display::createEventBuffer ()
{
  return new SDLEventBuffer ();
}

SDLInputEvent *
Display::createInputEvent (void *event, const int symbol)
{
  SDLInputEvent *ie = NULL;

  if (event != NULL)
    {
      ie = new SDLInputEvent (*(SDL_Event *)event);
    }

  if (symbol >= 0)
    {
      ie = new SDLInputEvent (symbol);
    }

  return ie;
}

SDLInputEvent *
Display::createApplicationEvent (int type, void *data)
{
  return new SDLInputEvent (type, data);
}

int
Display::fromMBToGinga (int keyCode)
{
  map<int, int>::iterator i;
  int translated;

  checkMutexInit ();

  Thread::mutexLock (&sieMutex);

  translated = CodeMap::KEY_NULL;
  i = sdlToGingaCodeMap.find (keyCode);
  if (i != sdlToGingaCodeMap.end ())
    {
      translated = i->second;
    }
  else
    {
      clog << "Display::fromMBToGinga can't find code '";
      clog << keyCode << "' returning KEY_NULL" << endl;
    }

  Thread::mutexUnlock (&sieMutex);

  return translated;
}

int
Display::fromGingaToMB (int keyCode)
{
  map<int, int>::iterator i;
  int translated;

  checkMutexInit ();

  Thread::mutexLock (&sieMutex);

  translated = CodeMap::KEY_NULL;
  i = gingaToSDLCodeMap.find (keyCode);
  if (i != gingaToSDLCodeMap.end ())
    {
      translated = i->second;
    }
  else
    {
      clog << "Display::fromGingaToMB can't find code '";
      clog << keyCode << "' returning KEY_NULL" << endl;
    }

  Thread::mutexUnlock (&sieMutex);

  return translated;
}

/* libgingaccmbsdl internal use*/

/* input */
int
Display::convertEventCodeStrToInt (string strEvent)
{
  int intEvent = -1;
  map<string, int>::iterator i;

  i = sdlStrToSdlCode.find (strEvent);
  if (i != sdlStrToSdlCode.end ())
    {
      intEvent = i->second;
    }

  return intEvent;
}

void
Display::initCodeMaps ()
{
  checkMutexInit ();

  Thread::mutexLock (&sieMutex);
  if (!gingaToSDLCodeMap.empty ())
    {
      Thread::mutexUnlock (&sieMutex);
      return;
    }

  // sdlStrToSdlCode
  sdlStrToSdlCode["GIEK:QUIT"] = SDL_QUIT;
  sdlStrToSdlCode["GIEK:UNKNOWN"] = SDLK_UNKNOWN;
  sdlStrToSdlCode["GIEK:0"] = SDLK_0;
  sdlStrToSdlCode["GIEK:1"] = SDLK_1;
  sdlStrToSdlCode["GIEK:2"] = SDLK_2;
  sdlStrToSdlCode["GIEK:3"] = SDLK_3;
  sdlStrToSdlCode["GIEK:4"] = SDLK_4;
  sdlStrToSdlCode["GIEK:5"] = SDLK_5;
  sdlStrToSdlCode["GIEK:6"] = SDLK_6;
  sdlStrToSdlCode["GIEK:7"] = SDLK_7;
  sdlStrToSdlCode["GIEK:8"] = SDLK_8;
  sdlStrToSdlCode["GIEK:9"] = SDLK_9;

  sdlStrToSdlCode["GIEK:a"] = SDLK_a;
  sdlStrToSdlCode["GIEK:b"] = SDLK_b;
  sdlStrToSdlCode["GIEK:c"] = SDLK_c;
  sdlStrToSdlCode["GIEK:d"] = SDLK_d;
  sdlStrToSdlCode["GIEK:e"] = SDLK_e;
  sdlStrToSdlCode["GIEK:f"] = SDLK_f;
  sdlStrToSdlCode["GIEK:g"] = SDLK_g;
  sdlStrToSdlCode["GIEK:h"] = SDLK_h;
  sdlStrToSdlCode["GIEK:i"] = SDLK_i;
  sdlStrToSdlCode["GIEK:j"] = SDLK_j;
  sdlStrToSdlCode["GIEK:k"] = SDLK_k;
  sdlStrToSdlCode["GIEK:l"] = SDLK_l;
  sdlStrToSdlCode["GIEK:m"] = SDLK_m;
  sdlStrToSdlCode["GIEK:n"] = SDLK_n;
  sdlStrToSdlCode["GIEK:o"] = SDLK_o;
  sdlStrToSdlCode["GIEK:p"] = SDLK_p;
  sdlStrToSdlCode["GIEK:q"] = SDLK_q;
  sdlStrToSdlCode["GIEK:r"] = SDLK_r;
  sdlStrToSdlCode["GIEK:s"] = SDLK_s;
  sdlStrToSdlCode["GIEK:t"] = SDLK_t;
  sdlStrToSdlCode["GIEK:u"] = SDLK_u;
  sdlStrToSdlCode["GIEK:v"] = SDLK_v;
  sdlStrToSdlCode["GIEK:w"] = SDLK_w;
  sdlStrToSdlCode["GIEK:x"] = SDLK_x;
  sdlStrToSdlCode["GIEK:y"] = SDLK_y;
  sdlStrToSdlCode["GIEK:z"] = SDLK_z;

  sdlStrToSdlCode["GIEK:A"] = SDLK_a + 5000;
  sdlStrToSdlCode["GIEK:B"] = SDLK_b + 5000;
  sdlStrToSdlCode["GIEK:C"] = SDLK_c + 5000;
  sdlStrToSdlCode["GIEK:D"] = SDLK_d + 5000;
  sdlStrToSdlCode["GIEK:E"] = SDLK_e + 5000;
  sdlStrToSdlCode["GIEK:F"] = SDLK_f + 5000;
  sdlStrToSdlCode["GIEK:G"] = SDLK_g + 5000;
  sdlStrToSdlCode["GIEK:H"] = SDLK_h + 5000;
  sdlStrToSdlCode["GIEK:I"] = SDLK_i + 5000;
  sdlStrToSdlCode["GIEK:J"] = SDLK_j + 5000;
  sdlStrToSdlCode["GIEK:K"] = SDLK_k + 5000;
  sdlStrToSdlCode["GIEK:L"] = SDLK_l + 5000;
  sdlStrToSdlCode["GIEK:M"] = SDLK_m + 5000;
  sdlStrToSdlCode["GIEK:N"] = SDLK_n + 5000;
  sdlStrToSdlCode["GIEK:O"] = SDLK_o + 5000;
  sdlStrToSdlCode["GIEK:P"] = SDLK_p + 5000;
  sdlStrToSdlCode["GIEK:Q"] = SDLK_q + 5000;
  sdlStrToSdlCode["GIEK:R"] = SDLK_r + 5000;
  sdlStrToSdlCode["GIEK:S"] = SDLK_s + 5000;
  sdlStrToSdlCode["GIEK:T"] = SDLK_t + 5000;
  sdlStrToSdlCode["GIEK:U"] = SDLK_u + 5000;
  sdlStrToSdlCode["GIEK:V"] = SDLK_v + 5000;
  sdlStrToSdlCode["GIEK:W"] = SDLK_w + 5000;
  sdlStrToSdlCode["GIEK:X"] = SDLK_x + 5000;
  sdlStrToSdlCode["GIEK:Y"] = SDLK_y + 5000;
  sdlStrToSdlCode["GIEK:Z"] = SDLK_z + 5000;

  sdlStrToSdlCode["GIEK:PAGEDOWN"] = SDLK_PAGEDOWN;
  sdlStrToSdlCode["GIEK:PAGEUP"] = SDLK_PAGEUP;

  sdlStrToSdlCode["GIEK:F1"] = SDLK_F1;
  sdlStrToSdlCode["GIEK:F2"] = SDLK_F2;
  sdlStrToSdlCode["GIEK:F3"] = SDLK_F3;
  sdlStrToSdlCode["GIEK:F4"] = SDLK_F4;
  sdlStrToSdlCode["GIEK:F5"] = SDLK_F5;
  sdlStrToSdlCode["GIEK:F6"] = SDLK_F6;
  sdlStrToSdlCode["GIEK:F7"] = SDLK_F7;
  sdlStrToSdlCode["GIEK:F8"] = SDLK_F8;
  sdlStrToSdlCode["GIEK:F9"] = SDLK_F9;
  sdlStrToSdlCode["GIEK:F10"] = SDLK_F10;
  sdlStrToSdlCode["GIEK:F11"] = SDLK_F11;
  sdlStrToSdlCode["GIEK:F12"] = SDLK_F12;

  sdlStrToSdlCode["GIEK:PLUS"] = SDLK_PLUS;
  sdlStrToSdlCode["GIEK:MINUS"] = SDLK_MINUS;

  sdlStrToSdlCode["GIEK:ASTERISK"] = SDLK_ASTERISK;
  sdlStrToSdlCode["GIEK:HASH"] = SDLK_HASH;

  sdlStrToSdlCode["GIEK:PERIOD"] = SDLK_PERIOD;

  sdlStrToSdlCode["GIEK:CAPSLOCK"] = SDLK_CAPSLOCK;
  sdlStrToSdlCode["GIEK:PRINTSCREEN"] = SDLK_PRINTSCREEN;
  sdlStrToSdlCode["GIEK:MENU"] = SDLK_MENU;
  sdlStrToSdlCode["GIEK:F14"] = SDLK_F14;
  sdlStrToSdlCode["GIEK:QUESTION"] = SDLK_QUESTION;

  sdlStrToSdlCode["GIEK:DOWN"] = SDLK_DOWN;
  sdlStrToSdlCode["GIEK:LEFT"] = SDLK_LEFT;
  sdlStrToSdlCode["GIEK:RIGHT"] = SDLK_RIGHT;
  sdlStrToSdlCode["GIEK:UP"] = SDLK_UP;

  sdlStrToSdlCode["GIEK:F15"] = SDLK_F15;
  sdlStrToSdlCode["GIEK:F16"] = SDLK_F16;

  sdlStrToSdlCode["GIEK:VOLUMEDOWN"] = SDLK_VOLUMEDOWN;
  sdlStrToSdlCode["GIEK:VOLUMEUP"] = SDLK_VOLUMEUP;

  sdlStrToSdlCode["GIEK:RETURN"] = SDLK_RETURN;
  sdlStrToSdlCode["GIEK:RETURN2"] = SDLK_RETURN2;

  sdlStrToSdlCode["GIEK:F17"] = SDLK_F17;
  sdlStrToSdlCode["GIEK:F18"] = SDLK_F18;
  sdlStrToSdlCode["GIEK:F19"] = SDLK_F19;
  sdlStrToSdlCode["GIEK:F20"] = SDLK_F20;

  sdlStrToSdlCode["GIEK:SPACE"] = SDLK_SPACE;
  sdlStrToSdlCode["GIEK:BACKSPACE"] = SDLK_BACKSPACE;
  sdlStrToSdlCode["GIEK:AC_BACK"] = SDLK_AC_BACK;
  sdlStrToSdlCode["GIEK:ESCAPE"] = SDLK_ESCAPE;
  sdlStrToSdlCode["GIEK:OUT"] = SDLK_OUT;

  sdlStrToSdlCode["GIEK:POWER"] = SDLK_POWER;
  sdlStrToSdlCode["GIEK:F21"] = SDLK_F21;
  sdlStrToSdlCode["GIEK:STOP"] = SDLK_STOP;
  sdlStrToSdlCode["GIEK:EJECT"] = SDLK_EJECT;
  sdlStrToSdlCode["GIEK:EXECUTE"] = SDLK_EXECUTE;
  sdlStrToSdlCode["GIEK:F22"] = SDLK_F22;
  sdlStrToSdlCode["GIEK:PAUSE"] = SDLK_PAUSE;

  sdlStrToSdlCode["GIEK:GREATER"] = SDLK_GREATER;
  sdlStrToSdlCode["GIEK:LESS"] = SDLK_LESS;

  sdlStrToSdlCode["GIEK:TAB"] = SDLK_TAB;
  sdlStrToSdlCode["GIEK:F23"] = SDLK_F23;

  // gingaToSDLCodeMap
  gingaToSDLCodeMap[CodeMap::KEY_QUIT] = SDL_QUIT;
  gingaToSDLCodeMap[CodeMap::KEY_NULL] = SDLK_UNKNOWN;
  gingaToSDLCodeMap[CodeMap::KEY_0] = SDLK_0;
  gingaToSDLCodeMap[CodeMap::KEY_1] = SDLK_1;
  gingaToSDLCodeMap[CodeMap::KEY_2] = SDLK_2;
  gingaToSDLCodeMap[CodeMap::KEY_3] = SDLK_3;
  gingaToSDLCodeMap[CodeMap::KEY_4] = SDLK_4;
  gingaToSDLCodeMap[CodeMap::KEY_5] = SDLK_5;
  gingaToSDLCodeMap[CodeMap::KEY_6] = SDLK_6;
  gingaToSDLCodeMap[CodeMap::KEY_7] = SDLK_7;
  gingaToSDLCodeMap[CodeMap::KEY_8] = SDLK_8;
  gingaToSDLCodeMap[CodeMap::KEY_9] = SDLK_9;

  gingaToSDLCodeMap[CodeMap::KEY_SMALL_A] = SDLK_a;
  gingaToSDLCodeMap[CodeMap::KEY_SMALL_B] = SDLK_b;
  gingaToSDLCodeMap[CodeMap::KEY_SMALL_C] = SDLK_c;
  gingaToSDLCodeMap[CodeMap::KEY_SMALL_D] = SDLK_d;
  gingaToSDLCodeMap[CodeMap::KEY_SMALL_E] = SDLK_e;
  gingaToSDLCodeMap[CodeMap::KEY_SMALL_F] = SDLK_f;
  gingaToSDLCodeMap[CodeMap::KEY_SMALL_G] = SDLK_g;
  gingaToSDLCodeMap[CodeMap::KEY_SMALL_H] = SDLK_h;
  gingaToSDLCodeMap[CodeMap::KEY_SMALL_I] = SDLK_i;
  gingaToSDLCodeMap[CodeMap::KEY_SMALL_J] = SDLK_j;
  gingaToSDLCodeMap[CodeMap::KEY_SMALL_K] = SDLK_k;
  gingaToSDLCodeMap[CodeMap::KEY_SMALL_L] = SDLK_l;
  gingaToSDLCodeMap[CodeMap::KEY_SMALL_M] = SDLK_m;
  gingaToSDLCodeMap[CodeMap::KEY_SMALL_N] = SDLK_n;
  gingaToSDLCodeMap[CodeMap::KEY_SMALL_O] = SDLK_o;
  gingaToSDLCodeMap[CodeMap::KEY_SMALL_P] = SDLK_p;
  gingaToSDLCodeMap[CodeMap::KEY_SMALL_Q] = SDLK_q;
  gingaToSDLCodeMap[CodeMap::KEY_SMALL_R] = SDLK_r;
  gingaToSDLCodeMap[CodeMap::KEY_SMALL_S] = SDLK_s;
  gingaToSDLCodeMap[CodeMap::KEY_SMALL_T] = SDLK_t;
  gingaToSDLCodeMap[CodeMap::KEY_SMALL_U] = SDLK_u;
  gingaToSDLCodeMap[CodeMap::KEY_SMALL_V] = SDLK_v;
  gingaToSDLCodeMap[CodeMap::KEY_SMALL_W] = SDLK_w;
  gingaToSDLCodeMap[CodeMap::KEY_SMALL_X] = SDLK_x;
  gingaToSDLCodeMap[CodeMap::KEY_SMALL_Y] = SDLK_y;
  gingaToSDLCodeMap[CodeMap::KEY_SMALL_Z] = SDLK_z;

  gingaToSDLCodeMap[CodeMap::KEY_CAPITAL_A] = SDLK_a + 5000;
  gingaToSDLCodeMap[CodeMap::KEY_CAPITAL_B] = SDLK_b + 5000;
  gingaToSDLCodeMap[CodeMap::KEY_CAPITAL_C] = SDLK_c + 5000;
  gingaToSDLCodeMap[CodeMap::KEY_CAPITAL_D] = SDLK_d + 5000;
  gingaToSDLCodeMap[CodeMap::KEY_CAPITAL_E] = SDLK_e + 5000;
  gingaToSDLCodeMap[CodeMap::KEY_CAPITAL_F] = SDLK_f + 5000;
  gingaToSDLCodeMap[CodeMap::KEY_CAPITAL_G] = SDLK_g + 5000;
  gingaToSDLCodeMap[CodeMap::KEY_CAPITAL_H] = SDLK_h + 5000;
  gingaToSDLCodeMap[CodeMap::KEY_CAPITAL_I] = SDLK_i + 5000;
  gingaToSDLCodeMap[CodeMap::KEY_CAPITAL_J] = SDLK_j + 5000;
  gingaToSDLCodeMap[CodeMap::KEY_CAPITAL_K] = SDLK_k + 5000;
  gingaToSDLCodeMap[CodeMap::KEY_CAPITAL_L] = SDLK_l + 5000;
  gingaToSDLCodeMap[CodeMap::KEY_CAPITAL_M] = SDLK_m + 5000;
  gingaToSDLCodeMap[CodeMap::KEY_CAPITAL_N] = SDLK_n + 5000;
  gingaToSDLCodeMap[CodeMap::KEY_CAPITAL_O] = SDLK_o + 5000;
  gingaToSDLCodeMap[CodeMap::KEY_CAPITAL_P] = SDLK_p + 5000;
  gingaToSDLCodeMap[CodeMap::KEY_CAPITAL_Q] = SDLK_q + 5000;
  gingaToSDLCodeMap[CodeMap::KEY_CAPITAL_R] = SDLK_r + 5000;
  gingaToSDLCodeMap[CodeMap::KEY_CAPITAL_S] = SDLK_s + 5000;
  gingaToSDLCodeMap[CodeMap::KEY_CAPITAL_T] = SDLK_t + 5000;
  gingaToSDLCodeMap[CodeMap::KEY_CAPITAL_U] = SDLK_u + 5000;
  gingaToSDLCodeMap[CodeMap::KEY_CAPITAL_V] = SDLK_v + 5000;
  gingaToSDLCodeMap[CodeMap::KEY_CAPITAL_W] = SDLK_w + 5000;
  gingaToSDLCodeMap[CodeMap::KEY_CAPITAL_X] = SDLK_x + 5000;
  gingaToSDLCodeMap[CodeMap::KEY_CAPITAL_Y] = SDLK_y + 5000;
  gingaToSDLCodeMap[CodeMap::KEY_CAPITAL_Z] = SDLK_z + 5000;

  gingaToSDLCodeMap[CodeMap::KEY_PAGE_DOWN] = SDLK_PAGEDOWN;
  gingaToSDLCodeMap[CodeMap::KEY_PAGE_UP] = SDLK_PAGEUP;

  gingaToSDLCodeMap[CodeMap::KEY_F1] = SDLK_F1;
  gingaToSDLCodeMap[CodeMap::KEY_F2] = SDLK_F2;
  gingaToSDLCodeMap[CodeMap::KEY_F3] = SDLK_F3;
  gingaToSDLCodeMap[CodeMap::KEY_F4] = SDLK_F4;
  gingaToSDLCodeMap[CodeMap::KEY_F5] = SDLK_F5;
  gingaToSDLCodeMap[CodeMap::KEY_F6] = SDLK_F6;
  gingaToSDLCodeMap[CodeMap::KEY_F7] = SDLK_F7;
  gingaToSDLCodeMap[CodeMap::KEY_F8] = SDLK_F8;
  gingaToSDLCodeMap[CodeMap::KEY_F9] = SDLK_F9;
  gingaToSDLCodeMap[CodeMap::KEY_F10] = SDLK_F10;
  gingaToSDLCodeMap[CodeMap::KEY_F11] = SDLK_F11;
  gingaToSDLCodeMap[CodeMap::KEY_F12] = SDLK_F12;

  gingaToSDLCodeMap[CodeMap::KEY_PLUS_SIGN] = SDLK_PLUS;
  gingaToSDLCodeMap[CodeMap::KEY_MINUS_SIGN] = SDLK_MINUS;

  gingaToSDLCodeMap[CodeMap::KEY_ASTERISK] = SDLK_ASTERISK;
  gingaToSDLCodeMap[CodeMap::KEY_NUMBER_SIGN] = SDLK_HASH;

  gingaToSDLCodeMap[CodeMap::KEY_PERIOD] = SDLK_PERIOD;

  gingaToSDLCodeMap[CodeMap::KEY_SUPER] = SDLK_CAPSLOCK;
  gingaToSDLCodeMap[CodeMap::KEY_PRINTSCREEN] = SDLK_PRINTSCREEN;
  gingaToSDLCodeMap[CodeMap::KEY_MENU] = SDLK_MENU;
  gingaToSDLCodeMap[CodeMap::KEY_INFO] = SDLK_F14;
  gingaToSDLCodeMap[CodeMap::KEY_EPG] = SDLK_QUESTION;

  gingaToSDLCodeMap[CodeMap::KEY_CURSOR_DOWN] = SDLK_DOWN;
  gingaToSDLCodeMap[CodeMap::KEY_CURSOR_LEFT] = SDLK_LEFT;
  gingaToSDLCodeMap[CodeMap::KEY_CURSOR_RIGHT] = SDLK_RIGHT;
  gingaToSDLCodeMap[CodeMap::KEY_CURSOR_UP] = SDLK_UP;

  gingaToSDLCodeMap[CodeMap::KEY_CHANNEL_DOWN] = SDLK_F15;
  gingaToSDLCodeMap[CodeMap::KEY_CHANNEL_UP] = SDLK_F16;

  gingaToSDLCodeMap[CodeMap::KEY_VOLUME_DOWN] = SDLK_VOLUMEDOWN;
  gingaToSDLCodeMap[CodeMap::KEY_VOLUME_UP] = SDLK_VOLUMEUP;

  gingaToSDLCodeMap[CodeMap::KEY_ENTER] = SDLK_RETURN;

  gingaToSDLCodeMap[CodeMap::KEY_RED] = SDLK_F17;
  gingaToSDLCodeMap[CodeMap::KEY_GREEN] = SDLK_F18;
  gingaToSDLCodeMap[CodeMap::KEY_YELLOW] = SDLK_F19;
  gingaToSDLCodeMap[CodeMap::KEY_BLUE] = SDLK_F20;

  gingaToSDLCodeMap[CodeMap::KEY_SPACE] = SDLK_SPACE;
  gingaToSDLCodeMap[CodeMap::KEY_BACKSPACE] = SDLK_BACKSPACE;
  gingaToSDLCodeMap[CodeMap::KEY_BACK] = SDLK_AC_BACK;
  gingaToSDLCodeMap[CodeMap::KEY_ESCAPE] = SDLK_ESCAPE;
  gingaToSDLCodeMap[CodeMap::KEY_EXIT] = SDLK_OUT;

  gingaToSDLCodeMap[CodeMap::KEY_POWER] = SDLK_POWER;
  gingaToSDLCodeMap[CodeMap::KEY_REWIND] = SDLK_F21;
  gingaToSDLCodeMap[CodeMap::KEY_STOP] = SDLK_STOP;
  gingaToSDLCodeMap[CodeMap::KEY_EJECT] = SDLK_EJECT;
  gingaToSDLCodeMap[CodeMap::KEY_PLAY] = SDLK_EXECUTE;
  gingaToSDLCodeMap[CodeMap::KEY_RECORD] = SDLK_F22;
  gingaToSDLCodeMap[CodeMap::KEY_PAUSE] = SDLK_PAUSE;

  gingaToSDLCodeMap[CodeMap::KEY_GREATER_THAN_SIGN] = SDLK_GREATER;
  gingaToSDLCodeMap[CodeMap::KEY_LESS_THAN_SIGN] = SDLK_LESS;

  gingaToSDLCodeMap[CodeMap::KEY_TAB] = SDLK_TAB;
  gingaToSDLCodeMap[CodeMap::KEY_TAP] = SDLK_F23;

  // sdlToGingaCodeMap
  map<int, int>::iterator i;
  i = gingaToSDLCodeMap.begin ();
  while (i != gingaToSDLCodeMap.end ())
    {
      sdlToGingaCodeMap[i->second] = i->first;
      ++i;
    }

  Thread::mutexUnlock (&sieMutex);
}

/* output */
void
Display::renderMapInsertWindow (SDLWindow *iWin, double z)
{
  map<int, map<double, set<SDLWindow *> *> *>::iterator i;
  map<double, set<SDLWindow *> *>::iterator j;

  map<double, set<SDLWindow *> *> *sortedMap;
  set<SDLWindow *> *windows;

  checkMutexInit ();

  Thread::mutexLock (&renMutex);
  i = renderMap.find (0);
  if (i != renderMap.end ())
    {
      sortedMap = i->second;
    }
  else
    {
      sortedMap = new map<double, set<SDLWindow *> *>;
      renderMap[0] = sortedMap;
    }

  j = sortedMap->find (z);
  if (j != sortedMap->end ())
    {
      windows = j->second;
    }
  else
    {
      windows = new set<SDLWindow *>;
      (*sortedMap)[z] = windows;
    }

  windows->insert (iWin);
  Thread::mutexUnlock (&renMutex);
}

void
Display::renderMapRemoveWindow (SDLWindow *iWin, double z)
{
  map<int, map<double, set<SDLWindow *> *> *>::iterator i;
  map<double, set<SDLWindow *> *>::iterator j;
  set<SDLWindow *>::iterator k;

  map<double, set<SDLWindow *> *> *sortedMap;
  set<SDLWindow *> *windows;

  checkMutexInit ();

  Thread::mutexLock (&renMutex);
  i = renderMap.find (0);
  if (i != renderMap.end ())
    {
      sortedMap = i->second;
      j = sortedMap->find (z);
      if (j != sortedMap->end ())
        {
          windows = j->second;
          k = windows->find (iWin);
          if (k != windows->end ())
            {
              windows->erase (k);
            }
          if (windows->empty ())
            {
              delete windows;
              sortedMap->erase (j);
            }
        }
    }
  Thread::mutexUnlock (&renMutex);
}

bool
Display::drawSDLWindow (SDL_Renderer *renderer,
                                SDL_Texture *srcTxtr, SDLWindow *dstWin)
{
  SDL_Rect dstRect;
  Color *bgColor;
  Uint8 rr, rg, rb, ra;
  int i, bw;
  guint8 r, g, b, a;
  int alpha = 0;

  bool drawing = false;

  //DrawData *dd;
  //SDL_Rect dr;
  //vector<DrawData *> *drawData;
  vector<DrawData *>::iterator it;

  lockSDL ();

  if (dstWin != NULL)
    {
      /* getting renderer previous state */
      SDL_GetRenderDrawColor (renderer, &rr, &rg, &rb, &ra);

      dstRect.x = dstWin->getX ();
      dstRect.y = dstWin->getY ();
      dstRect.w = dstWin->getW ();
      dstRect.h = dstWin->getH ();

      alpha = dstWin->getTransparencyValue ();
      if (srcTxtr != NULL)
        {
          SDL_SetTextureAlphaMod (srcTxtr, (unsigned char) (255 - (unsigned char) (CLAMP (alpha, 0, 255))));
        }

      /* setting window background */
      bgColor = dstWin->getBgColor ();
      if (bgColor != NULL)
        {
          drawing = true;
          if (alpha == 0)
            {
              alpha = 255 - bgColor->getAlpha ();
            }

          r = bgColor->getR ();
          g = bgColor->getG ();
          b = bgColor->getB ();

          SDL_SetRenderDrawColor (renderer, (gint8) bgColor->getR (),
                                  (gint8) bgColor->getG (), (gint8) bgColor->getB (),
                                  (gint8)(255 - (gint8) alpha));

          if (SDL_RenderFillRect (renderer, &dstRect) < 0)
            {
              clog << "Display::drawWindow ";
              clog << "Warning! Can't use render to fill rect ";
              clog << SDL_GetError ();
              clog << endl;
            }
        }

      /* geometric figures (lua only) */
#if 0
      drawData = ((SDLWindow *)dstWin)->createDrawDataList ();
      if (drawData != NULL)
        {
          drawing = true;
          it = drawData->begin ();
          while (it != drawData->end ())
            {
              dd = (*it);
              SDL_SetRenderDrawColor (renderer, (gint8) dd->r, (gint8) dd->g, (gint8) dd->b, (gint8) dd->a);

              switch (dd->dataType)
                {
                case SDLWindow::DDT_LINE:
                  if ((dd->coord1 < dstRect.x) || (dd->coord2 < dstRect.y)
                      || (dd->coord1 > dstRect.w)
                      || (dd->coord2 > dstRect.h)
                      || (dd->coord3 > dstRect.w)
                      || (dd->coord4 > dstRect.h))
                    {
                      clog << "Display::drawWindow Warning!";
                      clog << " Invalid line coords: " << endl;
                      clog << dd->coord1 << ", ";
                      clog << dd->coord2 << ", ";
                      clog << dd->coord3 << ", ";
                      clog << dd->coord4 << "'";
                      clog << endl;
                      clog << "Window rect coords: " << endl;
                      clog << dstRect.x << ", ";
                      clog << dstRect.y << ", ";
                      clog << dstRect.w << ", ";
                      clog << dstRect.h << "'";
                      clog << endl;
                      break;
                    }

                  if (SDL_RenderDrawLine (renderer, dd->coord1 + dstRect.x,
                                          dd->coord2 + dstRect.y,
                                          dd->coord3 + dstRect.x,
                                          dd->coord4 + dstRect.y)
                      < 0)
                    {
                      clog << "Display::drawWindow ";
                      clog << "Warning! Can't draw line ";
                      clog << SDL_GetError ();
                      clog << endl;
                    }

                  break;

                case SDLWindow::DDT_RECT:
                  dr.x = dd->coord1 + dstRect.x;
                  dr.y = dd->coord2 + dstRect.y;
                  dr.w = dd->coord3;
                  dr.h = dd->coord4;

                  if ((dr.x > +dstRect.x + dstRect.w)
                      || (dr.y > +dstRect.y + dstRect.h)
                      || (dd->coord1 + dr.w > dstRect.w)
                      || (dd->coord2 + dr.h > dstRect.h))
                    {
                      clog << "Display::drawWindow Warning!";
                      clog << " Invalid rect coords: " << endl;
                      clog << dr.x << ", ";
                      clog << dr.y << ", ";
                      clog << dr.w << ", ";
                      clog << dr.h << "'";
                      clog << endl;
                      clog << "Window rect coords: " << endl;
                      clog << dstRect.x << ", ";
                      clog << dstRect.y << ", ";
                      clog << dstRect.w << ", ";
                      clog << dstRect.h << "'";
                      clog << endl;
                      break;
                    }

                  if (dd->dataType == SDLWindow::DDT_RECT)
                    {
                      if (SDL_RenderDrawRect (renderer, &dr) < 0)
                        {
                          clog << "Display::drawWindow ";
                          clog << "Warning! Can't draw rect ";
                          clog << SDL_GetError ();
                          clog << endl;
                        }
                    }
                  else
                    {
                      if (SDL_RenderFillRect (renderer, &dr) < 0)
                        {
                          clog << "Display::drawWindow ";
                          clog << "Warning! Can't fill rect ";
                          clog << SDL_GetError ();
                          clog << endl;
                        }
                    }
                  break;

                default:
                  g_assert_not_reached ();
                }
              ++it;
            }
          delete drawData;
        }
#endif
      /* window rendering */
      if (hasTexture (srcTxtr))
        {
          /*void* pixels;
          int tpitch;
          bool locked;*/

          // trying to lock texture
          /*locked = SDL_LockTexture(
                          texture, NULL, &pixels, &tpitch) == 0;*/

          /*
           * Warning: there is no need to lock the texture
           * lock the texture can imply some delay in
           * the decoder procedure
           */

          drawing = true;
          if (SDL_RenderCopy (renderer, srcTxtr, NULL, &dstRect) < 0)
            {
              clog << "Display::drawWindow Warning! ";
              clog << "can't perform render copy " << SDL_GetError ();
              clog << endl;
            }

          /*if (locked) {
                  SDL_UnlockTexture(texture);
          }*/
        }

      /* window border */
      dstWin->getBorder (&r, &g, &b, &a, &bw);
      if (bw != 0)
        {
          SDL_SetRenderDrawColor (renderer, (gint8) r, (gint8) g, (gint8) b, (gint8) a);

          i = 0;
          while (i != bw)
            {
              dstRect.x = dstWin->getX () - i;
              dstRect.y = dstWin->getY () - i;
              dstRect.w = dstWin->getW () + 2 * i;
              dstRect.h = dstWin->getH () + 2 * i;

              if (SDL_RenderDrawRect (renderer, &dstRect) < 0)
                {
                  clog << "Display::drawWindow SDL error: '";
                  clog << SDL_GetError () << "'" << endl;
                }

              if (bw < 0)
                {
                  i--;
                }
              else
                {
                  i++;
                }
            }
        }

      /* setting renderer previous state */
      SDL_SetRenderDrawColor (renderer, rr, rg, rb, ra);
    }
  else
    {
      clog << "Display::drawWindow Warning! ";
      clog << "NULL interface window";
      clog << endl;
    }

  unlockSDL ();

  return (drawing);
}

SDL_Texture *
Display::createTextureFromSurface (SDL_Renderer *renderer,
                                           SDL_Surface *surface)
{
  SDL_Texture *texture = NULL;

  checkMutexInit ();

  lockSDL ();
  Thread::mutexLock (&surMutex);

  if (Display::hasUnderlyingSurface (surface))
    {
      texture = SDL_CreateTextureFromSurface (renderer, surface);
      if (texture == NULL)
        {
          clog << "Display::createTextureFromSurface Warning! ";
          clog << "Couldn't create texture: " << SDL_GetError ();
          clog << endl;
        }
      else
        {
          uTexPool.insert (texture);

          /* allowing alpha */
          SDL_SetTextureBlendMode (texture, SDL_BLENDMODE_BLEND);
        }
    }

  Thread::mutexUnlock (&surMutex);
  unlockSDL ();

  return texture;
}

SDL_Texture *
Display::createTexture (SDL_Renderer *renderer, int w, int h)
{
  SDL_Texture *texture;

  lockSDL ();

  texture = SDL_CreateTexture (renderer, SDL_PIXELFORMAT_RGB24,
                               SDL_TEXTUREACCESS_STREAMING, w, h);

  // w > maxW || h > maxH || format is not supported
  assert (texture != NULL);

  /* allowing alpha */
  SDL_SetTextureBlendMode (texture, SDL_BLENDMODE_BLEND);

  uTexPool.insert (texture);

  unlockSDL ();

  return texture;
}

bool
Display::hasTexture (SDL_Texture *uTex)
{
  set<SDL_Texture *>::iterator i;
  bool hasIt = false;

  checkMutexInit ();

  lockSDL ();
  i = uTexPool.find (uTex);
  if (i != uTexPool.end ())
    {
      hasIt = true;
    }
  unlockSDL ();

  return hasIt;
}

void
Display::releaseTexture (SDL_Texture *texture)
{
  set<SDL_Texture *>::iterator i;

  checkMutexInit ();

  lockSDL ();
  i = uTexPool.find (texture);
  if (i != uTexPool.end ())
    {
      uTexPool.erase (i);
      SDL_DestroyTexture (texture);
    }
  unlockSDL ();
}

void
Display::addUnderlyingSurface (SDL_Surface *uSur)
{
  checkMutexInit ();

  Thread::mutexLock (&surMutex);
  uSurPool.insert (uSur);
  Thread::mutexUnlock (&surMutex);
}

SDL_Surface *
Display::createUnderlyingSurface (int width, int height)
{
  SDL_Surface *newUSur = NULL;
  Uint32 rmask, gmask, bmask, amask;
  int bpp;

  checkMutexInit ();

  lockSDL ();

  SDL_PixelFormatEnumToMasks (SDL_PIXELFORMAT_RGB24, &bpp, &rmask, &gmask, &bmask,
                              &amask);

  newUSur = SDL_CreateRGBSurface (0, width, height, bpp, rmask, gmask,
                                  bmask, amask);

  SDL_SetColorKey (newUSur, 1, *((Uint8 *)newUSur->pixels));
  unlockSDL ();

  Thread::mutexLock (&surMutex);
  if (newUSur != NULL)
    {
      uSurPool.insert (newUSur);
    }
  else
    {
      clog << "Display::createUnderlyingSurface SDL error: '";
      clog << SDL_GetError () << "'" << endl;
    }
  Thread::mutexUnlock (&surMutex);

  return newUSur;
}

SDL_Surface *
Display::createUnderlyingSurfaceFromTexture (SDL_Texture *texture)
{
  SDL_Surface *uSur = NULL;
  void *pixels;
  int tpitch[3];
  Uint32 rmask, gmask, bmask, amask, format;
  int textureAccess, w, h, bpp;

  lockSDL ();

  SDL_QueryTexture (texture, &format, &textureAccess, &w, &h);
  if (textureAccess & SDL_TEXTUREACCESS_STREAMING)
    {
      bool locked = true;

      // trying to lock texture
      if (SDL_LockTexture (texture, NULL, &pixels, &tpitch[0]) != 0)
        {
          locked = false;
        }

      SDL_PixelFormatEnumToMasks (SDL_PIXELFORMAT_RGB24, &bpp, &rmask, &gmask,
                                  &bmask, &amask);

      uSur = SDL_CreateRGBSurfaceFrom (pixels, w, h, bpp, tpitch[0], rmask,
                                       gmask, bmask, amask);

      if (locked)
        {
          SDL_UnlockTexture (texture);
        }
    }

  unlockSDL ();

  Thread::mutexLock (&surMutex);
  if (uSur != NULL)
    {
      uSurPool.insert (uSur);
    }
  Thread::mutexUnlock (&surMutex);

  return uSur;
}

bool
Display::hasUnderlyingSurface (SDL_Surface *uSur)
{
  set<SDL_Surface *>::iterator i;
  bool hasIt = false;

  checkMutexInit ();

  Thread::mutexLock (&surMutex);
  i = uSurPool.find (uSur);
  if (i != uSurPool.end ())
    {
      hasIt = true;
    }
  Thread::mutexUnlock (&surMutex);

  return hasIt;
}

void
Display::releaseUnderlyingSurface (SDL_Surface *uSur)
{
  set<SDL_Surface *>::iterator i;

  checkMutexInit ();

  lockSDL ();
  Thread::mutexLock (&surMutex);

  i = uSurPool.find (uSur);
  if (i != uSurPool.end ())
    {
      uSurPool.erase (i);

      SDL_FreeSurface (uSur);
    }

  Thread::mutexUnlock (&surMutex);
  unlockSDL ();
}

GINGA_MB_END
