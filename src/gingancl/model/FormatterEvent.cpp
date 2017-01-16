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
#include "FormatterEvent.h"
#include "PresentationEvent.h"

#include "CompositeExecutionObject.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::components;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace event {

	set<FormatterEvent*> FormatterEvent::instances;
	bool FormatterEvent::init = false;
	pthread_mutex_t FormatterEvent::iMutex;

	FormatterEvent::FormatterEvent(string id, void* execObject) {
		this->id         = id;
		currentState     = EventUtil::ST_SLEEPING;
		occurrences      = 0;
		executionObject  = execObject;
		deleting         = false;
		eventType        = -1;

		if (!init) {
			init = true;
			Thread::mutexInit(&iMutex, false);
		}

		typeSet.insert("FormatterEvent");
		Thread::mutexInit(&mutex, false);

		addInstance(this);
	}

	FormatterEvent::~FormatterEvent() {
		deleting = true;

		removeInstance(this);

		destroyListeners();
		Thread::mutexDestroy(&mutex);
	}

	bool FormatterEvent::hasInstance(FormatterEvent* event, bool remove) {
		set<FormatterEvent*>::iterator i;
		bool inst = false;

		if (!init) {
			return false;
		}

		Thread::mutexLock(&iMutex);
		i = instances.find(event);
		if (i != instances.end()) {
			if (remove) {
				instances.erase(i);
			}
			inst = true;
		}
		Thread::mutexUnlock(&iMutex);

		return inst;
	}

	void FormatterEvent::addInstance(FormatterEvent* event) {
		Thread::mutexLock(&iMutex);
		instances.insert(event);
		Thread::mutexUnlock(&iMutex);
	}

	bool FormatterEvent::removeInstance(FormatterEvent* event) {
		set<FormatterEvent*>::iterator i;
		bool inst = false;

		Thread::mutexLock(&iMutex);
		i = instances.find(event);
		if (i != instances.end()) {
			instances.erase(i);
			inst = true;
		}
		Thread::mutexUnlock(&iMutex);

		return inst;
	}

	bool FormatterEvent::instanceOf(string s) {
		if (typeSet.empty()) {
			return false;
		} else {
			return (typeSet.find(s) != typeSet.end());
		}
	}

	bool FormatterEvent::hasNcmId(FormatterEvent* event, string anchorId) {
		Anchor* anchor;
		string anchorName = " ";

		if (event->instanceOf("AnchorEvent")) {
			anchor = ((AnchorEvent*)event)->getAnchor();
			if (anchor != NULL) {
				if (anchor->instanceOf("IntervalAnchor")) {
					anchorName = anchor->getId();

				} else if (anchor->instanceOf("LabeledAnchor")) {
					anchorName = ((LabeledAnchor*)anchor)->getLabel();

				} else if (anchor->instanceOf("LambdaAnchor")) {
					anchorName = "";
				}

				if (anchorName == anchorId &&
						!event->instanceOf("SelectionEvent")) {

					return true;
				}
			}

		} else if (event->instanceOf("AttributionEvent")) {
			anchor = ((AttributionEvent*)event)->getAnchor();
			if (anchor != NULL) {
				anchorName = ((PropertyAnchor*)anchor)->getPropertyName();
				if (anchorName == anchorId) {
					return true;
				}
			}
		}

		return false;
	}

	void FormatterEvent::setEventType(short eventType) {
		this->eventType = eventType;
	}

	short FormatterEvent::getEventType() {
		return eventType;
	}

	void FormatterEvent::destroyListeners() {
		Thread::mutexLock(&mutex);
		this->executionObject = NULL;

		// TODO, avoid to leave a link bind with an inconsistent event
		coreListeners.clear();
		linksListeners.clear();
		objectsListeners.clear();

		Thread::mutexUnlock(&mutex);
	}

	void FormatterEvent::setId(string id) {
		this->id = id;
	}

	void FormatterEvent::addEventListener(IEventListener* listener) {
		short pType = listener->getPriorityType();

		Thread::mutexLock(&mutex);

		switch (pType) {
			case IEventListener::PT_CORE:
				coreListeners.insert(listener);
				break;

			case IEventListener::PT_LINK:
				linksListeners.insert(listener);
				break;

			case IEventListener::PT_OBJECT:
				objectsListeners.insert(listener);
				break;
		}

		Thread::mutexUnlock(&mutex);
	}

	bool FormatterEvent::containsEventListener(IEventListener* listener) {
		Thread::mutexLock(&mutex);
		if (coreListeners.count(listener) != 0 ||
				linksListeners.count(listener) != 0 ||
				objectsListeners.count(listener) != 0) {

			Thread::mutexUnlock(&mutex);
			return true;
		}
		Thread::mutexUnlock(&mutex);
		return false;
	}

	void FormatterEvent::removeEventListener(IEventListener* listener) {
		set<IEventListener*>::iterator i;

		Thread::mutexLock(&mutex);
		i = coreListeners.find(listener);
		if (i != coreListeners.end()) {
			coreListeners.erase(i);
		}

		i = linksListeners.find(listener);
		if (i != linksListeners.end()) {
			linksListeners.erase(i);
		}

		i = objectsListeners.find(listener);
		if (i != objectsListeners.end()) {
			objectsListeners.erase(i);
		}

		Thread::mutexUnlock(&mutex);
	}

	short FormatterEvent::getNewState(short transition) {
		switch (transition) {
			case EventUtil::TR_STOPS:
				return EventUtil::ST_SLEEPING;

			case EventUtil::TR_STARTS:
			case EventUtil::TR_RESUMES:
				return EventUtil::ST_OCCURRING;

			case EventUtil::TR_PAUSES:
				return EventUtil::ST_PAUSED;

			case EventUtil::TR_ABORTS:
				return ST_ABORTED;

			default:
				return -1;
		}
	}

	short FormatterEvent::getTransition(short newState) {
		return getTransistion(currentState, newState);
	}

	bool FormatterEvent::abort() {
		switch (currentState) {
			case EventUtil::ST_OCCURRING:
			case EventUtil::ST_PAUSED:
				return changeState(ST_ABORTED, EventUtil::TR_ABORTS);

			default:
				return false;
		}
	}

	bool FormatterEvent::start() {
		switch (currentState) {
			case EventUtil::ST_SLEEPING:
				return changeState(
					    EventUtil::ST_OCCURRING, EventUtil::TR_STARTS);
			default:
				return false;
		}
	}

	bool FormatterEvent::stop() {
		switch (currentState) {
			case EventUtil::ST_OCCURRING:
			case EventUtil::ST_PAUSED:
				return changeState(EventUtil::ST_SLEEPING, EventUtil::TR_STOPS);
			default:
				return false;
		}
	}

	bool FormatterEvent::pause() {
		switch (currentState) {
			case EventUtil::ST_OCCURRING:
				return changeState(EventUtil::ST_PAUSED, EventUtil::TR_PAUSES);

			default:
				return false;
		}
	}

	bool FormatterEvent::resume() {
		switch (currentState) {
			case EventUtil::ST_PAUSED:
				return changeState(
					    EventUtil::ST_OCCURRING, EventUtil::TR_RESUMES);

			default:
				return false;
		}
	}

	void FormatterEvent::setCurrentState(short newState) {
		previousState = currentState;
		currentState = newState;
	}

	bool FormatterEvent::changeState(short newState, short transition) {
		set<IEventListener*>::iterator i;
		set<IEventListener*>* coreClone, *linkClone, *objectClone;

		Thread::mutexLock(&mutex);

		if (transition == EventUtil::TR_STOPS) {
			occurrences++;
		}

		previousState = currentState;
		currentState  = newState;

		if (deleting) {
			Thread::mutexUnlock(&mutex);
			return false;
		}

		coreClone   = new set<IEventListener*>(coreListeners);
		linkClone   = new set<IEventListener*>(linksListeners);
		objectClone = new set<IEventListener*>(objectsListeners);

		Thread::mutexUnlock(&mutex);

		i = coreClone->begin();
		while (i != coreClone->end()) {
			if (deleting) {
				break;
			}

			if (*i != NULL) {
				((IEventListener*)(*i))->eventStateChanged(
					    (void*)this, transition, previousState);
			}
			++i;
		}

		i = linkClone->begin();
		while (i != linkClone->end()) {
			if (deleting) {
				break;
			}

			if (*i != NULL) {
				((IEventListener*)(*i))->eventStateChanged(
					    (void*)this, transition, previousState);
			}
			++i;
		}

		i = objectClone->begin();
		while (i != objectClone->end()) {
			if (deleting) {
				break;
			}

			if (*i != NULL) {
				((IEventListener*)(*i))->eventStateChanged(
					    (void*)this, transition, previousState);
			}
			++i;
		}

		coreClone->clear();
		delete coreClone;
		coreClone = NULL;

		linkClone->clear();
		delete linkClone;
		linkClone = NULL;

		objectClone->clear();
		delete objectClone;
		objectClone = NULL;

		if (currentState == ST_ABORTED) {
			currentState = EventUtil::ST_SLEEPING;
		}

		return true;
	}

	short FormatterEvent::getCurrentState() {
		return currentState;
	}

	short FormatterEvent::getPreviousState() {
		return previousState;
	}

	short FormatterEvent::getTransistion(
			short previousState, short newState) {

		switch (previousState) {
			case EventUtil::ST_SLEEPING:
				switch (newState) {
					case EventUtil::ST_OCCURRING:
						return EventUtil::TR_STARTS;
					default:
						return -1;
				}
				break;

			case EventUtil::ST_OCCURRING:
				switch (newState) {
					case EventUtil::ST_SLEEPING:
						return EventUtil::TR_STOPS;
					case EventUtil::ST_PAUSED:
						return EventUtil::TR_PAUSES;
					case ST_ABORTED:
						return EventUtil::TR_ABORTS;
					default:
						return -1;
				}
				break;

			case EventUtil::ST_PAUSED:
				switch (newState) {
					case EventUtil::ST_OCCURRING:
						return EventUtil::TR_RESUMES;
					case EventUtil::ST_SLEEPING:
						return EventUtil::TR_STOPS;
					case ST_ABORTED:
						return EventUtil::TR_ABORTS;
					default:
						return -1;
				}
				break;

			default:
				break;
		}

		return -1;
	}

	void* FormatterEvent::getExecutionObject() {
		return executionObject;
	}

	void FormatterEvent::setExecutionObject(void* object) {
		executionObject = object;
	}

	string FormatterEvent::getId() {
		return id;
	}

	long FormatterEvent::getOccurrences() {
		return occurrences;
	}

	string FormatterEvent::getStateName(short state) {
		switch (state) {
			case EventUtil::ST_OCCURRING:
				return "occurring";

			case EventUtil::ST_PAUSED:
				return "paused";

			case EventUtil::ST_SLEEPING:
				return "sleeping";

			default:
				return "";
		}
	}
}
}
}
}
}
}
}
