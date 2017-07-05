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

#ifndef _CAUSALCONNECTOR_H_
#define _CAUSALCONNECTOR_H_

#include "Connector.h"
#include "Action.h"
#include "AssessmentStatement.h"
#include "CausalConnector.h"
#include "CompoundAction.h"
#include "CompoundStatement.h"
#include "CompoundCondition.h"
#include "Condition.h"
#include "SimpleAction.h"
#include "SimpleCondition.h"
#include "Role.h"

GINGA_NCL_BEGIN

class CausalConnector : public Connector
{
public:
  CausalConnector (const string &_id);
  virtual ~CausalConnector ();

  Action *getAction ();
  Condition *getCondition ();
  void setAction (Action *newAction);
  void setCondition (Condition *newCondition);

  vector<Role *> *getRoles ();

private:
  Condition *_conditionExpression;
  Action *_actionExpression;

  void getConditionRoles (Condition *condition,
                          vector<Role *> *roles);

  void getActionRoles (Action *action, vector<Role *> *roles);
};

GINGA_NCL_END

#endif //_CAUSALCONNECTOR_H_
