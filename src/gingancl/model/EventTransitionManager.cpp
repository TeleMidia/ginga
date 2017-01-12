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

#include "EventTransitionManager.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace event {
namespace transition {
	EventTransitionManager::EventTransitionManager() {
		Thread::mutexInit(&transMutex, false);
	}

	EventTransitionManager::~EventTransitionManager() {
		map<short int, vector<EventTransition*>*>::iterator i;
		vector<EventTransition*>::iterator j;

		Thread::mutexLock(&transMutex);

		i = transTable.begin();
		while (i != transTable.end()) {
			j = i->second->begin();
			while (j != i->second->end()) {
				delete *j;
				++j;
			}
			delete i->second;
			++i;
		}

		transTable.clear();
		currentTransitionIndex.clear();
		startTransitionIndex.clear();

		Thread::mutexUnlock(&transMutex);
		Thread::mutexDestroy(&transMutex);
	}

	short int EventTransitionManager::getType(PresentationEvent* event) {
		ContentAnchor* anchor;
		short int valueSyntax;

		anchor = event->getAnchor();
		if (anchor->instanceOf("RelativeTimeIntervalAnchor") ||
				anchor->instanceOf("LambdaAnchor") ||
				anchor->instanceOf("LabeledAnchor")) {

			return ContentAnchor::CAT_TIME;

		} else if (anchor->instanceOf("SampleIntervalAnchor")) {
			((SampleIntervalAnchor*)anchor)->getValueSyntax(
					&valueSyntax, &valueSyntax);

			return valueSyntax;

		} else {
			return ContentAnchor::CAT_NONE;
		}
	}

	vector<EventTransition*>* EventTransitionManager::getTransitionEvents(
			short int type) {

		vector<EventTransition*>* transitionEvents;
		map<short int, vector<EventTransition*>*>::iterator i;

		if (type == ContentAnchor::CAT_ALL || type == ContentAnchor::CAT_NONE) {
			return NULL;
		}

		Thread::mutexLock(&transMutex);
		i = transTable.find(type);
		if (i == transTable.end()) {
			transitionEvents = new vector<EventTransition*>;
			transTable[type] = transitionEvents;

		} else {
			transitionEvents = i->second;
		}
		Thread::mutexUnlock(&transMutex);

		return transitionEvents;
	}

	void EventTransitionManager::addEventTransition(
			EventTransition* transition, short int type) {

		int beg, end, pos;
		EventTransition* auxTransition;
		vector<EventTransition*>* transitionEvents;

		transitionEvents = getTransitionEvents(type);

		if (transitionEvents == NULL) {
			clog << "EventTransitionManager::addEventTransition Can't add ";
			clog << "transition, transitionEvents is NULL for type '";
			clog << type << "'" << endl;
			return;
		}

		// binary search
		beg = 0;
		end = transitionEvents->size() - 1;
		while (beg <= end) {
			pos = (beg + end) / 2;
			auxTransition = (*transitionEvents)[pos];
			switch (transition->compareTo(auxTransition)) {
				case 0:
					clog << "EventTransitionManager::addEventTransition ";
					clog << "event transition already exists" << endl;
					return;

				case -1:
					end = pos - 1;
					break;

				case 1:
					beg = pos + 1;
					break;
			}
		}

		clog << "EventTransitionManager::addEventTransition(";
		clog << this << ") for event '";
		clog << transition->getEvent()->getId() << "': begin = '";
		clog << transition->getEvent()->getBegin() << "'; end = '";
		clog << transition->getEvent()->getEnd() << "'; pos = '";
		clog << beg << "'; type = '" << type << "'. Transition time = '";
		clog << transition->getTime() << "'" << endl;

		transitionEvents->insert((transitionEvents->begin() + beg), transition);
	}

	void EventTransitionManager::removeEventTransition(
			PresentationEvent* event) {

		int i, size = -1;
		vector<EventTransition*>::iterator j;
		EventTransition* transition;
		EventTransition* endTransition;
		vector<EventTransition*>* transitionEvents;
		short int type;

		type             = getType(event);
		transitionEvents = getTransitionEvents(type);

		if (transitionEvents != NULL) {
			size = transitionEvents->size();
		}

		for (i = 0; i < size; i++) {
			transition = (*transitionEvents)[i];
			if (transition->getEvent() == event) {
				if (transition->instanceOf("BeginEventTransition") &&
					    ((BeginEventTransition*)transition)->
					    getEndTransition() != NULL) {

					endTransition = ((BeginEventTransition*)transition)->
						    getEndTransition();

					for (j = transitionEvents->begin();
						    j != transitionEvents->end(); ++j) {

						if (*j == endTransition) {
							transitionEvents->erase(j);
							size = transitionEvents->size();
							i = 0;
							break;
						}
					}
				}

				for (j = transitionEvents->begin();
					    j != transitionEvents->end(); ++j) {

					if (*j == transition) {
						transitionEvents->erase(j);
						size = transitionEvents->size();
						i = 0;
						break;
					}
				}
			}
		}
	}

