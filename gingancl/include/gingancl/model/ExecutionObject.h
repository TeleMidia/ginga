/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware
Ginga (Ginga-NCL).

Direitos Autorais Reservados (c) 1989-2007 PUC-Rio/Laboratorio TeleMidia

Este programa eh software livre; voce pode redistribui-lo e/ou modificah-lo sob
os termos da Licenca Publica Geral GNU versao 2 conforme publicada pela Free
Software Foundation.

Este programa eh distribuido na expectativa de que seja util, porem, SEM
NENHUMA GARANTIA; nem mesmo a garantia implicita de COMERCIABILIDADE OU
ADEQUACAO A UMA FINALIDADE ESPECIFICA. Consulte a Licenca Publica Geral do
GNU versao 2 para mais detalhes.

Voce deve ter recebido uma copia da Licenca Publica Geral do GNU versao 2 junto
com este programa; se nao, escreva para a Free Software Foundation, Inc., no
endereco 59 Temple Street, Suite 330, Boston, MA 02111-1307 USA.

Para maiores informacoes:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
******************************************************************************
This file is part of the declarative environment of middleware Ginga (Ginga-NCL)

Copyright: 1989-2007 PUC-RIO/LABORATORIO TELEMIDIA, All Rights Reserved.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License version 2 as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License version 2 for more
details.

You should have received a copy of the GNU General Public License version 2
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

For further information contact:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
*******************************************************************************/

#ifndef _EXECUTIONOBJECT_H_
#define _EXECUTIONOBJECT_H_

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "system/thread/Thread.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::thread;

#include "mb/interface/CodeMap.h"
using namespace ::br::pucrio::telemidia::ginga::core::mb;

#include "player/IPlayer.h"
using namespace ::br::pucrio::telemidia::ginga::core::player;

#include "ncl/animation/Animation.h"
using namespace ::br::pucrio::telemidia::ncl::animation;

#include "ncl/descriptor/GenericDescriptor.h"
using namespace ::br::pucrio::telemidia::ncl::descriptor;

#include "ncl/components/ReferenceContent.h"
using namespace ::br::pucrio::telemidia::ncl::components;

#include "ncl/interfaces/LabeledAnchor.h"
#include "ncl/interfaces/LambdaAnchor.h"
#include "ncl/interfaces/IntervalAnchor.h"
#include "ncl/interfaces/PropertyAnchor.h"
using namespace ::br::pucrio::telemidia::ncl::interfaces;

#include "ncl/layout/LayoutRegion.h"
using namespace ::br::pucrio::telemidia::ncl::layout;

#include "ncl/connectors/EventUtil.h"
using namespace ::br::pucrio::telemidia::ncl::connectors;

#include "FormatterEvent.h"
#include "PresentationEvent.h"
#include "SelectionEvent.h"
#include "AttributionEvent.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::event;

#include "BeginEventTransition.h"
#include "EndEventTransition.h"
#include "EventTransition.h"
#include "EventTransitionManager.h"
#include "TransitionDispatcher.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::event::transition;

#include "CascadingDescriptor.h"
#include "FormatterRegion.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::presentation;

#include "LinkSimpleAction.h"
#include "ILinkActionListener.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::link;

#include "NodeNesting.h"

