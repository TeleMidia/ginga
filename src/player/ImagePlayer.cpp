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

ImagePlayer::ImagePlayer (const string &_mrl) : Player (_mrl)
{
  this->surface = Ginga_Display->createSurfaceFrom (NULL);
}

bool
ImagePlayer::play ()
{
  SDL_Surface *sfc;
  SDLWindow *win;

  sfc = IMG_Load (mrl.c_str ());
  if (unlikely (sfc == NULL))
    g_error ("cannot load image file %s: %s", mrl.c_str (),
             IMG_GetError ());

  g_assert_nonnull (this->surface);
  this->surface->setContent (sfc);
  ginga::mb::Display::addUnderlyingSurface (sfc);

  win = surface->getParentWindow ();
  g_assert_nonnull (win);
  win->setRenderedSurface (sfc);

  return Player::play ();
}

GINGA_PLAYER_END
