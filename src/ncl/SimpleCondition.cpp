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
#include "SimpleCondition.h"

GINGA_NCL_BEGIN

SimpleCondition::SimpleCondition (EventType type, const string &label)
    : TriggerExpression (), Role (type, label)
{
  _key = "";
  _qualifier = -1;
}

string
SimpleCondition::getKey ()
{
  return _key;
}

void
SimpleCondition::setKey (const string &key)
{
  this->_key = key;
}

EventStateTransition
SimpleCondition::getTransition ()
{
  return _transition;
}

void
SimpleCondition::setTransition (EventStateTransition transition)
{
  this->_transition = transition;
}

short
SimpleCondition::getQualifier ()
{
  return _qualifier;
}

void
SimpleCondition::setQualifier (short qualifier)
{
  this->_qualifier = qualifier;
}

GINGA_NCL_END
