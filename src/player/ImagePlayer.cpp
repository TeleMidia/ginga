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
#include "ImagePlayer.h"

#include "mb/Display.h"
#include "mb/SDLWindow.h"
using namespace ::ginga::mb;

GINGA_PLAYER_BEGIN


// Private methods.

void
ImagePlayer::lock (void)
{
  g_rec_mutex_lock (&this->mutex);
}

void
ImagePlayer::unlock (void)
{
  g_rec_mutex_unlock (&this->mutex);
}

void
ImagePlayer::displayJobWrapper (SDL_Renderer *renderer, void *data)
{
  g_assert_nonnull (data);
  ((ImagePlayer *) data)->displayJob (renderer);
}

void
ImagePlayer::displayJob (SDL_Renderer *renderer)
{
  SDL_Texture *texture;
  SDLWindow *window;

  texture = IMG_LoadTexture (renderer, mrl.c_str ());
  if (unlikely (texture == NULL))
    g_error ("cannot load image file %s: %s", mrl.c_str (),
             IMG_GetError ());

  this->lock ();
  window = surface->getParentWindow ();
  g_assert_nonnull (window);
  window->setTexture (texture);
  this->unlock ();

  g_mutex_lock (&this->display_job_mutex);
  this->display_job_done = true;
  g_cond_signal (&this->display_job_cond);
  g_mutex_unlock (&this->display_job_mutex);
}


// Public methods.

ImagePlayer::ImagePlayer (const string &_mrl) : Player (_mrl)
{
  g_rec_mutex_init (&this->mutex);
  this->display_job_done = false;
  g_mutex_init (&this->display_job_mutex);
  g_cond_init (&this->display_job_cond);

  this->surface = new SDLSurface ();
}

ImagePlayer::~ImagePlayer (void) // FIXME: Destroy texture
{
  this->lock ();
  g_mutex_clear (&this->display_job_mutex);
  g_cond_clear (&this->display_job_cond);
  this->unlock ();
  g_rec_mutex_clear (&this->mutex);

}

bool
ImagePlayer::play ()
{
  Ginga_Display->addJob (displayJobWrapper, this);

  g_mutex_lock (&this->display_job_mutex);
  while (!this->display_job_done)
    g_cond_wait (&this->display_job_cond, &this->display_job_mutex);
  g_mutex_unlock (&this->display_job_mutex);

  return Player::play ();
}

GINGA_PLAYER_END
