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

#include "config.h"
#include "ncl/CausalConnector.h"

GINGA_NCL_BEGIN

	CausalConnector::CausalConnector(string id) : Connector(id) {
		conditionExpression = NULL;
		actionExpression    = NULL;

		typeSet.insert("CausalConnector");
	}

	CausalConnector::CausalConnector(
		    string id,
		    ConditionExpression *condition,
		    Action *action) : Connector(id) {

		conditionExpression = (TriggerExpression*)condition;
		actionExpression    = (Action*)action;

		typeSet.insert("CausalConnector");
	}

	CausalConnector::~CausalConnector() {
		releaseAction();
		releaseCondition();
	}

	void CausalConnector::releaseAction() {
		if (actionExpression != NULL) {
			delete actionExpression;
			actionExpression = NULL;
		}
	}

	void CausalConnector::releaseCondition() {
		if (conditionExpression != NULL) {
			//delete conditionExpression;
			//conditionExpression = NULL;
		}
	}

	Action* CausalConnector::getAction() {
		return actionExpression;
	}

	ConditionExpression *CausalConnector::getConditionExpression() {
		return conditionExpression;
	}

	void CausalConnector::setAction(Action *newAction) {
		releaseAction();
		actionExpression = newAction;
	}

	void CausalConnector::setConditionExpression(
		    ConditionExpression *newConditionExpression) {

		releaseCondition();
		conditionExpression = newConditionExpression;
	}

	void CausalConnector::getConditionRoles(
		    ConditionExpression *condition, vector<Role*> *roles) {

		vector<Role*> *childRoles;

		if (conditionExpression->instanceOf("SimpleCondition")) {
			roles->push_back((SimpleCondition*)condition);

		} else {
			if (conditionExpression->instanceOf("AssessmentStatement")) {
				childRoles = ((AssessmentStatement*)condition)->getRoles();

			} else if (conditionExpression->instanceOf(
				    "CompoundCondition")) {

				childRoles = ((CompoundCondition*)condition)->
					    getRoles();

			} else {
				childRoles = ((CompoundStatement*)condition)->getRoles();
			}

			vector<Role*>::iterator it;
			for (it = childRoles->begin(); it != childRoles->end(); ++it) {
				roles->push_back(*it);
			}

			delete childRoles;
		}
	}

	void CausalConnector::getActionRoles(
		    Action *action, vector<Role*> *roles) {

		vector<Role*>* childRoles;

		if (actionExpression->instanceOf("SimpleAction")) {
			roles->push_back((SimpleAction*)action);

		} else {
			childRoles = ((CompoundAction*)action)->getRoles();
			vector<Role*>::iterator it;
			for (it = childRoles->begin(); it != childRoles->end(); ++it) {
				roles->push_back( *it );
			}

			delete childRoles;
		}
	}

	vector<Role*> *CausalConnector::getRoles() {
		vector<Role*> *roles;
		roles = new vector<Role*>;
		getConditionRoles(conditionExpression, roles);
		getActionRoles(actionExpression, roles);
		return roles;
	}

GINGA_NCL_END
