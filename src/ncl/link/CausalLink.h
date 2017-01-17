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

#ifndef _CAUSALLINK_H_
#define _CAUSALLINK_H_

#include "../components/Node.h"
using namespace ::br::pucrio::telemidia::ncl::components;

#include "../descriptor/GenericDescriptor.h"
using namespace ::br::pucrio::telemidia::ncl::descriptor;

#include "../connectors/Connector.h"
#include "../connectors/AttributeAssessment.h"
#include "../connectors/SimpleAction.h"
#include "../connectors/SimpleCondition.h"
using namespace ::br::pucrio::telemidia::ncl::connectors;

#include "Bind.h"
#include "Link.h"

#include <string>
#include <vector>
using namespace std;

BR_PUCRIO_TELEMIDIA_NCL_LINK_BEGIN

	class CausalLink : public Link {
		public:
			CausalLink(string uid, Connector *connector);
			virtual ~CausalLink(){};
			bool containsSourceNode(Node *node,
				     GenericDescriptor *descriptor);

			vector<Bind*> *getActionBinds();
			vector<Bind*> *getConditionBinds();
	};

BR_PUCRIO_TELEMIDIA_NCL_LINK_END
#endif //_CAUSALLINK_H_
