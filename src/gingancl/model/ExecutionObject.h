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

#ifndef _EXECUTIONOBJECT_H_
#define _EXECUTIONOBJECT_H_

#include "util/functions.h"
using namespace ::ginga::util;

#include "system/Thread.h"
using namespace ::ginga::system;

#include "mb/CodeMap.h"
using namespace ::ginga::mb;

#include "player/IPlayer.h"
using namespace ::ginga::player;

#include "ncl/Animation.h"
using namespace ::ginga::ncl;

#include "ncl/GenericDescriptor.h"
using namespace ::ginga::ncl;

#include "ncl/ReferenceContent.h"
using namespace ::ginga::ncl;

#include "ncl/LabeledAnchor.h"
#include "ncl/LambdaAnchor.h"
#include "ncl/IntervalAnchor.h"
#include "ncl/PropertyAnchor.h"
using namespace ::ginga::ncl;

#include "ncl/LayoutRegion.h"
using namespace ::ginga::ncl;

#include "ncl/EventUtil.h"
using namespace ::ginga::ncl;

#include "FormatterEvent.h"
#include "PresentationEvent.h"
#include "SelectionEvent.h"
#include "AttributionEvent.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::event;

#include "BeginEventTransition.h"
#include "EndEventTransition.h"
#include "EventTransition.h"
#include "EventTransitionManager.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::event::transition;

#include "CascadingDescriptor.h"
#include "FormatterRegion.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::presentation;

#include "LinkSimpleAction.h"
#include "ILinkActionListener.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::link;

#include "NodeNesting.h"


BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_COMPONENTS_BEGIN

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
		ExecutionObject* mirrorSrc;

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

		ExecutionObject* getMirrorSrc();
		void setMirrorSrc(ExecutionObject* mirrorSrc);

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

BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_COMPONENTS_END
#endif //_EXECUTIONOBJECT_H_