#include <pthread.h>
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <set>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace components {
  class ExecutionObject {
	protected:
		string id;
		Node* dataObject;
		CascadingDescriptor* descriptor;
		double offsetTime;
		double startTime;
		PresentationEvent* wholeContent;

		set<string> typeSet;
		pthread_mutex_t mutex;
		pthread_mutex_t mutexEvent;
		pthread_mutex_t mutexParentTable;

		ILinkActionListener* seListener;

		bool isLocked;
		bool deleting;
		bool isHandler;
		bool isHandling;

	private:
		map<Node*, Node*> nodeParentTable;

	protected:
		map<Node*, void*> parentTable; //CompositionExecutionObject
		bool visible;

	private:
		bool isItCompiled;

	protected:
		map<string, FormatterEvent*> events;
		vector<PresentationEvent*> presEvents;
		set<SelectionEvent*> selectionEvents;
		vector<FormatterEvent*> otherEvents;
		int pauseCount;
		FormatterEvent* mainEvent;
		EventTransitionManager* transMan;

	private:
		static set<ExecutionObject*> objects;
		static bool initMutex;
		static pthread_mutex_t _objMutex;

	protected:
		static void addInstance(ExecutionObject* object);
		static bool removeInstance(ExecutionObject* object);

	public:
		ExecutionObject(
				string id,
				Node* node,
				bool handling,
				ILinkActionListener* seListener);

		ExecutionObject(
				string id,
				Node* node,
				GenericDescriptor* descriptor,
				bool handling,
				ILinkActionListener* seListener);

		ExecutionObject(
				string id,
				Node* node,
				CascadingDescriptor* descriptor,
				bool handling,
				ILinkActionListener* seListener);

		virtual ~ExecutionObject();

		static bool hasInstance(ExecutionObject* object, bool eraseFromList);

	private:
		void initializeExecutionObject(
			    string id,
			    Node* node,
			    CascadingDescriptor* descriptor,
			    bool handling,
			    ILinkActionListener* seListener);

	protected:
		void destroyEvents();
		virtual void unsetParentsAsListeners();
		virtual void removeParentListenersFromEvent(FormatterEvent* event);

	public:
		virtual bool isSleeping();
		virtual bool isPaused();
		bool instanceOf(string s);
		int compareToUsingId(ExecutionObject* object);
		Node* getDataObject();
		CascadingDescriptor* getDescriptor();
		string getId();
		void* getParentObject(); //CompositeExecutionObject
		void* getParentObject(Node* node); //CompositeExecutionObject
		void addParentObject(void* parentObject, Node* parentNode);
		void addParentObject(Node* node, void* parentObject, Node* parentNode);
		virtual void removeParentObject(Node* parentNode, void* parentObject);

		void setDescriptor(CascadingDescriptor* cascadingDescriptor);
		void setDescriptor(GenericDescriptor* descriptor);
		string toString();
		virtual bool addEvent(FormatterEvent* event);
		void addPresentationEvent(PresentationEvent* event);
		int compareTo(ExecutionObject* object);
		int compareToUsingStartTime(ExecutionObject* object);
		bool containsEvent(FormatterEvent* event);
		FormatterEvent* getEventFromAnchorId(string anchorId);

	public:
		FormatterEvent* getEvent(string id);
		vector<FormatterEvent*>* getEvents();
		bool hasSampleEvents();
		set<AnchorEvent*>* getSampleEvents();
		double getExpectedStartTime();
		PresentationEvent* getWholeContentPresentationEvent();
		void setStartTime(double t);
		void updateEventDurations();
		void updateEventDuration(PresentationEvent* event);
		bool removeEvent(FormatterEvent* event);
		bool isCompiled();
		void setCompiled(bool status);
		void removeNode(Node* node);
		vector<Node*>* getNodes();
		vector<Anchor*>* getNCMAnchors();
		PropertyAnchor* getNCMProperty(string propertyName);
		NodeNesting* getNodePerspective();
		NodeNesting* getNodePerspective(Node* node);
		vector<ExecutionObject*>* getObjectPerspective();
		vector<ExecutionObject*>* getObjectPerspective(Node* node);
		vector<Node*>* getParentNodes();
		FormatterEvent* getMainEvent();
		virtual bool prepare(FormatterEvent* event, double offsetTime);
		virtual bool start();

		void timeBaseNaturalEnd(int64_t timeValue, short int transType);
		void updateTransitionTable(
				double value, IPlayer* player, short int transType);

		void resetTransitionEvents(short int transType);
		void prepareTransitionEvents(short int transType, double startTime);
		set<double>* getTransitionsValues(short int transType);
		virtual EventTransition* getNextTransition();
		virtual bool stop();
		virtual bool abort();
		virtual bool pause();
		virtual bool resume();
		virtual bool setPropertyValue(
				AttributionEvent* event, string value);

		virtual string getPropertyValue(string propName);

		virtual bool unprepare();

		void setHandling(bool isHandling);
		void setHandler(bool isHandler);
		bool selectionEvent(int keyCode, double currentTime);
		set<int>* getInputEvents();

	protected:
		bool lock();
		bool unlock();

	private:
		void lockEvents();
		void unlockEvents();

		void lockParentTable();
		void unlockParentTable();
  };
}
}
}
}
}
}
}

#endif //_EXECUTIONOBJECT_H_
