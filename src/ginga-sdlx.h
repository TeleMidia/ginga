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

#ifndef GINGA_SDLX_H
#define GINGA_SDLX_H

GINGA_PRAGMA_DIAG_PUSH ()
GINGA_PRAGMA_DIAG_IGNORE (-Wconversion)
GINGA_PRAGMA_DIAG_IGNORE (-Wswitch-default)
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_syswm.h>
GINGA_PRAGMA_DIAG_POP ()

#define SDLx_assert(f) g_assert ((f) == 0)

#define SDLx_SetRenderDrawBlendMode(R, m)\
  SDLx_assert (SDL_SetRenderDrawBlendMode ((R), (m)))

#define SDLx_SetRenderDrawColor(R, r, g, b, a)\
  SDLx_assert (SDL_SetRenderDrawColor((R), (r), (g), (b), (a)))

#define SDLx_RenderCopy(R, T, r1, r2)\
  SDLx_assert (SDL_RenderCopy ((R), (T), (r1), (r2)))

#define SDLx_RenderDrawRect(R, r)\
  SDLx_assert (SDL_RenderDrawRect ((R), (r)))

#define SDLx_RenderFillRect(R, r)\
  SDLx_assert (SDL_RenderFillRect ((R), (r)))

#define SDLx_SetTextureAlphaMod(T, a)\
  SDLx_assert (SDL_SetTextureAlphaMod ((T), (a)))

#endif /* GINGA_SDLX_H */
