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


// Public.

void
ImagePlayer::redraw (SDL_Renderer *renderer)
{
  if (_texture == NULL)
    {
      _texture = IMG_LoadTexture (renderer, _uri.c_str ());
      if (unlikely (_texture == nullptr))
        {
          ERROR ("cannot load image file %s: %s", _uri.c_str (),
                 IMG_GetError ());
        }
    }
  Player::redraw (renderer);
}

GINGA_PLAYER_END
