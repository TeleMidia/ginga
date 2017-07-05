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
#include "LayoutRegion.h"

#include "mb/Display.h"
using namespace ginga::mb;

GINGA_PRAGMA_DIAG_IGNORE (-Wfloat-conversion)

GINGA_NCL_BEGIN

void
LayoutRegion::setRect (SDL_Rect rect)
{
  this->_rect = rect;
}

SDL_Rect
LayoutRegion::getRect (void)
{
  return this->_rect;
}

void
LayoutRegion::setZ (int z, int zorder)
{
  this->_z = z;
  this->_zorder = zorder;
}

void
LayoutRegion::getZ (int *z, int *zorder)
{
  set_if_nonnull (z, this->_z);
  set_if_nonnull (zorder, this->_zorder);
}

void
LayoutRegion::dump ()
{
  TRACE ("%s at (%d,%d) size %dx%d z %d,%d",
         this->getId ().c_str (), this->_rect.x, this->_rect.y,
         this->_rect.w, this->_rect.h, this->_z, this->_zorder);
}

LayoutRegion::LayoutRegion (const string &id) : Entity (id)
{
  this->_rect.x = 0;
  this->_rect.y = 0;
  Ginga_Display->getSize (&this->_rect.w, &this->_rect.h);
  this->_z = 0;
  this->_zorder = 0;
}

LayoutRegion::~LayoutRegion ()
{
}

int
LayoutRegion::getLeft ()
{
  return this->_rect.x;
}

int
LayoutRegion::getTop ()
{
  return this->_rect.y;
}

int
LayoutRegion::getRight ()
{
  return this->_rect.x + this->_rect.w;
}

int
LayoutRegion::getBottom ()
{
  return this->_rect.y + this->_rect.h;
}

int
LayoutRegion::getHeight ()
{
  return this->_rect.h;
}

int
LayoutRegion::getWidth ()
{
  return this->_rect.w;
}


GINGA_NCL_END
