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
  this->rect = rect;
}

SDL_Rect
LayoutRegion::getRect (void)
{
  return this->rect;
}

void
LayoutRegion::setZ (int z, int zorder)
{
  this->z = z;
  this->zorder = zorder;
}

void
LayoutRegion::getZ (int *z, int *zorder)
{
  set_if_nonnull (z, this->z);
  set_if_nonnull (zorder, this->zorder);
}

void
LayoutRegion::dump ()
{
  g_debug ("%s at (%d,%d) size %dx%d z %d,%d",
           this->getId ().c_str (), this->rect.x, this->rect.y,
           this->rect.w, this->rect.h, this->z, this->zorder);
}

LayoutRegion::LayoutRegion (const string &id) : Entity (id)
{
  this->rect.x = 0;
  this->rect.y = 0;
  Ginga_Display->getSize (&this->rect.w, &this->rect.h);
  this->z = 0;
  this->zorder = 0;
}

LayoutRegion::~LayoutRegion ()
{
  Entity::hasInstance (this, true);
}

int
LayoutRegion::getLeft ()
{
  return this->rect.x;
}

int
LayoutRegion::getTop ()
{
  return this->rect.y;
}

int
LayoutRegion::getRight ()
{
  return this->rect.x + this->rect.w;
}

int
LayoutRegion::getBottom ()
{
  return this->rect.y + this->rect.h;
}

int
LayoutRegion::getHeight ()
{
  return this->rect.h;
}

int
LayoutRegion::getWidth ()
{
  return this->rect.w;
}


GINGA_NCL_END
