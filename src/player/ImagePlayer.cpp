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

GINGA_PLAYER_BEGIN

bool
ImagePlayer::displayJobCallbackWrapper (DisplayJob *job,
                                        SDL_Renderer *renderer,
                                        void *self)
{
  return ((ImagePlayer *) self)->displayJobCallback (job, renderer);
}

bool
ImagePlayer::displayJobCallback (arg_unused (DisplayJob *job),
                                 SDL_Renderer *renderer)
{
  if (this->window == NULL)
    return true;                // nothing to do

  this->texture = IMG_LoadTexture (renderer, mrl.c_str ());
  if (unlikely (this->texture == NULL))
    ERROR ("cannot load image %s: %s", mrl.c_str (), IMG_GetError ());

  return false;                 // remove job
}

bool
ImagePlayer::play ()
{
  Ginga_Display->addJob (displayJobCallbackWrapper, this);
  return Player::play ();
}

GINGA_PLAYER_END
