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

SimpleAction::SimpleAction (const string &role) : Action (), Role ()
{
  SimpleAction::setLabel (role);
  _qualifier = CompoundAction::OP_PAR;
  _repeat = "0";
  _repeatDelay = "0";
  _value = "";
  _animation = NULL;
  _typeSet.insert ("SimpleAction");
  _typeSet.insert ("Role");
}

SimpleAction::~SimpleAction ()
{
  if (_animation != NULL)
    {
      delete _animation;
      _animation = NULL;
    }
}

void
SimpleAction::setLabel (const string &id)
{
  _label = id;

  if (xstrcaseeq (_label, "start"))
    {
      _actionType = ACT_START;
      _eventType = EventType::PRESENTATION;
    }
  else if (xstrcaseeq (_label, "stop"))
    {
      _actionType = ACT_STOP;
      _eventType = EventType::PRESENTATION;
    }
  else if (xstrcaseeq (_label, "set"))
    {
      _actionType = ACT_START;
      _eventType = EventType::ATTRIBUTION;
    }
  else if (xstrcaseeq (_label, "abort"))
    {
      _actionType = ACT_ABORT;
      _eventType = EventType::PRESENTATION;
    }
  else if (xstrcaseeq (_label, "pause"))
    {
      _actionType = ACT_PAUSE;
      _eventType = EventType::PRESENTATION;
    }
  else if (xstrcaseeq (_label, "resume"))
    {
      _actionType = ACT_RESUME;
      _eventType = EventType::PRESENTATION;
    }
}

short
SimpleAction::getQualifier ()
{
  return _qualifier;
}

void
SimpleAction::setQualifier (short qualifier)
{
  this->_qualifier = qualifier;
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

void
SimpleAction::setRepeatDelay (const string &time)
{
  _repeatDelay = time;
}

void
SimpleAction::setRepeat (const string &newRepetitions)
{
  _repeat = newRepetitions;
}

SimpleActionType
SimpleAction::getActionType ()
{
  return _actionType;
}

void
SimpleAction::setActionType (SimpleActionType action)
{
  _actionType = action;
}

string
SimpleAction::getValue ()
{
  return _value;
}

void
SimpleAction::setValue (const string &value)
{
  this->_value = value;
}

Animation *
SimpleAction::getAnimation ()
{
  return _animation;
}

void
SimpleAction::setAnimation (Animation *animation)
{
  this->_animation = animation;
}

GINGA_NCL_END
