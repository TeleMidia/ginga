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
#include "DisplayDebug.h"

#include "AudioProvider.h"
#include "SDLSurface.h"
#include "VideoProvider.h"
#include "SDLWindow.h"

#include <cairo.h>
#include <pango/pangocairo.h>

GINGA_MB_BEGIN

// Global display; initialized by main().
Display *_Ginga_Display = NULL;

bool Display::mutexInitialized = false;
set<SDL_Texture *> Display::uTexPool;
set<SDL_Surface *> Display::uSurPool;


// BEGIN SANITY ------------------------------------------------------------

// Entry in display job list.
struct _DisplayJob
{
  DisplayJobCallback func;
  void *data;
};

// Deletes job entry.
static void
job_delete (DisplayJob *job)
{
  delete job;
}

// Compares the z-index of two windows.
static gint
win_cmp_z (SDLWindow *w1, SDLWindow *w2)
{
  double z1 = w1->getZ ();
  double z2 = w2->getZ ();
  return (z1 < z2) ? -1 : (z1 > z2) ? 1 : 0;
}

// Deletes window.
static void
win_delete (SDLWindow *win)
{
  delete win;
}

// Deletes provider.
static void
prov_delete (gpointer p)
{
  IContinuousMediaProvider *prov = (IContinuousMediaProvider *) p;
  delete prov;
}


// Private methods.

gpointer
Display::add (GList **list, gpointer data)
{
  this->lock ();
  if (unlikely (g_list_find (*list, data)))
    {
      g_warning ("object %p already in list %p", data, *list);
      goto done;
    }
  *list = g_list_append (*list, data);
 done:
  this->unlock ();
  return data;
}

gpointer
Display::remove (GList **list, gpointer data)
{
  GList *elt;

  this->lock ();
  elt = g_list_find (*list, data);
  if (unlikely (elt == NULL))
    {
      g_warning ("object %p not in list %p", data, *list);
      goto done;
    }
  *list = g_list_remove_link (*list, elt);
 done:
  this->unlock ();
  return data;
}

gboolean
Display::find (GList *list, gconstpointer data)
{
  GList *elt;

  this->lock ();
  elt = g_list_find (list, data);
  this->unlock ();

  return elt != NULL;
}

// FIXME:
//
// - We should expose the main window background color, e.g., via
//   command-line argument --background.
// - Window transparency attribute should be called alpha; 0.0 means
//   transparent and 1.0 opaque.
// - The alpha component of colors is inverted.
// - Alpha blending is not working.
// - Handle border width.
//
void
Display::renderLoop ()
{
  bool doquit = false;

  //fps control vars
  gint32 curTime=0,preTime=SDL_GetTicks(),elapsedTime=0;

  DisplayDebug* displayDebug = new DisplayDebug(this->width, this->height);

  while (!this->hasQuitted())   // render loop
    {
      curTime = SDL_GetTicks();
      elapsedTime = curTime - preTime;
      preTime = curTime;

      if(elapsedTime < this->frameTime ){
        guint32 sleepTime = this-> frameTime - elapsedTime;
        elapsedTime = this-> frameTime;
        SDL_Delay(sleepTime);
      }
      
      notifyTimeAnchorListeners();

      SDL_Event evt;
      GList *l;

      while (SDL_PollEvent (&evt)) // handle input
        {
          switch (evt.type)
            {
            case SDL_KEYDOWN:
                this->notifyKeyEventListeners(SDL_KEYDOWN, evt.key.keysym.sym);
                break;
            case SDL_KEYUP:
                this->notifyKeyEventListeners(SDL_KEYUP, evt.key.keysym.sym);
                goto quit;
                break;
            case SDL_MOUSEBUTTONUP:
                this->notifyMouseEventListeners(SDL_MOUSEBUTTONUP);
                break;    
              // fall-through
            case SDL_QUIT:
              this->quit ();
              goto quit;
            default:
              break;
            }
        }

      this->lock ();            //  update providers
      for (l = this->providers; l != NULL; l = l->next)
        {
          SDL_Texture *texture;
          IContinuousMediaProvider *prov;
          int width, height;

          prov = (IContinuousMediaProvider *) l->data;
          g_assert_nonnull (prov);
          if (!prov->hasVisual ())
            continue;

          if (prov->getProviderContent () == NULL)
            {
              prov->getOriginalResolution (&width, &height);
              texture = createTexture (this->renderer, width, height);
              g_assert_nonnull (texture);
              prov->setProviderContent (texture);
            }
          prov->refreshDR (NULL);
        }
      this->unlock ();

      this->lock ();            // run jobs
      l = this->jobs;           // list may be modified while being iterated
      while (l != NULL)
        {
          GList *next = l->next;
          DisplayJob *job = (DisplayJob *) l->data;
          g_assert_nonnull (job);
          if (!job->func (job, this->renderer, job->data))
            this->jobs = g_list_remove_link (this->jobs, l);
          l = next;
        }
      this->unlock ();

      this->lock ();            // redraw windows
      SDL_SetRenderDrawColor (this->renderer, 255, 0, 255, 255);
      SDL_RenderClear (this->renderer);
      this->windows = g_list_sort (this->windows, (GCompareFunc) win_cmp_z);
      for (l = this->windows; l != NULL; l = l->next)
        {
          SDLWindow * window = (SDLWindow *) l->data;
          g_assert_nonnull (window);
          if (window->isVisible () && !window->isGhostWindow ())
            window->redraw (this->renderer);
        }
      displayDebug->draw(this->renderer,elapsedTime);
      SDL_RenderPresent (this->renderer);
      this->unlock ();

    quit:
      this->lock ();            // destroy dead textures
      g_list_free_full (this->textures,
                        (GDestroyNotify) SDL_DestroyTexture);
      this->textures = NULL;
      this->unlock ();
    }
    
  delete displayDebug;

  if (doquit)
    goto beach;

  doquit = true;
  this->lock ();
  this->unlock ();
//  goto quit;

  beach:
  this->lock ();
  SDL_Quit ();
  this->unlock ();
}

