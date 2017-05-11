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

#include "SDLWindow.h"

#include <cairo.h>
#include <pango/pangocairo.h>

GINGA_MB_BEGIN

// Global display; initialized by main().
Display *_Ginga_Display = NULL;

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

// Compares the z-index of two players.
static gint
win_cmp_z (Player *p1, Player *p2)
{
  double z1 = p1->getZ ();
  double z2 = p2->getZ ();
  return (z1 < z2) ? -1 : (z1 > z2) ? 1 : 0;
}

// Deletes window.
static void
win_delete (SDLWindow *win)
{
  delete win;
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
  guint32 curTime=0,preTime=SDL_GetTicks(),elapsedTime=0,accTime=0;

  DisplayDebug* displayDebug = new DisplayDebug(this->width, this->height);

  while (!this->hasQuitted())   // render loop
    {
      curTime = SDL_GetTicks();
      elapsedTime = curTime - preTime;
      preTime = curTime;

      if(elapsedTime < this->frameTime ){
        guint32 sleepTime = this-> frameTime - elapsedTime;
        elapsedTime = this->frameTime;
        SDL_Delay(sleepTime);
        accTime +=sleepTime;
      }
      else 
        accTime += elapsedTime;
     
      if(accTime >= 100){
           notifyTimeAnchorListeners();
           accTime=0;
      }

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
             //   goto quit;
                break;
            // case SDL_MOUSEMOTION:
            //     this->notifyMouseEventListeners(SDL_MOUSEMOTION);
            //     break; 

            case SDL_MOUSEBUTTONDOWN:
                this->notifyMouseEventListeners(SDL_MOUSEBUTTONDOWN);
                break; 
            case SDL_MOUSEBUTTONUP:
                this->notifyMouseEventListeners(SDL_MOUSEBUTTONUP);
                break;    
              // fall-through
            case SDL_QUIT:
              this->quit ();
         //     goto quit;
            default:
              break;
            }
        }

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
    
      this->lock (); // redraw windows
      SDL_SetRenderDrawColor (this->renderer, 255, 0, 255, 255);
      SDL_RenderClear (this->renderer);
      this->players = g_list_sort (this->players, (GCompareFunc) win_cmp_z);
      l =  this->players;           
      while (l != NULL)
        {
          GList *next = l->next;
           Player* pl = (Player *) l->data;
           if(!pl)
              this->players = g_list_remove_link (this->players, l);
           else 
              pl->redraw(this->renderer);
          l = next;
      }
      displayDebug->draw(this->renderer,elapsedTime);    
      SDL_RenderPresent (this->renderer);
      this->unlock ();

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
  
  this->fps = fps;
  if(this->fps> 0) //controlled fps 
    this->frameTime = (guint32)(1000/fps);
  else //go horse fps
    this->frameTime=0;

  this->renderer = NULL;
  this->screen = NULL;

  this->_quit = false;

  this->jobs = NULL;
  this->textures = NULL;
  this->windows = NULL; 
  this->players = NULL;

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
  this->unlock ();
  this->mutexClear ();
 
}

double
Display::getFps(){
  return this->fps;
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

void
Display::registerPlayer(Player * obj){
  g_assert_nonnull (obj);
  this->add (&this->players, obj);
}

void
Display::unregisterPlayer(Player *obj){
  g_assert_nonnull (obj);
  this->remove(&this-> players, obj);
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
    set<NclExecutionObject*>::iterator it;
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
Display::registerTimeAnchorListener(NclExecutionObject* obj){
   timeAnchorListeners.insert(obj);
}

void 
Display::unregisterTimeAnchorListener(NclExecutionObject* obj){
   timeAnchorListeners.erase (obj);
}

GINGA_MB_END