	void EventTransitionManager::resetTimeIndex() {
		map<short int, int>::iterator i;

		Thread::mutexLock(&transMutex);
		i = startTransitionIndex.begin();
		while (i != startTransitionIndex.end()) {
			currentTransitionIndex[i->first] = i->second;
			++i;
		}
		Thread::mutexUnlock(&transMutex);
	}

	void EventTransitionManager::resetTimeIndexByType(short int type) {
		map<short int, int>::iterator i;

		Thread::mutexLock(&transMutex);
		i = startTransitionIndex.find(type);
		if (i != startTransitionIndex.end()) {
			currentTransitionIndex[type] = i->second;
		}
		Thread::mutexUnlock(&transMutex);
	}

	void EventTransitionManager::prepare(
			bool wholeContent, double startTime, short int type) {

		vector<EventTransition*>* transitionEvents;
		EventTransition* transition;
		unsigned int transIx, size;

		if (wholeContent && startTime == 0.0) {
			Thread::mutexLock(&transMutex);
			startTransitionIndex[type] = 0;
			Thread::mutexUnlock(&transMutex);

		} else {
			transitionEvents = getTransitionEvents(type);
			size             = transitionEvents->size();
			transIx          = 0;
			Thread::mutexLock(&transMutex);
			startTransitionIndex[type] = transIx;
			Thread::mutexUnlock(&transMutex);
			while (transIx < size) {
				transition = (*transitionEvents)[transIx];
				if (transition->getTime() >= startTime) {
					break;
				}

				if (transition->instanceOf("BeginEventTransition")) {
					transition->getEvent()->
						    setCurrentState(EventUtil::ST_OCCURRING);

				} else {
					clog << "EventTransitionManager::prepare set '";
					clog << transition->getEvent()->getId();
					clog << "' to SLEEP" << endl;

					transition->getEvent()->
						    setCurrentState(EventUtil::ST_SLEEPING);

					transition->getEvent()->incrementOccurrences();
				}
				transIx++;
				Thread::mutexLock(&transMutex);
				startTransitionIndex[type] = transIx;
				Thread::mutexUnlock(&transMutex);
			}
		}

		resetTimeIndex();
	}

	void EventTransitionManager::start(double offsetTime) {
		vector<EventTransition*>* transitionEvents;
		EventTransition* transition;
		unsigned int transIx, size;

		transitionEvents = getTransitionEvents(ContentAnchor::CAT_TIME);
		size             = transitionEvents->size();

		Thread::mutexLock(&transMutex);
		if (currentTransitionIndex.count(ContentAnchor::CAT_TIME) == 0) {
			currentTransitionIndex[ContentAnchor::CAT_TIME] = 0;
		}

		transIx = currentTransitionIndex[ContentAnchor::CAT_TIME];
		Thread::mutexUnlock(&transMutex);

		while (transIx < size) {
			transition = (*transitionEvents)[transIx];
			if (transition->getTime() <= offsetTime) {
				if (transition->instanceOf("BeginEventTransition")) {
					transition->getEvent()->start();
				}
				transIx++;
				Thread::mutexLock(&transMutex);
				currentTransitionIndex[ContentAnchor::CAT_TIME] = transIx;
				Thread::mutexUnlock(&transMutex);

			} else {
				break;
			}
		}
	}

	void EventTransitionManager::stop(double endTime, bool applicationType) {
		vector<EventTransition*>* transitionEvents;
		vector<EventTransition*>::iterator i;
		EventTransition* transition;
		FormatterEvent* fev;

		transitionEvents = getTransitionEvents(ContentAnchor::CAT_TIME);

		i = transitionEvents->begin();
		while (i != transitionEvents->end()) {
			transition = *i;
			if (!applicationType ||
					(applicationType && !isInfinity(transition->getTime()))) {

				fev = transition->getEvent();
				if (transition->getTime() > endTime) {
					fev->setCurrentState(EventUtil::ST_SLEEPING);

				} else if (transition->instanceOf("EndEventTransition")) {
					fev->stop();
				}
			}
			++i;
		}
	}

