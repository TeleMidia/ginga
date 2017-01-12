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

#ifndef _FORMATTERSCHEDULER_H_
#define _FORMATTERSCHEDULER_H_

extern "C" {
#include <assert.h>
}

#include "ctxmgmt/IContextListener.h"
using namespace ::br::pucrio::telemidia::ginga::core::contextmanager;

#include "model/ExecutionObject.h"
#include "model/NodeNesting.h"
#include "model/CompositeExecutionObject.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::components;

#include "model/ExecutionObjectSwitch.h"
#include "model/SwitchEvent.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::switches;

#include "model/AttributionEvent.h"
#include "model/IEventListener.h"
#include "model/FormatterEvent.h"
#include "model/PresentationEvent.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::event;

#include "model/LinkAssignmentAction.h"
#include "model/LinkSimpleAction.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::link;

#include "model/FormatterLayout.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::presentation;

#include "adaptation/RuleAdapter.h"
#include "adaptation/IPresentationContext.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adaptation::context;

#include "adapters/ApplicationPlayerAdapter.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adapters::application;

#include "adapters/FormatterPlayerAdapter.h"
#include "adapters/PlayerAdapterManager.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adapters;

#include "ncl/connectors/SimpleAction.h"
#include "ncl/connectors/EventUtil.h"
using namespace ::br::pucrio::telemidia::ncl::connectors;

#include "ncl/components/CompositeNode.h"
#include "ncl/components/ContentNode.h"
#include "ncl/components/Node.h"
#include "ncl/components/NodeEntity.h"
using namespace ::br::pucrio::telemidia::ncl::components;

#include "ncl/interfaces/Port.h"
#include "ncl/interfaces/Anchor.h"
#include "ncl/interfaces/ContentAnchor.h"
#include "ncl/interfaces/PropertyAnchor.h"
#include "ncl/interfaces/SwitchPort.h"
using namespace ::br::pucrio::telemidia::ncl::interfaces;

#include "ncl/reuse/ReferNode.h"
using namespace ::br::pucrio::telemidia::ncl::reuse;

#include "FormatterFocusManager.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::focus;

#include "IFormatterMultiDevice.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::multidevice;

#include "IFormatterSchedulerListener.h"
#include "ObjectCreationForbiddenException.h"

#include "AnimationController.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::animation;

#include <iostream>
#include <string>
#include <map>
#include <vector>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
	class FormatterScheduler :
			public ILinkActionListener,
			public IFormatterScheduler,
			public IEventListener,
			public IContextListener {

		private:
			RuleAdapter* ruleAdapter;
			PlayerAdapterManager* playerManager;
			IPresentationContext* presContext;
			IFormatterMultiDevice* multiDevPres;
			FormatterFocusManager* focusManager;

			void* compiler; //FormatterConverter*
			vector<IFormatterSchedulerListener*> schedulerListeners;
			vector<FormatterEvent*> documentEvents;
			map<FormatterEvent*, bool> documentStatus;
			set<void*> actions;

			bool running;

			set<string> typeSet;
			pthread_mutex_t mutexD;
			pthread_mutex_t mutexActions;

			set<FormatterEvent*> listening;
			pthread_mutex_t lMutex;

		public:
			FormatterScheduler(
				    PlayerAdapterManager* playerManager,
				    RuleAdapter* ruleAdapter,
				    IFormatterMultiDevice* multiDevice,
				    void* compiler); //FormatterConverter

			virtual ~FormatterScheduler();

			void addAction(void* action);
			void removeAction(void* action);

			bool setKeyHandler(bool isHandler);
			//void setStandByState(bool standBy);
			FormatterFocusManager* getFocusManager();
			void* getFormatterLayout(void* descriptor, void* object);

		private:
			bool isDocumentRunning(FormatterEvent* event);

			void setTimeBaseObject(
				    ExecutionObject* object,
				    FormatterPlayerAdapter* objectPlayer, string nodeId);

			static void printAction(
					string action,
					LinkCondition* condition,
					LinkSimpleAction* linkAction);

		public:
			void scheduleAction(void* condition, void* action);

		private:
			void runAction(LinkCondition* condition, LinkSimpleAction* action);

			void runAction(
					FormatterEvent* event,
					LinkCondition* condition,
					LinkSimpleAction* action);

			void runActionOverProperty(
					FormatterEvent* event,
					LinkSimpleAction* action);

			void runActionOverApplicationObject(
					ApplicationExecutionObject* executionObject,
					FormatterEvent* event,
					FormatterPlayerAdapter* player,
					LinkSimpleAction* action);

			void runActionOverComposition(
				    CompositeExecutionObject* compositeObject,
				    LinkSimpleAction* action);

			void runActionOverSwitch(
				    ExecutionObjectSwitch* switchObject,
				    SwitchEvent* event,
				    LinkSimpleAction* action);

			void runSwitchEvent(
				    ExecutionObjectSwitch* switchObject,
				    SwitchEvent* switchEvent,
				    ExecutionObject* selectedObject,
				    LinkSimpleAction* action);

			string solveImplicitRefAssessment(
					string propValue, AttributionEvent* event);

		public:
			void startEvent(FormatterEvent* event);
			void stopEvent(FormatterEvent* event);
			void pauseEvent(FormatterEvent* event);
			void resumeEvent(FormatterEvent* event);

		private:
			void initializeDefaultSettings();
			void initializeDocumentSettings(Node* node);

		public:
			void startDocument(
				    FormatterEvent* documentEvent,
				    vector<FormatterEvent*>* entryEvents);

		private:
			void removeDocument(FormatterEvent* documentEvent);

		public:
			void stopDocument(FormatterEvent* documentEvent);
			void pauseDocument(FormatterEvent* documentEvent);
			void resumeDocument(FormatterEvent* documentEvent);
			void stopAllDocuments();
			void pauseAllDocuments();
			void resumeAllDocuments();
			void eventStateChanged(
				    void* someEvent, short transition, short previousState);

			short getPriorityType();
			void addSchedulerListener(IFormatterSchedulerListener* listener);
			void removeSchedulerListener(IFormatterSchedulerListener* listener);
			void receiveGlobalAttribution(string propertyName, string value);
	};
}
}
}
}
}

#endif //_FORMATTERSCHEDULER_H_
