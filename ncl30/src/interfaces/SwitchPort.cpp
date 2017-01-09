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

#include "ncl/interfaces/SwitchPort.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace interfaces {
	SwitchPort::SwitchPort(
		    string id,
		    void *switchNode) : Port(id, (Node*)switchNode, NULL) {

		portList = new vector<Port*>;
		typeSet.insert("SwitchPort");
	}

	SwitchPort::~SwitchPort() {
		vector<Port*>::iterator i;

		if (portList != NULL) {
			i = portList->begin();
			while (i != portList->end()) {
				delete *i;
				++i;
			}

			delete portList;
			portList = NULL;
		}
	}

	bool SwitchPort::addPort(Port* port) {
		if (containsMap(port->getNode()))
			return false;

		portList->push_back(port);
		return true;
	}

	bool SwitchPort::containsMap(Node* node) {
		vector<Port*>::iterator i;
		Port* port;

		for (i = portList->begin(); i < portList->end(); ++i) {
			port = (Port*)(*i);
			if (port->getNode()->getId() == node->getId())
				return true;
		}
		return false;
	}

	vector<Port*>* SwitchPort::getPorts() {
		return portList;
	}

	bool SwitchPort::removePort(Port* port) {
		vector<Port*>::iterator i;

		for (i = portList->begin(); i != portList->end(); ++i) {
			if ((*i)->getId() == port->getId()) {
				portList->erase(i);
				return true;
			}
		}
		return false;
	}

	Node* SwitchPort::getEndNode() {
		return node;
	}

	InterfacePoint* SwitchPort::getEndInterfacePoint() {
		//Polimorfismo
		return (InterfacePoint*)(this);
	}
}
}
}
}
}
