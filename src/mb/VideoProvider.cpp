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
#include "SDLSurface.h"
#include "SDLWindow.h"

GINGA_MB_BEGIN

VideoProvider::VideoProvider (string uri)
    : SDLAudioProvider (uri)
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

void
VideoProvider::setLoadSymbol (string symbol)
{
  this->symbol = symbol;
}

string
VideoProvider::getLoadSymbol ()
{
  return this->symbol;
}

bool
VideoProvider::getHasVisual ()
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

bool
VideoProvider::checkVideoResizeEvent (arg_unused (SDLSurface* frame))
{
  return false;
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
  return SDLAudioProvider::getTotalMediaTime ();
}

int64_t
VideoProvider::getVPts ()
{
  return SDLAudioProvider::getVPts ();
}

double
VideoProvider::getMediaTime ()
{
  return SDLAudioProvider::getMediaTime ();
}

void
VideoProvider::setMediaTime (double pos)
{
  SDLAudioProvider::setMediaTime (pos);
}

void
VideoProvider::playOver (SDLSurface* surface)
{
  SDLWindow* parentId;
  SDLWindow *parent;

  parentId = surface->getParentWindow ();

  if (parentId == 0)
    {
      SDLAudioProvider::playOver (surface);
      return;
    }

  parent = parentId;

  clog << "VideoProvider::playOver parent(" << parent << ")" << endl;
  if (Ginga_Display->hasWindow (parentId))
    {
      win = parentId;
      if (hasTex)
        {
          ((SDLWindow *)win)->setTexture (tex);
          decoder->play ();
        }
    }
  else
    {
      clog << "VideoProvider::playOver parent(" << parent << ") ";
      clog << "Warning! hasWindow(parent) has returned false" << endl;
    }
}

void
VideoProvider::resume (SDLSurface* surface)
{
  SDLAudioProvider::resume (surface);
}

void
VideoProvider::pause ()
{
  SDLAudioProvider::pause ();
}

void
VideoProvider::stop ()
{
  SDLAudioProvider::stop ();
}

void
VideoProvider::setSoundLevel (double level)
{
  SDLAudioProvider::setSoundLevel (level);
}

bool
VideoProvider::releaseAll ()
{
  // TODO: release all structures
  return false;
}

void
VideoProvider::refreshDR (void *data)
{
  SDLAudioProvider::refreshDR (data);
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
