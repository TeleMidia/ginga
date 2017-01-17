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

#ifndef _CONNECTOR_H_
#define _CONNECTOR_H_

#include "../Entity.h"
#include "../Parameter.h"
using namespace ::br::pucrio::telemidia::ncl;

#include "Role.h"

#include <map>
#include <vector>
#include <string>
using namespace std;

BR_PUCRIO_TELEMIDIA_NCL_CONNECTORS_BEGIN

	class Connector : public Entity {
		private:
			map<string, Parameter*>* parameters;

		public:
			Connector(string id);
			virtual ~Connector();
			int getNumRoles();
			Role* getRole(string roleId);
			virtual vector<Role*>* getRoles()=0;
			void addParameter(Parameter* parameter);
			vector<Parameter*>* getParameters();
			Parameter* getParameter(string name);
			bool removeParameter(string name);
	};

BR_PUCRIO_TELEMIDIA_NCL_CONNECTORS_END
#endif //_CONNECTOR_H_
