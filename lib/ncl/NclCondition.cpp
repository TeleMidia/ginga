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
#include "NclCondition.h"

GINGA_NAMESPACE_BEGIN

NclCondition::NclCondition (FormatterEvent::Type type,
                            FormatterEvent::Transition transition,
                            FormatterPredicate *predicate,
                            const string &label,
                            const string &key) : NclRole (type, label)
{
  _transition = transition;
  _predicate = predicate;
  _key = key;
}

NclCondition::~NclCondition ()
{
  if (_predicate)
    delete _predicate;
}

FormatterEvent::Transition
NclCondition::getTransition ()
{
  return _transition;
}

FormatterPredicate *
NclCondition::getPredicate ()
{
  return _predicate;
}

string
NclCondition::getKey ()
{
  return _key;
}


// Public: Static.

bool
NclCondition::isReserved (const string &role,
                          FormatterEvent::Type *type,
                          FormatterEvent::Transition *trans)
{
  static map<string, pair<int,int>> reserved =
    {
     {"onBegin",
      {(int) FormatterEvent::Type::PRESENTATION,
       (int) FormatterEvent::Transition::START}},
     {"onEnd",
      {(int) FormatterEvent::Type::PRESENTATION,
       (int) FormatterEvent::Transition::STOP}},
     {"onAbort",
      {(int) FormatterEvent::Type::PRESENTATION,
       (int) FormatterEvent::Transition::ABORT}},
     {"onPause",
      {(int) FormatterEvent::Type::PRESENTATION,
       (int) FormatterEvent::Transition::PAUSE}},
     {"onResumes",
      {(int) FormatterEvent::Type::PRESENTATION,
       (int) FormatterEvent::Transition::RESUME}},
     {"onBeginAttribution",
      {(int) FormatterEvent::Type::ATTRIBUTION,
       (int) FormatterEvent::Transition::START}},
     {"onEndAttribution",
      {(int) FormatterEvent::Type::SELECTION,
       (int) FormatterEvent::Transition::STOP}},
     {"onSelection",
      {(int) FormatterEvent::Type::SELECTION,
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
