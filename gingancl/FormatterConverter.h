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

#ifndef FORMATTERCONVERTER_H_
#define FORMATTERCONVERTER_H_

#include "ncl/switches/SwitchNode.h"
using namespace ::br::pucrio::telemidia::ncl::switches;

#include "ncl/interfaces/PropertyAnchor.h"
#include "ncl/interfaces/ContentAnchor.h"
#include "ncl/interfaces/LambdaAnchor.h"
#include "ncl/interfaces/SwitchPort.h"
#include "ncl/interfaces/Port.h"
#include "ncl/interfaces/InterfacePoint.h"
using namespace ::br::pucrio::telemidia::ncl::interfaces;

#include "ncl/components/CompositeNode.h"
#include "ncl/components/ContextNode.h"
#include "ncl/components/ContentNode.h"
#include "ncl/components/Node.h"
#include "ncl/components/NodeEntity.h"
using namespace ::br::pucrio::telemidia::ncl::components;

#include "ncl/connectors/EventUtil.h"
using namespace ::br::pucrio::telemidia::ncl::connectors;

#include "ncl/descriptor/GenericDescriptor.h"
using namespace ::br::pucrio::telemidia::ncl::descriptor;

#include "ncl/link/Bind.h"
#include "ncl/link/CausalLink.h"
#include "ncl/link/Link.h"
using namespace ::br::pucrio::telemidia::ncl::link;

#include "ncl/reuse/ReferNode.h"
using namespace ::br::pucrio::telemidia::ncl::reuse;

#include "model/ExecutionObjectSwitch.h"
#include "model/SwitchEvent.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::switches;

#include "model/AttributionEvent.h"
#include "model/PresentationEvent.h"
#include "model/SelectionEvent.h"
#include "model/IEventListener.h"
#include "model/FormatterEvent.h"
#include "model/PresentationEvent.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::event;

#include "model/FormatterCausalLink.h"
#include "model/FormatterLink.h"
#include "model/LinkAction.h"
#include "model/LinkCompoundAction.h"
#include "model/LinkSimpleAction.h"
#include "model/ILinkActionListener.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::link;

#include "model/CascadingDescriptor.h"
#include "model/FormatterLayout.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::presentation;

#include "model/CompositeExecutionObject.h"
#include "model/ApplicationExecutionObject.h"
#include "model/ExecutionObject.h"
#include "model/NodeNesting.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::components;

#include "adaptation/RuleAdapter.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adaptation::context;

#include "adapters/PlayerAdapterManager.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adapters;

#include "IFormatterScheduler.h"
using namespace ::br::pucrio::telemidia::ginga::ncl;

#include "ObjectCreationForbiddenException.h"

#include <map>
#include <vector>
#include <set>
#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace emconverter {
  class FormatterConverter : public IEventListener {
	private:
		int depthLevel;
		static int dummyCount;
		map<string, ExecutionObject*> executionObjects;
		set<FormatterEvent*> listening;
		set<ExecutionObject*> settingObjects;
		void* linkCompiler; //FormatterLinkConverter*
		IFormatterScheduler* scheduler;
		ILinkActionListener* actionListener;
		RuleAdapter* ruleAdapter;
		pthread_mutex_t objectsMutex;
		pthread_mutex_t lMutex;
		bool handling;

	public:
		FormatterConverter(RuleAdapter* ruleAdapter);
		virtual ~FormatterConverter();

		void executionObjectReleased(string objectId);
		set<ExecutionObject*>* getRunningObjects();
		void setHandlingStatus(bool hanling);
		ExecutionObject* getObjectFromNodeId(string id);

		void setScheduler(IFormatterScheduler* scheduler);
		void setLinkActionListener(ILinkActionListener* actionListener);

		void setDepthLevel(int level);
		int getDepthLevel();
		void checkGradSameInstance(
				set<ReferNode*>* gradSame, ExecutionObject* object);

		CompositeExecutionObject* addSameInstance(
			    ExecutionObject* executionObject,
			    ReferNode* referNode);

	private:
		void addExecutionObject(
			    ExecutionObject* executionObject,
			    CompositeExecutionObject* parentObject,
			    int depthLevel);

	public:
		void compileExecutionObjectLinks(
			    ExecutionObject* executionObject, int depthLevel);

		ExecutionObject* getExecutionObjectFromPerspective(
			    NodeNesting* perspective,
			    GenericDescriptor* descriptor,
			    int depthLevel) throw(ObjectCreationForbiddenException*);

	private:
		void checkMirror(ExecutionObject* object, int depthLevel);

	public:
		set<ExecutionObject*>* getSettingNodeObjects();

	private:
		CompositeExecutionObject* getParentExecutionObject(
			    NodeNesting* perspective,
			    int depthLevel) throw(ObjectCreationForbiddenException*);

	public:
		FormatterEvent* getEvent(
			    ExecutionObject* executionObject,
			    InterfacePoint* interfacePoint,
			    int ncmEventType,
			    string key);

	private:
		void createMultichannelObject(
		    CompositeExecutionObject* compositeObject, int depthLevel);

		ExecutionObject* createExecutionObject(
			    string id,
			    NodeNesting* perspective,
			    CascadingDescriptor* descriptor,
			    int depthLevel);

		static bool hasDescriptorPropName(string name);

		static Descriptor* createDummyDescriptor(Node* node);
		static CascadingDescriptor* createDummyCascadingDescriptor(
				Node* node);

		static CascadingDescriptor* checkCascadingDescriptor(Node* node);
		static CascadingDescriptor* checkContextCascadingDescriptor(
				NodeNesting* nodePerspective,
				CascadingDescriptor* cascadingDescriptor,
				Node* ncmNode);

	public:
		static CascadingDescriptor* getCascadingDescriptor(
			    NodeNesting* nodePerspective,
			    GenericDescriptor* descriptor);

	private:
		void processLink(
				Link* ncmLink,
				Node* dataObject,
				ExecutionObject* executionObject,
			    CompositeExecutionObject* parentObject);

	public:
		void compileExecutionObjectLinks(
			    ExecutionObject* executionObject,
			    Node* dataObject,
			    CompositeExecutionObject* parentObject,
			    int depthLevel);

	private:
		void setActionListener(LinkAction* action);

	public:
		ExecutionObject* processExecutionObjectSwitch(
			    ExecutionObjectSwitch* switchObject);

	private:
		void resolveSwitchEvents(
			    ExecutionObjectSwitch* switchObject, int depthLevel);

		FormatterEvent* insertNode(
			    NodeNesting* perspective,
			    InterfacePoint* interfacePoint,
			    GenericDescriptor* descriptor);

	public:
		FormatterEvent* insertContext(
			    NodeNesting* contextPerspective, Port* port);

		bool removeExecutionObject(
			    ExecutionObject* executionObject, ReferNode* referNode);

		bool removeExecutionObject(ExecutionObject* executionObject);

	private:
		bool ntsRemoveExecutionObject(ExecutionObject* executionObject);

	public:
		ExecutionObject* hasExecutionObject(
			    Node* node, GenericDescriptor* descriptor);

		FormatterCausalLink* addCausalLink(
			    ContextNode* context, CausalLink* link);

		void eventStateChanged(
			    void* someEvent, short transition, short previousState);

		short getPriorityType();
		void reset();
  };
}
}
}
}
}
}

#endif /*FORMATTERCONVERTER_H_*/
