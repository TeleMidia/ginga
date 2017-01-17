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

#ifndef _SWITCHNODE_H_
#define _SWITCHNODE_H_

#include "util/functions.h"
using namespace ::ginga::util;

#include "../components/ContextNode.h"
#include "../components/CompositeNode.h"
#include "../components/NodeEntity.h"
using namespace ::br::pucrio::telemidia::ncl::components;

#include "../interfaces/Port.h"
#include "../interfaces/InterfacePoint.h"
#include "../interfaces/SwitchPort.h"
using namespace ::br::pucrio::telemidia::ncl::interfaces;

#include "Rule.h"
#include "DescriptorSwitch.h"
#include "SwitchContent.h"

#include <vector>
#include <string>
using namespace std;

BR_PUCRIO_TELEMIDIA_NCL_SWITCHES_BEGIN

	class SwitchNode : public CompositeNode {
		private:
			Node* defaultNode;
			vector<Rule*> ruleList;

		public:
			SwitchNode(string id);
			virtual ~SwitchNode();
			bool addNode(unsigned int index, Node* node, Rule* rule);
			bool addNode(Node* node, Rule* rule);

			//virtual from CompositeNode
			bool addNode(Node* node);

			bool addSwitchPortMap(SwitchPort* switchPort,
				    Node* node, InterfacePoint* interfacePoint);

			//virtual from CompositeNode
			bool addPort(Port* port);

			//virtual from CompositeNode
			bool addPort(int index, Port* port);

			void exchangeNodesAndRules(
				    unsigned int index1, unsigned int index2);

			Node* getDefaultNode();

			//virtual from CompositeNode
			InterfacePoint *getMapInterface(Port *port);

			//virtual from CompositeNode
			Node* getNode(string nodeId);

			Node* getNode(unsigned int index);
			Node* getNode(Rule* rule);
			unsigned int getNumRules();
			Rule* getRule(unsigned int index);
			unsigned int indexOfRule(Rule* rule);

			//virtual from CompositeNode
			bool recursivelyContainsNode(Node* node);

			//virtual from CompositeNode
			bool recursivelyContainsNode(string nodeId);

			//virtual from CompositeNode
			Node* recursivelyGetNode(string nodeId);

			//virtual from CompositeNode
			bool removeNode(Node* node);

			bool removeNode(unsigned int index);
			bool removeRule(Rule* rule);
			void setDefaultNode(Node* node);
	};

BR_PUCRIO_TELEMIDIA_NCL_SWITCHES_END
#endif //_SWITCHNODE_H_
