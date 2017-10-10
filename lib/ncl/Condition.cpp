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
#include "Condition.h"

GINGA_NCL_BEGIN

/**
 * @brief Creates a new condition.
 * @param type Event type.
 * @param transition Transition.
 * @param predicate Predicate.
 * @param label Role.
 * @param delay Delay.
 * @param key Key.
 */
Condition::Condition (EventType type,
                      EventStateTransition transition,
                      Predicate *predicate,
                      const string &label,
                      const string &key) : Role (type, label)
{
  _transition = transition;
  _predicate = predicate;
  _key = key;
}

/**
 * @brief Destroys condition.
 */
Condition::~Condition ()
{
  if (_predicate)
    delete _predicate;
}

/**
 * @brief Gets transition.
 * @return Transition.
 */
EventStateTransition
Condition::getTransition ()
{
  return _transition;
}

/**
 * @brief Gets predicate.
 * @return Predicate.
 */
Predicate *
Condition::getPredicate ()
{
  return _predicate;
}

/**
 * @brief Gets key.
 * @return Key.
 */
string
Condition::getKey ()
{
  return _key;
}


// Public: Static.

/**
 * @brief Tests whether role is a reserved condition role.
 * @param type Address of variable to store role type.
 * @param trans Address of variable to store role transition.
 * @return True if successful, or false otherwise.
 */
bool
Condition::isReserved (const string &role,
                             EventType *type,
                             EventStateTransition *trans)
{
  static map<string, pair<int,int>> reserved =
    {
     {"onBegin",
      {(int) EventType::PRESENTATION,
       (int) EventStateTransition::START}},
     {"onEnd",
      {(int) EventType::PRESENTATION,
       (int) EventStateTransition::STOP}},
     {"onAbort",
      {(int) EventType::PRESENTATION,
       (int) EventStateTransition::ABORT}},
     {"onPause",
      {(int) EventType::PRESENTATION,
       (int) EventStateTransition::PAUSE}},
     {"onResumes",
      {(int) EventType::PRESENTATION,
       (int) EventStateTransition::RESUME}},
     {"onBeginAttribution",
      {(int) EventType::ATTRIBUTION,
       (int) EventStateTransition::START}},
     {"onEndAttribution",
      {(int) EventType::SELECTION,
       (int) EventStateTransition::STOP}},
     {"onSelection",
      {(int) EventType::SELECTION,
       (int) EventStateTransition::START}},
    };
  map<string, pair<int,int>>::iterator it;
  if ((it = reserved.find (role)) == reserved.end ())
    return false;
  tryset (type, (EventType) it->second.first);
  tryset (trans, (EventStateTransition) it->second.second);
  return true;
}

GINGA_NCL_END
