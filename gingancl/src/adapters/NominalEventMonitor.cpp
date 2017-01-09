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

#include "gingancl/adapters/NominalEventMonitor.h"

#include "ncl/interfaces/SampleIntervalAnchor.h"
using namespace ::br::pucrio::telemidia::ncl::interfaces;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace adapters {
	const double NominalEventMonitor::DEFAULT_SLEEP_TIME = 5000;
	const double NominalEventMonitor::DEFAULT_ERROR = 50.0;

	NominalEventMonitor::NominalEventMonitor(
			ExecutionObject* object, IPlayerAdapter* player) : Thread() {

		this->running           = false;
		this->adapter           = player;
		this->executionObject   = object;
		this->sleepTime         = DEFAULT_SLEEP_TIME;
		this->expectedSleepTime = 0;
		this->timeBaseId        = -1;
		this->timeBaseProvider  = NULL;
	}

	NominalEventMonitor::~NominalEventMonitor() {
		isDeleting = true;
		running	   = false;
		stopped	   = true;

		wakeUp();
		unlockConditionSatisfied();

		unregisterFromTimeBase();

		lock();
		adapter          = NULL;
		executionObject  = NULL;
		timeBaseProvider = NULL;
		unlock();
	}

	void NominalEventMonitor::unregisterFromTimeBase() {
		if (timeBaseProvider != NULL) {
			timeBaseProvider->removeTimeListener(timeBaseId, this);
			timeBaseProvider->removeIdListener(this);
			timeBaseProvider->removeLoopListener(timeBaseId, this);
		}
	}

	void NominalEventMonitor::setTimeBaseProvider(
			ITimeBaseProvider* timeBaseProvider) {

		if (timeBaseProvider == NULL) {
			clog << "NominalEventMonitor::setTimeBaseProvider ";
			clog << "nothing to do (1)." << endl;
			return;
		}

		if (this->timeBaseProvider != timeBaseProvider) {
			unregisterFromTimeBase();
		}

		this->timeBaseId       = timeBaseProvider->getOccurringTimeBaseId();
		this->timeBaseProvider = timeBaseProvider;

		timeBaseProvider->addIdListener(this);
		timeBaseProvider->addLoopListener(timeBaseId, this);

		prepareNptTransitionsEvents();
	}

	void NominalEventMonitor::prepareNptTransitionsEvents() {
		AttributionEvent* event;
		set<double>* transValues;
		set<double>::iterator i;

		executionObject->prepareTransitionEvents(
				ContentAnchor::CAT_NPT,
				timeBaseProvider->getCurrentTimeValue(timeBaseId));

		transValues = executionObject->getTransitionsValues(
				ContentAnchor::CAT_NPT);

		if (transValues == NULL || transValues->empty()) {
			if (transValues != NULL) {
				delete transValues;
			}

			clog << "NominalEventMonitor::prepareNptTransitionsEvents ";
			clog << "nothing to do (2)." << endl;
			return;
		}

		executionObject->updateTransitionTable(
				timeBaseProvider->getCurrentTimeValue(timeBaseId),
				NULL,
				ContentAnchor::CAT_NPT);

		i = transValues->begin();
		while (i != transValues->end()) {
			if (!isInfinity(*i)) {
				clog << "NominalEventMonitor::prepareNptTransitionsEvents ";
				clog << "add listener contentId '" << timeBaseId;
				clog << "' for value '" << *i;
				clog << "' (current time = '";
				clog << timeBaseProvider->getCurrentTimeValue(timeBaseId);
				clog << "')" << endl;

				timeBaseProvider->addTimeListener(timeBaseId, *i, this);
			}
			++i;
		}

		delete transValues;

		event = (AttributionEvent*)(executionObject->getEventFromAnchorId(
				"contentId"));

		if (event != NULL) {
			adapter->setPropertyValue(event, itos(timeBaseId));
			event->stop();
		}
	}

	void NominalEventMonitor::updateTimeBaseId(
			unsigned char oldContentId,
			unsigned char newContentId) {

		AttributionEvent* event;

		//checking if is an NPT loop
		if (oldContentId == newContentId) {
			return;
		}

		event = (AttributionEvent*)(
				executionObject->getEventFromAnchorId("standby"));

		clog << "NominalEventMonitor::updateTimeBaseId event = '";
		clog << event << "' old = '" << (oldContentId & 0xFF);
		clog << "' new = '" << (newContentId & 0xFF);
		clog << endl;

		if (event != NULL) {
			if (oldContentId == timeBaseId) {
				event->start();
				adapter->setPropertyValue(event, "true");

			} else if (newContentId == timeBaseId) {
				event->start();
				adapter->setPropertyValue(event, "false");
			}
			event->stop();
		}
	}

	void NominalEventMonitor::timeBaseNaturalEnd(
			unsigned char contentId, double timeValue) {

		if (contentId == timeBaseId) {
			clog << "NominalEventMonitor::timeBaseNaturalEnd ";
			clog << "timeBaseId '" << timeBaseId;
			clog << "' value '" << timeValue;
			clog << "'" << endl;

			executionObject->timeBaseNaturalEnd(
					timeValue, ContentAnchor::CAT_NPT);
		}
	}

	void NominalEventMonitor::loopDetected() {
		clog << "NominalEventMonitor::loopDetected" << endl;

		executionObject->resetTransitionEvents(ContentAnchor::CAT_NPT);
		prepareNptTransitionsEvents();
	}

	void NominalEventMonitor::valueReached(
			unsigned char timeBaseId, double timeValue) {

		if (this->timeBaseId != timeBaseId) {
			clog << "NominalEventMonitor::valueReached ";
			clog << "Warning! receiving wrong timeBaseId '" << timeBaseId;
			clog << "' (my timeBaseId = '" << this->timeBaseId;
			clog << "')" << endl;
			return;
		}

		clog << "NominalEventMonitor::valueReached ";
		clog << "timeBaseId '" << (timeBaseId & 0xFF);
		clog << "' value '" << timeValue;
		clog << "'" << endl;

		executionObject->updateTransitionTable(
				timeValue, adapter->getPlayer(), ContentAnchor::CAT_NPT);
	}

	void NominalEventMonitor::startMonitor() {
		set<double>* timeSet = NULL;

		clog << "NominalEventMonitor::startMonitor" << endl;

		if (!running && !isDeleting) {
			timeSet = executionObject->getTransitionsValues(
					ContentAnchor::CAT_TIME);

			if (timeSet != NULL) {
				// start monitor only if there is predictable events
				running = true;
				paused  = false;
				stopped = false;
				Thread::startThread();

				delete timeSet;
			}

		} else {
			clog << "NominalEventMonitor::startMonitor(";
			clog << this << ") Warning! Wrong way to ";
			clog << "start monitor: running = '" << running;
			clog << "' isDeleting = '" << isDeleting << "'" << endl;
		}
	}

	void NominalEventMonitor::pauseMonitor() {
		clog << "NominalEventMonitor::pauseMonitor" << endl;

		if (!isInfinity(expectedSleepTime)) {
			wakeUp();
			paused = true;
		}
	}

	void NominalEventMonitor::resumeMonitor() {
		if (!isInfinity(expectedSleepTime) && paused) {
			paused = false;
			unlockConditionSatisfied();
		}
	}

	void NominalEventMonitor::stopMonitor() {
		stopped = true;

		clog << "NominalEventMonitor::stopMonitor" << endl;

		unregisterFromTimeBase();

		if (running) {
			running = false;
			if (isInfinity(expectedSleepTime) || paused) {
				unlockConditionSatisfied();

			} else {
				wakeUp();
			}
		}
	}

	void NominalEventMonitor::run() {
		EventTransition* nextTransition = NULL;
		double time;
		double mediaTime = 0;
		double nextEntryTime;
		double outTransTime;

		lock();

		clog << "====== Anchor Monitor Activated for '";
		clog << executionObject->getId().c_str() << "' (";
		clog << this << " ======" << endl;

		while (running) {
			if (isDeleting) {
				break;
			}

			if (paused) {
				waitForUnlockCondition();

			} else {
				if (executionObject != NULL && !isDeleting) {
					nextTransition = executionObject->getNextTransition();

				} else {
					nextTransition = NULL;
					running = false;
				}

				if (nextTransition == NULL) {
					running = false;

				} else {
					outTransTime  = adapter->getOutTransTime();
					nextEntryTime = nextTransition->getTime();

					if (outTransTime > 0.0 && outTransTime < nextEntryTime) {
						Thread::mSleep((int)(outTransTime));
						adapter->getPlayer()->notifyPlayerListeners(
								IPlayer::PL_NOTIFY_OUTTRANS,
								"",
								IPlayer::TYPE_PRESENTATION,
								"");
					}

					if (isInfinity(nextEntryTime)) {
						expectedSleepTime = infinity();

					} else {
						mediaTime = (adapter->getPlayer()->getMediaTime()
								* 1000);

	  					expectedSleepTime = nextEntryTime - mediaTime;
					}

					clog << "ANCHORMONITOR NEXTTRANSITIONTIME = '";
					clog << nextEntryTime << "' MEDIATIME = '" << mediaTime;
					clog << "' EXPECTEDSLEEPTIME = '" << expectedSleepTime;
					clog << "' (" << this << ")" << endl;

					if (running && !isDeleting) {
						if (isInfinity(expectedSleepTime)) {
							waitForUnlockCondition();

						} else {
							Thread::mSleep((int)(expectedSleepTime));
						}

						if (running && !isDeleting) {
							if (executionObject == NULL || adapter == NULL) {
								unlock();
								return;
							}

							mediaTime = (adapter->getPlayer()->getMediaTime()
									* 1000);

							time = nextEntryTime - mediaTime;
							if (time < 0) {
								time = 0;
							}

							if (!paused && time <= DEFAULT_ERROR) {
								// efetua a transicao no estado do evento
								executionObject->updateTransitionTable(
									   mediaTime + DEFAULT_ERROR,
									   adapter->getPlayer(),
									   ContentAnchor::CAT_TIME);
							}
						}
					}
				}
			}
		}

		if (!stopped && adapter != NULL && !isDeleting &&
				!adapter->instanceOf("ProgramAVPlayerAdapter")) {

			clog << "NominalEventMonitor::run calling force natural end for '";
			clog << executionObject->getId() << "'" << endl;

			adapter->getPlayer()->forceNaturalEnd(true);
		}
		unlock();
	}
}
}
}
}
}
}
