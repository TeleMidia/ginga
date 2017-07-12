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

/**
 * @brief Creates a new simple condition.
 * @param type Event type.
 * @param transition Condition type.
 * @param label Role.
 * @param conj True if multiple binds are to be interpreted as conjunctions.
 * @param delay Delay.
 * @param key Key.
 */
SimpleCondition::SimpleCondition (EventType type,
                                  EventStateTransition transition,
                                  const string &label,
                                  bool conj,
                                  const string &delay,
                                  const string &key)
  : TriggerExpression (), Role (type, label)
{
  _transition = transition;
  _conjunction = conj;
  _delay = delay;
  _key = key;
}

/**
 * @brief Gets transition.
 */
EventStateTransition
SimpleCondition::getTransition ()
{
  return _transition;
}

/**
 * @brief Tests whether simple condition is a conjunction.
 */
bool
SimpleCondition::isConjunction ()
{
  return _conjunction;
}

/**
 * @brief Gets delay.
 */
string
SimpleCondition::getDelay ()
{
  return _delay;
}

/**
 * @brief Gets key.
 */
string
SimpleCondition::getKey ()
{
  return _key;
}

GINGA_NCL_END
