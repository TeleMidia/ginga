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

pthread_mutex_t Display::sdlMutex;
pthread_mutex_t Display::sieMutex;
pthread_mutex_t Display::renMutex;
pthread_mutex_t Display::scrMutex;
pthread_mutex_t Display::recMutex;
pthread_mutex_t Display::winMutex;
pthread_mutex_t Display::surMutex;
pthread_mutex_t Display::proMutex;
pthread_mutex_t Display::cstMutex;


// BEGIN SANITY ------------------------------------------------------------

// Compares the z-index of two windows.
static gint
win_cmp_z (gconstpointer p1, gconstpointer p2)
{
  SDLWindow *w1 = deconst (SDLWindow *, p1);
  SDLWindow *w2 = deconst (SDLWindow *, p2);
  double z1 = w1->getZ ();
  double z2 = w2->getZ ();
  return (z1 < z2) ? -1 : (z1 > z2) ? 1 : 0;
}

// Deletes window.
static void
win_delete (gpointer p)
{
  SDLWindow *win = (SDLWindow *) p;
  delete win;
}

// Deletes provider.
static void
prov_delete (gpointer p)
{
  IContinuousMediaProvider *prov = (IContinuousMediaProvider *) p;
  delete prov;
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
              goto quit;
            default:
              break;
            }

          InputManager *im = display->getInputManager ();
          SDLEventBuffer *buf = im->getEventBuffer ();
          buf->feed (evt, false, false);
        }
      display->redraw ();       // redraw providers and windows
    }
 quit:
  SDL_Quit ();
  exit (EXIT_SUCCESS);
  return NULL;
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


// Public methods.

/**
 * Redraws the registered providers and windows.
 * The z-index order of windows determines their redraw order.
 */
void
Display::redraw ()
{
  GList *l;

  this->lock ();

  // Update providers.
  for (l = this->providers; l != NULL; l = l->next)
    {
      SDL_Texture *texture;
      IContinuousMediaProvider *prov;
      int width, height;

      prov = (IContinuousMediaProvider *) l->data;
      if (!prov->getHasVisual ())
        continue;               // nothing to do

      if (prov->getProviderContent () == NULL)
        {
          prov->getOriginalResolution (&width, &height);
          texture = createTexture (this->renderer, width, height);
          g_assert_nonnull (texture);
          prov->setProviderContent (texture);
        }
      prov->refreshDR (NULL);
    }

  // Redraw windows.
  SDL_RenderClear (this->renderer);
  this->windows = g_list_sort (this->windows, win_cmp_z);
  for (l = this->windows; l != NULL; l = l->next)
    {
      SDL_Texture *texture;
      SDLWindow *win;
      SDLWindow *mir;

      win = (SDLWindow *) l->data;
      if (!win->isVisible () || win->isGhostWindow ())
        continue;               // nothing to do

      for (mir = win; mir != NULL; mir = mir->getMirrorSrc ());
      texture = (mir)
        ? mir->getTexture (this->renderer)
        : win->getTexture (this->renderer);
      // g_assert_nonnull (texture);
      drawSDLWindow (this->renderer, texture, win);
      win->rendered ();
    }
  SDL_RenderPresent (this->renderer);

  this->unlock ();
}

/**
 * Creates a display with the given dimensions.
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

/**
 * Destroys display.
 */
Display::~Display ()
{
  this->quit ();
  g_assert_null (g_thread_join (this->render_thread));

  this->lock ();
  SDL_DestroyRenderer (this->renderer);
  SDL_DestroyWindow (this->screen);
  delete im;
  g_list_free_full (this->windows, win_delete);
  g_list_free_full (this->providers, prov_delete);
  this->unlock ();
  g_mutex_clear (&this->mutex);
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
  int flags;

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
 * Creates managed window with the given position, dimensions, and z-index.
 */
SDLWindow *
Display::createWindow (int x, int y, int w, int h, int z)
{
  SDLWindow *win;

  win = new SDLWindow (x, y, w, h, z);
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
  g_assert_nonnull (win);
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

  prov = new SDLVideoProvider (uri);
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
      dmpPool.erase (i);

      Thread::mutexUnlock (&proMutex);
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

/* interfacing input */

InputManager *
Display::getInputManager ()
{
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
