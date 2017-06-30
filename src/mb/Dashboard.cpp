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
#include "Dashboard.h"
#include "Display.h"

#include "player/TextPlayer.h"
using namespace ::ginga::player;

GINGA_MB_BEGIN

Dashboard::Dashboard ()
{
}

Dashboard::~Dashboard()
{
}

void
Dashboard::redraw (SDL_Renderer *renderer, GingaTime total, double fps,
                   int frameno)
{
  static SDL_Color fg = {255, 255, 255, 255};
  static SDL_Color bg = {0, 0, 0, 0};
  static SDL_Rect rect = {0, 0, 0, 0};

  string info;
  SDL_Texture *debug;
  SDL_Rect ink;

  info = xstrbuild ("#%d %" GINGA_TIME_FORMAT "  %.1ffps",
                    frameno, GINGA_TIME_ARGS (total), fps);

  Ginga_Display->getSize (&rect.w, &rect.h);
  debug = TextPlayer::renderTexture
    (renderer, info, "monospace", "", "bold", "10", fg, bg,
     rect, "center", "", false, &ink);

  SDLx_SetTextureBlendMode (debug, SDL_BLENDMODE_BLEND);
  ink = {0, 0, rect.w, ink.h - ink.y + 4};
  SDLx_SetRenderDrawBlendMode (renderer, SDL_BLENDMODE_BLEND);
  SDLx_SetRenderDrawColor (renderer, 255, 0, 0, 127);
  SDLx_RenderFillRect (renderer, &ink);
  SDLx_RenderCopy (renderer, debug, nullptr, &rect);
  SDL_DestroyTexture (debug);
}

GINGA_MB_END
