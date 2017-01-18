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

#include "config.h"
#include "ncl/SimpleRule.h"

GINGA_NCL_BEGIN

SimpleRule::SimpleRule (string id, string attr, short op, string val)
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
SimpleRule::setValue (string newValue)
{
  value = newValue;
}

string
SimpleRule::toString ()
{
  ostringstream os;
  os << ruleOperator;
  return attribute + " " + os.str () + " " + value;
}

void
SimpleRule::setAttribute (string someAttribute)
{
  attribute = someAttribute;
}

GINGA_NCL_END