	void EventTransitionManager::abort(double endTime, bool applicationType) {
		vector<EventTransition*>* transitionEvents;
		unsigned int transIx, i, size;
		EventTransition* transition;
		FormatterEvent* fev;

		Thread::mutexLock(&transMutex);
		if (currentTransitionIndex.count(ContentAnchor::CAT_TIME) == 0) {
			currentTransitionIndex[ContentAnchor::CAT_TIME] = 0;
		}

		transIx = currentTransitionIndex[ContentAnchor::CAT_TIME];
		Thread::mutexUnlock(&transMutex);

		transitionEvents = getTransitionEvents(ContentAnchor::CAT_TIME);
		size             = transitionEvents->size();

		for (i = transIx; i < size; i++) {
			transition = (*transitionEvents)[i];
			if (!applicationType ||
					(applicationType && !isInfinity(transition->getTime()))) {

				fev = transition->getEvent();
				if (transition->getTime() > endTime) {
					fev->setCurrentState(EventUtil::ST_SLEEPING);

				} else if (transition->instanceOf("EndEventTransition")) {
					fev->abort();
				}
			}
		}
	}

	void EventTransitionManager::addPresentationEvent(
			PresentationEvent* event) {

		double begin, end;
		BeginEventTransition* beginTransition;
		EndEventTransition* endTransition;
		vector<EventTransition*>* transitionEvents;
		EventTransition* lastTransition = NULL;
		vector<EventTransition*>::iterator i;
		double lTime;
		short int type;

		type = getType(event);
		transitionEvents = getTransitionEvents(type);

		if ((event->getAnchor())->instanceOf("LambdaAnchor")) {
			beginTransition  = new BeginEventTransition(0, event);
			transitionEvents->insert(transitionEvents->begin(), beginTransition);
			if (event->getEnd() >= 0) {
				endTransition = new EndEventTransition(
					    event->getEnd(), event, beginTransition);

				i = transitionEvents->begin();
				while (i != transitionEvents->end()) {
					lastTransition = *i;
					lTime = lastTransition->getTime();
					if (IntervalAnchor::isObjectDuration(lTime) ||
							endTransition->getTime() < lTime) {

						transitionEvents->insert(i, endTransition);
						break;
					}

					++i;

					if (i == transitionEvents->end()) {
						transitionEvents->push_back(endTransition);
						break;
					}
				}
			}

			/*clog << "EventTransitionManager::addPresentationEvent '";
			clog << event->getId() << "' (lambda): ";
			clog << "begin = '" << event->getBegin() << "'; end = '";
			clog << event->getEnd() << "'. BeginTransition = '";
			clog << beginTransition->getTime() << "'; endTransition = '";
			clog << endTransition->getTime() << "'";
			clog << endl;*/

		} else {
			begin = event->getBegin();

			// undefined events are not inserted into transition table
			if (PresentationEvent::isUndefinedInstant(begin)) {
				clog << "EventTransitionManager::addPresentationEvent ";
				clog << "Can't add event '" << event->getId() << "': ";
				clog << "undefined begin" << endl;
				return;
			}

			beginTransition = new BeginEventTransition(begin, event);
			addEventTransition(beginTransition, type);
			end = event->getEnd();

			if (!PresentationEvent::isUndefinedInstant(end)) {
				endTransition = new EndEventTransition(
					    end, event, beginTransition);

				addEventTransition(endTransition, type);

			} else {
				clog << "EventTransitionManager::addPresentationEvent ";
				clog << "Can't add event '" << event->getId() << "': ";
				clog << "undefined end" << endl;
			}
		}
	}

	void EventTransitionManager::timeBaseNaturalEnd(
			int64_t timeValue,
			FormatterEvent* mainEvent,
			short int transType) {

		PresentationEvent* ev;
		vector<EventTransition*>* transitionEvents;
		vector<EventTransition*>::iterator i;

		Thread::mutexLock(&transMutex);

		clog << "EventTransitionManager::timeBaseNaturalEnd" << endl;
		if (transTable.count(transType) != 0) {
			transitionEvents = transTable[transType];

			i = transitionEvents->begin();
			while (i != transitionEvents->end()) {
				ev = (*i)->getEvent();

				if (ev->getCurrentState() == EventUtil::ST_OCCURRING) {
					if (ev->getEnd() > timeValue) {
						ev->setCurrentState(EventUtil::ST_SLEEPING);

					} else {
						ev->stop();
					}
				}

				++i;
			}
		}

		Thread::mutexUnlock(&transMutex);

		mainEvent->stop();
	}

