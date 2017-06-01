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
#include "Dashboard.h"

#include "SDLWindow.h"

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
  g_assert_nonnull (job);
  delete job;
}

// Compares the z-index and z-order of two players.
static gint
win_cmp_z (Player *p1, Player *p2)
{
  int z1, zo1, z2, zo2;

  g_assert_nonnull (p1);
  g_assert_nonnull (p2);

  p1->getZ (&z1, &zo1);
  p2->getZ (&z2, &zo2);

  if (z1 < z2)
    return -1;
  if (z1 > z2)
    return 1;
  if (zo1 < zo2)
    return -1;
  if (zo1 > zo2)
    return 1;
  return 0;
}


// Private methods.

bool
Display::add (GList **list, gpointer data)
{
  bool found;

  g_assert_nonnull (list);
  this->lock ();
  if (unlikely (found = g_list_find (*list, data)))
    {
      g_warning ("object %p already in list %p", data, *list);
      goto done;
    }
  *list = g_list_append (*list, data);
 done:
  this->unlock ();
  return !found;
}

bool
Display::remove (GList **list, gpointer data)
{
  GList *elt;

  g_assert_nonnull (list);
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
  return elt != NULL;
}

bool
Display::find (GList *list, gconstpointer data)
{
  GList *elt;
  this->lock ();
  elt = g_list_find (list, data);
  this->unlock ();
  return elt != NULL;
}

void
Display::notifyTickListeners (GingaTime total, GingaTime diff, int frameno)
{
  GList *l = this->listeners;
  while (l != NULL)
    {
      GList *next = l->next;
      IEventListener *obj = (IEventListener *) l->data;
      g_assert_nonnull (obj);
      obj->handleTickEvent (total, diff, frameno);
      l = next;
    }
}

