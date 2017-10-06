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

#include "aux-ginga.h"
#include "Region.h"

GINGA_NCL_BEGIN

/**
 * @brief Creates a new region.
 * @param ncl Parent document.
 * @param id Region id.
 */
Region::Region (NclDocument *ncl, const string &id) : Entity (ncl, id)
{
  _rect = {0, 0, 0, 0};
  _top = "0%";
  _left = "0%";
  _bottom = "";
  _right = "";
  _width = "100%";
  _height = "100%";
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
GingaRect
Region::getRect (void)
{
  return this->_rect;
}

/**
 * @brief Sets region rectangle.
 * @param rect Rectangle.
 */
void
Region::setRect (GingaRect rect)
{
  _rect = rect;
}

/**
 * @brief Gets region top position.
 */
string
Region::getTop (void)
{
  return _top;
}

/**
 * @brief Sets region top position.
 * @param top Top position.
 */
void
Region::setTop (const string &top)
{
  _top = top;
}

/**
 * @brief Gets region left position.
 */
string
Region::getLeft ()
{
  return _left;
}

/**
 * @brief Sets region left position.
 * @param left Left position.
 */
void
Region::setLeft (const string &left)
{
  _left = left;
}

/**
 * @brief Gets region bottom position.
 */
string
Region::getBottom ()
{
  return _bottom;
}

/**
 * @brief Sets region bottom position.
 * @param bottom Bottom position.
 */
void
Region::setBottom (const string &bottom)
{
  _bottom = bottom;
}

/**
 * @brief Gets region right position.
 */
string
Region::getRight ()
{
  return _right;
}

/**
 * @brief Sets region right position.
 * @param right Right position.
 */
void
Region::setRight (const string &right)
{
  _right = right;
}

/**
 * @brief Gets region width.
 */
string
Region::getWidth ()
{
  return _width;
}

/**
 * @brief Sets region width.
 * @param width Region width.
 */
void
Region::setWidth (const string &width)
{
  _width = width;
}

/**
 * @brief Gets region height.
 */
string
Region::getHeight ()
{
  return _height;
}

/**
 * @brief Sets region height
 * @param height Region height.
 */
void
Region::setHeight (const string &height)
{
  _height = height;
}

/**
 * @brief Gets region z-index and z-order.
 */
void
Region::getZ (int *z, int *zorder)
{
  tryset (z, _z);
  tryset (zorder, _zorder);
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
