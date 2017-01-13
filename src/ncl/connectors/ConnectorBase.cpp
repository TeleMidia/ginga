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
#include "ncl/connectors/ConnectorBase.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace connectors {
	ConnectorBase::ConnectorBase(string id) : Base(id) {
		typeSet.insert("ConnectorBase");
	}

	ConnectorBase::~ConnectorBase() {
		set<Connector*>::iterator i;

		i = connectorSet.begin();
		while (i != connectorSet.end()) {
			if (Entity::hasInstance(*i, true)) {
				delete *i;
			}
			++i;
		}
		connectorSet.clear();
	}

	bool ConnectorBase::addConnector(Connector* connector) {
		if (connector == NULL || containsConnector(connector)) {
			return false;
		}

		connectorSet.insert(connector);
		return true;
	}

	bool ConnectorBase::addBase(Base* base, string alias, string location)
		     throw(IllegalBaseTypeException*) {

		if (Base::hasInstance(base, false) &&
				base->instanceOf("ConnectorBase")) {

			return Base::addBase(base, alias, location);
		}

		return false;
	}

	void ConnectorBase::clear() {
		connectorSet.clear();
		Base::clear();
	}

	bool ConnectorBase::containsConnector(string connectorId) {
		if (getConnectorLocally(connectorId) != NULL) {
			return true;

		} else {
			return false;
		}
	}

	bool ConnectorBase::containsConnector(Connector* connector) {
		set<Connector*>::iterator i;

		i = connectorSet.find(connector);
		if (i != connectorSet.end()) {
			return true;
		}
		return false;
	}

	Connector* ConnectorBase::getConnectorLocally(string connectorId) {
		set<Connector*>::iterator i;

		i = connectorSet.begin();
		while (i != connectorSet.end()) {
			if ((*i)->getId() == connectorId) {
				return (*i);
			}

			++i;
		}
		return NULL;
	}

	Connector* ConnectorBase::getConnector(string connectorId) {
		string::size_type index;
		string prefix, suffix;
		ConnectorBase* base;
		Connector* conn = NULL;
		map<string, Base*>::iterator i;

		index = connectorId.find_first_of("#");
		if (index == string::npos) {
			return getConnectorLocally(connectorId);
		}

		prefix = connectorId.substr(0, index);
		index++;
		suffix = connectorId.substr(index, connectorId.length() - index);

		i = baseAliases.find(prefix);
		if (i != baseAliases.end()) {
			base = (ConnectorBase*)(i->second);
			conn = base->getConnector(suffix);
		}

		if (conn == NULL) {
			i = baseLocations.find(prefix);
			if (i != baseLocations.end()) {
				base = (ConnectorBase*)(i->second);
				conn = base->getConnector(suffix);
			}
		}

		if (conn == NULL) {
			cout << "ConnectorBase::getConnector can't find connector '";
			cout << connectorId << "'";
			cout << " using prefix = '" << prefix << "'";
			cout << " and suffix = '" << suffix << "'";
			cout << " baseAli has = '" << baseAliases.size() << "' aliases";
			cout << " baseLoc has = '" << baseLocations.size() << "' locations";
			cout << endl;
		}
		return conn;
	}

	bool ConnectorBase::removeConnector(string connectorId) {
		Connector* connector;

		connector = getConnectorLocally(connectorId);
		return removeConnector(connector);
	}

	bool ConnectorBase::removeConnector(Connector* connector) {
		set<Connector*>::iterator i;

		i = connectorSet.find(connector);
		if (i != connectorSet.end()) {
			connectorSet.erase(i);
			return true;
		}
		return false;
	}
}
}
}
}
}
