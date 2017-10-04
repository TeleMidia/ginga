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
#include "SimpleAction.h"

GINGA_NCL_BEGIN

/**
 * @brief Creates a new simple action.
 * @param type Event type.
 * @param actType Action type.
 * @param label Role.
 * @param delay Delay.
 * @param repeat Repeat.
 * @param repeatDelay Repeat delay.
 * @param value Value.
 * @param duration Duration.
 * @param by By.
 */
SimpleAction::SimpleAction (EventType type,
                            EventStateTransition actType,
                            const string &label,
                            const string &delay,
                            const string &repeat,
                            const string &repeatDelay,
                            const string &value,
                            const string &duration,
                            const string &by)
  : Action (), Role (type, label)
{
  _actionType = actType;
  _delay = delay;
  _repeat = repeat;
  _repeatDelay = repeatDelay;
  _value = value;
  _duration = duration;
  _by = by;
}

/**
 * @brief Destroys simple action.
 */
SimpleAction::~SimpleAction ()
{
}

/**
 * @brief Gets simple action type.
 */
EventStateTransition
SimpleAction::getActionType ()
{
  return _actionType;
}

/**
 * @brief Gets simple action delay.
 */
string
SimpleAction::getDelay ()
{
  return _delay;
}

/**
 * @brief Gets simple action repeat.
 */
string
SimpleAction::getRepeat ()
{
  return _repeat;
}

/**
 * @brief Gets simple action repeat delay.
 */
string
SimpleAction::getRepeatDelay ()
{
  return _repeatDelay;
}

/**
 * @brief Gets simple action value.
 */
string
SimpleAction::getValue ()
{
  return _value;
}

/**
 * @brief Gets simple action duration.
 */
string
SimpleAction::getDuration ()
{
  return _duration;
}

/**
 * @brief Gets simple action by.
 */
string
SimpleAction::getBy ()
{
  return _by;
}


// Public: Static.

/**
 * @brief Tests whether role is a reserved action role.
 * @param type Address of variable to store role type.
 * @param trans Address of variable to store role transition.
 * @return True if successful, or false otherwise.
 */
bool
SimpleAction::isReserved (const string &role,
                          EventType *type,
                          EventStateTransition *trans)
{
  static map<string, pair<int,int>> reserved =
    {
     {"start",
      {(int) EventType::PRESENTATION,
       (int) EventStateTransition::START}},
     {"stop",
      {(int) EventType::PRESENTATION,
       (int) EventStateTransition::STOP}},
     {"abort",
      {(int) EventType::PRESENTATION,
       (int) EventStateTransition::ABORT}},
     {"pause",
      {(int) EventType::PRESENTATION,
       (int) EventStateTransition::PAUSE}},
     {"resume",
      {(int) EventType::PRESENTATION,
       (int) EventStateTransition::RESUME}},
     {"set",
      {(int) EventType::ATTRIBUTION,
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
