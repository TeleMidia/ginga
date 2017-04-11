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


#include "SDL.h"

GINGA_MB_BEGIN

class SDLWindow
{
// BEGIN SANITY ------------------------------------------------------------
private:
  GINGA_MUTEX_DEFN ();
  SDL_Texture *texture;         // window texture

public:
  SDLWindow (int, int, int, int, int);
  virtual ~SDLWindow ();

  void setTexture (SDL_Texture *);
  SDL_Texture *getTexture ();

  void redraw (SDL_Renderer *); // called only by the render thread
// END SANITY --------------------------------------------------------------

private:
 
  int borderWidth;
  SDL_Color bgColor;
  SDL_Color borderColor;

  SDL_Rect rect;

  double z;
  guint8 transparencyValue;
  bool visible;
  bool ghost;

  pthread_mutex_t _mutex;    // external mutex
  pthread_mutex_t texMutex; // texture mutex
  pthread_mutex_t surMutex; // underlying surface mutex

  pthread_mutex_t rMutex; // render mutex
  pthread_mutex_t cMutex; // condition mutex
  pthread_cond_t cond;

private:
  void initialize (SDLWindow* parentWindowID,
                   int x, int y, int width, int height, double z);


public:
// SANITY BEGIN ------------------------------------------------------------
  double getAlpha ();
  void setAlpha (double);
  void setBgColor (SDL_Color);
  SDL_Color getBgColor ();
  SDL_Rect getRect ();
  void setRect (SDL_Rect);
// SANITY END --------------------------------------------------------------

  void setBorder (SDL_Color, int);
  void getBorder (SDL_Color *, int *);

  
  void setBounds (int x, int y, int width, int height);
  void moveTo (int x, int y);
  void resize (int width, int height);

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


public:
  void _lock ();
  void _unlock ();


};

GINGA_MB_END

#endif /*SDLWINDOW_H_*/
