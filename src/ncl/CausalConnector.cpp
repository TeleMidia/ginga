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
#include "CausalConnector.h"

GINGA_NCL_BEGIN

CausalConnector::CausalConnector (const string &id) : Connector (id)
{
  _conditionExpression = NULL;
  _actionExpression = NULL;
}

CausalConnector::CausalConnector (const string &id, ConditionExpression *condition,
                                  Action *action)
    : Connector (id)
{
  _conditionExpression = (TriggerExpression *)condition;
  _actionExpression = (Action *)action;
}

CausalConnector::~CausalConnector ()
{
  releaseAction ();
  releaseCondition ();
}

void
CausalConnector::releaseAction ()
{
  if (_actionExpression != NULL)
    {
      delete _actionExpression;
      _actionExpression = NULL;
    }
}

void
CausalConnector::releaseCondition ()
{
  if (_conditionExpression != NULL)
    {
      // delete conditionExpression;
      // conditionExpression = NULL;
    }
}

Action *
CausalConnector::getAction ()
{
  return _actionExpression;
}

ConditionExpression *
CausalConnector::getConditionExpression ()
{
  return _conditionExpression;
}

void
CausalConnector::setAction (Action *newAction)
{
  releaseAction ();
  _actionExpression = newAction;
}

void
CausalConnector::setConditionExpression (
    ConditionExpression *newConditionExpression)
{
  releaseCondition ();
  _conditionExpression = newConditionExpression;
}

void
CausalConnector::getConditionRoles (ConditionExpression *condition,
                                    vector<Role *> *roles)
{
  vector<Role *> *childRoles;

  if (instanceof (SimpleCondition *, _conditionExpression))
    {
      roles->push_back ((SimpleCondition *)condition);
    }
  else
    {
      if (instanceof (AssessmentStatement *, _conditionExpression))
        {
          childRoles = ((AssessmentStatement *)condition)->getRoles ();
        }
      else if (instanceof (CompoundCondition *, _conditionExpression))
        {
          childRoles = ((CompoundCondition *)condition)->getRoles ();
        }
      else
        {
          childRoles = ((CompoundStatement *)condition)->getRoles ();
        }

      vector<Role *>::iterator it;
      for (it = childRoles->begin (); it != childRoles->end (); ++it)
        {
          roles->push_back (*it);
        }

      delete childRoles;
    }
}

void
CausalConnector::getActionRoles (Action *action, vector<Role *> *roles)
{
  vector<Role *> *childRoles;

  if (instanceof (SimpleAction *, _actionExpression))
    {
      roles->push_back ((SimpleAction *)action);
    }
  else
    {
      childRoles = ((CompoundAction *)action)->getRoles ();
      vector<Role *>::iterator it;
      for (it = childRoles->begin (); it != childRoles->end (); ++it)
        {
          roles->push_back (*it);
        }

      delete childRoles;
    }
}

vector<Role *> *
CausalConnector::getRoles ()
{
  vector<Role *> *roles;
  roles = new vector<Role *>;
  getConditionRoles (_conditionExpression, roles);
  getActionRoles (_actionExpression, roles);
  return roles;
}

GINGA_NCL_END