	void EventTransitionManager::updateTransitionTable(
			double value,
			IPlayer* player,
			FormatterEvent* mainEvent,
			short int transType) {

		EventTransition* transition;
		FormatterEvent* ev;
		vector<EventTransition*>* transitionEvents;
		unsigned int currentIx;

		Thread::mutexLock(&transMutex);
		if (currentTransitionIndex.count(transType) == 0 ||
				transTable.count(transType) == 0) {

			Thread::mutexUnlock(&transMutex);

			clog << "EventTransitionManager::updateTransitionTable ";
			clog << "nothing to do." << endl;
			return;
		}

		transitionEvents = transTable[transType];
		currentIx        = currentTransitionIndex[transType];
		Thread::mutexUnlock(&transMutex);

		while (currentIx < transitionEvents->size()) {
			transition = (*transitionEvents)[currentIx];

			if (transition->getTime() <= value) {
				ev = transition->getEvent();
				if (transition->instanceOf("BeginEventTransition")) {
					clog << "EventTransitionManager::updateTransitionTable ";
					clog << "starting event '" << ev->getId() << "' ";
					clog << "current state '" << ev->getCurrentState();
					clog << "'" << endl;

					ev->start();

				} else {
					if (ev == mainEvent && player != NULL) {
						player->setPresented(true);
					}

					clog << "EventTransitionManager::updateTransitionTable ";
					clog << "stopping event '" << ev->getId() << "' ";
					clog << "current state '" << ev->getCurrentState();
					clog << "'" << endl;

					ev->stop();
				}

				currentIx++;
				Thread::mutexLock(&transMutex);
				currentTransitionIndex[transType] = currentIx;
				Thread::mutexUnlock(&transMutex);

			} else {
				break;
			}
		}
	}

	set<double>* EventTransitionManager::getTransitionsValues(
			short int transType) {

		set<double>* transValues;
		unsigned int currentIx, ix;
		vector<EventTransition*>* transitionEvents;
		vector<EventTransition*>::iterator i;

		Thread::mutexLock(&transMutex);
		if (transTable.count(transType) == 0) {
			Thread::mutexUnlock(&transMutex);
			return NULL;
		}

		if (currentTransitionIndex.count(transType) == 0) {
			if (startTransitionIndex.count(transType) == 0) {
				currentTransitionIndex[transType] = 0;

			} else {
				currentTransitionIndex[transType] =
						startTransitionIndex[transType];
			}
		}

		Thread::mutexUnlock(&transMutex);
		transitionEvents = getTransitionEvents(transType);
		transValues      = new set<double>;

		Thread::mutexLock(&transMutex);
		currentIx        = currentTransitionIndex[transType];
		Thread::mutexUnlock(&transMutex);

		ix = 0;
		i  = transitionEvents->begin();
		while (i != transitionEvents->end()) {
			if (ix >= currentIx) {
				transValues->insert((*i)->getTime());
			}
			++ix;
			++i;
		}

		return transValues;
	}

	EventTransition* EventTransitionManager::getNextTransition(
			FormatterEvent* mainEvent) {

		EventTransition* transition;
		vector<EventTransition*>* transitionEvents;
		unsigned int currentIx;
		double transTime;
		double eventEnd;

		Thread::mutexLock(&transMutex);
		if (currentTransitionIndex.count(ContentAnchor::CAT_TIME) == 0 ||
				transTable.count(ContentAnchor::CAT_TIME) == 0) {

			clog << "EventTransitionManager::getNextTransition(" << this;
			clog << ") for '" << mainEvent->getId();
			clog << "'. There is no transition tables";
			clog << endl;
			Thread::mutexUnlock(&transMutex);
			return NULL;
		}

		transitionEvents = transTable[ContentAnchor::CAT_TIME];
		currentIx        = currentTransitionIndex[ContentAnchor::CAT_TIME];
		Thread::mutexUnlock(&transMutex);

		if (currentIx < transitionEvents->size()) {
			transition = transitionEvents->at(currentIx);

			eventEnd   = ((PresentationEvent*)mainEvent)->getEnd();
			transTime  = transition->getTime();

			/*clog << "EventTransitionManager::getNextTransition for '";
			clog << mainEvent->getId() << "'. EventEnd = '" << eventEnd;
			clog << "' transition time = '" << transTime << "'";
			clog << endl;*/

			if (IntervalAnchor::isObjectDuration(eventEnd) ||
					transTime <= eventEnd) {

				return transition;
			}
		}

		return NULL;
	}
}
}
}
}
}
}
}
}
