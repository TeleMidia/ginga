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
#include "PropertyAnchor.h"

GINGA_NCL_BEGIN

PropertyAnchor::PropertyAnchor (const string &attName) : Anchor (attName)
{
  _value = "";
}

string
PropertyAnchor::getName ()
{
  return Anchor::getId ();
}

void
PropertyAnchor::setName (const string &name)
{
  Anchor::setId (name);
}

string
PropertyAnchor::getValue ()
{
  return _value;
}

void
PropertyAnchor::setValue (const string &value)
{
  this->_value = value;
}

PropertyAnchor *
PropertyAnchor::clone ()
{
  PropertyAnchor *pAnchor;

  pAnchor = new PropertyAnchor (Entity::getId ());
  pAnchor->setValue (_value);

  return pAnchor;
}

GINGA_NCL_END
