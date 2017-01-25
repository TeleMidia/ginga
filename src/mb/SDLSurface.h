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

#ifndef SDLSURFACE_H_
#define SDLSURFACE_H_

#include "IFontProvider.h"

#include "util/Color.h"
using namespace ::ginga::util;

GINGA_MB_BEGIN

typedef struct DrawData
{
  int coord1;
  int coord2;
  int coord3;
  int coord4;
  short dataType;
  int r;
  int g;
  int b;
  int a;
} DrawData;

class SDLSurface
{
private:
  static GingaSurfaceID refIdCounter;
  GingaSurfaceID myId;
  SDL_Surface *sur;
  SDL_Surface *pending;
  void *parent;
  bool hasExtHandler;
  Color *chromaColor;
  Color *borderColor;
  Color *bgColor;
  Color *surfaceColor;
  IFontProvider *iFont;
  int caps;
  bool isDeleting;

  vector<DrawData *> drawData;
  pthread_mutex_t ddMutex;

  pthread_mutex_t sMutex;
  pthread_mutex_t pMutex;

public:
  SDLSurface ();
  SDLSurface (void *underlyingSurface);

  virtual ~SDLSurface ();

  GingaSurfaceID getId () const;
  void setId (const GingaSurfaceID &surId);

private:
  void releasePendingSurface ();
  bool createPendingSurface ();
  void checkPendingSurface ();
  void fill ();
  void releaseChromaColor ();
  void releaseBgColor ();
  void releaseBorderColor ();
  void releaseSurfaceColor ();

  void releaseFont ();
  void releaseDrawData ();

  void initialize (const GingaSurfaceID &id);

public:
  void takeOwnership ();

  SDL_Surface *getPendingSurface ();

  void setExternalHandler (bool extHandler);
  bool hasExternalHandler ();

  void addCaps (int caps);
  void setCaps (int caps);
  int getCap (string cap);
  int getCaps ();
  bool setParentWindow (void *parentWindow);
  void *getParentWindow ();
  void *getSurfaceContent ();
  void setSurfaceContent (void *surface);
  void clearContent ();
  void clearSurface ();

  vector<DrawData *> *createDrawDataList ();

private:
  void pushDrawData (int c1, int c2, int c3, int c4, short type);

public:
  void drawLine (int x1, int y1, int x2, int y2);
  void drawRectangle (int x, int y, int w, int h);
  void fillRectangle (int x, int y, int w, int h);
  void drawString (int x, int y, const char *txt);
  void setChromaColor (guint8 r, guint8 g, guint8 b, guint8 alpha);
  Color *getChromaColor ();
  void setBorderColor (guint8 r, guint8 g, guint8 b, guint8 alpha);
  Color *getBorderColor ();
  void setBgColor (guint8 r, guint8 g, guint8 b, guint8 alpha);
  Color *getBgColor ();
  void setColor (guint8 r, guint8 g, guint8 b, guint8 alpha);
  Color *getColor ();
  void setSurfaceFont (void *font);
  void getStringExtents (const char *text, int *w, int *h);
  void flip ();
  void scale (double x, double y);

private:
  void initContentSurface ();
  SDL_Surface *createSurface ();

public:
  void blit (int x, int y, SDLSurface *src = NULL, int srcX = -1,
             int srcY = -1, int srcW = -1, int srcH = -1);

  void setClip (int x, int y, int w, int h);
  void getSize (int *width, int *height);
  string getDumpFileUri ();
};

GINGA_MB_END

#endif /*SDLSURFACE_H_*/
