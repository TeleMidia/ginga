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

#ifndef SDLVIDEOPROVIDER_H_
#define SDLVIDEOPROVIDER_H_

#include "IContinuousMediaProvider.h"

#include "AudioProvider.h"

GINGA_MB_BEGIN

class VideoProvider : public AudioProvider
{
private:
  int wRes;
  int hRes;

  SDLWindow *win;
  SDL_Texture *tex;

  bool hasTex;

public:
  VideoProvider (const string &);
  virtual ~VideoProvider ();
  bool hasVisual ();
  void *getProviderContent ();
  void setProviderContent (void *texture);
  virtual void setAVPid (arg_unused (int aPid), arg_unused (int vPid)){};
  void feedBuffers ();

private:
  void getVideoSurfaceDescription (void *dsc);

public:


  void getOriginalResolution (int *width, int *height);
  double getTotalMediaTime ();
  virtual int64_t getVPts ();
  double getMediaTime ();
  void setMediaTime (double pos);


  void pause ();
  void stop ();
  void setSoundLevel (double level);
  bool releaseAll ();

  void refreshDR (void *data);

private:
  bool textureCreated ();
};

GINGA_MB_END

#endif /*SDLVIDEOPROVIDER_H_*/
