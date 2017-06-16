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
#include "Entity.h"

#include "RectangleSpatialAnchor.h"

GINGA_NCL_BEGIN

RectangleSpatialAnchor::RectangleSpatialAnchor (const string &id, int left,
                                                int top, int width,
                                                int height)
    : SpatialAnchor (id)
{
  this->_left = left;
  this->_top = top;
  this->_width = width;
  this->_height = height;
  _typeSet.insert ("RectangleSpatialAnchor");
}

int
RectangleSpatialAnchor::getLeft ()
{
  return _left;
}

int
RectangleSpatialAnchor::getTop ()
{
  return _top;
}

int
RectangleSpatialAnchor::getWidth ()
{
  return _width;
}

int
RectangleSpatialAnchor::getHeight ()
{
  return _height;
}

void
RectangleSpatialAnchor::setArea (int left, int top, int width,
                                 int height)
{
  this->_left = left;
  this->_top = top;
  this->_width = width;
  this->_height = height;
}

GINGA_NCL_END
