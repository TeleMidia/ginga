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

#ifndef _CONTEXTNODE_H_
#define _CONTEXTNODE_H_

#include "../descriptor/GenericDescriptor.h"
using namespace ::br::pucrio::telemidia::ncl::descriptor;

#include "../link/Link.h"
#include "../link/LinkComposition.h"
using namespace ::br::pucrio::telemidia::ncl::link;

#include "NodeEntity.h"
#include "IllegalNodeTypeException.h"
#include "CompositeNode.h"

#include <map>
#include <vector>
#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace components {
	class ContextNode : public CompositeNode, public LinkComposition {
		protected:
			map<string, GenericDescriptor*> descriptorCollection;

		private:
			set<Link*> linkSet;

		public:
			ContextNode(string id);
			virtual ~ContextNode();

			bool instanceOf(string className) {
				return Entity::instanceOf(className);
			};

			bool addLink(Link* link);
			bool addNode(Node* node)
				    throw(IllegalNodeTypeException*);

			void clearLinks();
			bool containsLink(Link* link);
			virtual set<Link*>* getLinks();
			Link* getLink(string linkId);
			GenericDescriptor* getNodeDescriptor(Node *node);
			int getNumLinks();
			bool removeLink(string linkId);
			bool removeLink(Link* link);
			bool setNodeDescriptor(
				    string nodeId,
				    GenericDescriptor* descriptor);
	};
}
}
}
}
}

#endif //_CONTEXTNODE_H_
