/* Copyright (C) 1989-2017 PUC-Rio/Laboratorio TeleMidia

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

#include "ncl/connectors/Connector.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace connectors {
	Connector::Connector(string id) : Entity(id) {
		parameters = new map<string, Parameter*>;

		typeSet.insert("Connector");
	}

	Connector::~Connector() {
		map<string, Parameter*>::iterator i;

		if (parameters != NULL) {
			i = parameters->begin();
			while (i != parameters->end()) {
				delete i->second;
				++i;
			}
			delete parameters;
			parameters = NULL;
		}
	}

	int Connector::getNumRoles() {
		int numOfRoles;

		vector<Role*>* childRoles;
		childRoles = getRoles();
		numOfRoles = childRoles->size();
		delete childRoles;

		return numOfRoles;
	}

	Role* Connector::getRole(string roleId) {
		Role* wRole;
		vector<Role*> *roles = getRoles();
		if (roles == NULL) {
			return NULL;
		}

		vector<Role*>::iterator i;
		for (i=roles->begin(); i != roles->end(); ++i) {
			wRole = *i;
			if (wRole->getLabel() == roleId) {
				delete roles;
				return wRole;
			}
		}
		delete roles;
		return NULL;
	}

	void Connector::addParameter(Parameter* parameter) {
		if (parameter == NULL)
			return;

		map<string, Parameter*>::iterator i;
		for (i=parameters->begin(); i!=parameters->end(); ++i)
			if (i->first == parameter->getName())
				return;

		(*parameters)[parameter->getName()] = parameter;
	}

	vector<Parameter*>* Connector::getParameters() {
		if (parameters->empty())
			return NULL;

		vector<Parameter*>* params;
		params = new vector<Parameter*>;
		map<string, Parameter*>::iterator i;
		for (i=parameters->begin(); i!=parameters->end(); ++i)
			params->push_back(i->second);

		return params;
	}

	Parameter* Connector::getParameter(string name) {
		if (parameters->empty())
			return NULL;

		map<string, Parameter*>::iterator i;
		for (i=parameters->begin(); i!=parameters->end(); ++i)
			if (i->first == name)
				return (Parameter*)(i->second);

		return NULL;
	}

	bool Connector::removeParameter(string name) {
		if (parameters->empty())
			return false;

		map<string, Parameter*>::iterator i;
		for (i=parameters->begin(); i!=parameters->end(); ++i) {
			if (i->first == name) {
				parameters->erase(i);
				return true;
			}
		}

		return false;
	}
}
}
}
}
}
