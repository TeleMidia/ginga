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

	this->loopListeners = new map<unsigned char, set<INPTListener*>*>;
	this->timeListeners = new map<
			unsigned char, map<double, set<INPTListener*>*>*>;

	this->cidListeners  = new set<INPTListener*>;
	this->running       = true;
	this->currentCid    = INPTListener::INVALID_CID;
	loopControlMax      = false;
	loopControlMin      = false;

	pthread_mutex_init(&loopMutex, NULL);

	start();
}

NPTProcessor::~NPTProcessor() {
	map<unsigned char, NPTReference*>::iterator  i;
	map<unsigned char, TimeBaseClock*>::iterator j;

	if (scheduledNpts != NULL) {
		i = scheduledNpts->begin();
		while (i != scheduledNpts->end()) {
			delete i->second;
			++i;
		}
		delete scheduledNpts;
		scheduledNpts = NULL;
	}

	if (timeBaseClock != NULL) {
		j = timeBaseClock->begin();
		while (j != timeBaseClock->end()) {
			delete j->second;
			++j;
		}
		delete timeBaseClock;
		timeBaseClock = NULL;
	}

	lock();
	if (timeListeners != NULL) {
		delete timeListeners;
		timeListeners = NULL;
	}
	unlock();

	if (cidListeners != NULL) {
		delete cidListeners;
		cidListeners = NULL;
	}

	pthread_mutex_lock(&loopMutex);
	if (loopListeners != NULL) {
		delete loopListeners;
		loopListeners = NULL;
	}
	pthread_mutex_unlock(&loopMutex);
	pthread_mutex_destroy(&loopMutex);
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
	map<unsigned char, set<INPTListener*>*>::iterator i;
	set<INPTListener*>* listeners;

	clog << "NPTProcessor::addLoopListener " << endl;

	pthread_mutex_lock(&loopMutex);
	i = loopListeners->find(cid);
	if (i != loopListeners->end()) {
		listeners = i->second;

	} else {
		listeners = new set<INPTListener*>;
		(*loopListeners)[cid] = listeners;
	}

	listeners->insert((INPTListener*)ltn);
	pthread_mutex_unlock(&loopMutex);

	return true;
}

bool NPTProcessor::addTimeListener(
		unsigned char cid, double nptValue, ITimeBaseListener* ltn) {

	map<unsigned char, map<double, set<INPTListener*>*>*>::iterator i;
	map<double, set<INPTListener*>*>::iterator j;
	set<INPTListener*>::iterator k;

	map<double, set<INPTListener*>*>* valueListeners;
	set<INPTListener*>* setListeners;
	bool added;

	clog << "NPTProcessor::addTimeListener TIME = " << nptValue << endl;

	lock();
	i = timeListeners->find(cid);
	if (i == timeListeners->end()) {
		valueListeners = new map<double, set<INPTListener*>*>;
		(*timeListeners)[cid] = valueListeners;

	} else {
		valueListeners = i->second;
	}

	j = valueListeners->find(nptValue);
	if (j == valueListeners->end()) {
		setListeners = new set<INPTListener*>;
		(*valueListeners)[nptValue] = setListeners;

	} else {
		setListeners = j->second;
	}

	k = setListeners->find((INPTListener*)ltn);
	added = (k == setListeners->end());
	setListeners->insert((INPTListener*)ltn);

	unlockConditionSatisfied();
	wakeUp();
	unlock();
	return added;
}

