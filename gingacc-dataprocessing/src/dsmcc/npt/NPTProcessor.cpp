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

	this->stcProvider   = stcProvider;
	this->scheduledNpts = new map<unsigned char, NPTReference*>;
	this->timeBaseClock = new map<unsigned char, TimeBaseClock*>;
	timeBaseLife = new map<unsigned char, Stc*>;
	timeBaseNaturalEnd = new map<unsigned char, set<ITimeBaseProvider*>*>;

	this->loopListeners = new map<unsigned char, set<ITimeBaseProvider*>*>;
	this->timeListeners = new map<
			unsigned char, map<TimeControl*, set<ITimeBaseProvider*>*>*>;
			
	isFirstStc = true;

	this->cidListeners  = new set<ITimeBaseProvider*>;
	this->running       = true;
	this->currentCid    = INVALID_CID;
	loopControlMax      = false;
	loopControlMin      = false;
	reScheduleIt		= false;

	Thread::mutexInit(&loopMutex, false);
	Thread::mutexInit(&schedMutex, false);
	Thread::mutexInit(&tbEndMutex, false);

	startThread();
}

NPTProcessor::~NPTProcessor() {
	map<unsigned char, map<TimeControl*, set<ITimeBaseProvider*>*>*>::iterator i;
	map<TimeControl*, set<ITimeBaseProvider*>*>::iterator j;
	set<ITimeBaseProvider*>::iterator k;
	map<unsigned char, set<ITimeBaseProvider*>*>::iterator l;

	clearTables();
	delete scheduledNpts;
	delete timeBaseClock;
	delete timeBaseLife;
	
	lock();

	if (timeListeners != NULL) {
		i = timeListeners->begin();
		while (i != timeListeners->end()) {
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
		delete timeListeners;
	}

	unlock();

	if (cidListeners != NULL) {
		delete cidListeners;
	}

	Thread::mutexLock(&loopMutex);
	if (loopListeners != NULL) {
		delete loopListeners;
	}
	Thread::mutexUnlock(&loopMutex);
	Thread::mutexDestroy(&loopMutex);

	Thread::mutexLock(&schedMutex);
	Thread::mutexUnlock(&schedMutex);
	Thread::mutexDestroy(&schedMutex);

	Thread::mutexLock(&tbEndMutex);
	if (timeBaseNaturalEnd) {
		l = timeBaseNaturalEnd->begin();
		while (l != timeBaseNaturalEnd->end()) {
			delete l->second;
			++l;
		}
		delete timeBaseNaturalEnd;
	}
	Thread::mutexUnlock(&tbEndMutex);
	Thread::mutexDestroy(&tbEndMutex);
}

void NPTProcessor::clearTables() {
	map<unsigned char, NPTReference*>::iterator  i;
	map<unsigned char, TimeBaseClock*>::iterator j;
	map<unsigned char, Stc*>::iterator k;

	lock();

	if (timeListeners != NULL) {
		map<unsigned char, map<TimeControl*, set<ITimeBaseProvider*>*>*>::iterator a;
		map<TimeControl*, set<ITimeBaseProvider*>*>::iterator b;
		a = timeListeners->begin();
		while (a != timeListeners->end()) {
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
		timeListeners->clear();
	}

	unlock();

	Thread::mutexLock(&schedMutex);
	if (scheduledNpts != NULL) {
		i = scheduledNpts->begin();
		while (i != scheduledNpts->end()) {
			delete i->second;
			++i;
		}
		scheduledNpts->clear();
	}
	Thread::mutexUnlock(&schedMutex);

	if (timeBaseClock != NULL) {
		j = timeBaseClock->begin();
		while (j != timeBaseClock->end()) {
			delete j->second;
			++j;
		}
		timeBaseClock->clear();
	}
	if (timeBaseLife != NULL) {
		k = timeBaseLife->begin();
		while (k != timeBaseLife->end()) {
			delete k->second;
			++k;
		}
		timeBaseLife->clear();
	}
}

void NPTProcessor::clearUnusedTimebase() {
	map<unsigned char, Stc*>::iterator itLife;
	map<unsigned char, TimeBaseClock*>::iterator i;
	TimeBaseClock* clk;
	bool restart = true;

	while (restart) {
		restart = false;
		i = timeBaseClock->begin();
		while (i != timeBaseClock->end()) {
			clk = i->second;
			if ((clk->getEndpointAvailable()) && (clk->getStcBase() >= clk->getStopNpt())) {
				notifyNaturalEndListeners(clk->getContentId(), clk->getStopNpt());
				clog << "NPTProcessor::clearUnusedTimebase - Deleted by endpoint: CID = " <<
					(clk->getContentId() & 0xFF) << endl;
				delete i->second;
				timeBaseClock->erase(i);
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
		itLife = timeBaseLife->begin();
		if (itLife != timeBaseLife->end()) {
			if (itLife->second->getStcBase() > 5400000) {// 1 minute
				clk = getTimeBaseClock(itLife->first);
				notifyNaturalEndListeners(clk->getContentId(),
					clk->getBaseToSecond());
				clog << "NPTProcessor::clearUnusedTimebase - Deleted by lifetime: CID = "
					<< (clk->getContentId() & 0xFF) << endl;
				delete itLife->second;
				timeBaseLife->erase(itLife);
				restart = true;
			}
			++itLife;
		}
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
	i = loopListeners->find(cid);
	if (i != loopListeners->end()) {
		listeners = i->second;

	} else {
		listeners = new set<ITimeBaseProvider*>;
		(*loopListeners)[cid] = listeners;
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

	i = loopListeners->find(cid);
	if (i != loopListeners->end()) {
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
	i = timeListeners->find(cid);
	if (i == timeListeners->end()) {
		valueListeners = new map<TimeControl*, set<ITimeBaseProvider*>*>;
		(*timeListeners)[cid] = valueListeners;

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
	i = timeListeners->find(cid);
	if (i != timeListeners->end()) {
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

	i = cidListeners->find((ITimeBaseProvider*)ltn);
	added = (i == cidListeners->end());
	cidListeners->insert((ITimeBaseProvider*)ltn);

	unlockConditionSatisfied();
	wakeUp();

	clog << "NPTProcessor::addIdListener" << endl;
	return added;
}

bool NPTProcessor::removeIdListener(ITimeBaseListener* ltn) {
	set<ITimeBaseProvider*>::iterator i;

	clog << "NPTProcessor::removeIdListener()" << endl;

	i = cidListeners->find((ITimeBaseProvider*)ltn);
	if (i == cidListeners->end()) {
		return false;
	}

	cidListeners->erase(i);

	unlockConditionSatisfied();
	wakeUp();

	return true;
}

bool NPTProcessor::addTimeBaseNaturalEndListener(unsigned char contentId,
			ITimeBaseListener* ltn) {

	clog << "NPTProcessor::addTimeBaseNaturalEndListener cid = " << contentId << endl;

	map<unsigned char, set<ITimeBaseProvider*>*>::iterator i;
	set<ITimeBaseProvider*>* listeners;

	Thread::mutexLock(&tbEndMutex);

	i = timeBaseNaturalEnd->find(contentId);
	if (i != timeBaseNaturalEnd->end()) {
		listeners = i->second;

	} else {
		listeners = new set<ITimeBaseProvider*>;
		(*timeBaseNaturalEnd)[contentId] = listeners;
	}

	listeners->insert((ITimeBaseProvider*)ltn);
	
	Thread::mutexUnlock(&tbEndMutex);

	unlockConditionSatisfied();
	wakeUp();

	return true;
}

bool NPTProcessor::removeTimeBaseNaturalEndListener(unsigned char cid,
			ITimeBaseListener* ltn) {
	map<unsigned char, set<ITimeBaseProvider*>*>::iterator i;
	set<ITimeBaseProvider*>::iterator j;

	clog << "NPTProcessor::removeTimeBaseNaturalEndListener()" << endl;

	Thread::mutexLock(&tbEndMutex);

	i = timeBaseNaturalEnd->find(cid);
	if (i != timeBaseNaturalEnd->end()) {
		j = i->second->find((ITimeBaseProvider*)ltn);
		if (j == i->second->end()) {
			Thread::mutexUnlock(&tbEndMutex);
			return false;
		} else {
			i->second->erase(j);
		}
	}

	Thread::mutexUnlock(&tbEndMutex);

	unlockConditionSatisfied();
	wakeUp();

	return true;
}

unsigned char NPTProcessor::getCurrentTimeBaseId() {
	map<unsigned char, TimeBaseClock*>::iterator i;
	TimeBaseClock* clk;

	i = timeBaseClock->begin();
	while (i != timeBaseClock->end()) {
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

	i = timeBaseClock->begin();
	while (i != timeBaseClock->end()) {
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
	i = loopListeners->begin();
	if (i != loopListeners->end()) {
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

	Thread::mutexLock(&tbEndMutex);

	if (timeBaseNaturalEnd) {
		i = timeBaseNaturalEnd->find(cid);
		while (i != timeBaseNaturalEnd->end()) {
			j = i->second->begin();
			while (j != i->second->end()) {
				clog << "NPTProcessor::notifyNaturalEndListeners " << "cid '" << (cid & 0xFF) <<
					", nptValue = " << nptValue << endl;
				((ITimeBaseListener*)(*j))->timeBaseNaturalEnd(cid, nptValue);
				++j;
			}
		}
	}

	Thread::mutexUnlock(&tbEndMutex);
}

void NPTProcessor::notifyTimeListeners(unsigned char cid, double nptValue) {
	map<unsigned char, map<TimeControl*, set<ITimeBaseProvider*>*>*>::iterator i;
	map<TimeControl*, set<ITimeBaseProvider*>*>::iterator j;
	set<ITimeBaseProvider*>::iterator k;

	clog << "NPTProcessor::notifyTimeListeners " << "cid '" << (cid & 0xFF);
	clog << "' nptvalue '" << nptValue << "'" << endl;

	i = timeListeners->find(cid);
	if (i != timeListeners->end()) {
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

	i = cidListeners->begin();
	while (i != cidListeners->end()) {
		clog << "NPTProcessor::notifyIdListeners" << endl;
		((ITimeBaseListener*)(*i))->updateTimeBaseId(oldCid, newCid);
		++i;
	}
}

TimeBaseClock* NPTProcessor::getTimeBaseClock(unsigned char cid) {
	map<unsigned char, TimeBaseClock*>::iterator i;

	i = timeBaseClock->find(cid);
	if (i != timeBaseClock->end()) {
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

	i = timeBaseClock->find(timeBaseId);
	if (i != timeBaseClock->end()) {
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
				i = timeBaseLife->begin();
				while (i != timeBaseLife->end()) {
					notifyNaturalEndListeners(i->first,
						getTimeBaseClock(i->first)->getBaseToSecond());
					++i;
				}
				//possible loop in TS
				isFirstStc = true;
				clearTables();
				if (loopListeners->size()) {
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

	if (list == NULL) {
		return -1;
	}

	detectLoop();

	it = list->begin();
	while (it != list->end()) {
		desc = *it;
		if (desc->getDescriptorTag() == 0x01) {
			if (getNPTValue(getCurrentTimeBaseId()) != 0.0) {
				clog << "NPTProcessor::decodeDescriptors - cmp = "
					<< getNPTValue(getCurrentTimeBaseId()) << endl;
			}
			//NPT reference
			npt = (NPTReference*) desc;

			//Search for existing time base
			itBase = timeBaseClock->find(npt->getContentId());

			if (itBase == timeBaseClock->end()) {
				//Time base not exists
				newNpt = new NPTReference();
				nptLen = npt->getStream(&stream);
				newNpt->addData(stream, nptLen);

				Thread::mutexLock(&schedMutex);
				if (scheduledNpts->count(npt->getContentId())) {
					delete (*scheduledNpts)[npt->getContentId()];
				}

				(*scheduledNpts)[npt->getContentId()] = newNpt;
				Thread::mutexUnlock(&schedMutex);

				wakeUp();
				unlockConditionSatisfied();
				if (newNpt->getScaleNumerator()) {
					clog << "NPTProcessor::decodeDescriptors - Scheduling new timebase: Transition to "
						<< (newNpt->getContentId() & 0xFF) << endl;
				} else {
					clog << "NPTProcessor::decodeDescriptors - Scheduling new timebase: CID "
						<< (newNpt->getContentId() & 0xFF) << " will be kept as paused." << endl;
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
						if (scheduledNpts->count(npt->getContentId())) {
							delete (*scheduledNpts)[npt->getContentId()];
						}

						(*scheduledNpts)[npt->getContentId()] = newNpt;
						Thread::mutexUnlock(&schedMutex);

						unlockConditionSatisfied();
						wakeUp();
						if (npt->getScaleNumerator()) {
							clog << "NPTProcessor::decodeDescriptors - Scheduling existent timebase: Transition to "
								<< (newNpt->getContentId() & 0xFF) << endl;
						} else {
							clog << "Scheduling existent timebase: CID "
								<< (newNpt->getContentId() & 0xFF) << " will be paused." << endl;
						}
				} else {
					//Just an update
					updateTimeBase(clk, npt);
				}
				//else it's just the current NPT value and should be ignored
				itLife = timeBaseLife->find(npt->getContentId());
				if (itLife != timeBaseLife->end()) {
					//redefine time base lifetime
					itLife->second->setReference(0);
				}
			}
		} else if (desc->getDescriptorTag() == 0x02) {
			//NPT endpoint
			nptEP = (NPTEndpoint*) desc;
			clk = getCurrentTimebase();
			//set NPT start and stop
			if (clk != NULL) {
				if (!clk->getEndpointAvailable()) {
					clk->setStartNpt(nptEP->getStartNPT());
					clk->setStopNpt(nptEP->getStopNPT());
					clk->setEndpointAvailable(true);
					clog << "NPTProcessor::decodeDescriptors - CID "
						<< (clk->getContentId() & 0xFF) << " starts at " <<
						Stc::stcToSecond(clk->getStartNpt()*300) << " and stops at " <<
						Stc::stcToSecond(clk->getStopNpt()*300) << endl;
				}
			}
		}
		++it;
	}

	return timeBaseClock->size();
}

double NPTProcessor::getNPTValue(unsigned char contentId) {
	TimeBaseClock* clk;

	clk = getTimeBaseClock(contentId);
	if (clk != NULL) {
		return clk->getBaseToSecond();
	}

	return 0;
}

char NPTProcessor::getNextNptValue(char cid, double *nextNptValue, double* sleepTime) {

	map<unsigned char, NPTReference*>::iterator it;
	map<unsigned char, map<TimeControl*, set<ITimeBaseProvider*>*>*>::iterator i;
	map<TimeControl*, set<ITimeBaseProvider*>*>::iterator j;
	double remaining1 = MAX_NPT_VALUE;
	double remaining2 = MAX_NPT_VALUE;
	double remaining3 = MAX_NPT_VALUE;
	double value, minor, r;
	TimeBaseClock* clk;
	bool isListener = false;
	uint64_t stcValue;

	stcValue = getSTCValue();
	if (stcValue) {
		Thread::mutexLock(&schedMutex);
		it = scheduledNpts->begin();
		while (it != scheduledNpts->end()) {
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
		i = timeListeners->find(cid);
		if (i != timeListeners->end()) {
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

	Thread::mutexLock(&schedMutex);
	if (scheduledNpts->empty() && !clk) minor = 0.0; else {
		if (!scheduledNpts->empty() && clk) {
			if (remaining1 > remaining2) {
				minor = remaining2;
				isListener = true;
			} else {
				minor = remaining1;
			} //which one is lower?
		} else if (!scheduledNpts->empty()) {
			if (stcValue) minor = remaining1; else
				minor = 0.0;
		} else {
			minor = remaining2;
			isListener = true;
		}
	}
	Thread::mutexUnlock(&schedMutex);

	//TODO: check this code
	if (minor > 45000) {
		*sleepTime = 0.0;
		return -1;
	}

	*sleepTime = minor * 1000; //convert to milliseconds

	return isListener;
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

	clearUnusedTimebase();

	if (notify > 0) {
		lock();
		i = timeListeners->find(cid);
		if (i != timeListeners->end()) {
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
	} else if (notify == 0) {
		while (restart) {
			restart = false;
			Thread::mutexLock(&schedMutex);
			it = scheduledNpts->begin();
			while (it != scheduledNpts->end()) {
				npt = it->second;
				if (getSTCValue() >= npt->getStcRef()) {
					itClk = timeBaseClock->find(npt->getContentId());
					if (itClk == timeBaseClock->end()) {
						clk = new TimeBaseClock();
						(*timeBaseClock)[npt->getContentId()] = clk;
						tblife = new Stc();
						tblife->setReference(0);
						(*timeBaseLife)[npt->getContentId()] = tblife;
						clog << "NPTProcessor::processNptValues - Added CID: "
							<< (npt->getContentId() & 0xFF) << endl;
					} else {
						clk = (*timeBaseClock)[npt->getContentId()];
					}
					updateTimeBase(clk, npt);
					delete it->second;
					scheduledNpts->erase(it);
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
	}

	return true;
}

void NPTProcessor::run() {
	bool hasTimeListeners = false;

	while (running) {
		lock();
		hasTimeListeners = !timeListeners->empty();
		unlock();

		Thread::mutexLock(&schedMutex);
		if (scheduledNpts->empty() && !hasTimeListeners) {
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
