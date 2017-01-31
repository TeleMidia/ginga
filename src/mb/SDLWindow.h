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

#ifndef SDLWINDOW_H_
#define SDLWINDOW_H_

#include "util/Color.h"
using namespace ::ginga::util;

#include "SDL.h"

#include "SDLSurface.h"

typedef uint32_t Uint32;
typedef Uint32 SDL_WindowID;

GINGA_MB_BEGIN

class SDLWindow
{
public:
  static const short DDT_LINE = 0;
  static const short DDT_RECT = 1;
  static const short DDT_FILL_RECT = 2;

private:
  SDL_Texture *texture;
  SDL_Surface *curSur;

  SDLSurface* winISur;

  bool textureUpdate;
  bool textureOwner;

  int borderWidth;
  SDL_Color bgColor;
  SDL_Color borderColor;
  Color *winColor;
  Color *colorKey;

  SDL_Rect rect;

  double z;
  guint8 transparencyValue;
  bool visible;
  bool ghost;

  SDLSurface *childSurface;
  bool fit;
  bool stretch;
  int caps;

  set<SDLWindow *> mirrors;
  SDLWindow *mirrorSrc;

  pthread_mutex_t mutex;    // external mutex
  pthread_mutex_t mutexC;   // childs mutex
  pthread_mutex_t texMutex; // texture mutex
  pthread_mutex_t surMutex; // underlying surface mutex

  bool isWaiting;
  pthread_mutex_t rMutex; // render mutex
  pthread_mutex_t cMutex; // condition mutex
  pthread_cond_t cond;

public:
  SDLWindow (int x, int y, int width, int height,
             int z);

  virtual ~SDLWindow ();

private:
  void initialize (SDLWindow* parentWindowID,
                   int x, int y, int width, int height, double z);

  void releaseWinISur ();
  void releaseBGColor ();
  void releaseWinColor ();
  void releaseColorKey ();
  void releaseBorderColor ();

public:
// SANITY BEGIN ------------------------------------------------------------
  double getAlpha ();
  void setAlpha (double);
  void setBgColor (SDL_Color);
  SDL_Color getBgColor ();
  SDL_Rect getRect ();
  void setRect (SDL_Rect);
// SANITY END --------------------------------------------------------------

  void addMirror (SDLWindow *window);
  bool removeMirror (SDLWindow *window);
  void setMirrorSrc (SDLWindow *mirrorSrc);
  SDLWindow *getMirrorSrc ();


  void setColorKey (guint8 r, guint8 g, guint8 b);
  Color *getColorKey ();
  void setWindowColor (guint8 r, guint8 g, guint8 b, guint8 alpha);
  Color *getWindowColor ();

  void setBorder (SDL_Color, int);
  void getBorder (SDL_Color *, int *);

  void revertContent ();
  void setChildSurface (SDLSurface *iSur);
  int getCap (const string &cap);
  void setCaps (int caps);
  void addCaps (int capability);
  int getCaps ();

  void draw ();
  void setBounds (int x, int y, int width, int height);
  void moveTo (int x, int y);
  void resize (int width, int height);
  void raiseToTop ();
  void lowerToBottom ();
  void setCurrentTransparency (guint8 alpha);
  guint8 getTransparencyValue ();
  SDLWindow* getId ();
  void show ();
  void hide ();

  int getX ();
  int getY ();
  int getW ();
  int getH ();
  double getZ ();

  void setX (int x);
  void setY (int y);
  void setW (int w);
  void setH (int h);
  void setZ (double z);

  bool isGhostWindow ();
  void setGhostWindow (bool ghost);
  bool isVisible ();
  void validate ();

private:
  void unprotectedValidate ();

public:
  vector<DrawData *> *createDrawDataList ();
  void setStretch (bool stretchTo);
  bool getStretch ();
  void setFit (bool fitTo);
  bool getFit ();
  void clearContent ();
  void setRenderedSurface (SDL_Surface *uSur);
  SDL_Surface *getContent ();
  void setTexture (SDL_Texture *texture);
  SDL_Texture *getTexture (SDL_Renderer *renderer);
  bool isTextureOwner (SDL_Texture *texture);

private:
  bool isMine (SDLSurface *surface);

public:
  void renderImgFile (const string &serializedImageUrl);
  void renderFrom (SDLSurface *s);

  void blit (SDLWindow *src);
  void stretchBlit (SDLWindow *src);
  string getDumpFileUri (int quality, int dumpW, int dumpH);

  void lock ();
  void unlock ();

  void lockChilds ();
  void unlockChilds ();

  bool rendered ();

private:
  void waitRenderer ();

  void lockTexture ();
  void unlockTexture ();

  void lockSurface ();
  void unlockSurface ();
};

GINGA_MB_END

#endif /*SDLWINDOW_H_*/