// Public methods.

/**
 * Creates a display with the given dimensions.
 * If FULLSCREEN is true, enable full-screen mode.
 */
Display::Display (int width, int height, bool fullscreen, gdouble fps)
{
  this->mutexInit ();

  this->width = width;
  this->height = height;
  this->fullscreen = fullscreen;
  
  if(fps> 0) //controlled fps 
    this->frameTime = (guint32)(1000/fps);
  else //go horse fps
    this->frameTime=0;

  this->renderer = NULL;
  this->screen = NULL;
 // this->im = NULL;

  this->_quit = false;

  this->jobs = NULL;
  this->textures = NULL;
  this->windows = NULL;
  this->providers = NULL;

  checkMutexInit ();            // FIXME


  //--
  g_assert (!SDL_WasInit (0));
  if (unlikely (SDL_Init (0) != 0))
    g_critical ("cannot initialize SDL: %s", SDL_GetError ());

  SDL_SetHint (SDL_HINT_NO_SIGNAL_HANDLERS, "1");
  SDL_SetHint (SDL_HINT_RENDER_SCALE_QUALITY, "1");

  this->lock ();
  guint flags = SDL_WINDOW_SHOWN;
  if (this->fullscreen)
    flags |= SDL_WINDOW_FULLSCREEN;

  g_assert_null (this->screen);
  g_assert_null (this->renderer);
 
  SDL_Init(SDL_INIT_VIDEO); 

  this->screen = SDL_CreateWindow("ginga",0,0, this->width, this->height, flags);
  this->renderer = SDL_CreateRenderer( this->screen , -1, SDL_RENDERER_PRESENTVSYNC);
                               
  this->unlock ();
 
}

/**
 * Destroys display.
 */
Display::~Display ()
{
  this->quit ();
 

  this->lock ();
  SDL_DestroyRenderer (this->renderer);
  SDL_DestroyWindow (this->screen);
 // delete im;
  g_list_free_full (this->jobs, (GDestroyNotify) job_delete);
  g_assert (g_list_length (this->textures) == 0);
  g_list_free_full (this->windows, (GDestroyNotify) win_delete);
  g_list_free_full (this->providers, (GDestroyNotify) prov_delete);
  this->unlock ();
  this->mutexClear ();
 
}

/**
 * Gets display size.
 */
void
Display::getSize (int *width, int *height)
{
  this->lock ();
  set_if_nonnull (width, this->width);
  set_if_nonnull (height, this->height);
  this->unlock ();
}

/**
 * Sets display size.
 */
void
Display::setSize (int width, int height)
{
  this->lock ();
  SDL_SetWindowSize (this->screen, width, height); // don't return a status
  
  this->width = width;
  this->height = height;
  this->unlock ();
}

/**
 * Gets display full-screen mode.
 */
bool
Display::getFullscreen ()
{
  bool fullscreen;

  this->lock ();
  fullscreen = this->fullscreen;
  this->unlock ();
  return fullscreen;
}

/**
 * Sets display full-screen mode.
 */
