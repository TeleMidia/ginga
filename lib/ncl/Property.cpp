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
#include "Property.h"

GINGA_NCL_BEGIN

/**
 * @brief Creates a new property.
 * @param ncl Parent document.
 * @param name Property name.
 */
Property::Property (NclDocument *ncl, const string &name)
  : Anchor (ncl, name)
{
  _value = "";
}

/**
 * @brief Destroys property.
 */
Property::~Property ()
{
}

/**
 * @brief Gets property name.
 */
string
Property::getName ()
{
  return Anchor::getId ();
}

/**
 * @brief Gets property value.
 */
string
Property::getValue ()
{
  return _value;
}

/**
 * @brief Sets property value.
 */
void
Property::setValue (const string &value)
{
  _value = value;
}

GINGA_NCL_END
