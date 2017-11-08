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

GINGA_NAMESPACE_BEGIN

NclAction::NclAction (FormatterEvent::Type type,
                      FormatterEvent::Transition transition,
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

FormatterEvent::Transition
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
                    FormatterEvent::Type *type,
                    FormatterEvent::Transition *trans)
{
  static map<string, pair<int,int>> reserved =
    {
     {"start",
      {(int) FormatterEvent::Type::PRESENTATION,
       (int) FormatterEvent::Transition::START}},
     {"stop",
      {(int) FormatterEvent::Type::PRESENTATION,
       (int) FormatterEvent::Transition::STOP}},
     {"abort",
      {(int) FormatterEvent::Type::PRESENTATION,
       (int) FormatterEvent::Transition::ABORT}},
     {"pause",
      {(int) FormatterEvent::Type::PRESENTATION,
       (int) FormatterEvent::Transition::PAUSE}},
     {"resume",
      {(int) FormatterEvent::Type::PRESENTATION,
       (int) FormatterEvent::Transition::RESUME}},
     {"set",
      {(int) FormatterEvent::Type::ATTRIBUTION,
       (int) FormatterEvent::Transition::START}},
    };
  map<string, pair<int,int>>::iterator it;
  if ((it = reserved.find (role)) == reserved.end ())
    return false;
  tryset (type, (FormatterEvent::Type) it->second.first);
  tryset (trans, (FormatterEvent::Transition) it->second.second);
  return true;
}

GINGA_NAMESPACE_END
