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
#include "NewTextPlayer.h"

#include "mb/Display.h"
#include "mb/SDLWindow.h"
using namespace ::ginga::mb;

GINGA_PLAYER_BEGIN

// Private methods.

bool
NewTextPlayer::displayJobCallbackWrapper (DisplayJob *job,
                                        SDL_Renderer *renderer,
                                        void *self)
{
  return ((NewTextPlayer *) self)->displayJobCallback (job, renderer);
}

bool
NewTextPlayer::displayJobCallback (arg_unused (DisplayJob *job),
                                   arg_unused (SDL_Renderer *renderer))
{
  // SDL_Texture *texture;
  // SDLWindow *window;
  //texture =
  this->lock ();
  // window = surface->getParentWindow ();
  // g_assert_nonnull (window);
  // window->setTexture (texture);
  this->unlock ();
  this->condDisplayJobSignal ();
  return false;                 // remove job
}


// Public methods.

NewTextPlayer::NewTextPlayer (const string &uri) : Player (uri)
{
  this->mutexInit ();
  this->condDisplayJobInit ();
  this->surface = new SDLSurface ();
}

NewTextPlayer::~NewTextPlayer (void)
{
  this->condDisplayJobClear ();
  this->mutexClear ();
}

bool
NewTextPlayer::play ()
{
  g_debug ("hello!!");
  Ginga_Display->addJob (displayJobCallbackWrapper, this);
  this->condDisplayJobWait ();
  return Player::play ();
}

GINGA_PLAYER_END
