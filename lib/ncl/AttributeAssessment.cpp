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
#include "AttributeAssessment.h"

GINGA_NCL_BEGIN

/**
 * @brief Creates a new attribute assessment.
 * @param type Assessment type.
 * @param label Role label.
 * @param key Assessment key.
 * @param offset Assessment offset.
 */
AttributeAssessment::AttributeAssessment (EventType type,
                                          const string &label,
                                          const string &key,
                                          const string &offset)
  : Assessment (), Role (type, label)
{
  _key = key;
  _offset = offset;
}

/**
 * @brief Gets assessment key.
 */
string
AttributeAssessment::getKey ()
{
  return _key;
}

/**
 * @brief Gets assessment offset.
 */
string
AttributeAssessment::getOffset ()
{
  return _offset;
}

GINGA_NCL_END
