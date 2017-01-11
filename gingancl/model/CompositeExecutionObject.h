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

#ifndef _COMPOSITEEXECUTIONOBJECT_H_
#define _COMPOSITEEXECUTIONOBJECT_H_

extern "C" {
#include <assert.h>
}

#include "ncl/components/Node.h"
#include "ncl/components/ContextNode.h"
using namespace ::br::pucrio::telemidia::ncl::components;

#include "ncl/connectors/EventUtil.h"
using namespace ::br::pucrio::telemidia::ncl::connectors;

#include "ncl/link/Link.h"
using namespace ::br::pucrio::telemidia::ncl::link;

#include "IEventListener.h"
#include "FormatterEvent.h"
#include "PresentationEvent.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::event;

#include "FormatterCausalLink.h"
#include "FormatterLink.h"
#include "LinkListener.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::link;

#include "NodeNesting.h"
#include "CascadingDescriptor.h"
#include "ExecutionObject.h"

#include <vector>
#include <map>
#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace components {
	class CompositeExecutionObject :
			public ExecutionObject, public LinkListener, public IEventListener,
			public Thread {

		private:
			static const short mSleepTime = 800;
			set<FormatterLink*> links;
			set<Link*> uncompiledLinks;

			set<FormatterEvent*> runningEvents; // child events occurring
			set<FormatterEvent*> pausedEvents;  // child events paused
			short lastTransition;

			map<FormatterLink*, int> pendingLinks;
			bool running;

			map<string, ExecutionObject*> execObjList;

			pthread_mutex_t compositeMutex;
			pthread_mutex_t parentMutex;
			pthread_mutex_t stlMutex;

		public:
			CompositeExecutionObject(
					string id,
					Node *dataObject,
					bool handling,
					ILinkActionListener* seListener);

			CompositeExecutionObject(
					string id,
					Node *dataObject,
					CascadingDescriptor *descriptor,
					bool handling,
					ILinkActionListener* seListener);

			virtual ~CompositeExecutionObject();

		protected:
			void initializeCompositeExecutionObject(
					string id,
					Node *dataObject,
					CascadingDescriptor *descriptor);

		public:
			CompositeExecutionObject* getParentFromDataObject(Node* dataObject);
			void suspendLinkEvaluation(bool suspend);
			bool addExecutionObject(ExecutionObject *execObj);
			bool containsExecutionObject(string execObjId);
			ExecutionObject* getExecutionObject(string execObjId);
			map<string, ExecutionObject*>* getExecutionObjects();
			map<string, ExecutionObject*>* recursivellyGetExecutionObjects();
			int getNumExecutionObjects();
			bool removeExecutionObject(ExecutionObject* execObj);
			set<Link*>* getUncompiledLinks();
			bool containsUncompiledLink(Link* dataLink);
			void removeLinkUncompiled(Link* ncmLink);
			void setLinkCompiled(FormatterLink *formatterLink);
			void addNcmLink(Link *ncmLink);
			void removeNcmLink(Link *ncmLink);
			void setAllLinksAsUncompiled(bool isRecursive);
			void setParentsAsListeners();
			void unsetParentsAsListeners();
			void eventStateChanged(
					void* event, short transition, short previousState);

			short getPriorityType();

			void linkEvaluationStarted(FormatterCausalLink *link);
			void linkEvaluationFinished(FormatterCausalLink *link, bool start);

			bool setPropertyValue(AttributionEvent* event, string value);

		private:
			void checkLinkConditions();
			void run();
			void listRunningObjects();
			void listPendingLinks();

			void lockComposite();
			void unlockComposite();

			void lockSTL();
			void unlockSTL();
	};
}
}
}
}
}
}
}

#endif //_COMPOSITEEXECUTIONOBJECT_H_
