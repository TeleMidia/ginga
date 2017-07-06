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
#include "Connector.h"

#include "AssessmentStatement.h"
#include "CompoundAction.h"
#include "CompoundCondition.h"
#include "SimpleAction.h"
#include "SimpleCondition.h"

GINGA_NCL_BEGIN

/**
 * @brief Creates a new connector.
 * @param id Connector id.
 */
Connector::Connector (const string &id) : Entity (id)
{
  _condition = nullptr;
  _action = nullptr;
}

/**
 * Destroys connector.
 */
Connector::~Connector ()
{
  _parameters.clear ();
  _roles.clear ();
  delete _condition;
  delete _action;
}

/**
 * @brief Adds parameter to connector.
 * @param parameter Parameter.
 */
void
Connector::addParameter (Parameter *parameter)
{
  g_assert_nonnull (parameter);
  _parameters.push_back (parameter);
}

/**
 * @brief Gets all connector parameters.
 * @return All connector parameters.
 */
const vector<Parameter *> *
Connector::getParameters ()
{
  return &_parameters;
}

/**
 * @brief Gets connector parameter.
 * @param name Parameter name.
 * @return Parameter if successful, or null otherwise.
 */
Parameter *
Connector::getParameter (const string &name)
{
  for (auto param: _parameters)
    if (param->getName () == name)
      return param;
  return nullptr;
}

/**
 * @brief Gets connector condition.
 */
Condition *
Connector::getCondition ()
{
  return _condition;
}

/**
 * @brief Sets connector condition.  (Can only be called once.)
 */
void
Connector::setCondition (Condition *condition)
{
  g_assert_nonnull (condition);
  g_assert_null (_condition);
  _condition = condition;

  if (instanceof (SimpleCondition *, _condition))
    {
      _roles.push_back (cast (Role *, _condition));
    }
  else
    {
      for (auto role: *(((CompoundCondition *) _condition)->getRoles ()))
        _roles.push_back (role);
    }
}

/**
 * @brief Gets connector action.
 */
Action *
Connector::getAction ()
{
  return _action;
}

/**
 * @brief Sets connector action.  (Can only be called once.)
 */
void
Connector::setAction (Action *action)
{
  g_assert_nonnull (action);
  g_assert_null (_action);
  _action = action;

  if (instanceof (SimpleAction *, _action))
    {
      _roles.push_back (cast (Role *, _action));
    }
  else
    {
      for (auto role: *(((CompoundAction *) _action)->getRoles ()))
        _roles.push_back (role);
    }
}

/**
 * @brief Gets connector role.
 * @param label Role label.
 * @return Connector role if successful, or null otherwise.
 */
Role *
Connector::getRole (const string &label)
{
  for (auto role: _roles)
    if (role->getLabel () == label)
      return role;
  return nullptr;
}

GINGA_NCL_END
