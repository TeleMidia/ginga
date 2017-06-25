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
#include "AttributeAssessment.h"

GINGA_NCL_BEGIN

AttributeAssessment::AttributeAssessment (const string &role)
    : Assessment (), Role ()
{
  Role::setLabel (role);
  _eventType = EventType::ATTRIBUTION;
  _attributeType = AttributeType::NODE_PROPERTY;
  _key = "";
  _offset = "";
  _maxCon = 1;
}

void
AttributeAssessment::setMaxCon (int max)
{
  _maxCon = max;
}

void
AttributeAssessment::setMinCon (int min)
{
  _minCon = min;
}

short
AttributeAssessment::getAttributeType ()
{
  return _attributeType;
}

void
AttributeAssessment::setAttributeType (short attribute)
{
  _attributeType = attribute;
}

string
AttributeAssessment::getKey ()
{
  return _key;
}

void
AttributeAssessment::setKey (const string &key)
{
  this->_key = key;
}

string
AttributeAssessment::getOffset ()
{
  return _offset;
}

void
AttributeAssessment::setOffset (const string &offset)
{
  this->_offset = offset;
}

GINGA_NCL_END
