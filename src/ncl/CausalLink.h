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

#include "Node.h"
using namespace ::ginga::ncl;

#include "GenericDescriptor.h"
using namespace ::ginga::ncl;

#include "Connector.h"
#include "AttributeAssessment.h"
#include "SimpleAction.h"
#include "SimpleCondition.h"
using namespace ::ginga::ncl;

#include "Bind.h"
#include "Link.h"


GINGA_NCL_BEGIN

	class CausalLink : public Link {
		public:
			CausalLink(string uid, Connector *connector);
			virtual ~CausalLink(){};
			bool containsSourceNode(Node *node,
				     GenericDescriptor *descriptor);

			vector<Bind*> *getActionBinds();
			vector<Bind*> *getConditionBinds();
	};

GINGA_NCL_END

#endif //_CAUSALLINK_H_
