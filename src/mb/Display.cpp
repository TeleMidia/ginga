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

#include "InputManager.h"
#include "AudioProvider.h"
#include "EventBuffer.h"
#include "InputEvent.h"
#include "SDLSurface.h"
#include "VideoProvider.h"
#include "SDLWindow.h"

#include <cairo.h>
#include <pango/pangocairo.h>

GINGA_MB_BEGIN

// Global display; initialized by main().
Display *_Ginga_Display = NULL;

bool Display::mutexInitialized = false;
map<Key::KeyCode, int> Display::gingaToSDLCodeMap;
map<int, Key::KeyCode> Display::sdlToGingaCodeMap;
map<string, int> Display::sdlStrToSdlCode;
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
      
      SDL_Event evt;
      GList *l;

      while (SDL_PollEvent (&evt)) // handle input
        {
          switch (evt.type)
            {
            case SDL_KEYDOWN:
                break;
            case SDL_KEYUP:
              if(evt.key.keysym.sym == SDLK_d)
                   displayDebug->toggle();
              else if (evt.key.keysym.sym == SDLK_ESCAPE)
                   this->quit ();
                   goto quit;
                break;
              // fall-through
            case SDL_QUIT:
              this->quit ();
              goto quit;
            default:
              break;
            }

          InputManager *im = this->getInputManager ();
          EventBuffer *buf = im->getEventBuffer ();
          buf->feed (evt, false, false);
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
  this->im->postInputEvent (Key::KEY_QUIT);
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
  this->im = NULL;

  this->_quit = false;

  this->jobs = NULL;
  this->textures = NULL;
  this->windows = NULL;
  this->providers = NULL;

  this->im = new InputManager ();
  g_assert_nonnull (this->im);
  this->im->setAxisBoundaries (this->width, this->height, 0);

  checkMutexInit ();            // FIXME
  initCodeMaps ();              // FIXME

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
  delete im;
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

/* interfacing input */

InputManager *
Display::getInputManager ()
{
  return im;
}

EventBuffer *
Display::createEventBuffer ()
{
  return new EventBuffer ();
}

InputEvent *
Display::createInputEvent (void *event, const int symbol)
{
  InputEvent *ie = NULL;

  if (event != NULL)
    {
      ie = new InputEvent (*(SDL_Event *)event);
    }

  if (symbol >= 0)
    {
      ie = new InputEvent (symbol);
    }

  return ie;
}

InputEvent *
Display::createApplicationEvent (int type, void *data)
{
  return new InputEvent (type, data);
}

Key::KeyCode
Display::fromMBToGinga (int keyCode)
{
  map<int, Key::KeyCode>::iterator it;
  Key::KeyCode translated;

  checkMutexInit ();

 

  translated = Key::KEY_NULL;
  it = sdlToGingaCodeMap.find (keyCode);
  if (it != sdlToGingaCodeMap.end ())
    {
      translated = it->second;
    }
  else
    {
      clog << "Display::fromMBToGinga can't find code '";
      clog << keyCode << "' returning KEY_NULL" << endl;
    }


  return translated;
}

int
Display::fromGingaToMB (Key::KeyCode keyCode)
{
  map<Key::KeyCode, int>::iterator i;
  int translated;

  checkMutexInit ();


  translated = Key::KEY_NULL;
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


  return translated;
}

/* libgingaccmbsdl internal use*/

/* input */
int
Display::convertEventCodeStrToInt (const string &strEvent)
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

  if (!gingaToSDLCodeMap.empty ())
    {
      return;
    }

  // sdlStrToSdlCode
  sdlStrToSdlCode = {
    {"GIEK:QUIT", SDL_QUIT},
    {"GIEK:UNKNOWN", SDLK_UNKNOWN},
    {"GIEK:0", SDLK_0},
    {"GIEK:1", SDLK_1},
    {"GIEK:2", SDLK_2},
    {"GIEK:3", SDLK_3},
    {"GIEK:4", SDLK_4},
    {"GIEK:5", SDLK_5},
    {"GIEK:6", SDLK_6},
    {"GIEK:7", SDLK_7},
    {"GIEK:8", SDLK_8},
    {"GIEK:9", SDLK_9},

    {"GIEK:a", SDLK_a},
    {"GIEK:b", SDLK_b},
    {"GIEK:c", SDLK_c},
    {"GIEK:d", SDLK_d},
    {"GIEK:e", SDLK_e},
    {"GIEK:f", SDLK_f},
    {"GIEK:g", SDLK_g},
    {"GIEK:h", SDLK_h},
    {"GIEK:i", SDLK_i},
    {"GIEK:j", SDLK_j},
    {"GIEK:k", SDLK_k},
    {"GIEK:l", SDLK_l},
    {"GIEK:m", SDLK_m},
    {"GIEK:n", SDLK_n},
    {"GIEK:o", SDLK_o},
    {"GIEK:p", SDLK_p},
    {"GIEK:q", SDLK_q},
    {"GIEK:r", SDLK_r},
    {"GIEK:s", SDLK_s},
    {"GIEK:t", SDLK_t},
    {"GIEK:u", SDLK_u},
    {"GIEK:v", SDLK_v},
    {"GIEK:w", SDLK_w},
    {"GIEK:x", SDLK_x},
    {"GIEK:y", SDLK_y},
    {"GIEK:z", SDLK_z},

    {"GIEK:A", SDLK_a + 5000},
    {"GIEK:B", SDLK_b + 5000},
    {"GIEK:C", SDLK_c + 5000},
    {"GIEK:D", SDLK_d + 5000},
    {"GIEK:E", SDLK_e + 5000},
    {"GIEK:F", SDLK_f + 5000},
    {"GIEK:G", SDLK_g + 5000},
    {"GIEK:H", SDLK_h + 5000},
    {"GIEK:I", SDLK_i + 5000},
    {"GIEK:J", SDLK_j + 5000},
    {"GIEK:K", SDLK_k + 5000},
    {"GIEK:L", SDLK_l + 5000},
    {"GIEK:M", SDLK_m + 5000},
    {"GIEK:N", SDLK_n + 5000},
    {"GIEK:O", SDLK_o + 5000},
    {"GIEK:P", SDLK_p + 5000},
    {"GIEK:Q", SDLK_q + 5000},
    {"GIEK:R", SDLK_r + 5000},
    {"GIEK:S", SDLK_s + 5000},
    {"GIEK:T", SDLK_t + 5000},
    {"GIEK:U", SDLK_u + 5000},
    {"GIEK:V", SDLK_v + 5000},
    {"GIEK:W", SDLK_w + 5000},
    {"GIEK:X", SDLK_x + 5000},
    {"GIEK:Y", SDLK_y + 5000},
    {"GIEK:Z", SDLK_z + 5000},

    {"GIEK:PAGEDOWN", SDLK_PAGEDOWN},
    {"GIEK:PAGEUP", SDLK_PAGEUP},

    {"GIEK:F1", SDLK_F1},
    {"GIEK:F2", SDLK_F2},
    {"GIEK:F3", SDLK_F3},
    {"GIEK:F4", SDLK_F4},
    {"GIEK:F5", SDLK_F5},
    {"GIEK:F6", SDLK_F6},
    {"GIEK:F7", SDLK_F7},
    {"GIEK:F8", SDLK_F8},
    {"GIEK:F9", SDLK_F9},
    {"GIEK:F10", SDLK_F10},
    {"GIEK:F11", SDLK_F11},
    {"GIEK:F12", SDLK_F12},

    {"GIEK:PLUS", SDLK_PLUS},
    {"GIEK:MINUS", SDLK_MINUS},

    {"GIEK:ASTERISK", SDLK_ASTERISK},
    {"GIEK:HASH", SDLK_HASH},

    {"GIEK:PERIOD", SDLK_PERIOD},

    {"GIEK:CAPSLOCK", SDLK_CAPSLOCK},
    {"GIEK:PRINTSCREEN", SDLK_PRINTSCREEN},
    {"GIEK:MENU", SDLK_MENU},
    {"GIEK:F14", SDLK_F14},
    {"GIEK:QUESTION", SDLK_QUESTION},

    {"GIEK:DOWN", SDLK_DOWN},
    {"GIEK:LEFT", SDLK_LEFT},
    {"GIEK:RIGHT", SDLK_RIGHT},
    {"GIEK:UP", SDLK_UP},

    {"GIEK:F15", SDLK_F15},
    {"GIEK:F16", SDLK_F16},

    {"GIEK:VOLUMEDOWN", SDLK_VOLUMEDOWN},
    {"GIEK:VOLUMEUP", SDLK_VOLUMEUP},

    {"GIEK:RETURN", SDLK_RETURN},
    {"GIEK:RETURN2", SDLK_RETURN2},

    {"GIEK:F17", SDLK_F17},
    {"GIEK:F18", SDLK_F18},
    {"GIEK:F19", SDLK_F19},
    {"GIEK:F20", SDLK_F20},

    {"GIEK:SPACE", SDLK_SPACE},
    {"GIEK:BACKSPACE", SDLK_BACKSPACE},
    {"GIEK:AC_BACK", SDLK_AC_BACK},
    {"GIEK:ESCAPE", SDLK_ESCAPE},
    {"GIEK:OUT", SDLK_OUT},

    {"GIEK:POWER", SDLK_POWER},
    {"GIEK:F21", SDLK_F21},
    {"GIEK:STOP", SDLK_STOP},
    {"GIEK:EJECT", SDLK_EJECT},
    {"GIEK:EXECUTE", SDLK_EXECUTE},
    {"GIEK:F22", SDLK_F22},
    {"GIEK:PAUSE", SDLK_PAUSE},

    {"GIEK:GREATER", SDLK_GREATER},
    {"GIEK:LESS", SDLK_LESS},

    {"GIEK:TAB", SDLK_TAB},
    {"GIEK:F23", SDLK_F23}
  };

  // gingaToSDLCodeMap
  gingaToSDLCodeMap = {
    {Key::KEY_QUIT, SDL_QUIT},
    {Key::KEY_NULL, SDLK_UNKNOWN},
    {Key::KEY_0, SDLK_0},
    {Key::KEY_1, SDLK_1},
    {Key::KEY_2, SDLK_2},
    {Key::KEY_3, SDLK_3},
    {Key::KEY_4, SDLK_4},
    {Key::KEY_5, SDLK_5},
    {Key::KEY_6, SDLK_6},
    {Key::KEY_7, SDLK_7},
    {Key::KEY_8, SDLK_8},
    {Key::KEY_9, SDLK_9},

    {Key::KEY_SMALL_A, SDLK_a},
    {Key::KEY_SMALL_B, SDLK_b},
    {Key::KEY_SMALL_C, SDLK_c},
    {Key::KEY_SMALL_D, SDLK_d},
    {Key::KEY_SMALL_E, SDLK_e},
    {Key::KEY_SMALL_F, SDLK_f},
    {Key::KEY_SMALL_G, SDLK_g},
    {Key::KEY_SMALL_H, SDLK_h},
    {Key::KEY_SMALL_I, SDLK_i},
    {Key::KEY_SMALL_J, SDLK_j},
    {Key::KEY_SMALL_K, SDLK_k},
    {Key::KEY_SMALL_L, SDLK_l},
    {Key::KEY_SMALL_M, SDLK_m},
    {Key::KEY_SMALL_N, SDLK_n},
    {Key::KEY_SMALL_O, SDLK_o},
    {Key::KEY_SMALL_P, SDLK_p},
    {Key::KEY_SMALL_Q, SDLK_q},
    {Key::KEY_SMALL_R, SDLK_r},
    {Key::KEY_SMALL_S, SDLK_s},
    {Key::KEY_SMALL_T, SDLK_t},
    {Key::KEY_SMALL_U, SDLK_u},
    {Key::KEY_SMALL_V, SDLK_v},
    {Key::KEY_SMALL_W, SDLK_w},
    {Key::KEY_SMALL_X, SDLK_x},
    {Key::KEY_SMALL_Y, SDLK_y},
    {Key::KEY_SMALL_Z, SDLK_z},

    {Key::KEY_CAPITAL_A, SDLK_a + 5000},
    {Key::KEY_CAPITAL_B, SDLK_b + 5000},
    {Key::KEY_CAPITAL_C, SDLK_c + 5000},
    {Key::KEY_CAPITAL_D, SDLK_d + 5000},
    {Key::KEY_CAPITAL_E, SDLK_e + 5000},
    {Key::KEY_CAPITAL_F, SDLK_f + 5000},
    {Key::KEY_CAPITAL_G, SDLK_g + 5000},
    {Key::KEY_CAPITAL_H, SDLK_h + 5000},
    {Key::KEY_CAPITAL_I, SDLK_i + 5000},
    {Key::KEY_CAPITAL_J, SDLK_j + 5000},
    {Key::KEY_CAPITAL_K, SDLK_k + 5000},
    {Key::KEY_CAPITAL_L, SDLK_l + 5000},
    {Key::KEY_CAPITAL_M, SDLK_m + 5000},
    {Key::KEY_CAPITAL_N, SDLK_n + 5000},
    {Key::KEY_CAPITAL_O, SDLK_o + 5000},
    {Key::KEY_CAPITAL_P, SDLK_p + 5000},
    {Key::KEY_CAPITAL_Q, SDLK_q + 5000},
    {Key::KEY_CAPITAL_R, SDLK_r + 5000},
    {Key::KEY_CAPITAL_S, SDLK_s + 5000},
    {Key::KEY_CAPITAL_T, SDLK_t + 5000},
    {Key::KEY_CAPITAL_U, SDLK_u + 5000},
    {Key::KEY_CAPITAL_V, SDLK_v + 5000},
    {Key::KEY_CAPITAL_W, SDLK_w + 5000},
    {Key::KEY_CAPITAL_X, SDLK_x + 5000},
    {Key::KEY_CAPITAL_Y, SDLK_y + 5000},
    {Key::KEY_CAPITAL_Z, SDLK_z + 5000},

    {Key::KEY_PAGE_DOWN, SDLK_PAGEDOWN},
    {Key::KEY_PAGE_UP, SDLK_PAGEUP},

    {Key::KEY_F1, SDLK_F1},
    {Key::KEY_F2, SDLK_F2},
    {Key::KEY_F3, SDLK_F3},
    {Key::KEY_F4, SDLK_F4},
    {Key::KEY_F5, SDLK_F5},
    {Key::KEY_F6, SDLK_F6},
    {Key::KEY_F7, SDLK_F7},
    {Key::KEY_F8, SDLK_F8},
    {Key::KEY_F9, SDLK_F9},
    {Key::KEY_F10, SDLK_F10},
    {Key::KEY_F11, SDLK_F11},
    {Key::KEY_F12, SDLK_F12},

    {Key::KEY_PLUS_SIGN, SDLK_PLUS},
    {Key::KEY_MINUS_SIGN, SDLK_MINUS},

    {Key::KEY_ASTERISK, SDLK_ASTERISK},
    {Key::KEY_NUMBER_SIGN, SDLK_HASH},

    {Key::KEY_PERIOD, SDLK_PERIOD},

    {Key::KEY_SUPER, SDLK_CAPSLOCK},
    {Key::KEY_PRINTSCREEN, SDLK_PRINTSCREEN},
    {Key::KEY_MENU, SDLK_MENU},
    {Key::KEY_INFO, SDLK_F14},
    {Key::KEY_EPG, SDLK_QUESTION},

    {Key::KEY_CURSOR_DOWN, SDLK_DOWN},
    {Key::KEY_CURSOR_LEFT, SDLK_LEFT},
    {Key::KEY_CURSOR_RIGHT, SDLK_RIGHT},
    {Key::KEY_CURSOR_UP, SDLK_UP},

    {Key::KEY_CHANNEL_DOWN, SDLK_F15},
    {Key::KEY_CHANNEL_UP, SDLK_F16},

    {Key::KEY_VOLUME_DOWN, SDLK_VOLUMEDOWN},
    {Key::KEY_VOLUME_UP, SDLK_VOLUMEUP},

    {Key::KEY_ENTER, SDLK_RETURN},

    {Key::KEY_RED, SDLK_F17},
    {Key::KEY_GREEN, SDLK_F18},
    {Key::KEY_YELLOW, SDLK_F19},
    {Key::KEY_BLUE, SDLK_F20},

    {Key::KEY_SPACE, SDLK_SPACE},
    {Key::KEY_BACKSPACE, SDLK_BACKSPACE},
    {Key::KEY_BACK, SDLK_AC_BACK},
    {Key::KEY_ESCAPE, SDLK_ESCAPE},
    {Key::KEY_EXIT, SDLK_OUT},

    {Key::KEY_POWER, SDLK_POWER},
    {Key::KEY_REWIND, SDLK_F21},
    {Key::KEY_STOP, SDLK_STOP},
    {Key::KEY_EJECT, SDLK_EJECT},
    {Key::KEY_PLAY, SDLK_EXECUTE},
    {Key::KEY_RECORD, SDLK_F22},
    {Key::KEY_PAUSE, SDLK_PAUSE},

    {Key::KEY_GREATER_THAN_SIGN, SDLK_GREATER},
    {Key::KEY_LESS_THAN_SIGN, SDLK_LESS},

    {Key::KEY_TAB, SDLK_TAB},
    {Key::KEY_TAP, SDLK_F23}
  };

  // sdlToGingaCodeMap
  map<Key::KeyCode, int>::iterator it;
  it = gingaToSDLCodeMap.begin ();
  while (it != gingaToSDLCodeMap.end ())
    {
      sdlToGingaCodeMap[it->second] = it->first;
      ++it;
    }

  
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

GINGA_MB_END
