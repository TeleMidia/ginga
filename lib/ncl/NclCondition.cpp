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

NclCondition::NclCondition (Event::Type type,
                            Event::Transition transition,
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

Event::Transition
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
                          Event::Type *type,
                          Event::Transition *trans)
{
  static map<string, pair<int,int>> reserved =
    {
     {"onBegin",
      {(int) Event::PRESENTATION,
       (int) Event::START}},
     {"onEnd",
      {(int) Event::PRESENTATION,
       (int) Event::STOP}},
     {"onAbort",
      {(int) Event::PRESENTATION,
       (int) Event::ABORT}},
     {"onPause",
      {(int) Event::PRESENTATION,
       (int) Event::PAUSE}},
     {"onResumes",
      {(int) Event::PRESENTATION,
       (int) Event::RESUME}},
     {"onBeginAttribution",
      {(int) Event::ATTRIBUTION,
       (int) Event::START}},
     {"onEndAttribution",
      {(int) Event::SELECTION,
       (int) Event::STOP}},
     {"onSelection",
      {(int) Event::SELECTION,
       (int) Event::START}},
    };
  map<string, pair<int,int>>::iterator it;
  if ((it = reserved.find (role)) == reserved.end ())
    return false;
  tryset (type, (Event::Type) it->second.first);
  tryset (trans, (Event::Transition) it->second.second);
  return true;
}

GINGA_NAMESPACE_END
