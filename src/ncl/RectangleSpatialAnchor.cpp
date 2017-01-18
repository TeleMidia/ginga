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

#include "config.h"
#include "ncl/Entity.h"
using namespace ::ginga::ncl;

#include "ncl/RectangleSpatialAnchor.h"

GINGA_NCL_BEGIN

RectangleSpatialAnchor::RectangleSpatialAnchor (string id, long left, long top,
                                                long width, long height)
    : SpatialAnchor (id)
{

  this->left = left;
  this->top = top;
  this->width = width;
  this->height = height;
  typeSet.insert ("RectangleSpatialAnchor");
}

long
RectangleSpatialAnchor::getLeft ()
{
  return left;
}

long
RectangleSpatialAnchor::getTop ()
{
  return top;
}

long
RectangleSpatialAnchor::getWidth ()
{
  return width;
}

long
RectangleSpatialAnchor::getHeight ()
{
  return height;
}

void
RectangleSpatialAnchor::setArea (long left, long top, long width, long height)
{

  this->left = left;
  this->top = top;
  this->width = width;
  this->height = height;
}

GINGA_NCL_END
