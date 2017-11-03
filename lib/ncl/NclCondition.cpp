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

NclCondition::NclCondition (NclEventType type,
                            NclEventStateTransition transition,
                            Predicate *predicate,
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

NclEventStateTransition
NclCondition::getTransition ()
{
  return _transition;
}

Predicate *
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
                          NclEventType *type,
                          NclEventStateTransition *trans)
{
  static map<string, pair<int,int>> reserved =
    {
     {"onBegin",
      {(int) NclEventType::PRESENTATION,
       (int) NclEventStateTransition::START}},
     {"onEnd",
      {(int) NclEventType::PRESENTATION,
       (int) NclEventStateTransition::STOP}},
     {"onAbort",
      {(int) NclEventType::PRESENTATION,
       (int) NclEventStateTransition::ABORT}},
     {"onPause",
      {(int) NclEventType::PRESENTATION,
       (int) NclEventStateTransition::PAUSE}},
     {"onResumes",
      {(int) NclEventType::PRESENTATION,
       (int) NclEventStateTransition::RESUME}},
     {"onBeginAttribution",
      {(int) NclEventType::ATTRIBUTION,
       (int) NclEventStateTransition::START}},
     {"onEndAttribution",
      {(int) NclEventType::SELECTION,
       (int) NclEventStateTransition::STOP}},
     {"onSelection",
      {(int) NclEventType::SELECTION,
       (int) NclEventStateTransition::START}},
    };
  map<string, pair<int,int>>::iterator it;
  if ((it = reserved.find (role)) == reserved.end ())
    return false;
  tryset (type, (NclEventType) it->second.first);
  tryset (trans, (NclEventStateTransition) it->second.second);
  return true;
}

GINGA_NAMESPACE_END
