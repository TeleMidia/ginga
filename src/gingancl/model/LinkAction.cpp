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
#include "LinkAction.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace link {
	LinkAction::LinkAction() : Thread() {
		initLinkAction(0.0);
	}

	LinkAction::LinkAction(double delay) : Thread() {
		initLinkAction(delay);
	}

	LinkAction::~LinkAction() {
		isDeleting = true;

		Thread::mutexLock(&plMutex);
		if (progressionListeners != NULL) {
			delete progressionListeners;
			progressionListeners = NULL;
		}
		Thread::mutexUnlock(&plMutex);
		Thread::mutexDestroy(&plMutex);
	}

	void LinkAction::initLinkAction(double delay) {
		satisfiedCondition = NULL;
		this->delay = delay;
		progressionListeners = new vector<LinkActionProgressionListener*>;
		Thread::mutexInit(&plMutex, NULL);
		typeSet.insert("LinkAction");
	}

	bool LinkAction::instanceOf(string s) {
		if (typeSet.empty()) {
			return false;

		} else {
			return (typeSet.find(s) != typeSet.end());
		}
	}

	double LinkAction::getWaitDelay() {
		return this->delay;
	}

	void LinkAction::setSatisfiedCondition(
			void* satisfiedCondition) {

		this->satisfiedCondition = satisfiedCondition;
	}

	void LinkAction::run(void* satisfiedCondition) {
		this->satisfiedCondition = satisfiedCondition;
		run();
	}

	void LinkAction::run() {
		if (hasDelay()) {
			clog << "LinkAction::run uSleeping '" << delay*1000 << "'" << endl;
			SystemCompat::uSleep((long)(delay*1000));
		}
	}

	void LinkAction::setWaitDelay(double delay) {
		this->delay = delay;
	}

	bool LinkAction::hasDelay() {
		if (this->delay > 0) {
			return true;
		}
		return false;
	}

	void LinkAction::addActionProgressionListener(
		    LinkActionProgressionListener* listener) {

		vector<LinkActionProgressionListener*>::iterator i;

		if (tryLock()) {
			if (progressionListeners != NULL && !isDeleting) {
				i = progressionListeners->begin();
				while (i != progressionListeners->end()) {
					if (listener == *i) {
						clog << "LinkAction::addActionProgressionListener ";
						clog << "Warning! Trying to add the same listener twice";
						clog << endl;
						Thread::mutexUnlock(&plMutex);
						return;
					}
					++i;
				}
				progressionListeners->push_back(listener);
			}
			Thread::mutexUnlock(&plMutex);
		}
	}

	void LinkAction::removeActionProgressionListener(
		    LinkActionProgressionListener* listener) {

		vector<LinkActionProgressionListener*>::iterator i;

		if (tryLock()) {
			if (progressionListeners != NULL && !isDeleting) {
				for (i = progressionListeners->begin();
						i != progressionListeners->end();
						++i) {

					if (*i == listener) {
						progressionListeners->erase(i);
						break;
					}
				}
			}
			Thread::mutexUnlock(&plMutex);
		}
	}

	void LinkAction::notifyProgressionListeners(bool start) {
		int i, size;
		LinkActionProgressionListener* listener;
		vector<LinkActionProgressionListener*>* notifyList;

		if (tryLock()) {
			if (progressionListeners != NULL && !isDeleting) {
				notifyList = new vector<LinkActionProgressionListener*>(
						*progressionListeners);

				Thread::mutexUnlock(&plMutex);

				size = notifyList->size();
				for (i = 0; i < size; i++) {
					listener = (*notifyList)[i];
					listener->actionProcessed(start);
					if (isDeleting) {
						break;
					}
				}
				delete notifyList;

			} else {
				Thread::mutexUnlock(&plMutex);
			}
		}
	}

	bool LinkAction::tryLock() {
		if (isDeleting) {
			return false;
		}

		Thread::mutexLock(&plMutex);
		return true;
	}
}
}
}
}
}
}
}
