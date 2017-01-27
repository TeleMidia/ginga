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
#include "SDLVideoProvider.h"

#include "DisplayManager.h"
#include "SDLDisplay.h"
#include "SDLSurface.h"
#include "SDLWindow.h"

GINGA_MB_BEGIN

SDLVideoProvider::SDLVideoProvider (const char *mrl)
    : SDLAudioProvider (mrl)
{
  type = VideoProvider;

  win = NULL;
  hasTex = false;

  if (decoder != NULL)
    {
      getOriginalResolution (&wRes, &hRes);
    }
}

SDLVideoProvider::~SDLVideoProvider () { hasTex = false; }

void
SDLVideoProvider::setLoadSymbol (string symbol)
{
  this->symbol = symbol;
}

string
SDLVideoProvider::getLoadSymbol ()
{
  return this->symbol;
}

bool
SDLVideoProvider::getHasVisual ()
{
  assert (decoder != NULL);

  return decoder->hasVideoStream ();
}

void *
SDLVideoProvider::getProviderContent ()
{
  assert (decoder != NULL);

  return (void *)(decoder->getTexture ());
}

void
SDLVideoProvider::setProviderContent (void *texture)
{
  assert (decoder != NULL);
  assert (texture != NULL);

  decoder->setTexture ((SDL_Texture *)texture);
  textureCreated ();
}

void
SDLVideoProvider::feedBuffers ()
{
}

void
SDLVideoProvider::getVideoSurfaceDescription (arg_unused (void *dsc))
{
}

bool
SDLVideoProvider::checkVideoResizeEvent (arg_unused (SDLSurface* frame))
{
  return false;
}

void
SDLVideoProvider::getOriginalResolution (int *width, int *height)
{
  assert (decoder != NULL);
  assert (width != NULL);
  assert (height != NULL);

  decoder->getOriginalResolution (width, height);
}

double
SDLVideoProvider::getTotalMediaTime ()
{
  return SDLAudioProvider::getTotalMediaTime ();
}

int64_t
SDLVideoProvider::getVPts ()
{
  return SDLAudioProvider::getVPts ();
}

double
SDLVideoProvider::getMediaTime ()
{
  return SDLAudioProvider::getMediaTime ();
}

void
SDLVideoProvider::setMediaTime (double pos)
{
  SDLAudioProvider::setMediaTime (pos);
}

void
SDLVideoProvider::playOver (SDLSurface* surface)
{
  SDLWindow* parentId;
  SDLWindow *parent;

  SDLDisplay::addCMPToRendererList (this);
  parentId = Ginga_Display_M->getSurfaceParentWindow (
      surface);

  if (parentId == 0)
    {
      SDLAudioProvider::playOver (surface);
      return;
    }

  parent = parentId;

  clog << "SDLVideoProvider::playOver parent(" << parent << ")" << endl;
  if (Ginga_Display_M->hasWindow (parentId))
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
      clog << "SDLVideoProvider::playOver parent(" << parent << ") ";
      clog << "Warning! hasWindow(parent) has returned false" << endl;
    }
}

void
SDLVideoProvider::resume (SDLSurface* surface)
{
  SDLAudioProvider::resume (surface);
}

void
SDLVideoProvider::pause ()
{
  SDLAudioProvider::pause ();
}

void
SDLVideoProvider::stop ()
{
  SDLAudioProvider::stop ();
}

void
SDLVideoProvider::setSoundLevel (double level)
{
  SDLAudioProvider::setSoundLevel (level);
}

bool
SDLVideoProvider::releaseAll ()
{
  // TODO: release all structures
  return false;
}

void
SDLVideoProvider::refreshDR (void *data)
{
  SDLAudioProvider::refreshDR (data);
}

bool
SDLVideoProvider::textureCreated ()
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
