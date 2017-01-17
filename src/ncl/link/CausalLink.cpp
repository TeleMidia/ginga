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
#include "ncl/link/CausalLink.h"

BR_PUCRIO_TELEMIDIA_NCL_LINK_BEGIN

	CausalLink::CausalLink(string uid, Connector* connector) :
		     Link(uid, connector) {

		typeSet.insert("CausalLink");
	}

	bool CausalLink::containsSourceNode(
			Node *node, GenericDescriptor* descriptor) {

		bool contains;
		vector<Bind*>* conds;

		conds = getConditionBinds();
		contains = Link::containsNode(node, descriptor, conds);

		delete conds;
		return contains;
	}

	vector<Bind*>* CausalLink::getActionBinds() {
		vector<Bind*>* actionsVector;
		actionsVector = new vector<Bind*>;
		vector<Bind*>::iterator i;

		if (binds->empty()) {
			delete actionsVector;
			return NULL;
		}

		for (i = binds->begin(); i != binds->end(); ++i) {
			if (((*i)->getRole())->instanceOf("SimpleAction"))
				actionsVector->push_back(*i);
		}

		if (actionsVector->empty()) {
			delete actionsVector;
			return NULL;
		}

		return actionsVector;
	}

	vector<Bind*>* CausalLink::getConditionBinds() {
		vector<Bind*>* conditionsVector;
		vector<Bind*>::iterator iterator;
		Bind* bind;
		Role* role;

		if (binds->empty()) {
			return NULL;
		}

		conditionsVector = new vector<Bind*>;
		for (iterator = binds->begin(); iterator != binds->end(); ++iterator) {
			bind = (Bind*)(*iterator);
			role = bind->getRole();
			if (role != NULL) {
				if (role->instanceOf("SimpleCondition") ||
						role->instanceOf("AttributeAssessment")) {

					conditionsVector->push_back(bind);
				}
			}
		}

		if (conditionsVector->empty()) {
			delete conditionsVector;
			conditionsVector = NULL;
		}

		return conditionsVector;
	}

BR_PUCRIO_TELEMIDIA_NCL_LINK_END