void
Display::setFullscreen (bool fullscreen)
{
  int status;
  guint flags;

  this->lock ();
  flags = (fullscreen) ? SDL_WINDOW_FULLSCREEN : 0;
  status = SDL_SetWindowFullscreen (this->screen, flags);
  if (unlikely (status != 0))
    {
      g_warning ("cannot change display full-screen mode to %s: %s",
                 (fullscreen) ? "true" : "false", SDL_GetError ());
      goto done;
    }
  this->fullscreen = fullscreen;
 done:
  this->unlock ();
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
 * Pushes a new job to renderer job list.
 * Returns the job id.
 */
DisplayJob *
Display::addJob (DisplayJobCallback func, void *data)
{
  DisplayJob *job;

  this->lock ();
  job = new DisplayJob;
  g_assert_nonnull (job);
  job->func = func;
  job->data = data;
  this->add (&this->jobs, job);
  this->unlock ();

  return job;
}

/**
 * Removes from the renderer job list the job with the given id.
 * Returns true if job was removed.
 */
bool
Display::removeJob (DisplayJob *job)
{
  GList *elt;

  this->lock ();
  elt = g_list_find (this->jobs, job);
  if (elt == NULL)
    {
      this->unlock ();
      return false;
    }
  this->jobs = g_list_remove_link (this->jobs, elt);
  delete (DisplayJob *) elt->data;
  this->unlock ();
  return true;
}

/**
 * Schedules the destruction of texture by render thread.
 */
void
Display::destroyTexture (SDL_Texture *texture)
{
  this->lock ();
  g_assert_nonnull (texture);
  this->add (&this->textures, texture);
  this->unlock ();
}

/**
 * Creates managed window with the given position, dimensions, and z-index.
 */
SDLWindow *
Display::createWindow (int x, int y, int w, int h, int z)
{
  SDLWindow *win;

  win = new SDLWindow (x, y, z, w, h);
  g_assert_nonnull (win);
  this->add (&this->windows, win);

  return win;
}

/**
 * Tests whether window WIN managed by display.
 */
bool
Display::hasWindow (const SDLWindow *win)
{
  g_assert_nonnull (win);
  return this->find (this->windows, win);
}

/**
 * Destroys managed window.
 */
void
Display::destroyWindow (SDLWindow *win)
{
  SDL_Texture *texture;

  g_assert_nonnull (win);
  texture = win->getTexture ();
  if (texture != NULL)
    {
      this->destroyTexture (texture);
      win->setTexture (NULL);
    }
  this->remove (&this->windows, win);
  delete win;
}

/**
 * Creates managed continuous media provider to decode URI.
 */
IContinuousMediaProvider *
Display::createContinuousMediaProvider (const string &uri)
{
  IContinuousMediaProvider *prov;

  prov = new VideoProvider (uri);
  g_assert_nonnull (prov);
  this->add (&this->providers, prov);
  return prov;
}

/**
 * Destroys managed continuous media provider.
 */
void
Display::destroyContinuousMediaProvider (IContinuousMediaProvider *prov)
{
  g_assert_nonnull (prov);
  this->remove (&this->providers, prov);
  delete prov;
}


// END SANITY --------------------------------------------------------------

void
Display::checkMutexInit ()
{
  if (!mutexInitialized)
    {
      mutexInitialized = true;

    
    }
}

void
Display::lockSDL ()
{
  checkMutexInit ();

}

void
Display::unlockSDL ()
{
  checkMutexInit ();

}

/* interfacing output */

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

    
  surfacePool.insert (iSur);
   

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

 
  surfacePool.insert (iSur);
 

  return iSur;
}

bool
Display::hasSurface (SDLSurface *s)
{
  set<SDLSurface *>::iterator i;
  bool hasSur = false;

    
  i = surfacePool.find (s);
  if (i != surfacePool.end ())
    {
      hasSur = true;
    }
   

  return hasSur;
}

