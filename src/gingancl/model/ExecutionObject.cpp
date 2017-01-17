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

#include "config.h"
#include "ExecutionObject.h"
#include "CompositeExecutionObject.h"
#include "ApplicationExecutionObject.h"

#include "SwitchEvent.h"
using namespace br::pucrio::telemidia::ginga::ncl::model::switches;

BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_COMPONENTS_BEGIN

	bool ExecutionObject::initMutex = false;
	set<ExecutionObject*> ExecutionObject::objects;
	pthread_mutex_t ExecutionObject::_objMutex;

	void ExecutionObject::addInstance(ExecutionObject* object) {
		Thread::mutexLock(&_objMutex);
		objects.insert(object);
		Thread::mutexUnlock(&_objMutex);
	}

	bool ExecutionObject::removeInstance(ExecutionObject* object) {
		set<ExecutionObject*>::iterator i;
		bool removed = false;

		Thread::mutexLock(&_objMutex);
		i = objects.find(object);
		if (i != objects.end()) {
			objects.erase(i);
			removed = true;
		}
		Thread::mutexUnlock(&_objMutex);

		return removed;
	}

	bool ExecutionObject::hasInstance(
			ExecutionObject* object, bool eraseFromList) {

		set<ExecutionObject*>::iterator i;
		bool hasObject;

		if (!initMutex) {
			return false;
		}

		Thread::mutexLock(&_objMutex);
		hasObject = false;
		i = objects.find(object);
		if (i != objects.end()) {
			if (eraseFromList) {
				objects.erase(i);
			}
			hasObject = true;
		}
		Thread::mutexUnlock(&_objMutex);

		return hasObject;
	}

	ExecutionObject::ExecutionObject(
			string id,
			Node* node,
			bool handling,
			ILinkActionListener* seListener) {

		initializeExecutionObject(id, node, NULL, handling, seListener);
	}

	ExecutionObject::ExecutionObject(
		    string id,
		    Node* node,
		    GenericDescriptor* descriptor,
		    bool handling,
		    ILinkActionListener* seListener) {

		initializeExecutionObject(
			    id,
			    node,
			    new CascadingDescriptor(descriptor),
			    handling,
			    seListener);
	}

	ExecutionObject::ExecutionObject(
		    string id,
		    Node* node,
		    CascadingDescriptor* descriptor,
		    bool handling,
		    ILinkActionListener* seListener) {

		initializeExecutionObject(id, node, descriptor, handling, seListener);
	}

	ExecutionObject::~ExecutionObject() {
		map<Node*, Node*>::iterator i;
		map<Node*, void*>::iterator j;

		Node* parentNode;
		CompositeExecutionObject* parentObject;

		clog << "ExecutionObject::~ExecutionObject(" << getId() << ")" << endl;

		removeInstance(this);
		unsetParentsAsListeners();
		deleting     = true;

		seListener   = NULL;
		dataObject   = NULL;
		wholeContent = NULL;
		mainEvent    = NULL;

		destroyEvents();

		lockParentTable();
		i = nodeParentTable.begin();
		while (i != nodeParentTable.end()) {
			parentNode = i->second;
			j = parentTable.find(parentNode);
			if (j != parentTable.end()) {
				parentObject = (CompositeExecutionObject*)(j->second);

				parentObject->removeExecutionObject(this);
			}
			++i;
		}

		nodeParentTable.clear();

		parentTable.clear();
		unlockParentTable();

		if (descriptor != NULL) {
			delete descriptor;
			descriptor = NULL;
		}
		unlock();
		Thread::mutexDestroy(&mutex);
		Thread::mutexDestroy(&mutexEvent);
		Thread::mutexDestroy(&mutexParentTable);
	}

	void ExecutionObject::initializeExecutionObject(
			    string id,
			    Node* node,
			    CascadingDescriptor* descriptor,
			    bool handling,
			    ILinkActionListener* seListener) {

		typeSet.insert("ExecutionObject");

		if (!initMutex) {
			initMutex = true;
			Thread::mutexInit(&ExecutionObject::_objMutex, false);
		}

		addInstance(this);

		this->seListener      = seListener;
		this->deleting        = false;
		this->id              = id;
		this->dataObject      = node;
		this->wholeContent    = NULL;
		this->startTime       = infinity();
		this->descriptor      = NULL;

		this->isItCompiled    = false;

		this->pauseCount      = 0;
		this->mainEvent       = NULL;
		this->descriptor      = descriptor;
		this->isLocked        = false;
		this->isHandler       = false;
		this->isHandling      = handling;
		this->mirrorSrc       = NULL;

		this->transMan        = new EventTransitionManager();

		Thread::mutexInit(&mutex, false);
		Thread::mutexInit(&mutexEvent, false);
		Thread::mutexInit(&mutexParentTable, false);
	}

	void ExecutionObject::destroyEvents() {
		map<string, FormatterEvent*>::iterator i;
		FormatterEvent* event;

		lockEvents();
		i = events.begin();
		while (i != events.end()) {
			event = i->second;
			events.erase(i);
			unlockEvents();
			if (FormatterEvent::hasInstance(event, true)) {
				delete event;
				event = NULL;
			}
			lockEvents();
			i = events.begin();
		}
		events.clear();

		if (transMan != NULL) {
			delete transMan;
			transMan = NULL;
		}

		presEvents.clear();
		selectionEvents.clear();
		otherEvents.clear();

		unlockEvents();
	}

	void ExecutionObject::unsetParentsAsListeners() {
		removeParentListenersFromEvent(mainEvent);
		removeParentListenersFromEvent(wholeContent);
	}

	void ExecutionObject::removeParentListenersFromEvent(
			FormatterEvent* event) {

		map<Node*, void*>::iterator i;
		CompositeExecutionObject* parentObject;

		if (FormatterEvent::hasInstance(event, false)) {
			lockParentTable();
			i = parentTable.begin();
			while (i != parentTable.end()) {
				parentObject = (CompositeExecutionObject*)(i->second);

				if (FormatterEvent::hasInstance(mainEvent, false)) {
					// register parent as a mainEvent listener
					mainEvent->removeEventListener(parentObject);

				} else {
					break;
				}
				++i;
			}
			unlockParentTable();
		}
	}

	bool ExecutionObject::isSleeping() {
		if (mainEvent == NULL ||
				mainEvent->getCurrentState() == EventUtil::ST_SLEEPING) {

			return true;
		}

		return false;
	}

	bool ExecutionObject::isPaused() {
		if (mainEvent != NULL &&
				mainEvent->getCurrentState() == EventUtil::ST_PAUSED) {

			return true;
		}

		return false;
	}

	bool ExecutionObject::instanceOf(string s) {
		return (typeSet.find(s) != typeSet.end());
	}

	int ExecutionObject::compareToUsingId(ExecutionObject* object) {
		return id.compare(object->getId());
	}

	Node* ExecutionObject::getDataObject() {
		return dataObject;
	}

	CascadingDescriptor* ExecutionObject::getDescriptor() {
		return descriptor;
	}

	string ExecutionObject::getId() {
		return id;
	}

	ExecutionObject* ExecutionObject::getMirrorSrc() {
		return mirrorSrc;
	}

	void ExecutionObject::setMirrorSrc(ExecutionObject* mirrorSrc) {
		this->mirrorSrc = mirrorSrc;
	}

	void* ExecutionObject::getParentObject() {
		return getParentObject(dataObject);
	}

	void* ExecutionObject::getParentObject(Node* node) {
		Node* parentNode;
		void* parentObj = NULL;

		map<Node*, Node*>::iterator i;
		map<Node*, void*>::iterator j;

		lockParentTable();
		i = nodeParentTable.find(node);
		if (i != nodeParentTable.end()) {
			parentNode = i->second;
			j = parentTable.find(parentNode);
			if (j != parentTable.end()) {
				parentObj = j->second;
			}
		}
		unlockParentTable();

		return parentObj;
	}

	void ExecutionObject::addParentObject(
		    void* parentObject, Node* parentNode) {

		addParentObject(dataObject, parentObject, parentNode);
	}

	void ExecutionObject::addParentObject(
		    Node* node,
		    void* parentObject,
		    Node* parentNode) {

		lockParentTable();
		nodeParentTable[node]   = parentNode;
		parentTable[parentNode] = parentObject;
		unlockParentTable();
	}

	void ExecutionObject::removeParentObject(
			Node* parentNode, void* parentObject) {

		map<Node*, void*>::iterator i;

		lockParentTable();
		i = parentTable.find(parentNode);
		if (i != parentTable.end() && i->second == parentObject) {
			if (wholeContent != NULL) {
				wholeContent->removeEventListener(
						(CompositeExecutionObject*)parentObject);
			}
			parentTable.erase(i);
		}
		unlockParentTable();
	}

	void ExecutionObject::setDescriptor(
		    CascadingDescriptor* cascadingDescriptor) {

		this->descriptor = cascadingDescriptor;
	}

	void ExecutionObject::setDescriptor(GenericDescriptor* descriptor) {
		CascadingDescriptor* cascade;
		cascade = new CascadingDescriptor(descriptor);

		if (this->descriptor != NULL) {
			delete this->descriptor;
		}

		this->descriptor = cascade;
	}

	string ExecutionObject::toString() {
		return id;
	}

	bool ExecutionObject::addEvent(FormatterEvent* event) {
		map<string, FormatterEvent*>::iterator i;

		lockEvents();
		i = events.find(event->getId());
		if (i != events.end()) {
			clog << "ExecutionObject::addEvent Warning! Trying to add ";
			clog << "the same event '" << event->getId() << "' twice";
			clog << " current event address '" << i->second << "' ";
			clog << " addEvent address '" << event << "'";
			clog << endl;

			unlockEvents();
			return false;
		}

		clog << "ExecutionObject::addEvent '" << event->getId() << "' in '";
		clog << getId() << "'" << endl;

		events[event->getId()] = event;
		unlockEvents();
		if (event->instanceOf("PresentationEvent")) {
			addPresentationEvent((PresentationEvent*)event);

		} else if (event->instanceOf("SelectionEvent")) {
			selectionEvents.insert(((SelectionEvent*)event));

		} else {
			otherEvents.push_back(event);
		}

		return true;
	}

	void ExecutionObject::addPresentationEvent(PresentationEvent* event) {
		PresentationEvent* auxEvent;
		double begin, auxBegin;
		int posBeg = -1;
		int posEnd, posMid;

		if ((event->getAnchor())->instanceOf("LambdaAnchor")) {
			presEvents.insert(presEvents.begin(), event);
			wholeContent = (PresentationEvent*)event;

		} else {
			begin = event->getBegin();

			// undefined events are not inserted into transition table
			if (PresentationEvent::isUndefinedInstant(begin)) {
				clog << "ExecutionObject::addPresentationEvent event '";
				clog << event->getId() << "' has an undefined begin instant '";
				clog << begin << "'" << endl;
				return;
			}

			posBeg = 0;
			posEnd = presEvents.size() - 1;
			while (posBeg <= posEnd) {
				posMid = (posBeg + posEnd) / 2;
				auxEvent = (PresentationEvent*)(presEvents[posMid]);
				auxBegin = auxEvent->getBegin();
				if (begin < auxBegin) {
					posEnd = posMid - 1;

				} else if (begin > auxBegin) {
					posBeg = posMid + 1;

				} else {
					posBeg = posMid + 1;
					break;
				}
			}

			presEvents.insert(
					(presEvents.begin() + posBeg), event);
		}

		clog << "ExecutionObject::addPresentationEvent in '";
		clog << getId() << "'. Call transition ";
		clog << "manager addPresentationEvent for '" << event->getId();
		clog << "' begin = '" << event->getBegin() << "'; end = '";
		clog << event->getEnd() << "' position = '" << posBeg << "'" << endl;
		transMan->addPresentationEvent(event);
	}

	int ExecutionObject::compareTo(ExecutionObject* object) {
		int ret;

		ret = compareToUsingStartTime(object);
		if (ret == 0)
			return compareToUsingId(object);
		else
			return ret;
	}

	int ExecutionObject::compareToUsingStartTime(ExecutionObject* object) {
		double thisTime, otherTime;

		thisTime  = startTime;
		otherTime = (object->getExpectedStartTime());

		if (thisTime < otherTime) {
			return -1;

		} else if (thisTime > otherTime) {
			return 1;
		}

		return 0;
	}

	bool ExecutionObject::containsEvent(FormatterEvent* event) {
		bool contains;

		lockEvents();
		contains = (events.count(event->getId()) != 0);
		unlockEvents();

		return contains;
	}

	FormatterEvent* ExecutionObject::getEventFromAnchorId(string anchorId) {
		map<string, FormatterEvent*>::iterator i;
		FormatterEvent* event;

		lockEvents();
		if (anchorId == "") {
			if (wholeContent != NULL) {
				unlockEvents();
				return wholeContent;
			}

		} else {
			if (wholeContent != NULL) {
				if (FormatterEvent::hasNcmId(wholeContent, anchorId)) {
					unlockEvents();
					return wholeContent;
				}
			}

			i = events.begin();
			clog << "ExecutionObject::getEventFromAnchorId searching '";
			clog << anchorId << "' for '" << id;
			clog << "' with following events = ";
			while (i != events.end()) {
				event = i->second;
				if (event != NULL) {
					if (FormatterEvent::hasNcmId(event, anchorId)) {
						unlockEvents();
						return event;
					}
				}
				++i;
			}
			clog << endl;
		}

		unlockEvents();
		return NULL;
	}

	FormatterEvent* ExecutionObject::getEvent(string id) {
		FormatterEvent* ev;
		lockEvents();
		if (events.count(id) != 0) {
			ev = events[id];
			unlockEvents();
			return ev;
		}

		unlockEvents();
		return NULL;
	}

	vector<FormatterEvent*>* ExecutionObject::getEvents() {
		vector<FormatterEvent*>* eventsVector = NULL;
		map<string, FormatterEvent*>::iterator i;

		lockEvents();
		if (events.empty()) {
			unlockEvents();
			return NULL;
		}

		eventsVector = new vector<FormatterEvent*>;
		for (i = events.begin(); i != events.end(); ++i) {
			eventsVector->push_back(i->second);
		}
		unlockEvents();

		return eventsVector;
	}

	bool ExecutionObject::hasSampleEvents() {
		map<string, FormatterEvent*>::iterator i;
		FormatterEvent* event;
		ContentAnchor* anchor;

		lockEvents();
		if (events.empty()) {
			unlockEvents();
			return false;
		}

		i = events.begin();
		while (i != events.end()) {
			event = i->second;
			if (event->instanceOf("AnchorEvent")) {
				anchor = ((AnchorEvent*)event)->getAnchor();
				if (anchor->instanceOf("SampleIntervalAnchor")) {
					unlockEvents();
					return true;
				}
			}
			++i;
		}
		unlockEvents();

		return false;
	}

	set<AnchorEvent*>* ExecutionObject::getSampleEvents() {
		set<AnchorEvent*>* eventsSet;
		map<string, FormatterEvent*>::iterator i;
		FormatterEvent* event;
		ContentAnchor* anchor;

		lockEvents();
		if (events.empty()) {
			unlockEvents();
			return NULL;
		}

		eventsSet = new set<AnchorEvent*>;
		i = events.begin();
		while (i != events.end()) {
			event = i->second;
			if (event->instanceOf("AnchorEvent")) {
				anchor = ((AnchorEvent*)event)->getAnchor();
				if (anchor->instanceOf("SampleIntervalAnchor")) {
					eventsSet->insert((AnchorEvent*)event);
				}
			}
			++i;
		}
		unlockEvents();

		return eventsSet;
	}

	double ExecutionObject::getExpectedStartTime() {
		return startTime;
	}

	PresentationEvent* ExecutionObject::getWholeContentPresentationEvent() {
		return wholeContent;
	}

	void ExecutionObject::setStartTime(double t) {
		startTime = t;
	}

	void ExecutionObject::updateEventDurations() {
		vector<PresentationEvent*>::iterator i;

		i = presEvents.begin();
		while (i != presEvents.end()) {
			updateEventDuration(*i);
			++i;
		}
	}

	void ExecutionObject::updateEventDuration(PresentationEvent* event) {
		double duration;

		if (!containsEvent((FormatterEvent*)event)) {
			return;
		}

		duration = NaN();

		if (descriptor != NULL) {
			if (descriptor->instanceOf("CascadingDescriptor")) {
				if (!isNaN(
					    descriptor->getExplicitDuration()) &&
					    event == wholeContent) {

					duration = descriptor->getExplicitDuration();

				} else if (event->getDuration() > 0) {
					duration = event->getDuration();

				} else {
					duration = 0;
				}
			}

		} else {
			if (event->getDuration() > 0) {
				duration = event->getDuration();

			} else {
				duration = 0;
			}
		}

		if (duration < 0) {
			event->setDuration(NaN());

		} else {
			event->setDuration(duration);
		}
	}

	bool ExecutionObject::removeEvent(FormatterEvent* event) {
		vector<PresentationEvent*>::iterator i;
		set<SelectionEvent*>::iterator j;
		vector<FormatterEvent*>::iterator k;
		map<string, FormatterEvent*>::iterator l;

		if (!containsEvent(event)) {
			return false;
		}

		clog << "ExecutionObject::removeEvent '" << event->getId() << "'";
		clog << "from '" << getId() << "'" << endl;

		if (event->instanceOf("PresentationEvent")) {
			for (i = presEvents.begin(); i != presEvents.end(); ++i) {
				if (*i == (PresentationEvent*)event) {
					presEvents.erase(i);
					break;
				}
			}
			transMan->removeEventTransition((PresentationEvent*)event);

		} else if (event->instanceOf("SelectionEvent")) {
			j = selectionEvents.find(((SelectionEvent*)event));
			if (j != selectionEvents.end()) {
				selectionEvents.erase(j);
			}

		} else {
			for (k = otherEvents.begin();
				    k != otherEvents.end(); ++k) {

				if (*k == event) {
					otherEvents.erase(k);
					break;
				}
			}
		}

		lockEvents();
		l = events.find(event->getId());
		if (l != events.end()) {
			events.erase(l);
		}
		unlockEvents();
		return true;
	}

	bool ExecutionObject::isCompiled() {
		return isItCompiled;
	}

	void ExecutionObject::setCompiled(bool status) {
		isItCompiled = status;
	}

	void ExecutionObject::removeNode(Node* node) {
		Node* parentNode;
		map<Node*, Node*>::iterator i;
		map<Node*, void*>::iterator j;

		if (node != dataObject) {
			lockParentTable();
			i = nodeParentTable.find(node);
			if (i != nodeParentTable.end()) {
				parentNode = i->second;
				nodeParentTable.erase(i);

				j = parentTable.find(parentNode);
				if (j != parentTable.end()) {
					parentTable.erase(j);
				}
			}
			unlockParentTable();
		}
	}

	vector<Node*>* ExecutionObject::getNodes() {
		vector<Node*>* nodes;
		map<Node*, Node*>::iterator i;

		lockParentTable();
		if (nodeParentTable.empty()) {
			unlockParentTable();
			return NULL;
		}

		nodes = new vector<Node*>;
		for (i = nodeParentTable.begin(); i != nodeParentTable.end(); ++i) {
			nodes->push_back(i->first);
		}

		if (nodeParentTable.count(dataObject) == 0) {
			nodes->push_back(dataObject);
		}
		unlockParentTable();

		return nodes;
	}

	vector<Anchor*>* ExecutionObject::getNCMAnchors() {
		vector<Anchor*>* anchors = NULL;

		if (dataObject != NULL) {
			anchors = dataObject->getAnchors();
		}

		return anchors;
	}

	PropertyAnchor* ExecutionObject::getNCMProperty(string propertyName) {
		PropertyAnchor* property = NULL;

		if (dataObject != NULL) {
			property = dataObject->getPropertyAnchor(propertyName);
		}

		return property;
	}

	NodeNesting* ExecutionObject::getNodePerspective() {
		return getNodePerspective(dataObject);
	}

	NodeNesting* ExecutionObject::getNodePerspective(Node* node) {
		Node* parentNode;
		NodeNesting* perspective;
		CompositeExecutionObject* parentObject;
		map<Node*, void*>::iterator i;

		lockParentTable();
		if (nodeParentTable.count(node) == 0) {
			if (dataObject == node) {
				perspective = new NodeNesting();

			} else {
				unlockParentTable();
				return NULL;
			}

		} else {
			parentNode = nodeParentTable[node];

			i = parentTable.find(parentNode);
			if (i != parentTable.end()) {

				parentObject = (CompositeExecutionObject*)(i->second);

				perspective = parentObject->getNodePerspective(parentNode);

			} else {
				unlockParentTable();
				return NULL;
			}
		}
		perspective->insertAnchorNode(node);
		unlockParentTable();
		return perspective;
	}

	vector<ExecutionObject*>* ExecutionObject::getObjectPerspective() {
		return getObjectPerspective(dataObject);
	}

	vector<ExecutionObject*>* ExecutionObject::getObjectPerspective(
		    Node* node) {

		Node* parentNode;
		vector<ExecutionObject*>* perspective = NULL;
		CompositeExecutionObject* parentObject;
		map<Node*, Node*>::iterator i;
		map<Node*, void*>::iterator j;

		lockParentTable();
		i = nodeParentTable.find(node);
		if (i == nodeParentTable.end()) {
			if (dataObject == node) {
				perspective = new vector<ExecutionObject*>;

			} else {
				unlockParentTable();
				return NULL;
			}

		} else {
			parentNode = i->second;
			j = parentTable.find(parentNode);
			if (j != parentTable.end()) {
				parentObject = (CompositeExecutionObject*)(j->second);

				perspective = parentObject->getObjectPerspective(parentNode);

			} else {
				unlockParentTable();
				return NULL;
			}
		}
		unlockParentTable();

		perspective->push_back(this);
		return perspective;
	}

	vector<Node*>* ExecutionObject::getParentNodes() {
		vector<Node*>* parents;
		map<Node*, Node*>::iterator i;

		lockParentTable();
		if (nodeParentTable.empty()) {
			unlockParentTable();
			return NULL;
		}

		parents = new vector<Node*>;
		for (i = nodeParentTable.begin(); i != nodeParentTable.end(); ++i) {
			parents->push_back(i->second);
		}

		unlockParentTable();
		return parents;
	}

	FormatterEvent* ExecutionObject::getMainEvent() {
		return mainEvent;
	}

	bool ExecutionObject::prepare(FormatterEvent* event, double offsetTime) {
		int size;
		map<Node*, void*>::iterator i;
		double startTime = 0;
		ContentAnchor* contentAnchor;
		FormatterEvent* auxEvent;
		AttributionEvent* attributeEvent;
		PropertyAnchor* attributeAnchor;
		int j;
		string value;

		//clog << "ExecutionObject::prepare(" << id << ")" << endl;
		if (event == NULL || !containsEvent(event) ||
				event->getCurrentState() != EventUtil::ST_SLEEPING) {

			//clog << "ExecutionObject::prepare(" << id << ") ret FALSE" << endl;
			return false;
		}

		if (mainEvent != NULL &&
				mainEvent->getCurrentState() != EventUtil::ST_SLEEPING) {

			return false;
		}

		if (!lock()) {
			return false;
		}

		//clog << "ExecutionObject::prepare(" << id << ") locked" << endl;
		mainEvent = event;
		if (mainEvent->instanceOf("AnchorEvent")) {
			contentAnchor = ((AnchorEvent*)mainEvent)->getAnchor();
			if (contentAnchor != NULL &&
					contentAnchor->instanceOf("LabeledAnchor")) {

				lockParentTable();
				i = parentTable.begin();
				while (i != parentTable.end()) {
					clog << "ExecutionObject::prepare(" << id << ") call ";
					clog << "addEventListener '" << i->second << "' or '";
					clog << (IEventListener*)(CompositeExecutionObject*)i->second;
					clog << "'" << endl;
					// register parent as a mainEvent listener
					mainEvent->addEventListener((IEventListener*)(
							CompositeExecutionObject*)i->second);
					++i;
				}
				unlockParentTable();
				return true;
			}
		}

		if (mainEvent->instanceOf("PresentationEvent")) {
			startTime = ((PresentationEvent*)mainEvent)->
					getBegin() + offsetTime;

			if (startTime > ((PresentationEvent*)mainEvent)->getEnd()) {
				return false;
			}
		}

		lockParentTable();
		i = parentTable.begin();
		while (i != parentTable.end()) {
			clog << "ExecutionObject::prepare(" << id << ") 2nd call ";
			clog << "addEventListener '" << i->second << "' or '";
			clog << (IEventListener*)(CompositeExecutionObject*)i->second;
			clog << "'" << endl;
			// register parent as a mainEvent listener
			mainEvent->addEventListener((IEventListener*)(
					CompositeExecutionObject*)i->second);
			++i;
		}
		unlockParentTable();

		prepareTransitionEvents(ContentAnchor::CAT_TIME, startTime);

		size = otherEvents.size();
		for (j = 0; j < size; j++) {
			auxEvent = otherEvents[j];
			if (auxEvent->instanceOf("AttributionEvent")) {
				attributeEvent = (AttributionEvent*)auxEvent;
				attributeAnchor = attributeEvent->getAnchor();
				value = attributeAnchor->getPropertyValue();
				if (value != "") {
					attributeEvent->setValue(value);
				}
			}
		}

		this->offsetTime = startTime;
		return true;
	}

	bool ExecutionObject::start() {
		ContentAnchor* contentAnchor;

		clog << "ExecutionObject::start(" << id << ")" << endl;
		if (mainEvent == NULL && wholeContent == NULL) {
			return false;
		}

		if (mainEvent != NULL &&
				mainEvent->getCurrentState() != EventUtil::ST_SLEEPING) {

			return true;
		}

		if (mainEvent == NULL) {
			prepare(wholeContent, 0.0);
		}

		if (mainEvent != NULL && mainEvent->instanceOf("AnchorEvent")) {
			contentAnchor = ((AnchorEvent*)mainEvent)->getAnchor();
			if (contentAnchor != NULL &&
					contentAnchor->instanceOf("LabeledAnchor")) {

				transMan->start(offsetTime);
				mainEvent->start();
				return true;
			}
		}

		transMan->start(offsetTime);
		return true;
	}

	void ExecutionObject::timeBaseNaturalEnd(
			int64_t timeValue, short int transType) {

		transMan->timeBaseNaturalEnd(timeValue, mainEvent, transType);
	}

	void ExecutionObject::updateTransitionTable(
			double value, IPlayer* player, short int transType) {

		transMan->updateTransitionTable(value, player, mainEvent, transType);
	}

	void ExecutionObject::resetTransitionEvents(short int transType) {
		transMan->resetTimeIndexByType(transType);
	}

	void ExecutionObject::prepareTransitionEvents(
			short int transType, double startTime) {

		transMan->prepare(mainEvent == wholeContent, startTime, transType);
	}

	set<double>* ExecutionObject::getTransitionsValues(short int transType) {
		return transMan->getTransitionsValues(transType);
	}

	EventTransition* ExecutionObject::getNextTransition() {
		if (isSleeping() || !mainEvent->instanceOf("PresentationEvent")) {
			return NULL;
		}

		return transMan->getNextTransition(mainEvent);
	}

	bool ExecutionObject::stop() {
		ContentAnchor* contentAnchor;
		double endTime;

		if (isSleeping()) {
			clog << "ExecutionObject::stop for '" << id << "'";
			clog << " returns false because mainEvent is SLEEPING" << endl;
			return false;
		}

		if (mainEvent->instanceOf("PresentationEvent")) {
			endTime = ((PresentationEvent*)mainEvent)->getEnd();
			transMan->stop(endTime);

		} else if (mainEvent->instanceOf("AnchorEvent")) {
			contentAnchor = ((AnchorEvent*)mainEvent)->getAnchor();
			if (contentAnchor != NULL &&
					contentAnchor->instanceOf("LabeledAnchor")) {

				/*clog << "ExecutionObject::stop for '" << id << "'";
				clog << " call mainEvent '" << mainEvent->getId();
				clog << "' stop" << endl;*/
				mainEvent->stop();
			}
		}

		transMan->resetTimeIndex();
		pauseCount = 0;
		return true;
	}

	bool ExecutionObject::abort() {
		ContentAnchor* contentAnchor;
		double endTime;

		//clog << "ExecutionObject::abort(" << id << ")" << endl;
		if (isSleeping()) {
			return false;
		}

		if (mainEvent->instanceOf("PresentationEvent")) {
			endTime = ((PresentationEvent*)mainEvent)->getEnd();
			transMan->abort(endTime);

		} else if (mainEvent->instanceOf("AnchorEvent")) {
			contentAnchor = ((AnchorEvent*)mainEvent)->getAnchor();
			if (contentAnchor != NULL &&
					contentAnchor->instanceOf("LabeledAnchor")) {

				mainEvent->abort();
			}
		}

		transMan->resetTimeIndex();
		pauseCount = 0;
		return true;
	}

	bool ExecutionObject::pause() {
		FormatterEvent* event;
		vector<FormatterEvent*>* evs;
		vector<FormatterEvent*>::iterator i;

		//clog << "ExecutionObject::pause(" << id << ")" << endl;
		if (isSleeping()) {
			return false;
		}

		evs = getEvents();
		if (evs != NULL) {
			if (pauseCount == 0) {
				i = evs->begin();
				while (i != evs->end()) {
					event = *i;
					if (event->getCurrentState() == EventUtil::ST_OCCURRING) {
						event->pause();
					}
					++i;
				}
			}
			delete evs;
			evs = NULL;
		}

		pauseCount++;
		return true;
	}

	bool ExecutionObject::resume() {
		FormatterEvent* event;
		vector<FormatterEvent*>* evs;
		vector<FormatterEvent*>::iterator i;

		//clog << "ExecutionObject::resume(" << id << ")" << endl;
		if (pauseCount == 0) {
			return false;

		} else {
			pauseCount--;
			if (pauseCount > 0) {
				return false;
			}
		}

		evs = getEvents();
		if (evs != NULL) {
			if (pauseCount == 0) {
				i = evs->begin();
				while (i != evs->end()) {
					event = *i;
					if (event->getCurrentState() == EventUtil::ST_PAUSED) {
						event->resume();
					}
					++i;
				}
			}
			delete evs;
			evs = NULL;
		}

		return true;
	}

	bool ExecutionObject::setPropertyValue(
			AttributionEvent* event, string value) {

		string propName;

		FormatterRegion* region = NULL;
		LayoutRegion* ncmRegion = NULL;
		bool hasProp            = true;
		vector<string>* params;
		string left = "", top = "";
		string width = "", height = "";
		string right = "", bottom = "";

		if (descriptor == NULL || descriptor->getFormatterRegion() == NULL) {
			clog << "ExecutionObject::setPropertyValue : setProperty could ";
			clog << "not be performed. Descriptor or formatterRegion is NULL";
			clog << endl;

			return false;
		}

		if (!containsEvent(event) || value == "") {
			clog << "ExecutionObject::setPropertyValue(" << getId() << ")";
			clog << " returns false: event '";
			clog << event->getId() << "' not found!" << endl;
			return false;
		}

		region    = descriptor->getFormatterRegion();
		ncmRegion = region->getLayoutRegion();
		if (ncmRegion == NULL) {
			clog << "ExecutionObject::setPropertyValue : The ncmRegion ";
			clog << " is NULL (PROBLEM)!" << endl;
			return false;
		}

		propName = (event->getAnchor())->getPropertyName();

		lock(); //lock the object

		if (propName == "zIndex") {
			region->setZIndex((int)util::stof(value));
			unlock();
			return true;
		}

		params = split(trim(value), ",");

		if (propName == "size") {
			if (params->size() == 2) {
				width = (*params)[0];
				height = (*params)[1];

			} else {
				hasProp = false;
			}

		} else if (propName == "location") {
			if (params->size() == 2) {
				left = (*params)[0];
				top = (*params)[1];

			} else {
				hasProp = false;
			}

		} else if (propName == "bounds") {
			if (params->size() == 4) {
				left = (*params)[0];
				top = (*params)[1];
				width = (*params)[2];
				height = (*params)[3];

			} else {
				hasProp = false;
			}

		} else if (propName == "left") {
			left = (*params)[0];

		} else if (propName == "top") {
			top = (*params)[0];

		} else if (propName == "width") {
			width = (*params)[0];

		} else if (propName == "height") {
			height = (*params)[0];

		} else if (propName == "bottom") {
			bottom = (*params)[0];

		} else if (propName == "right") {
			right = (*params)[0];

		} else {
			hasProp = false;
		}

		if (hasProp) {
			if (left != "") {
				if (isPercentualValue(left)) {
					ncmRegion->setTargetLeft(getPercentualValue(left), true);

				} else {
					ncmRegion->setTargetLeft((double)util::stof(left), false);
				}
			}

			if (top != "") {
				if (isPercentualValue(top)) {
					ncmRegion->setTargetTop(getPercentualValue(top), true);

				} else {
					ncmRegion->setTargetTop((double)util::stof(top), false);
				}
			}

			if (width != "") {
				if (isPercentualValue(width)) {
					ncmRegion->setTargetWidth(getPercentualValue(width), true);

				} else {
					ncmRegion->setTargetWidth((double)util::stof(width), false);
				}
			}

			if (height != "") {
				if (isPercentualValue(height)) {
					ncmRegion->setTargetHeight(getPercentualValue(height), true);

				} else {
					ncmRegion->setTargetHeight((double)util::stof(height), false);
				}
			}

			if (bottom != "") {
				if (isPercentualValue(bottom)) {
					ncmRegion->setTargetBottom(getPercentualValue(bottom), true);

				} else {
					ncmRegion->setTargetBottom((double)util::stof(bottom), false);
				}
			}

			if (right != "") {
				if (isPercentualValue(right)) {
					ncmRegion->setTargetRight(getPercentualValue(right), true);

				} else {
					ncmRegion->setTargetRight((double)util::stof(right), false);
				}
			}

			//set the values
			ncmRegion->validateTarget();

			region->updateRegionBounds();
			unlock();
			delete params;
			params = NULL;
			return true;
		}
		unlock();

		delete params;
		params = NULL;

		return false;
	}

	string ExecutionObject::getPropertyValue(string param){

		FormatterRegion* region = NULL;
		LayoutRegion* ncmRegion = NULL;
		string value = "";

		if (descriptor == NULL || descriptor->getFormatterRegion() == NULL) {
			clog << "ExecutionObject::setPropertyValue : setProperty could ";
			clog << "not be performed. Descriptor or formatterRegion is NULL";
			clog << endl;
			return value;
		}

		region = descriptor->getFormatterRegion();
		ncmRegion = region->getLayoutRegion();

		double current = 0.0;
		if (param == "left") {
			current = ncmRegion->getLeftInPixels();
			value = itos(current);

		} else if (param == "top") {
			current = ncmRegion->getTopInPixels();
			value = itos(current);

		} else if (param == "width") {
			current = ncmRegion->getWidthInPixels();
			value = itos(current);

		} else if (param == "height") {
			current = ncmRegion->getHeightInPixels();
			value = itos(current);

		} else if (param == "bottom") {
			current = ncmRegion->getBottomInPixels();
			value = itos(current);

		} else if (param == "right") {
			current = ncmRegion->getRightInPixels();
			value = itos(current);

		} else if (param == "transparency"){
			current = region->getTransparency();
			value = itos(current);

		} else if (param == "bounds") {
			current = ncmRegion->getLeftInPixels();
			value = itos(current);
			current = ncmRegion->getTopInPixels();
			value += "," + itos(current);
			current = ncmRegion->getWidthInPixels();
			value += "," + itos(current);
			current = ncmRegion->getHeightInPixels();
			value += "," + itos(current);

		} else if (param == "location") {
			current = ncmRegion->getLeftInPixels();
			value = itos(current);
			current = ncmRegion->getTopInPixels();
			value += "," + itos(current);

		} else if (param == "size") {
			current = ncmRegion->getWidthInPixels();
			value = itos(current);
			current = ncmRegion->getHeightInPixels();
			value += "," + itos(current);

		}

		return value;
	}

	bool ExecutionObject::unprepare() {
		if (mainEvent == NULL ||
				mainEvent->getCurrentState() != EventUtil::ST_SLEEPING) {

			unlock();
			clog << "ExecutionObject::unprepare(" << id << ") unlocked";
			clog << " ret FALSE" << endl;
			return false;
		}

		removeParentListenersFromEvent(mainEvent);

		mainEvent = NULL;
		unlock();
		/*clog << "ExecutionObject::unprepare(" << id << ") unlocked";
		clog << endl;*/
		return true;
	}

	void ExecutionObject::setHandling(bool isHandling) {
		this->isHandling = isHandling;
	}

	void ExecutionObject::setHandler(bool isHandler) {
		this->isHandler = isHandler;
	}

	bool ExecutionObject::selectionEvent(int keyCode, double currentTime) {
		int selCode;
		SelectionEvent* selectionEvent;
		IntervalAnchor* intervalAnchor;
		FormatterEvent* expectedEvent;
		Anchor* expectedAnchor;
		string anchorId = "";
		set<SelectionEvent*>* selectedEvents;
		set<SelectionEvent*>::iterator i;
		bool sleeping = isSleeping();
		bool paused = isPaused();
		bool selected = false;
		double intervalBegin;
		double intervalEnd;

		if ((!isHandling && !isHandler) || sleeping || paused) {
			clog << "ExecutionObject::selectionEvent Can't receive event on '";
			clog << getId() << "': isHandling = '" << isHandling << "' ";
			clog << "isHandler = '" << isHandler << "' ";
			clog << "isSleeping() = '" << sleeping << "' ";
			clog << "isPaused() = '" << paused << "' ";
			clog << endl;
			return false;
		}

		if (selectionEvents.empty()) {
			clog << "ExecutionObject::selectionEvent Can't receive event on '";
			clog << getId() << "': selection events is empty";
			clog << endl;
			return false;
		}

		selectedEvents = new set<SelectionEvent*>;
		i = selectionEvents.begin();
		while (i != selectionEvents.end()) {
			selectionEvent = (SelectionEvent*)(*i);
			selCode = selectionEvent->getSelectionCode();

			clog << "ExecutionObject::selectionEvent(" << id << ") event '";
			clog << selectionEvent->getId() << "' has selCode = '" << selCode;
			clog << "' (looking for key code '" << keyCode << "'" << endl;

			if (selCode == keyCode) {
				if (selectionEvent->getAnchor()->instanceOf(
					    "LambdaAnchor")) {

					selectedEvents->insert(selectionEvent);

				} else if (selectionEvent->getAnchor()->instanceOf(
					    "IntervalAnchor")) {

					intervalAnchor = (IntervalAnchor*)(
						    selectionEvent->getAnchor());

					intervalBegin = intervalAnchor->getBegin();
					intervalEnd   = intervalAnchor->getEnd();

					clog << "ExecutionObject::selectionEvent(" << id << ") ";
					clog << "interval anchor '" << intervalAnchor->getId();
					clog << "' begins at '" << intervalBegin << "', ends at ";
					clog << intervalEnd << "' (current time is '";
					clog << currentTime << "'";
					clog << endl;

					if (intervalBegin <= currentTime
							&& intervalEnd >= currentTime) {

						selectedEvents->insert(selectionEvent);
					}

				} else {
					expectedAnchor = selectionEvent->getAnchor();
					if (expectedAnchor->instanceOf("LabeledAnchor")) {
						anchorId = ((LabeledAnchor*)expectedAnchor)->getLabel();

					} else {
						anchorId = expectedAnchor->getId();
					}

					expectedEvent = getEventFromAnchorId(anchorId);
					if (expectedEvent != NULL) {
						clog << "ExecutionObject::selectionEvent(";
						clog << id << ")";
						clog << " analyzing event '";
						clog << expectedEvent->getId();

						if (expectedEvent->getCurrentState() ==
								EventUtil::ST_OCCURRING) {

							selectedEvents->insert(selectionEvent);

						} else {
							clog << "' not occurring";
						}

						clog << "'" << endl;

					} else {
						clog << "ExecutionObject::selectionEvent(" << id << ")";
						clog << " can't find event for anchorid = '";
						clog << anchorId << "'" << endl;
					}
				}
			}
			++i;
		}

		LinkSimpleAction* fakeAct;

		i = selectedEvents->begin();
		while (i != selectedEvents->end()) {
			selected = true;
			selectionEvent = (*i);

			if (seListener != NULL) {
				clog << "ExecutionObject::selectionEvent(" << id << ")";
				clog << " calling scheduler to execute fake action";
				clog << endl;

				fakeAct = new LinkSimpleAction(
						selectionEvent, SimpleAction::ACT_START);

				seListener->scheduleAction(NULL, fakeAct);
			}

			++i;
		}

		delete selectedEvents;
		selectedEvents = NULL;

		return selected;
	}

	set<int>* ExecutionObject::getInputEvents() {
		set<SelectionEvent*>::iterator i;
		set<int>* evs;
		SelectionEvent* ev;
		int keyCode;

		evs = new set<int>;
		i = selectionEvents.begin();
		while (i != selectionEvents.end()) {
			ev = (*i);
			keyCode = ev->getSelectionCode();
			evs->insert(keyCode);
			if (keyCode == CodeMap::KEY_RED) {
				evs->insert(CodeMap::KEY_F1);

			} else if (keyCode == CodeMap::KEY_GREEN) {
				evs->insert(CodeMap::KEY_F2);

			} else if (keyCode == CodeMap::KEY_YELLOW) {
				evs->insert(CodeMap::KEY_F3);

			} else if (keyCode == CodeMap::KEY_BLUE) {
				evs->insert(CodeMap::KEY_F4);

			} else if (keyCode == CodeMap::KEY_MENU) {
				evs->insert(CodeMap::KEY_F5);

			} else if (keyCode == CodeMap::KEY_INFO) {
				evs->insert(CodeMap::KEY_F6);

			} else if (keyCode == CodeMap::KEY_EPG) {
				evs->insert(CodeMap::KEY_F7);

			} else if (keyCode == CodeMap::KEY_VOLUME_UP) {
				evs->insert(CodeMap::KEY_PLUS_SIGN);

			} else if (keyCode == CodeMap::KEY_VOLUME_DOWN) {
				evs->insert(CodeMap::KEY_MINUS_SIGN);

			} else if (keyCode == CodeMap::KEY_CHANNEL_UP) {
				evs->insert(CodeMap::KEY_PAGE_UP);

			} else if (keyCode == CodeMap::KEY_CHANNEL_DOWN) {
				evs->insert(CodeMap::KEY_PAGE_DOWN);

			} else if (keyCode == CodeMap::KEY_BACK) {
				evs->insert(CodeMap::KEY_BACKSPACE);

			} else if (keyCode == CodeMap::KEY_EXIT) {
				evs->insert(CodeMap::KEY_ESCAPE);
			}
			++i;
		}

		return evs;
	}

	bool ExecutionObject::lock() {
		if (isLocked) {
			return false;
		}
		isLocked = true;
		Thread::mutexLock(&mutex);
		return true;
	}

	bool ExecutionObject::unlock() {
		if (!isLocked) {
			return false;
		}

		Thread::mutexUnlock(&mutex);
		isLocked = false;
		return true;
	}

	void ExecutionObject::lockEvents() {
		Thread::mutexLock(&mutexEvent);
	}

	void ExecutionObject::unlockEvents() {
		Thread::mutexUnlock(&mutexEvent);
	}

	void ExecutionObject::lockParentTable() {
		Thread::mutexLock(&mutexParentTable);
	}

	void ExecutionObject::unlockParentTable() {
		Thread::mutexUnlock(&mutexParentTable);
	}

BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_COMPONENTS_END