bool NPTProcessor::removeTimeListener(
		unsigned char cid, ITimeBaseListener* ltn) {

	map<unsigned char, map<double, set<INPTListener*>*>*>::iterator i;
	map<double, set<INPTListener*>*>::iterator j;
	set<INPTListener*>::iterator k;

	lock();
	i = timeListeners->find(cid);
	if (i != timeListeners->end()) {
		j = i->second->begin();
		while (j != i->second->end()) {
			k = j->second->find((INPTListener*)ltn);
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
	set<INPTListener*>::iterator i;
	bool added;

	//lock();
	i = cidListeners->find((INPTListener*)ltn);
	added = (i == cidListeners->end());
	cidListeners->insert((INPTListener*)ltn);

	unlockConditionSatisfied();
	wakeUp();
	//unlock();

	//clog << "NPTProcessor::addIdListener" << endl;
	return added;
}

bool NPTProcessor::removeIdListener(ITimeBaseListener* ltn) {
	set<INPTListener*>::iterator i;

	//lock();
	i = cidListeners->find((INPTListener*)ltn);
	if (i == cidListeners->end()) {
		//unlock();
		return false;
	}

	cidListeners->erase(i);

	unlockConditionSatisfied();
	wakeUp();
	//unlock();
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

	return INPTListener::INVALID_CID;
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

void NPTProcessor::notifyLoopToTimeListeners(unsigned char cid) {
	map<unsigned char, set<INPTListener*>*>::iterator i;
	set<INPTListener*>::iterator j;

	clog << "NPTProcessor::notifyLoopToTimeListeners ";
	clog << endl;

	pthread_mutex_lock(&loopMutex);
	i = loopListeners->find(cid);
	if (i != loopListeners->end()) {
		j = i->second->begin();
		while (j != i->second->end()) {
			clog << "NPTProcessor::notifyLoopToTimeListeners ";
			clog << "CALL loop detected" << endl;
			((ITimeBaseListener*)(*j))->loopDetected();
			++j;
		}
	}
	pthread_mutex_unlock(&loopMutex);
}

void NPTProcessor::notifyTimeListeners(unsigned char cid, double nptValue) {
	map<unsigned char, map<double, set<INPTListener*>*>*>::iterator i;
	map<double, set<INPTListener*>*>::iterator j;
	set<INPTListener*>::iterator k;

	clog << "NPTProcessor::notifyTimeListeners ";
	clog << "cid '" << (cid & 0xFF);
	clog << "' nptvalue '" << nptValue;
	clog << "'" << endl;

	lock();
	i = timeListeners->find(cid);
	if (i != timeListeners->end()) {
		j = i->second->find(nptValue);
		if (j != i->second->end()) {
			k = j->second->begin();
			while (k != j->second->end()) {
				((ITimeBaseListener*)(*k))->valueReached(cid, nptValue);
				++k;
			}
		}
	}
	unlock();
}

void NPTProcessor::notifyIdListeners(
		unsigned char oldCid, unsigned char newCid) {

	set<INPTListener*>::iterator i;

	//lock();
	i = cidListeners->begin();
	while (i != cidListeners->end()) {
		clog << "NPTProcessor::notifyIdListeners" << endl;
		((ITimeBaseListener*)(*i))->updateTimeBaseId(oldCid, newCid);
		++i;
	}
	//unlock();
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
	int64_t value;
	uint64_t newNpt;
	uint64_t stcValue;
	double nptSecs;

	if (!checkTimeBaseArgs("updateTimeBase", clk, npt)) {
		return -1;
	}

	stcValue = getSTCValue();

	value = stcValue - npt->getStcRef();
	value = TimeBaseClock::convertToNpt(
			value,
			npt->getScaleNumerator(),
			npt->getScaleDenominator());

	newNpt = npt->getNptRef() + value;

	clk->setReference(newNpt, 0);
	clk->setScaleNumerator(npt->getScaleNumerator());
	clk->setScaleDenominator(npt->getScaleDenominator());

	if (npt->getScaleNumerator() != 0 && npt->getContentId() != currentCid) {
		notifyIdListeners(currentCid, npt->getContentId());
		currentCid = npt->getContentId();
	}

	//TODO: find a clean way to do this
	nptSecs = Stc::baseToSecond(newNpt);
	if (nptSecs > 0.0 && nptSecs < 3.0) {
		if (!loopControlMin && loopControlMax) {
			clog << endl << endl << endl;
			clog << "NPTProcessor::updateTimeBase LOOP !!!!!!!!!!";
			clog << endl << endl << endl;
			notifyLoopToTimeListeners(npt->getContentId());
			loopControlMin = true;
		}

	} else {
		loopControlMax = true;
		loopControlMin = false;
	}

	/*clog << "NPTProcessor::updateTimeBase ";
	clog << "Clock '" << (clk->getContentId() & 0xFF);
	clog << "' set to '" << nptSecs;
	clog << "' STC = '" << stcValue << "'";
	clog << endl;*/
	return 0;
}

int NPTProcessor::scheduleTimeBase(NPTReference* npt) {
	map<unsigned char, NPTReference*>::iterator i;
	NPTReference* oldNpt;

	//lock();
	i = scheduledNpts->find(npt->getContentId());
	if (i != scheduledNpts->end()) {
		oldNpt = i->second;

		if (oldNpt->getScaleNumerator() != npt->getScaleNumerator() ||
				oldNpt->getScaleDenominator() != npt->getScaleDenominator() ||
				oldNpt->getStcRef() != npt->getStcRef()) {

			delete oldNpt;
			scheduledNpts->erase(i);
		}
	}

	(*scheduledNpts)[npt->getContentId()] = npt;

	unlockConditionSatisfied();
	wakeUp();
	//unlock();
	return 1;
}

bool NPTProcessor::checkTimeBaseArgs(
		string function, TimeBaseClock* clk, NPTReference* npt) {

	if (clk == NULL || npt == NULL) {
		clog << "NPTProcessor::" << function << " Warning!";
		clog << " clk(" << clk << ") npt(" << npt << ")";
		clog << endl;
		return false;
	}

	if (clk->getContentId() != npt->getContentId()) {
		clog << "NPTProcessor::" << function << " Warning! DIFF CIDS";
		clog << " clk->cid = '" << (clk->getContentId() & 0xFF) << "'";
		clog << " npt->cid = '" << (npt->getContentId() & 0xFF) << "'";
		clog << endl;
		return false;
	}

	return true;
}

int NPTProcessor::decodeNPT(vector<Descriptor*>* list) {
	vector<Descriptor*>::iterator it;
	TimeBaseClock* clk;
	map<unsigned char, TimeBaseClock*>::iterator itBase;
	vector<pair<bool, NPTReference*>*>::iterator itEvent;
	NPTReference* npt = NULL;
	Descriptor* desc = NULL;

	if (list == NULL) {
		return -1;
	}

	it = list->begin();
	while (it != list->end()) {
		desc = *it;
		if (desc->getDescriptorTag() == 0x01) {
			//Analyzing NPT reference
			npt = (NPTReference*) desc;
			clk = getTimeBaseClock(npt->getContentId());

			if (clk == NULL) {
				//Time base not exists, creating a new one
				clk = new TimeBaseClock();
				clk->setContentId(npt->getContentId());
				clk->setReference(npt->getNptRef(), 0);
				clk->setScaleNumerator(npt->getScaleNumerator());
				clk->setScaleDenominator(npt->getScaleDenominator());
				(*timeBaseClock)[npt->getContentId()] = clk;

				//first NPT reference, send to thread
				updateTimeBase(clk, npt);

			} else {//if (npt->getStcRef() > getSTCValue()) {
				//Time base already exists. Checking NPT change status
				if ((clk->getScaleNumerator() != npt->getScaleNumerator()) ||
						(clk->getScaleDenominator() !=
								npt->getScaleDenominator())) {

					//It's a change
					scheduleTimeBase(npt);

				} else {
					//It's not a change. Just update the clock
					updateTimeBase(clk, npt);
				}
			}
		}
		++it;
	}

	/*clog << "NPTProcessor::decodeNPT processed '" << list->size() << "'";
	clog << " descriptors" << endl;*/

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

bool NPTProcessor::getNextNptValue(
		double* nextNptValue,
		NPTReference* npt,
		unsigned char* cid,
		double* sleepTime) {

	map<unsigned char, map<double, set<INPTListener*>*>*>::iterator i;
	map<double, set<INPTListener*>*>::iterator j;
	double nptValue, minor;
	TimeBaseClock* clk;
	bool isListener = true;
	uint64_t stcValue;
	unsigned char currentcid;

	lock();

	*nextNptValue = INPTListener::MAX_NPT_VALUE;
	minor         = INPTListener::MAX_NPT_VALUE;
	currentcid    = getCurrentTimeBaseId();
	clk           = getTimeBaseClock(currentcid);

	if (clk != NULL) {
		i = timeListeners->find(currentcid);
		if (i != timeListeners->end()) {
			j = i->second->begin();
			while (j != i->second->end()) {
				nptValue = j->first;
				if (nptValue > clk->getBaseToSecond()) {
					*nextNptValue = nptValue;
					*cid = i->first;
					break;
				}
				++j;
			}
		}
	}

	if (!timeListeners->empty() &&
			(*nextNptValue != INPTListener::MAX_NPT_VALUE)) {

		nptValue = clk->getBaseToSecond();
		if (*nextNptValue > nptValue) {
			minor = *nextNptValue - nptValue;
		} else {
			minor = 0;
		}
	}
	unlock();

	if (npt == NULL) {
		isListener = true;

	} else {
		stcValue = getSTCValue();
		// next cid change value
		if (npt->getStcRef() > stcValue) {
			nptValue = Stc::baseToSecond(npt->getStcRef() - stcValue);
		} else {
			nptValue = 0;
		}

		//Comparison between remaining time of listener and CID change
		// if next event is cid change
		if (nptValue < minor) {
			*cid       = npt->getContentId();
			minor      = nptValue;
			isListener = false;
		}
	}

	*sleepTime = minor * 1000;
	return isListener;
}

bool NPTProcessor::processNptValues(NPTReference* npt, bool* isNotify) {
	double nextNptValue;
	unsigned char cid;
	double sleepTime;
	bool timedOut, notify;
	TimeBaseClock* clk;

	notify = getNextNptValue(&nextNptValue, npt, &cid, &sleepTime);
	if (isNotify != NULL) {
		*isNotify = notify;
	}

	if (sleepTime > 0.0) {
		timedOut = Thread::mSleep((long int)sleepTime);
		if (!timedOut) {
			return false;
		}
	}

	if (notify) {
		clk = getTimeBaseClock(cid);
		if (clk != NULL) {
			sleepTime = nextNptValue - clk->getBaseToSecond();
			if (sleepTime > 0.0) {
				 //TODO: find error ct
				Thread::mSleep((long int)((sleepTime + 0.1) * 1000));
			}
		}
		notifyTimeListeners(cid, nextNptValue);

	} else if (npt != NULL) {
		clk = getTimeBaseClock(cid);
		updateTimeBase(clk, npt);

	} else {
		clog << "NPTProcessor::processNptValues Warning! NULL npt && !notify";
		clog << endl;
		return false;
	}

	return true;
}

void NPTProcessor::run() {
	map<unsigned char, NPTReference*>::iterator i;
	bool pastNpt, processed;
	NPTReference* npt;
	TimeBaseClock* clk;
	uint64_t stcValue;
	bool isNotify;
	unsigned char cid;
	bool hasTimeListeners = false;

	while (running) {
		lock();
		hasTimeListeners = !timeListeners->empty();
		unlock();
		if (scheduledNpts->empty() && !hasTimeListeners) {
			waitForUnlockCondition();

		} else {
			if (!scheduledNpts->empty()) {
				i = scheduledNpts->begin();
				while (i != scheduledNpts->end()) {
					npt = i->second;
					stcValue = getSTCValue();
					if (npt->getStcRef() <= stcValue) {
						pastNpt = true;

					} else {
						pastNpt = false;

						processed = processNptValues(npt, &isNotify);
						if (!processed) {
							continue;

						} else if (!isNotify) {
							delete npt;
							scheduledNpts->erase(i);
							i = scheduledNpts->begin();
						}
					}

					if (pastNpt) {
						cid = npt->getContentId();
						clk = getTimeBaseClock(cid);
						updateTimeBase(clk, npt);

						delete npt;
						scheduledNpts->erase(i);
						i = scheduledNpts->begin();

					} else if (isNotify) {
						++i;
					}
				}
			}

			lock();
			hasTimeListeners = !timeListeners->empty();
			unlock();
			if (hasTimeListeners) {
				processNptValues(NULL, NULL);
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
}
