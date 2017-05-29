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

GINGA_MB_BEGIN

class SDLWindow
{
private:
  GINGA_MUTEX_DEFN ();
  SDL_Texture *texture;         // window texture

  int borderWidth;
  SDL_Color bgColor;
  SDL_Color borderColor;
  SDL_Rect rect;
  gint z;
  guint8 transparencyValue;
  bool visible;
  bool ghost;

  void initialize (int x, int y, int w, int h, int z);

public:
  SDLWindow (int, int, int, int, int);
  virtual ~SDLWindow ();

  void setTexture (SDL_Texture *);
  SDL_Texture *getTexture ();

  double getAlpha ();
  void setAlpha (double);
  void setBgColor (SDL_Color);
  SDL_Color getBgColor ();
  SDL_Rect getRect ();
  void setRect (SDL_Rect);

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
  gint getZ ();

  void setX (int x);
  void setY (int y);
  void setW (int w);
  void setH (int h);
  void setZ (gint z);

  bool isGhostWindow ();
  void setGhostWindow (bool ghost);
  bool isVisible ();
};

GINGA_MB_END

#endif /*SDLWINDOW_H_*/
