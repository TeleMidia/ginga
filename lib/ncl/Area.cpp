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

#include "ginga-internal.h"
#include "Area.h"

GINGA_NCL_BEGIN

/**
 * @brief Creates a new area.
 * @param id Area id.
 * @param begin Area begin time.
 * @param end Area end time.
 */
Area::Area (const string &id, GingaTime begin, GingaTime end) : Anchor (id)
{
  _begin = begin;
  _end = end;
}

/**
 * @brief Gets area begin time.
 */
GingaTime
Area::getBegin ()
{
  return _begin;
}

/**
 * @brief Gets area end time.
 */
GingaTime
Area::getEnd ()
{
  return _end;
}

GINGA_NCL_END
