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
#include "SimpleRule.h"

GINGA_NCL_BEGIN

SimpleRule::SimpleRule (const string &id, const string &attr, short op, const string &val)
    : Rule (id)
{
  attribute = attr;
  setOperator (op);
  value = val;
  typeSet.insert ("SimpleRule");
}

string
SimpleRule::getAttribute ()
{
  return attribute;
}

short
SimpleRule::getOperator ()
{
  return ruleOperator;
}

string
SimpleRule::getValue ()
{
  return value;
}

void
SimpleRule::setOperator (short newOp)
{
  if (newOp < 0 || newOp > 5)
    newOp = 0;

  ruleOperator = newOp;
}

void
SimpleRule::setValue (const string &newValue)
{
  value = newValue;
}

void
SimpleRule::setAttribute (const string &someAttribute)
{
  attribute = someAttribute;
}

GINGA_NCL_END
