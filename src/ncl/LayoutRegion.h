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

#ifndef REGION_H
#define REGION_H

#include "Entity.h"

GINGA_NCL_BEGIN

class LayoutRegion : public Entity
{
public:
  LayoutRegion (const string &_id);
  virtual ~LayoutRegion ();
  void setRect (SDL_Rect);
  SDL_Rect getRect ();
  void setZ (int, int);
  void getZ (int *, int *);
  void dump ();

  int getLeft ();
  int getTop ();
  int getRight ();
  int getBottom ();
  int getWidth ();
  int getHeight ();

private:
  SDL_Rect _rect;               // x, y, w, h in pixels
  int _z;                       // z-index
  int _zorder;                  // z-order
};

GINGA_NCL_END

#endif // REGION_H