bool
Display::releaseSurface (SDLSurface *s)
{
  set<SDLSurface *>::iterator i;
  bool released = false;

  i = surfacePool.find (s);
  if (i != surfacePool.end ())
    {
      surfacePool.erase (i);
      released = true;
    }


  return released;
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


SDL_Texture *
Display::createTextureFromSurface (SDL_Renderer *renderer,
                                           SDL_Surface *surface)
{
  SDL_Texture *texture = NULL;

  checkMutexInit ();

  lockSDL ();
    

  if (Display::hasUnderlyingSurface (surface))
    {
      g_assert_nonnull (surface);
      texture = SDL_CreateTextureFromSurface (renderer, surface);
      if (unlikely (texture == NULL))
        {
          g_error ("cannot create texture for surface %p: %s",
                   surface, SDL_GetError ());
        }
      g_assert_nonnull (texture);
      uTexPool.insert (texture);

      /* allowing alpha */
      g_assert (SDL_SetTextureBlendMode (texture, SDL_BLENDMODE_BLEND) == 0);
    }

   
  unlockSDL ();

  return texture;
}

SDL_Texture *
Display::createTexture (SDL_Renderer *renderer, int w, int h)
{
  SDL_Texture *texture;

  lockSDL ();

  texture = SDL_CreateTexture (renderer, SDL_PIXELFORMAT_ARGB32,
                               SDL_TEXTUREACCESS_STREAMING, w, h);

  // w > maxW || h > maxH || format is not supported
  assert (texture != NULL);

  /* allowing alpha */
  g_assert (SDL_SetTextureBlendMode (texture, SDL_BLENDMODE_BLEND) == 0);

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

    
  uSurPool.insert (uSur);
   
}

SDL_Surface *
Display::createUnderlyingSurface (int width, int height)
{
  SDL_Surface *newUSur = NULL;
  Uint32 rmask, gmask, bmask, amask;
  int bpp;

  checkMutexInit ();

  lockSDL ();

  SDL_PixelFormatEnumToMasks (SDL_PIXELFORMAT_ARGB32, &bpp, &rmask, &gmask, &bmask,
                              &amask);

  newUSur = SDL_CreateRGBSurface (0, width, height, bpp, rmask, gmask,
                                  bmask, amask);

  SDL_SetColorKey (newUSur, 1, *((Uint8 *)newUSur->pixels));
  unlockSDL ();

    
  if (newUSur != NULL)
    {
      uSurPool.insert (newUSur);
    }
  else
    {
      clog << "Display::createUnderlyingSurface SDL error: '";
      clog << SDL_GetError () << "'" << endl;
    }
   

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

      SDL_PixelFormatEnumToMasks (SDL_PIXELFORMAT_ARGB32, &bpp, &rmask, &gmask,
                                  &bmask, &amask);

      uSur = SDL_CreateRGBSurfaceFrom (pixels, w, h, bpp, tpitch[0], rmask,
                                       gmask, bmask, amask);

      if (locked)
        {
          SDL_UnlockTexture (texture);
        }
    }

  unlockSDL ();

    
  if (uSur != NULL)
    {
      uSurPool.insert (uSur);
    }
   

  return uSur;
}

bool
Display::hasUnderlyingSurface (SDL_Surface *uSur)
{
  set<SDL_Surface *>::iterator i;
  bool hasIt = false;

  checkMutexInit ();

    
  i = uSurPool.find (uSur);
  if (i != uSurPool.end ())
    {
      hasIt = true;
    }
   

  return hasIt;
}

void
Display::releaseUnderlyingSurface (SDL_Surface *uSur)
{
  set<SDL_Surface *>::iterator i;

  checkMutexInit ();

  lockSDL ();
    

  i = uSurPool.find (uSur);
  if (i != uSurPool.end ())
    {
      uSurPool.erase (i);

      SDL_FreeSurface (uSur);
    }

   
  unlockSDL ();
}

void 
Display::notifyKeyEventListeners(SDL_EventType evtType, SDL_Keycode key){
   if(key == SDLK_ESCAPE){
     this->quit();
     return;
   }
   set<IKeyInputEventListener*>::iterator it;
   for (it=keyEventListeners.begin(); it!=keyEventListeners.end(); ++it)
          (*it)->keyInputCallback(evtType, key);
}

void
Display::notifyMouseEventListeners(SDL_EventType evtType){
    int x, y;
    SDL_GetMouseState(&x, &y);
    set<IMouseEventListener*>::iterator it;
   for (it=mouseEventListeners.begin(); it!=mouseEventListeners.end(); ++it)
          (*it)->mouseInputCallback (evtType, x, y);
}

void
Display::notifyTimeAnchorListeners(){
    set<Player*>::iterator it;
    for (it=timeAnchorListeners.begin(); it!=timeAnchorListeners.end(); ++it)
          (*it)->notifyTimeAnchorCallBack();
}

void 
Display::registerKeyEventListener(IKeyInputEventListener* obj){
   keyEventListeners.insert(obj);
}

void 
Display::registerMouseEventListener(IMouseEventListener* obj){
   mouseEventListeners.insert(obj);
}

void
Display::unregisterKeyEventListener(IKeyInputEventListener* obj){
   keyEventListeners.erase (obj);
}

void 
Display::unregisterMouseEventListener(IMouseEventListener* obj){
   mouseEventListeners.erase (obj);
}

void
Display::postKeyInputEventListener(SDL_Keycode key){
   notifyKeyEventListeners(SDL_KEYUP, key);
}

void 
Display::registerTimeAnchorListener(Player* obj){
   g_debug("\n\n REGISTROU!!! \n\n");
   timeAnchorListeners.insert(obj);
}

void 
Display::unregisterTimeAnchorListener(Player* obj){
   timeAnchorListeners.erase (obj);
}

GINGA_MB_END
