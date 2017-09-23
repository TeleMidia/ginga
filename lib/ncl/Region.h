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

class Region : public Entity
{
public:
  Region (NclDocument *, const string &);
  virtual ~Region ();

  GingaRect getRect ();
  void setRect (GingaRect);

  string getTop ();
  void setTop (const string &);
  string getLeft ();
  void setLeft (const string &);
  string getBottom ();
  void setBottom (const string &);
  string getRight ();
  void setRight (const string &);
  string getWidth ();
  void setWidth (const string &);
  string getHeight ();
  void setHeight (const string &);

  void getZ (int *, int *);
  void setZ (int, int);

private:
  GingaRect _rect;
  string _top;
  string _left;
  string _bottom;
  string _right;
  string _height;
  string _width;
  int _z;
  int _zorder;
};

GINGA_NCL_END

#endif // REGION_H
