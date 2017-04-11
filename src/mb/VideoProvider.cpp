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
#include "VideoProvider.h"


#include "Display.h"
#include "SDLWindow.h"

GINGA_MB_BEGIN

VideoProvider::VideoProvider (const string &uri)
    : AudioProvider (uri)
{
  type = VideoProviderType;

  win = NULL;
  hasTex = false;

  if (decoder != NULL)
    {
      getOriginalResolution (&wRes, &hRes);
    }
}

VideoProvider::~VideoProvider () { hasTex = false; }

bool
VideoProvider::hasVisual ()
{
  assert (decoder != NULL);

  return decoder->hasVideoStream ();
}

void *
VideoProvider::getProviderContent ()
{
  assert (decoder != NULL);

  return (void *)(decoder->getTexture ());
}

void
VideoProvider::setProviderContent (void *texture)
{
  assert (decoder != NULL);
  assert (texture != NULL);

  decoder->setTexture ((SDL_Texture *)texture);
  textureCreated ();
}

void
VideoProvider::feedBuffers ()
{
}

void
VideoProvider::getVideoSurfaceDescription (arg_unused (void *dsc))
{
}


void
VideoProvider::getOriginalResolution (int *width, int *height)
{
  assert (decoder != NULL);
  assert (width != NULL);
  assert (height != NULL);

  decoder->getOriginalResolution (width, height);
}

double
VideoProvider::getTotalMediaTime ()
{
  return AudioProvider::getTotalMediaTime ();
}

int64_t
VideoProvider::getVPts ()
{
  return AudioProvider::getVPts ();
}

double
VideoProvider::getMediaTime ()
{
  return AudioProvider::getMediaTime ();
}

void
VideoProvider::setMediaTime (double pos)
{
  AudioProvider::setMediaTime (pos);
}





void
VideoProvider::pause ()
{
  AudioProvider::pause ();
}

void
VideoProvider::stop ()
{
  AudioProvider::stop ();
}

void
VideoProvider::setSoundLevel (double level)
{
  AudioProvider::setSoundLevel (level);
}


void
VideoProvider::refreshDR (void *data)
{
  AudioProvider::refreshDR (data);
}

bool
VideoProvider::textureCreated ()
{
  assert (decoder != NULL);

  if (!hasTex)
    {
      if (decoder == NULL)
        {
          return false;
        }

      tex = decoder->getTexture ();
      if (win != NULL)
        {
          ((SDLWindow *)win)->setTexture (tex);
          decoder->play ();
        }
      hasTex = true;

      return true;
    }

  return false;
}

GINGA_MB_END
