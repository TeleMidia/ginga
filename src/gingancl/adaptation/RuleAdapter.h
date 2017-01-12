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

#ifndef _RULEADAPTER_H_
#define _RULEADAPTER_H_

#include "gingancl/model/CascadingDescriptor.h"
#include "gingancl/model/CompositeExecutionObject.h"
#include "gingancl/model/ExecutionObject.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::components;

#include "gingancl/model/ExecutionObjectSwitch.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::switches;

#include "ncl/descriptor/GenericDescriptor.h"
using namespace ::br::pucrio::telemidia::ncl::descriptor;

#include "ncl/switches/DescriptorSwitch.h"
#include "ncl/switches/CompositeRule.h"
#include "ncl/switches/Rule.h"
#include "ncl/switches/SimpleRule.h"
#include "ncl/switches/SwitchNode.h"
using namespace ::br::pucrio::telemidia::ncl::switches;

#include "ncl/util/Comparator.h"
using namespace ::br::pucrio::telemidia::ncl::util;


#include "ncl/components/Node.h"
using namespace ::br::pucrio::telemidia::ncl::components;

#include "util/Observer.h"

#include "math.h"

#include "PresentationContext.h"

#include <map>
#include <vector>
#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace adaptation {
namespace context {
	class RuleAdapter : public Observer {
		private:
			PresentationContext* presContext;
			map<string, vector<Rule*>*>* ruleListenMap;
			map<Rule*, vector<ExecutionObjectSwitch*>*>* entityListenMap;
			map<Rule*, vector<DescriptorSwitch*>*>* descListenMap;

		public:
			RuleAdapter(PresentationContext* presContext);
			virtual ~RuleAdapter();

			void reset();

			PresentationContext* getPresentationContext();

			void adapt(CompositeExecutionObject* compositeObject, bool force);
			void initializeAttributeRuleRelation(
				    Rule* topRule,
				    Rule* rule);

			void initializeRuleObjectRelation(
				    ExecutionObjectSwitch* object);

			void adapt(
			    ExecutionObjectSwitch* objectAlternatives,
			    bool force);

			bool adaptDescriptor(ExecutionObject* executionObject);
			Node* adaptSwitch(SwitchNode* switchNode);
			bool evaluateRule(Rule* rule);

		private:
			bool evaluateCompositeRule(CompositeRule* rule);
			bool evaluateSimpleRule(SimpleRule* rule);

		public:
			virtual void update(void* arg0, void* arg1);
	};
}
}
}
}
}
}
}

#endif //_RULEADAPTER_H_
