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
#include "SimpleAction.h"

GINGA_NCL_BEGIN

SimpleAction::SimpleAction (SimpleActionType type,
                            const string &role,
                            const string &delay,
                            const string &repeat,
                            const string &repeatDelay,
                            const string &value,
                            const string &duration,
                            const string &by)
  : Action (), Role ()
{
  _type = type;
  Role::setLabel (role);
  _delay = delay;
  _repeat = repeat;
  _repeatDelay = repeatDelay;
  _value = value;
  _duration = duration;
  _by = by;
}

SimpleAction::~SimpleAction ()
{
}

SimpleActionType
SimpleAction::getActionType ()
{
  return _type;
}

string
SimpleAction::getDelay ()
{
  return _delay;
}

string
SimpleAction::getRepeat ()
{
  return _repeat;
}

string
SimpleAction::getRepeatDelay ()
{
  return _repeatDelay;
}

string
SimpleAction::getValue ()
{
  return _value;
}

string
SimpleAction::getDuration ()
{
  return _duration;
}

string
SimpleAction::getBy ()
{
  return _by;
}

GINGA_NCL_END
