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
#include "Connector.h"

#include "Action.h"
#include "Condition.h"

GINGA_NCL_BEGIN


// Public.

/**
 * @brief Creates a new connector.
 * @param ncl Parent document.
 * @param id Connector id.
 */
Connector::Connector (NclDocument *ncl, const string &id) : Entity (ncl, id)
{
}

/**
 * Destroys connector.
 */
Connector::~Connector ()
{
}

/**
 * @brief Gets connector conditions.
 * @return Connector conditions.
 */
const vector <Condition *> *
Connector::getConditions ()
{
  return &_conditions;
}

/**
 * @brief Adds condition to connector.
 * @param condition Condition to add.
 * @return True if successful, or false otherwise.
 */
bool
Connector::addCondition (Condition *condition)
{
  g_assert_nonnull (condition);
  for (auto cond: _conditions)
    if (cond == condition)
      return false;
  _conditions.push_back (condition);
  return true;
}

/**
 * @brief Gets connector actions.
 * @return Connector actions.
 */
const vector<Action *> *
Connector::getActions ()
{
  return &_actions;
}

/**
 * @brief Adds action to connector.
 * @param action Action to add.
 * @return True if successful, or false otherwise.
 */
bool
Connector::addAction (Action *action)
{
  g_assert_nonnull (action);
  for (auto act: _actions)
    if (act == action)
      return false;
  _actions.push_back (action);
  return true;
}

/**
 * @brief Gets connector role.
 * @param label Role label.
 * @return Connector role if successful, or null otherwise.
 */
Role *
Connector::getRole (const string &label)
{
  for (auto conn: _conditions)
    if (conn->getLabel () == label)
      return conn;
  for (auto act: _actions)
    if (act->getLabel () == label)
      return act;
  return nullptr;
}

GINGA_NCL_END
