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

#include "dataprocessing/dsmcc/npt/NPTProcessor.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace dataprocessing {
namespace dsmcc {
namespace npt {
NPTProcessor::NPTProcessor(ISTCProvider* stcProvider) : Thread() {

	this->stcProvider    = stcProvider;
	this->isFirstStc     = true;
	this->running        = true;
	this->currentCid     = INVALID_CID;
	this->loopControlMax = false;
	this->loopControlMin = false;
	this->reScheduleIt   = false;
	this->nptPrinter     = false;

	Thread::mutexInit(&loopMutex, false);
	Thread::mutexInit(&schedMutex, false);
	Thread::mutexInit(&lifeMutex, false);

	startThread();
}

NPTProcessor::~NPTProcessor() {
	map<unsigned char, map<TimeControl*, set<ITimeBaseProvider*>*>*>::iterator i;
	map<TimeControl*, set<ITimeBaseProvider*>*>::iterator j;
	set<ITimeBaseProvider*>::iterator k;
	map<unsigned char, set<ITimeBaseProvider*>*>::iterator l;

	clearTables();

	lock();

	i = timeListeners.begin();
	while (i != timeListeners.end()) {
		j = i->second->begin();
		while (j != i->second->end()) {
			assert (j->first != NULL);
			assert (j->second != NULL);

			delete j->first;
			delete j->second;

			++j;
		}
		delete i->second;
		++i;
	}
	timeListeners.clear();

	unlock();

	cidListeners.clear();

	Thread::mutexLock(&loopMutex);
	loopListeners.clear();
	Thread::mutexUnlock(&loopMutex);
	Thread::mutexDestroy(&loopMutex);

	Thread::mutexLock(&schedMutex);
	Thread::mutexUnlock(&schedMutex);
	Thread::mutexDestroy(&schedMutex);

	Thread::mutexLock(&lifeMutex);
	Thread::mutexUnlock(&lifeMutex);
	Thread::mutexDestroy(&lifeMutex);
}

void NPTProcessor::setNptPrinter(bool nptPrinter) {
	this->nptPrinter = nptPrinter;
}

void NPTProcessor::clearTables() {
	map<unsigned char, NPTReference*>::iterator  i;
	map<unsigned char, TimeBaseClock*>::iterator j;
	map<unsigned char, Stc*>::iterator k;

	lock();

	map<unsigned char, map<TimeControl*, set<ITimeBaseProvider*>*>*>::iterator a;
	map<TimeControl*, set<ITimeBaseProvider*>*>::iterator b;
	a = timeListeners.begin();
	while (a != timeListeners.end()) {
		b = a->second->begin();
		while (b != a->second->end()) {
			assert (b->first != NULL);
			assert (b->second != NULL);

			delete b->first;
			delete b->second;

			++b;
		}
		delete a->second;
		++a;
	}
	timeListeners.clear();

	unlock();

	Thread::mutexLock(&schedMutex);
	i = scheduledNpts.begin();
	while (i != scheduledNpts.end()) {
		delete i->second;
		++i;
	}
	scheduledNpts.clear();
	Thread::mutexUnlock(&schedMutex);

	j = timeBaseClock.begin();
	while (j != timeBaseClock.end()) {
		delete j->second;
		++j;
	}
	timeBaseClock.clear();

	Thread::mutexLock(&lifeMutex);
	k = timeBaseLife.begin();
	while (k != timeBaseLife.end()) {
		delete k->second;
		++k;
	}
	timeBaseLife.clear();
	Thread::mutexUnlock(&lifeMutex);
}

void NPTProcessor::clearUnusedTimebase() {
	map<unsigned char, Stc*>::iterator itLife;
	map<unsigned char, TimeBaseClock*>::iterator i;
	TimeBaseClock* clk;
	bool restart = true;

	while (restart) {
		restart = false;
		i = timeBaseClock.begin();
		while (i != timeBaseClock.end()) {
			clk = i->second;
			if ((clk->getEndpointAvailable()) &&
					(clk->getStcBase() >= clk->getStopNpt())) {

				notifyNaturalEndListeners(
						clk->getContentId(), clk->getStopNpt());

				clog << "NPTProcessor::clearUnusedTimebase - Deleted by ";
				clog << "endpoint: CID = " << (clk->getContentId() & 0xFF);
				clog << endl;

				delete i->second;
				timeBaseClock.erase(i);
				restart = true;
			}
			++i;
		}
	}

	restart = true;

	//TODO: Timebase should still be incremented after 1 second without no
	//      NPT Reference updates?
	while (restart) {
		restart = false;
		Thread::mutexLock(&lifeMutex);
		itLife = timeBaseLife.begin();
		if (itLife != timeBaseLife.end()) {
			if (itLife->second->getStcBase() > 5400000) {// 1 minute
				clk = getTimeBaseClock(itLife->first);
				notifyNaturalEndListeners(
						clk->getContentId(), clk->getBaseToSecond());

				clog << "NPTProcessor::clearUnusedTimebase - Deleted by ";
				clog << "lifetime: CID = " << (clk->getContentId() & 0xFF);
				clog << endl;

				delete itLife->second;
				timeBaseLife.erase(itLife);
				restart = true;
			}
			++itLife;
		}
		Thread::mutexUnlock(&lifeMutex);
	}
}

uint64_t NPTProcessor::getSTCValue() {
	uint64_t stcValue;
	int valueType;

	stcProvider->getSTCValue(&stcValue, &valueType);
	if (valueType == ST_27MHz) {
		stcValue = Stc::stcToBase(stcValue);
	}

	return stcValue;
}

bool NPTProcessor::addLoopListener(unsigned char cid, ITimeBaseListener* ltn) {
	map<unsigned char, set<ITimeBaseProvider*>*>::iterator i;
	set<ITimeBaseProvider*>* listeners;

	clog << "NPTProcessor::addLoopListener" << endl;

	Thread::mutexLock(&loopMutex);
	i = loopListeners.find(cid);
	if (i != loopListeners.end()) {
		listeners = i->second;

	} else {
		listeners = new set<ITimeBaseProvider*>;
		loopListeners[cid] = listeners;
	}

	listeners->insert((ITimeBaseProvider*)ltn);
	Thread::mutexUnlock(&loopMutex);

	return true;
}

bool NPTProcessor::removeLoopListener(unsigned char cid, ITimeBaseListener* ltn) {
	map<unsigned char, set<ITimeBaseProvider*>*>::iterator i;
	set<ITimeBaseProvider*>::iterator j;

	clog << "NPTProcessor::removeLoopListener() - cid = " << (cid & 0xFF) << endl;

	Thread::mutexLock(&loopMutex);

	i = loopListeners.find(cid);
	if (i != loopListeners.end()) {
		j = i->second->find((ITimeBaseProvider*)ltn);
		if (j == i->second->end()) {
			Thread::mutexUnlock(&loopMutex);
			return false;
		} else {
			i->second->erase(j);
		}
	}

	Thread::mutexUnlock(&loopMutex);

	unlockConditionSatisfied();
	wakeUp();

	return true;
}

bool NPTProcessor::addTimeListener(
		unsigned char cid, double nptValue, ITimeBaseListener* ltn) {

	map<unsigned char, map<TimeControl*, set<ITimeBaseProvider*>*>*>::iterator i;
	map<TimeControl*, set<ITimeBaseProvider*>*>::iterator j;
	set<ITimeBaseProvider*>::iterator k;

	map<TimeControl*, set<ITimeBaseProvider*>*>* valueListeners;
	set<ITimeBaseProvider*>* setListeners;
	bool added;
	TimeControl* tc;

	clog << "NPTProcessor::addTimeListener TIME = " << nptValue << endl;

	lock();
	i = timeListeners.find(cid);
	if (i == timeListeners.end()) {
		valueListeners = new map<TimeControl*, set<ITimeBaseProvider*>*>;
		timeListeners[cid] = valueListeners;

	} else {
		valueListeners = i->second;
	}

	j = valueListeners->begin();
	while (j != valueListeners->end()) {
		if (j->first->time == nptValue) {
			break;
		}		
		++j;
	}

	if (j == valueListeners->end()) {
		setListeners = new set<ITimeBaseProvider*>;
		tc = new TimeControl();
		tc->time = nptValue;
		tc->notified = false;
		(*valueListeners)[tc] = setListeners;

	} else {
		setListeners = j->second;
	}

	k = setListeners->find((ITimeBaseProvider*)ltn);
	added = (k == setListeners->end());
	setListeners->insert((ITimeBaseProvider*)ltn);
	unlock();

	unlockConditionSatisfied();
	wakeUp();
	return added;
}

bool NPTProcessor::removeTimeListener(
		unsigned char cid, ITimeBaseListener* ltn) {

	map<unsigned char, map<TimeControl*, set<ITimeBaseProvider*>*>*>::iterator i;
	map<TimeControl*, set<ITimeBaseProvider*>*>::iterator j;
	set<ITimeBaseProvider*>::iterator k;

	clog << "NPTProcessor::removeTimeListener()" << endl;

	lock();
	i = timeListeners.find(cid);
	if (i != timeListeners.end()) {
		j = i->second->begin();
		while (j != i->second->end()) {
			k = j->second->find((ITimeBaseProvider*)ltn);
			if (k != j->second->end()) {
				j->second->erase(k);
				if (j->second->empty()) {
					delete j->second;
					i->second->erase(j);
					j = i->second->begin();
				}
			}
			++j;
		}
		unlockConditionSatisfied();
		wakeUp();
		unlock();
		return true;
	}

	unlock();
	return false;
}

bool NPTProcessor::addIdListener(ITimeBaseListener* ltn) {
	set<ITimeBaseProvider*>::iterator i;
	bool added;

	i = cidListeners.find((ITimeBaseProvider*)ltn);
	added = (i == cidListeners.end());
	cidListeners.insert((ITimeBaseProvider*)ltn);

	unlockConditionSatisfied();
	wakeUp();

	clog << "NPTProcessor::addIdListener" << endl;
	return added;
}

bool NPTProcessor::removeIdListener(ITimeBaseListener* ltn) {
	set<ITimeBaseProvider*>::iterator i;

	clog << "NPTProcessor::removeIdListener()" << endl;

	i = cidListeners.find((ITimeBaseProvider*)ltn);
	if (i == cidListeners.end()) {
		return false;
	}

	cidListeners.erase(i);

	unlockConditionSatisfied();
	wakeUp();

	return true;
}

unsigned char NPTProcessor::getCurrentTimeBaseId() {
	map<unsigned char, TimeBaseClock*>::iterator i;
	TimeBaseClock* clk;

	i = timeBaseClock.begin();
	while (i != timeBaseClock.end()) {
		clk = i->second;
		if (clk->getScaleNumerator() != 0) {
			return i->first;
		}
		++i;
	}

	return INVALID_CID;
}

TimeBaseClock* NPTProcessor::getCurrentTimebase() {
	map<unsigned char, TimeBaseClock*>::iterator i;
	TimeBaseClock* clk;

	i = timeBaseClock.begin();
	while (i != timeBaseClock.end()) {
		clk = i->second;
		if (clk->getScaleNumerator()) {
			return i->second;
		}
		++i;
	}

	return NULL;
}

void NPTProcessor::notifyLoopToTimeListeners() {
	map<unsigned char, set<ITimeBaseProvider*>*>::iterator i;
	set<ITimeBaseProvider*>::iterator j;

	Thread::mutexLock(&loopMutex);
	i = loopListeners.begin();
	if (i != loopListeners.end()) {
		j = i->second->begin();
		while (j != i->second->end()) {
			clog << "NPTProcessor::notifyLoopToTimeListeners ";
			clog << "CALL loop detected" << endl;
			((ITimeBaseListener*)(*j))->loopDetected();
			++j;
		}
		++i;
	}
	Thread::mutexUnlock(&loopMutex);
}

void NPTProcessor::notifyNaturalEndListeners(unsigned char cid, double nptValue) {
	map<unsigned char, set<ITimeBaseProvider*>*>::iterator i;
	set<ITimeBaseProvider*>::iterator j;

	Thread::mutexLock(&loopMutex);

	i = loopListeners.find(cid);
	if (i != loopListeners.end()) {
		j = i->second->begin();
		while (j != i->second->end()) {
			clog << "NPTProcessor::notifyNaturalEndListeners ";
			clog << "cid " << (cid & 0xFF) << ", nptValue = " << nptValue;
			clog << endl;
			((ITimeBaseListener*)(*j))->timeBaseNaturalEnd(cid, nptValue);
			++j;
		}
	}

	Thread::mutexUnlock(&loopMutex);
}

void NPTProcessor::notifyTimeListeners(unsigned char cid, double nptValue) {
	map<unsigned char, map<TimeControl*, set<ITimeBaseProvider*>*>*>::iterator i;
	map<TimeControl*, set<ITimeBaseProvider*>*>::iterator j;
	set<ITimeBaseProvider*>::iterator k;

	clog << "NPTProcessor::notifyTimeListeners " << "cid '" << (cid & 0xFF);
	clog << "' nptvalue '" << nptValue << "'" << endl;

	i = timeListeners.find(cid);
	if (i != timeListeners.end()) {
		j = i->second->begin();
		while (j != i->second->end()) {
			if (j->first->time == nptValue) {
				break;
			}		
			++j;
		}
		if (j != i->second->end()) {
			k = j->second->begin();
			while (k != j->second->end()) {
				((ITimeBaseListener*)(*k))->valueReached(cid, nptValue);
				++k;
			}
		}
	}
}

void NPTProcessor::notifyIdListeners(
		unsigned char oldCid, unsigned char newCid) {

	set<ITimeBaseProvider*>::iterator i;

	i = cidListeners.begin();
	while (i != cidListeners.end()) {
		clog << "NPTProcessor::notifyIdListeners" << endl;
		((ITimeBaseListener*)(*i))->updateTimeBaseId(oldCid, newCid);
		++i;
	}
}

TimeBaseClock* NPTProcessor::getTimeBaseClock(unsigned char cid) {
	map<unsigned char, TimeBaseClock*>::iterator i;

	i = timeBaseClock.find(cid);
	if (i != timeBaseClock.end()) {
		return i->second;
	}

	return NULL;
}

int NPTProcessor::updateTimeBase(TimeBaseClock* clk, NPTReference* npt) {
	uint64_t value, stcValue;

	if ((!clk) || (!npt)) {
		return -1;
	}

	stcValue = getSTCValue();
	if (stcValue >= npt->getStcRef()) {
		value = stcValue - npt->getStcRef();
		value = TimeBaseClock::convertToNpt(value,
				npt->getScaleNumerator(),
				npt->getScaleDenominator());
		value = npt->getNptRef() + value;
	} else {
		value = npt->getStcRef() - stcValue;
		value = TimeBaseClock::convertToNpt(value,
				npt->getScaleNumerator(),
				npt->getScaleDenominator());
		if (npt->getNptRef() >= value) {
			value = npt->getNptRef() - value;
		} else {
			value = npt->getNptRef();
		}
	}
	clk->setContentId(npt->getContentId());
	clk->setScaleNumerator(npt->getScaleNumerator());
	clk->setScaleDenominator(npt->getScaleDenominator());
	clk->setReference(value, 0);

	return 0;
}

double NPTProcessor::getCurrentTimeValue(unsigned char timeBaseId) {
	map<unsigned char, TimeBaseClock*>::iterator i;
	TimeBaseClock* clk;

	i = timeBaseClock.find(timeBaseId);
	if (i != timeBaseClock.end()) {
		clk = i->second;
		return clk->getBaseToSecond();
	}

	return 0;
}

void NPTProcessor::detectLoop() {
	map<unsigned char, Stc*>::iterator i;

	if (getSTCValue()) {
		if (isFirstStc) {
			isFirstStc = false;
			firstStc = getSTCValue();
		} else {
			if ((firstStc + 70000) > getSTCValue()) {
				Thread::mutexLock(&lifeMutex);
				i = timeBaseLife.begin();
				while (i != timeBaseLife.end()) {
					notifyNaturalEndListeners(
							i->first,
							getTimeBaseClock(i->first)->getBaseToSecond());

					++i;
				}
				Thread::mutexUnlock(&lifeMutex);
				//possible loop in TS
				isFirstStc = true;
				clearTables();
				if (loopListeners.size()) {
					notifyLoopToTimeListeners();
				}
			}
		}
	}
}

int NPTProcessor::decodeDescriptors(vector<MpegDescriptor*>* list) {
	vector<MpegDescriptor*>::iterator it;
	TimeBaseClock* clk = NULL;
	map<unsigned char, TimeBaseClock*>::iterator itBase;
	map<unsigned char, Stc*>::iterator itLife;
	vector<pair<bool, NPTReference*>*>::iterator itEvent;
	NPTEndpoint* nptEP = NULL;
	NPTReference* npt = NULL;
	NPTReference* newNpt;
	int nptLen;
	char* stream;
	MpegDescriptor* desc = NULL;

	assert(list != NULL);

	if (list->empty()) {
		if (nptPrinter) {
			cout << "Can't decode NPT: 0 NPT descriptors found" << endl;
		}
		return -1;
	}

	detectLoop();

	it = list->begin();
	while (it != list->end()) {
		desc = *it;
		if (desc->getDescriptorTag() == 0x01) {
			if (getNPTValue(getCurrentTimeBaseId()) != 0.0) {
				clog << "NPTProcessor::decodeDescriptors - cmp = ";
				clog << getNPTValue(getCurrentTimeBaseId()) << endl;
			}

			//NPT reference
			npt = (NPTReference*) desc;

			if (nptPrinter) {
				cout << "FOUND NEW NPT REFERENCE DESCRIPTOR" << endl;
				cout << "CONTENTID: " << (npt->getContentId() & 0xFF) << endl;
				cout << "NPT REFERENCE: " << npt->getNptRef() << endl;
				cout << "STC REFERENCE: " << npt->getStcRef() << endl;
				cout << "DISCONTINUITY INDICATOR: ";
				cout << (npt->getPostDiscontinuityIndicator() & 0xFF) << endl;
				cout << "NPT SCALE NUMERATOR: ";
				cout << npt->getScaleNumerator() << endl;
				cout << "NPT SCALE DENOMINATOR: ";
				cout << npt->getScaleDenominator() << endl;
			}

			//Search for existing time base
			itBase = timeBaseClock.find(npt->getContentId());

			if (itBase == timeBaseClock.end()) {
				//Time base not exists
				newNpt = new NPTReference();
				nptLen = npt->getStream(&stream);
				newNpt->addData(stream, nptLen);

				Thread::mutexLock(&schedMutex);
				if (scheduledNpts.count(npt->getContentId())) {
					delete scheduledNpts[npt->getContentId()];
				}

				scheduledNpts[npt->getContentId()] = newNpt;
				Thread::mutexUnlock(&schedMutex);

				wakeUp();
				unlockConditionSatisfied();
				if (newNpt->getScaleNumerator()) {
					clog << "NPTProcessor::decodeDescriptors - Scheduling ";
					clog << "new timebase: Transition to ";
					clog << (newNpt->getContentId() & 0xFF) << endl;

				} else {
					clog << "NPTProcessor::decodeDescriptors - Scheduling ";
					clog << "new timebase: CID ";
					clog << (newNpt->getContentId() & 0xFF);
					clog << " will be kept as paused." << endl;
				}

			} else {
				//time base exists
				clk = itBase->second;

				if ((clk->getScaleNumerator() != npt->getScaleNumerator()) ||
						(clk->getScaleDenominator() != npt->getScaleDenominator()) ||
						(clk->getContentId() != npt->getContentId())) {

					//It's a future change
					newNpt = new NPTReference();
					nptLen = npt->getStream(&stream);
					newNpt->addData(stream, nptLen);

					Thread::mutexLock(&schedMutex);
					if (scheduledNpts.count(npt->getContentId())) {
						delete scheduledNpts[npt->getContentId()];
					}

					scheduledNpts[npt->getContentId()] = newNpt;
					Thread::mutexUnlock(&schedMutex);

					unlockConditionSatisfied();
					wakeUp();
					if (npt->getScaleNumerator()) {
						clog << "NPTProcessor::decodeDescriptors - ";
						clog << "Scheduling existent timebase: ";
						clog << "Transition to ";
						clog << (newNpt->getContentId() & 0xFF) << endl;

					} else {
						clog << "Scheduling existent timebase: CID ";
						clog << (newNpt->getContentId() & 0xFF);
						clog << " will be paused." << endl;
					}

				} else {
					//Just an update
					updateTimeBase(clk, npt);
				}

				//else it's just the current NPT value and should be ignored
				Thread::mutexLock(&lifeMutex);
				itLife = timeBaseLife.find(npt->getContentId());
				if (itLife != timeBaseLife.end()) {
					//redefine time base lifetime
					itLife->second->setReference(0);
				}
				Thread::mutexUnlock(&lifeMutex);
			}

		} else if (desc->getDescriptorTag() == 0x02) {
			//NPT endpoint
			nptEP = (NPTEndpoint*) desc;

			if (nptPrinter) {
				cout << "FOUND NEW NPT ENDPOINT DESCRIPTOR" << endl;
				cout << "START NPT: " << nptEP->getStartNPT() << endl;
				cout << "STOP NPT: " << nptEP->getStopNPT() << endl;
			}

			clk = getCurrentTimebase();
			//set NPT start and stop
			if (clk != NULL) {
				if (!clk->getEndpointAvailable()) {
					clk->setStartNpt(nptEP->getStartNPT());
					clk->setStopNpt(nptEP->getStopNPT());
					clk->setEndpointAvailable(true);
					clog << "NPTProcessor::decodeDescriptors - CID ";
					clog << (clk->getContentId() & 0xFF) << " starts at ";
					clog << Stc::stcToSecond(clk->getStartNpt()*300);
					clog << " and stops at ";
					clog << Stc::stcToSecond(clk->getStopNpt()*300) << endl;
				}
			}

		} else {
			clog << "NPTProcessor::decodeDescriptors unknown NPT descriptor ";
			clog << "tag: " << desc->getDescriptorTag() << endl;

			if (nptPrinter) {
				cout << "NPTProcessor::decodeDescriptors unknown NPT ";
				cout << "descriptor tag: " << desc->getDescriptorTag() << endl;
			}
		}
		++it;
	}

	return timeBaseClock.size();
}

double NPTProcessor::getNPTValue(unsigned char contentId) {
	TimeBaseClock* clk;

	clk = getTimeBaseClock(contentId);
	if (clk != NULL) {
		return clk->getBaseToSecond();
	}

	return 0;
}

char NPTProcessor::getNextNptValue(
		char cid, double *nextNptValue, double* sleepTime) {

	map<unsigned char, NPTReference*>::iterator it;
	map<unsigned char, map<TimeControl*, set<ITimeBaseProvider*>*>*>::iterator i;
	map<TimeControl*, set<ITimeBaseProvider*>*>::iterator j;
	map<unsigned char, Stc*>::iterator k;
	map<unsigned char, TimeBaseClock*>::iterator l;
	double remaining1 = MAX_NPT_VALUE;
	double remaining2 = MAX_NPT_VALUE;
	double remaining3 = MAX_NPT_VALUE;
	double value, r, minor = 0.0;
	TimeBaseClock* clk;
	char ret = 0; //0 == schedNpt, 1 == listener
	uint64_t stcValue;
	Stc* cstc;

	*sleepTime = 0.0;

	if (timeBaseLife.empty()) return -1;

	stcValue = getSTCValue();
	if (stcValue) {
		Thread::mutexLock(&schedMutex);
		it = scheduledNpts.begin();
		while (it != scheduledNpts.end()) {
			r = Stc::baseToSecond(it->second->getStcRef() - stcValue);
			if (r < 0) r = 0.0;
			if (r < remaining1) {
				remaining1 = r;
			}
			++it;
		} //remaining time for future changes in NPT or a new timebase.
		Thread::mutexUnlock(&schedMutex);
	}

	clk = getTimeBaseClock(cid);
	if (clk) {
		value = clk->getBaseToSecond();
		lock();
		i = timeListeners.find(cid);
		if (i != timeListeners.end()) {
			j = i->second->begin();
			while (j != i->second->end()) {
				if (!j->first->notified) {
					r = j->first->time - value;
					if (r < 0) r = 0.0;
					if (r < remaining2) {
						remaining2 = r;
						*nextNptValue = j->first->time;
					}
				}
				++j;
			}
		}
		unlock();
	} //remaining time for future events.

	Thread::mutexLock(&lifeMutex);
	k = timeBaseLife.begin();
	while (k != timeBaseLife.end()) {
		cstc = k->second;
		l = timeBaseClock.find(k->first);
		value = 60.0 - cstc->getBaseToSecond();
		if (value < 0) value = 0.0;
		if (value < remaining3) {
			remaining3 = value;
		}
		if (l->second->getScaleNumerator() &&
				l->second->getEndpointAvailable()) {

			r = Stc::baseToSecond(
					l->second->getStopNpt()) - l->second->getBaseToSecond();

			if (r < 0) {
				r = 0.0;
			}

			if (r < remaining3) {
				remaining3 = r;
				*nextNptValue = j->first->time;
			}
		}
		++k;
	}
	Thread::mutexUnlock(&lifeMutex);

	if ((remaining3 < remaining1) && (remaining3 < remaining2)) {
		minor = remaining3;

	} else {
		Thread::mutexLock(&schedMutex);
		if (scheduledNpts.empty() && !clk) {
			minor = 0.0;

		} else {
			if (!scheduledNpts.empty() && clk) {
				if (remaining1 > remaining2) {
					minor = remaining2;
					ret = 1;

				} else {
					minor = remaining1;
				} //which one is lower?

			} else if (!scheduledNpts.empty()) {
				if (stcValue) {
					minor = remaining1;

				} else {
					minor = 0.0;
				}

			} else {
				minor = remaining2;
				ret = 1;
			}
		}
		Thread::mutexUnlock(&schedMutex);
	}

	*sleepTime = minor * 1000; //convert to milliseconds

	return ret;
}

bool NPTProcessor::processNptValues() {
	double nextNptValue, nptValue, sleepTime;
	bool timedOut;
	char notify;
	bool restart = true;
	char cid;
	TimeBaseClock* clk;
	Stc* tblife;
	NPTReference* npt;
	map<unsigned char, TimeBaseClock*>::iterator itClk;
	map<unsigned char, NPTReference*>::iterator it;
	map<unsigned char, map<TimeControl*, set<ITimeBaseProvider*>*>*>::iterator i;
	map<TimeControl*, set<ITimeBaseProvider*>*>::iterator j;
	double sleeper;

	cid = getCurrentTimeBaseId();

	notify = getNextNptValue(cid, &nextNptValue, &sleepTime);
	if (sleepTime > 0.0) {
		sleeper = getCurrentTimeMillis();
		timedOut = Thread::mSleep((long int)sleepTime);
		if (!timedOut) {
			return false;
		}
	}

	if (notify == 1) {
		lock();
		i = timeListeners.find(cid);
		if (i != timeListeners.end()) {
			j = i->second->begin();
			while (j != i->second->end()) {
				if (!j->first->notified) {
					if (j->first->time == nextNptValue) {
						notifyTimeListeners(cid, nextNptValue);
						j->first->notified = true;
						reScheduleIt = true;
					}
				}
				++j;
			}
		}
		unlock();
	}
	
	while (restart) {
		restart = false;
		Thread::mutexLock(&schedMutex);
		it = scheduledNpts.begin();
		while (it != scheduledNpts.end()) {
			npt = it->second;
			if (getSTCValue() >= npt->getStcRef()) {
				itClk = timeBaseClock.find(npt->getContentId());
				if (itClk == timeBaseClock.end()) {
					clk = new TimeBaseClock();
					timeBaseClock[npt->getContentId()] = clk;
					tblife = new Stc();
					tblife->setReference(0);
					Thread::mutexLock(&lifeMutex);
					timeBaseLife[npt->getContentId()] = tblife;
					Thread::mutexUnlock(&lifeMutex);
					clog << "NPTProcessor::processNptValues - Added CID: ";
					clog << (npt->getContentId() & 0xFF) << endl;

				} else {
					clk = timeBaseClock[npt->getContentId()];
				}
				updateTimeBase(clk, npt);
				delete it->second;
				scheduledNpts.erase(it);
				restart = true;
				lock();
				notifyIdListeners(currentCid, clk->getContentId());
				unlock();
				currentCid = clk->getContentId();
				reScheduleIt = true;
				clog << "NPTProcessor::processNptValues - Executing scheduled NPT: "
					<< (clk->getContentId() & 0xFF) <<
					" as " << clk->getScaleNumerator() << endl;
				break;
			}
			++it;
		}
		Thread::mutexUnlock(&schedMutex);
	}

	clearUnusedTimebase();

	return true;
}

void NPTProcessor::run() {
	bool hasTimeListeners = false;

	while (running) {
		lock();
		hasTimeListeners = !timeListeners.empty();
		unlock();

		Thread::mutexLock(&schedMutex);
		if (scheduledNpts.empty() && !hasTimeListeners) {
			Thread::mutexUnlock(&schedMutex);
			if (reScheduleIt) {
				reScheduleIt = false;
				processNptValues();

			} else {
				waitForUnlockCondition();
			}

		} else {
			Thread::mutexUnlock(&schedMutex);
			processNptValues();
		}
	}
}

}
}
}
}
}
}
}
}
