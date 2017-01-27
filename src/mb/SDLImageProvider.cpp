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
#include "SDLImageProvider.h"

#include "DisplayManager.h"
#include "SDLDisplay.h"
#include "SDLSurface.h"
#include "SDLWindow.h"

#if defined (WITH_LIBRSVG) && WITH_LIBRSVG
#include "SDLSvgDecoder.h"
#endif

#if defined (WITH_LIBBPG) && WITH_LIBBPG
# include "SDLBpgDecoder.h"
#endif

#include "util/Color.h"
using namespace  ::ginga::util;

GINGA_MB_BEGIN

bool SDLImageProvider::mutexInit = false;
bool SDLImageProvider::initialized = false;
short SDLImageProvider::imageRefs = 0;
pthread_mutex_t SDLImageProvider::pMutex;

SDLImageProvider::SDLImageProvider (const char *mrl)
{
  type = ImageProvider;

  if (!mutexInit)
    {
      mutexInit = true;
      Thread::mutexInit (&pMutex, true);
    }

  imgUri = "";

  imgUri.assign (mrl);
}

SDLImageProvider::~SDLImageProvider ()
{
}

void
SDLImageProvider::playOver (SDLSurface* surface)
{
  SDL_Surface *renderedSurface;
  SDLWindow *parent;

  Thread::mutexLock (&pMutex);

  if (!initialized)
    {
      initialized = true;
      if (IMG_Init (0) < 0)
        {
          clog << "SDLFontProvider::playOver ";
          clog << "Couldn't initialize IMG: " << SDL_GetError ();
          clog << endl;
        }
    }

  if (surface != 0
      && Ginga_Display_M->hasSurface (surface))
    {
#if WITH_LIBRSVG
      if (imgUri.substr (imgUri.find_last_of (".") + 1) == "svg"
          || imgUri.substr (imgUri.find_last_of (".") + 1) == "svgz")
        {
          int w = 0, h = 0;
          // We need the region dimensions for SVG. How to do it?

          SDLSvgDecoder *svgdec = new SDLSvgDecoder (imgUri);
          SDLDisplay::lockSDL ();
          renderedSurface = svgdec->decode (w, h);
          SDLDisplay::unlockSDL ();
          delete svgdec;
        }
      else
#endif
#if defined WITH_LIBBPG && WITH_LIBBPG
          if (imgUri.substr (imgUri.find_last_of (".") + 1) == "bpg")
        {
          SDLBpgDecoder *bpgdec = new SDLBpgDecoder (imgUri);
          SDLDisplay::lockSDL ();
          renderedSurface = bpgdec->decode ();
          SDLDisplay::unlockSDL ();
        }
      else
#endif
        {
          SDLDisplay::lockSDL ();
          renderedSurface = IMG_Load (imgUri.c_str ());
          SDLDisplay::unlockSDL ();
        }

      if (renderedSurface != NULL)
        {
          SDLDisplay::addUnderlyingSurface (renderedSurface);
          SDLWindow* parentId = Ginga_Display_M
                                       ->getSurfaceParentWindow (surface);
          parent = parentId;

          if (parent != NULL)
            {
              parent->setRenderedSurface (renderedSurface);
            }
          Ginga_Display_M->setSurfaceContent (
              surface, (void *)renderedSurface);
        }
    }
  else
    {
      clog << "SDLImageProvider::playOver Warning! NULL content";
      clog << endl;
    }

  Thread::mutexUnlock (&pMutex);
}

bool
SDLImageProvider::releaseAll ()
{
  return false;
}

GINGA_MB_END
