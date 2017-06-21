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
GINGA_PRAGMA_DIAG_IGNORE (-Wpacked)
GINGA_PRAGMA_DIAG_IGNORE (-Wswitch-default)
GINGA_PRAGMA_DIAG_IGNORE (-Wvariadic-macros)
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_syswm.h>
GINGA_PRAGMA_DIAG_POP ()

#ifndef SDL_PIXELFORMAT_ARGB32
# if SDL_BYTEORDER == SDL_BIG_ENDIAN
#  define SDL_PIXELFORMAT_ARGB32 SDL_PIXELFORMAT_ARGB8888
# else
#  define SDL_PIXELFORMAT_ARGB32 SDL_PIXELFORMAT_BGRA8888
# endif
#endif

#define SDLx_assert(f) g_assert ((f) == 0)

#define SDLx_BlitSurface(S1, r1, S2, r2)\
  SDLx_assert (SDL_BlitSurface ((S1), (r1), (S2), (r2)))

#define SDLx_CreateWindowAndRenderer(w, h, f, S, R)\
  SDLx_assert (SDL_CreateWindowAndRenderer ((w), (h), (f), (S), (R)))

#if SDL_VERSION_ATLEAST (2,0,5)
# define SDLx_CreateSurfaceARGB32(w, h, S)              \
  G_STMT_START                                          \
  {                                                     \
    g_assert_nonnull ((S));                             \
    *(S) = SDL_CreateRGBSurfaceWithFormat               \
      (0, (w), (h), 32, SDL_PIXELFORMAT_ARGB8888);      \
    g_assert_nonnull (*(S));                            \
  }                                                     \
  G_STMT_END
#else
# define SDLx_CreateSurfaceARGB32(w, h, S)                              \
  G_STMT_START                                                          \
  {                                                                     \
    g_assert_nonnull ((S));                                             \
    *(S) =SDL_CreateRGBSurface                                          \
      (0, (w), (h), 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff); \
    g_assert_nonnull (*(S));                                            \
  }                                                                     \
  G_STMT_END
#endif

#define SDLx_LockSurface(S)\
  SDLx_assert (SDL_LockSurface ((S)))

#define SDLx_UnlockSurface(S)\
  SDL_UnlockSurface ((S))

#define SDLx_LockTexture(T, r, p, s)\
  SDLx_assert (SDL_LockTexture ((T), (r), (p), (s)))

#define SDLx_UnlockTexture(T)\
  SDL_UnlockTexture ((T))

#define SDLx_QueryTexture(T, f, a, w, h)\
  SDLx_assert (SDL_QueryTexture ((T), (f), (a), (w), (h)))

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