void
Display::notifyKeyListeners (SDL_EventType type, SDL_Keycode key)
{
  GList *l = this->listeners;
  while (l != NULL)
    {
      GList *next = l->next;
      IEventListener *obj = (IEventListener *) l->data;
      g_assert_nonnull (obj);
      obj->handleKeyEvent (type, key);
      l = next;
    }
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
  GingaTime epoch = ginga_gettime ();
  GingaTime last = epoch;
  int frameno = 1;
  bool doquit = false;

  while (!this->hasQuitted())   // render loop
    {
      SDL_Event evt;
      GList *l;

      GingaTime framedur = (GingaTime)(1 * GINGA_SECOND / this->fps);
      GingaTime now = ginga_gettime ();
      GingaTime elapsed = now - last;

      if (this->fps > 0 && elapsed < framedur)
        g_usleep ((framedur - elapsed) / 1000);

      now = ginga_gettime ();
      elapsed = now - last;
      last = now;
      this->notifyTickListeners (now - epoch, elapsed, frameno++);
      this->notifyTimeAnchorListeners ();

      while (SDL_PollEvent (&evt)) // handle input
        {
          SDL_EventType type = (SDL_EventType) evt.type;
          switch (type)
            {
            case SDL_KEYDOWN:
            case SDL_KEYUP:
              if (evt.key.keysym.sym == SDLK_ESCAPE)
                this->quit ();
              this->notifyKeyListeners (type, evt.key.keysym.sym);
              break;
            case SDL_MOUSEBUTTONDOWN:
              this->notifyMouseEventListeners (SDL_MOUSEBUTTONDOWN);
              break;
            case SDL_MOUSEBUTTONUP:
              this->notifyMouseEventListeners (SDL_MOUSEBUTTONUP);
              break;
            case SDL_QUIT:
              this->quit ();
              break;
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

      this->lock ();            // redraw windows
      SDL_SetRenderDrawColor (this->renderer, 255, 0, 255, 255);
      SDL_RenderClear (this->renderer);

      this->players = g_list_sort (this->players, (GCompareFunc) win_cmp_z);
      l =  this->players;
      while (l != NULL)
        {
          GList *next = l->next;
          Player *pl = (Player *) l->data;
          if (pl == NULL)
            this->players = g_list_remove_link (this->players, l);
          else
            pl->redraw (this->renderer);
          l = next;
        }

      this->dashboard->redraw (this->renderer, now - epoch,
                               ceil ((double)(1 * GINGA_SECOND / elapsed)),
                               frameno);

      SDL_RenderPresent (this->renderer);
      this->unlock ();

      this->lock ();            // destroy dead textures
      g_list_free_full (this->textures,
                        (GDestroyNotify) SDL_DestroyTexture);
      this->textures = NULL;
      this->unlock ();

    }

  if (doquit)
    goto beach;

  doquit = true;

 beach:
  this->lock ();
  SDL_Quit ();
  this->unlock ();
}


// Public methods.

/**
 * @brief Creates a new display with the given parameters.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param fullscreen Full-screen mode.
 * @param fps Target FPS rate.
 */
Display::Display (int width, int height, double fps, bool fullscreen)
{
  guint flags;

  this->mutexInit ();
  this->width = width;
  this->height = height;
  this->fps = fps;
  this->fullscreen = fullscreen;
  this->_quit = false;

  this->jobs = NULL;
  this->listeners = NULL;
  this->players = NULL;
  this->textures = NULL;

  g_assert (!SDL_WasInit (0));
  if (unlikely (SDL_Init (0) != 0))
    g_critical ("display: cannot initialize SDL: %s", SDL_GetError ());

#if SDL_VERSION_ATLEAST (2,0,4)
  SDL_SetHint (SDL_HINT_NO_SIGNAL_HANDLERS, "1");
#endif
  SDL_SetHint (SDL_HINT_RENDER_SCALE_QUALITY, "1");

  flags = SDL_WINDOW_SHOWN;
  if (this->fullscreen)
    flags |= SDL_WINDOW_FULLSCREEN;

  this->dashboard = new Dashboard ();
  this->screen = SDL_CreateWindow ("ginga", 0, 0, width, height, flags);
  g_assert_nonnull (this->screen);
  this->renderer = SDL_CreateRenderer (this->screen, -1,
                                       SDL_RENDERER_PRESENTVSYNC);
  g_assert_nonnull (this->renderer);
}

/**
 * @brief Destroys display.
 */
Display::~Display ()
{
  this->quit ();
  this->lock ();

  g_list_free_full (this->jobs, (GDestroyNotify) job_delete);
  g_list_free (this->listeners);
  g_list_free (this->players);
  g_assert (g_list_length (this->textures) == 0);

  SDL_DestroyRenderer (this->renderer);
  SDL_DestroyWindow (this->screen);
  delete this->dashboard;

  this->unlock ();
  this->mutexClear ();
}

/**
 * @brief Gets the target FPS rate.
 * @return Target FPS rate.
 */
double
Display::getFPS ()
{
  double fps;

  this->lock ();
  fps = this->fps;
  this->unlock ();

  return fps;
}

/**
 * @brief Sets the target FPS rate.
 * @param fps Target FPS rate.
 */
void
Display::setFPS (double fps)
{
  this->lock ();
  this->fps = MAX (fps, 0);
  this->unlock ();
}

/**
 * @brief Gets full-screen mode.
 * @return True if display is in full-screen.
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
 * @brief Sets full-screen mode.
 * @param fullscreen Full-screen mode.
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
      g_warning ("display: cannot set full-screen mode to %s: %s",
                 (fullscreen) ? "true" : "false", SDL_GetError ());
      goto done;
    }
  this->fullscreen = fullscreen;
 done:
  this->unlock ();
}

/**
 * @brief Gets display size.
 * @param width Variable to store width.
 * @param height Variable to store height.
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
 * @brief Sets display size.
 * @param width Width in pixels.
 * @param height Height in pixels.
 */
void
Display::setSize (int width, int height)
{
  this->lock ();
  g_assert (width > 0 && height > 0);
  SDL_SetWindowSize (this->screen, width, height);
  this->width = width;
  this->height = height;
  this->unlock ();
}

/**
 * @brief Quits render loop.
 */
void
Display::quit ()
{
  this->lock ();
  this->_quit = true;
  this->unlock ();
}

/**
 * @brief Tests whether display has quitted.
 * @return True if display has quitted.
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
 * @brief Adds job to display job list.
 * @param func Job callback.
 * @param data User-data.
 * @return New job.
 */
DisplayJob *
Display::addJob (DisplayJobCallback func, void *data)
{
  DisplayJob *job;

  job = new DisplayJob;
  g_assert_nonnull (job);

  job->func = func;
  job->data = data;

  this->add (&this->jobs, job);
  return job;
}

/**
 * @brief Removes job from the display job list.
 * @param job Job.
 * @return True if job was removed.
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
 * @brief Adds listener to display listener list.
 * @param obj Event listener.
 */
bool
Display::registerEventListener (IEventListener *obj)
{
  g_assert_nonnull (obj);
  return this->add (&this->listeners, obj);
}

/**
 * @brief Removes listener from display listener list.
 */
bool
Display::unregisterEventListener (IEventListener *obj)
{
  g_assert_nonnull (obj);
  return this->remove (&this->listeners, obj);
}

/**
 * @brief Schedules the destruction of texture.
 * @param texture Texture.
 */
void
Display::destroyTexture (SDL_Texture *texture)
{
  g_assert_nonnull (texture);
  this->add (&this->textures, texture);
}


// -------------------------------------------------------------------------

void
Display::registerPlayer (Player * obj)
{
  g_assert_nonnull (obj);
  this->add (&this->players, obj);
}

void
Display::unregisterPlayer (Player *obj)
{
  g_assert_nonnull (obj);
  this->remove(&this-> players, obj);
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
Display::registerMouseEventListener(IMouseEventListener* obj){
   mouseEventListeners.insert(obj);
}

void
Display::unregisterMouseEventListener(IMouseEventListener* obj){
   mouseEventListeners.erase (obj);
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
