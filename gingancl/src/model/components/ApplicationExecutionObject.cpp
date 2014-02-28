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

#include "gingancl/model/ApplicationExecutionObject.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace components {
	ApplicationExecutionObject::ApplicationExecutionObject(
			string id,
			Node* node,
			bool handling,
			ILinkActionListener* seListener) : ExecutionObject(
					id, node, handling, seListener) {

		initializeApplicationObject();
	}

	ApplicationExecutionObject::ApplicationExecutionObject(
			string id,
			Node* node,
			GenericDescriptor* descriptor,
			bool handling,
			ILinkActionListener* seListener) : ExecutionObject(
					id, node, descriptor, handling, seListener) {

		initializeApplicationObject();
	}

	ApplicationExecutionObject::ApplicationExecutionObject(
			string id,
			Node* node,
			CascadingDescriptor* descriptor,
			bool handling,
			ILinkActionListener* seListener) : ExecutionObject(
					id, node, descriptor, handling, seListener) {

		initializeApplicationObject();
	}

	ApplicationExecutionObject::~ApplicationExecutionObject() {
		map<string, FormatterEvent*>::iterator i;

		ExecutionObject::removeInstance(this);

		i = preparedEvents.begin();
		while (i != preparedEvents.end()) {
			removeParentListenersFromEvent(i->second);
			++i;
		}

		unprepare();
		unprepareEvents();
		removeEventListeners();

		destroyEvents();

		Thread::mutexDestroy(&eventsMutex);
	}

	void ApplicationExecutionObject::initializeApplicationObject() {
		clog << "ApplicationExecutionObject::initializeApplicationObject(";
		clog << id << ")" << endl;

		typeSet.insert("ApplicationExecutionObject");

		currentEvent = NULL;

		Thread::mutexInit(&eventsMutex, NULL);
	}

	bool ApplicationExecutionObject::isSleeping() {
		map<string, FormatterEvent*>::iterator i;

		lockEvents();
		i = preparedEvents.begin();
		while (i != preparedEvents.end()) {
			if (i->second->getCurrentState() != EventUtil::ST_SLEEPING) {
				unlockEvents();
				return false;
			}
			++i;
		}
		unlockEvents();
		return true;
	}

	bool ApplicationExecutionObject::isPaused() {
		map<string, FormatterEvent*>::iterator i;
		FormatterEvent* event;
		bool hasPaused = false;

		if (currentEvent != NULL) {
			if (currentEvent->getCurrentState() == EventUtil::ST_OCCURRING) {
				return false;
			}

			if (currentEvent->getCurrentState() == EventUtil::ST_PAUSED) {
				hasPaused = true;
			}
		}

		lockEvents();
		i = preparedEvents.begin();
		while (i != preparedEvents.end()) {
			event = i->second;
			if (event->getCurrentState() == EventUtil::ST_OCCURRING) {
				unlockEvents();
				return false;
			}

			if (event->getCurrentState() == EventUtil::ST_PAUSED) {
				hasPaused = true;
			}
			++i;
		}

		unlockEvents();
		return hasPaused;
	}

	FormatterEvent* ApplicationExecutionObject::getCurrentEvent() {
		return currentEvent;
	}

	bool ApplicationExecutionObject::hasPreparedEvent(FormatterEvent* event) {
		bool hasEvents;
		map<string, FormatterEvent*>::iterator i;

		lockEvents();
		i = preparedEvents.begin();
		while (i != preparedEvents.end()) {
			if (i->second != event) {
				unlockEvents();
				return true;
			}
			++i;
		}

		unlockEvents();
		return false;
	}

	void ApplicationExecutionObject::setCurrentEvent(FormatterEvent* event) {
		if (!containsEvent(event)) {
			currentEvent = NULL;
		} else {
			currentEvent = event;
		}
	}

	bool ApplicationExecutionObject::prepare(
		    FormatterEvent* event, double offsetTime) {

		int size;
		map<Node*, void*>::iterator i;
		double startTime = 0;
		ContentAnchor* contentAnchor;
		FormatterEvent* auxEvent;
		AttributionEvent* attributeEvent;
		PropertyAnchor* attributeAnchor;
		int j;

		if (event->getCurrentState() != EventUtil::ST_SLEEPING) {
			clog << "ApplicationExecutionObject::prepare can't prepare '";
			clog << event->getId() << "': event isn't sleeping" << endl;
			return false;
		}

		if (event->instanceOf("PresentationEvent")) {
			double duration = ((PresentationEvent*)event)->getDuration();
			if (duration <= 0) {
				clog << "ApplicationExecutionObject::prepare can't prepare '";
				clog << event->getId() << "': event duration <= 0" << endl;
				return false;
			}
		}

		addEvent(event);
		if (event->instanceOf("AnchorEvent")) {
			contentAnchor = ((AnchorEvent*)event)->getAnchor();
			if (contentAnchor != NULL &&
					contentAnchor->instanceOf("LabeledAnchor")) {

				i = parentTable.begin();
				while (i != parentTable.end()) {
					// register parent as a mainEvent listener
					event->addEventListener((IEventListener*)(
							CompositeExecutionObject*)i->second);

					++i;
				}

				lockEvents();
				preparedEvents[event->getId()] = event;
				unlockEvents();
				return true;
			}
		}

		if (event->instanceOf("PresentationEvent")) {
			startTime = ((PresentationEvent*)event)->getBegin() + offsetTime;
			if (startTime > ((PresentationEvent*)event)->getEnd()) {
				clog << "ApplicationExecutionObject::prepare skipping '";
				clog << event->getId() << "': past event (start = '";
				clog << startTime << "'; end = '";
				clog << ((PresentationEvent*)event)->getEnd() << "'";
				clog << endl;
				return false;
			}
		}

		i = parentTable.begin();
		while (i != parentTable.end()) {
			// register parent as a currentEvent listener
			event->addEventListener((IEventListener*)(
					CompositeExecutionObject*)i->second);
			++i;
		}

		transMan->prepare(
				event == wholeContent, startTime, ContentAnchor::CAT_TIME);

		size = otherEvents.size();
		for (j = 0; j < size; j++) {
			auxEvent = otherEvents[j];
			if (auxEvent->instanceOf("AttributionEvent")) {
				attributeEvent = (AttributionEvent*)auxEvent;
				attributeAnchor = attributeEvent->getAnchor();
				if (attributeAnchor->getPropertyValue() != "") {
					attributeEvent->setValue(
							attributeAnchor->getPropertyValue());
				}
			}
		}

		this->offsetTime = startTime;

		lockEvents();
		preparedEvents[event->getId()] = event;
		unlockEvents();

		return true;
	}

	bool ApplicationExecutionObject::start() {
		ContentAnchor* contentAnchor;

		lockEvents();
		if (currentEvent == NULL ||
				preparedEvents.count(currentEvent->getId()) == 0) {

			clog << "ApplicationExecutionObject::start nothing to do!" << endl;
			unlockEvents();
			return false;
		}

		unlockEvents();

		/*
		 * TODO: follow the event state machine or start instruction behavior
		 * if (currentEvent->getCurrentState() == EventUtil::ST_PAUSED) {
			return resume();
		}*/

		if (currentEvent->getCurrentState() != EventUtil::ST_SLEEPING) {
			clog << "ApplicationExecutionObject::start current event '";
			clog << currentEvent->getId() << "' is already running!" << endl;
			return false;
		}

		if (currentEvent->instanceOf("AnchorEvent")) {
			contentAnchor = ((AnchorEvent*)currentEvent)->getAnchor();
			if (contentAnchor != NULL &&
					(contentAnchor->instanceOf("LabeledAnchor"))) {

				transMan->start(offsetTime);
				currentEvent->start();

				clog << "ApplicationExecutionObject::start current event '";
				clog << currentEvent->getId() << "' started!" << endl;
				return true;
			}
		}

		clog << "ApplicationExecutionObject::start starting transition ";
		clog << "manager!" << endl;
		transMan->start(offsetTime);

		if (currentEvent->getCurrentState() != EventUtil::ST_SLEEPING) {
			clog << "ApplicationExecutionObject::start YEAP! ";
			clog << "Current event is running!" << endl;
		}
		return true;
	}

	EventTransition* ApplicationExecutionObject::getNextTransition() {
		if (currentEvent == NULL ||
				currentEvent->getCurrentState() == EventUtil::ST_SLEEPING ||
				!currentEvent->instanceOf("PresentationEvent")) {

			return NULL;
		}

		return transMan->getNextTransition(currentEvent);
	}

	bool ApplicationExecutionObject::stop() {
		ContentAnchor* contentAnchor;
		double endTime;
		bool isLabeled = false;

		if (isSleeping()) {
			if (wholeContent != NULL &&
					wholeContent->getCurrentState() != EventUtil::ST_SLEEPING) {

				clog << "ApplicationExecutionObject::stop WHOLECONTENT" << endl;
				wholeContent->stop();
			}
			return false;
		}

		if (currentEvent->instanceOf("AnchorEvent")) {
			contentAnchor = ((AnchorEvent*)currentEvent)->getAnchor();
			if (contentAnchor != NULL &&
					contentAnchor->instanceOf("LabeledAnchor")) {

				isLabeled = true;

				clog << "ApplicationExecutionObject::stop stopping event '";
				clog << contentAnchor->getId() << "'" << endl;
				currentEvent->stop();
			}
		}

		if (!isLabeled && currentEvent->instanceOf("PresentationEvent")) {
			endTime = ((PresentationEvent*)currentEvent)->getEnd();
			currentEvent->stop();
			if (endTime > 0) {
				transMan->stop(endTime, true);
			}
		}

		transMan->resetTimeIndex();
		pauseCount = 0;

		return true;
	}

	bool ApplicationExecutionObject::abort() {
		vector<FormatterEvent*>* evs;
		vector<FormatterEvent*>::iterator i;
		FormatterEvent* ev;
		ContentAnchor* contentAnchor;
		double endTime;
		bool isLabeled = false;

		if (isSleeping()) {
			if (wholeContent != NULL &&
					wholeContent->getCurrentState() != EventUtil::ST_SLEEPING) {

				wholeContent->abort();
			}
			return false;
		}

		if (currentEvent == wholeContent) {
			evs = getEvents();
			if (evs != NULL) {
				i = evs->begin();
				while (i != evs->end()) {
					ev = (*i);

					if (ev->instanceOf("AnchorEvent")) {
						contentAnchor = ((AnchorEvent*)ev)->getAnchor();
						if (contentAnchor != NULL &&
								contentAnchor->instanceOf("LabeledAnchor")) {

							isLabeled = true;

							clog << "ApplicationExecutionObject::abort event '";
							clog << contentAnchor->getId() << "'" << endl;
							ev->abort();
						}
					}

					if (!isLabeled && ev->instanceOf("PresentationEvent")) {
						endTime = ((PresentationEvent*)ev)->getEnd();
						ev->abort();
						if (endTime > 0) {
							transMan->abort(endTime, true);
						}
					}

					++i;
				}
				delete evs;
				evs = NULL;
			}
			transMan->resetTimeIndex();
			pauseCount = 0;

		} else {
			if (currentEvent->instanceOf("AnchorEvent")) {
				contentAnchor = ((AnchorEvent*)currentEvent)->getAnchor();
				if (contentAnchor != NULL &&
						contentAnchor->instanceOf("LabeledAnchor")) {

					isLabeled = true;

					clog << "ApplicationExecutionObject::abort event '";
					clog << contentAnchor->getId() << "'" << endl;
					currentEvent->abort();
				}
			}

			if (!isLabeled && currentEvent->instanceOf("PresentationEvent")) {
				endTime = ((PresentationEvent*)currentEvent)->getEnd();
				currentEvent->abort();
				if (endTime > 0) {
					transMan->abort(endTime, true);
				}
			}
		}

		return true;
	}

	bool ApplicationExecutionObject::pause() {
		FormatterEvent* ev;
		vector<FormatterEvent*>* evs;
		vector<FormatterEvent*>::iterator i;

		lockEvents();
		if (currentEvent == NULL ||
				currentEvent->getCurrentState() != EventUtil::ST_OCCURRING ||
				preparedEvents.count(currentEvent->getId()) == 0) {

			unlockEvents();
			return false;
		}
		unlockEvents();

		if (currentEvent == wholeContent) {
			evs = getEvents();
			if (evs != NULL) {
				if (pauseCount == 0) {
					i = evs->begin();
					while (i != evs->end()) {
						ev = *i;
						if (ev->getCurrentState() == EventUtil::ST_OCCURRING) {
							ev->pause();
						}
						++i;
					}
				}
				delete evs;
				evs = NULL;
			}

			pauseCount++;

		} else if (currentEvent->getCurrentState() == EventUtil::ST_OCCURRING) {
			currentEvent->pause();
		}

		return true;
	}

	bool ApplicationExecutionObject::resume() {
		FormatterEvent* event;
		vector<FormatterEvent*>* evs;
		vector<FormatterEvent*>::iterator i;

		if (currentEvent == wholeContent) {
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

		} else if (currentEvent->getCurrentState() == EventUtil::ST_PAUSED) {
			currentEvent->resume();
		}

		return true;
	}

	bool ApplicationExecutionObject::unprepare() {
		map<Node*, void*>::iterator i;
		map<string, FormatterEvent*>::iterator j;

		//clog << "ApplicationExecutionObject::unprepare(" << id << ")" << endl;

		lockEvents();
		if (currentEvent == NULL ||
				currentEvent->getCurrentState() != EventUtil::ST_SLEEPING ||
				preparedEvents.count(currentEvent->getId()) == 0) {

			unlockEvents();
			return false;
		}
		unlockEvents();

		if (currentEvent->instanceOf("AnchorEvent") &&
				((AnchorEvent*)currentEvent)->getAnchor() != NULL &&
				((AnchorEvent*)currentEvent)->getAnchor()->instanceOf(
						"LambdaAnchor")) {

			unprepareEvents();
		}

		removeParentListenersFromEvent(currentEvent);

		lockEvents();
		j = preparedEvents.find(currentEvent->getId());
		if (j != preparedEvents.end()) {
			preparedEvents.erase(j);
		}
		unlockEvents();

		return true;
	}

	void ApplicationExecutionObject::unprepareEvents() {
		FormatterEvent* event;
		vector<FormatterEvent*>* evs;
		vector<FormatterEvent*>::iterator i;

		evs = getEvents();
		if (evs != NULL) {
			i = evs->begin();
			while (i != evs->end()) {
				event = *i;
				if (event->getCurrentState() != EventUtil::ST_SLEEPING) {
					event->stop();
				}
				++i;
			}
			delete evs;
			evs = NULL;
		}
	}

	void ApplicationExecutionObject::removeEventListeners() {
		FormatterEvent* event;
		vector<FormatterEvent*>* evs;
		vector<FormatterEvent*>::iterator i;

		evs = getEvents();
		if (evs != NULL) {
			i = evs->begin();
			while (i != evs->end()) {
				event = *i;
				removeParentListenersFromEvent(event);
				++i;
			}
			delete evs;
			evs = NULL;
		}
	}

	void ApplicationExecutionObject::removeParentObject(
			Node* parentNode, void* parentObject) {

		map<Node*, void*>::iterator i;
		map<string, FormatterEvent*>::iterator j;

		lockEvents();
		if (mainEvent != NULL) {
			mainEvent->removeEventListener(
					(CompositeExecutionObject*)parentObject);
		}

		j = preparedEvents.begin();
		while (j != preparedEvents.end()) {
			j->second->removeEventListener(
					(CompositeExecutionObject*)parentObject);

			++j;
		}
		unlockEvents();

		ExecutionObject::removeParentObject(parentNode, parentObject);
	}

	void ApplicationExecutionObject::removeParentListenersFromEvent(
			FormatterEvent* event) {

		map<Node*, void*>::iterator i;
		CompositeExecutionObject* parentObject;

		i = parentTable.begin();
		while (i != parentTable.end()) {
			parentObject = (CompositeExecutionObject*)(i->second);
			// unregister parent as a currentEvent listener
			event->removeEventListener(parentObject);
			++i;
		}
	}

	void ApplicationExecutionObject::lockEvents() {
		Thread::mutexLock(&eventsMutex);
	}

	void ApplicationExecutionObject::unlockEvents() {
		Thread::mutexUnlock(&eventsMutex);
	}
}
}
}
}
}
}
}
