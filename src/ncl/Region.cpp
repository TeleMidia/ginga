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
#include "Region.h"

#include "mb/Display.h"
using namespace ginga::mb;

GINGA_NCL_BEGIN

/**
 * @brief Creates a new region.
 * @param Region id.
 */
Region::Region (const string &id) : Entity (id)
{
  int w, h;
  Ginga_Display->getSize (&w, &h);
  _rect = {0, 0, w, h};
  _z = 0;
  _zorder = 0;
}

/**
 * @brief Destroys region.
 */
Region::~Region ()
{
}

/**
 * @brief Gets region rectangle.
 */
SDL_Rect
Region::getRect (void)
{
  return this->_rect;
}

/**
 * @brief Sets region rectangle.
 * @param rect Rectangle.
 */
void
Region::setRect (SDL_Rect rect)
{
  _rect = rect;
}

/**
 * @brief Gets region z-index and z-order.
 */
void
Region::getZ (int *z, int *zorder)
{
  set_if_nonnull (z, _z);
  set_if_nonnull (zorder, _zorder);
}

/**
 * @brief Sets region z-index and z-order.
 */
void
Region::setZ (int z, int zorder)
{
  _z = z;
  _zorder = zorder;
}

GINGA_NCL_END
