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

#ifndef CONNECTORBASE_H_
#define CONNECTORBASE_H_

#include "Base.h"
using namespace ::ginga::ncl;

#include "Connector.h"

#include <set>
#include <string>
using namespace std;

GINGA_NCL_BEGIN

	class ConnectorBase : public Base {
		private:
			set<Connector*> connectorSet;

		public:
			ConnectorBase(string id);
			virtual ~ConnectorBase();

			bool addConnector(Connector* connector);
			bool addBase(
				    Base* base,
				    string alias,
				    string location);

			void clear();
			bool containsConnector(string connectorId);
			bool containsConnector(Connector* connector);

		private:
			Connector* getConnectorLocally(string connectorId);

		public:
			Connector* getConnector(string connectorId);
			bool removeConnector(string connectorId);
			bool removeConnector(Connector* connector);
	};

GINGA_NCL_END
#endif /*CONNECTORBASE_H_*/
