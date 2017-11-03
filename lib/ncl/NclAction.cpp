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
#include "NclAction.h"

GINGA_NCL_BEGIN

NclAction::NclAction (NclEventType type,
                      NclEventStateTransition transition,
                      const string &label,
                      const string &delay,
                      const string &value,
                      const string &duration)

  : NclRole (type, label)
{
  _transition = transition;
  _delay = delay;
  _value = value;
  _duration = duration;
}

NclAction::~NclAction ()
{
}

NclEventStateTransition
NclAction::getTransition ()
{
  return _transition;
}

string
NclAction::getDelay ()
{
  return _delay;
}

string
NclAction::getValue ()
{
  return _value;
}

string
NclAction::getDuration ()
{
  return _duration;
}


// Public: Static.

bool
NclAction::isReserved (const string &role,
                    NclEventType *type,
                    NclEventStateTransition *trans)
{
  static map<string, pair<int,int>> reserved =
    {
     {"start",
      {(int) NclEventType::PRESENTATION,
       (int) NclEventStateTransition::START}},
     {"stop",
      {(int) NclEventType::PRESENTATION,
       (int) NclEventStateTransition::STOP}},
     {"abort",
      {(int) NclEventType::PRESENTATION,
       (int) NclEventStateTransition::ABORT}},
     {"pause",
      {(int) NclEventType::PRESENTATION,
       (int) NclEventStateTransition::PAUSE}},
     {"resume",
      {(int) NclEventType::PRESENTATION,
       (int) NclEventStateTransition::RESUME}},
     {"set",
      {(int) NclEventType::ATTRIBUTION,
       (int) NclEventStateTransition::START}},
    };
  map<string, pair<int,int>>::iterator it;
  if ((it = reserved.find (role)) == reserved.end ())
    return false;
  tryset (type, (NclEventType) it->second.first);
  tryset (trans, (NclEventStateTransition) it->second.second);
  return true;
}

GINGA_NCL_END
