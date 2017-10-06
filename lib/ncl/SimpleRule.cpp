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
#include "SimpleRule.h"

GINGA_NCL_BEGIN

/**
 * @brief Creates a new simple rule.
 * @param ncl Parent document.
 * @param id Rule id.
 * @param attr Attribute.
 * @param op Operator.
 * @param value Value.
 */
SimpleRule::SimpleRule (NclDocument *ncl, const string &id,
                        const string &attr, const string &op,
                        const string &value)
  : Rule (ncl, id)
{
  _attribute = attr;
  _operator = op;
  _value = value;
}

/**
 * @brief Destroys simple rule.
 */
SimpleRule::~SimpleRule ()
{
}

/**
 * @brief Gets attribute.
 */
string
SimpleRule::getAttribute ()
{
  return _attribute;
}

/**
 * @brief Gets operator.
 */
string
SimpleRule::getOperator ()
{
  return _operator;
}

/**
 * @brief Gets value.
 */
string
SimpleRule::getValue ()
{
  return _value;
}

GINGA_NCL_END
