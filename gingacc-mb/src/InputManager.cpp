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

#include "mb/InputManager.h"
#include "mb/LocalScreenManager.h"
#include "mb/interface/CodeMap.h"

#include "config.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	InputManager::InputManager(GingaScreenID screenId) : Thread() {
		currentXAxis          = 0;
		currentYAxis          = 0;
		maxX                  = 0;
		maxY                  = 0;
		lastEventTime         = 0;
		myScreen              = screenId;
		eventBuffer           = NULL;
		running               = false;
		notifying             = false;
		notifyingApp          = false;
		ief                   = new InputEventFactory();

		pthread_mutex_init(&actInpMutex, NULL);
		pthread_mutex_init(&actAppMutex, NULL);

		pthread_mutex_init(&appMutex, NULL);

		pthread_mutex_init(&mlMutex, NULL);

		initializeInputIntervalTime();
	}

	InputManager::~InputManager() {
		release();

		if (ief != NULL) {
			delete ief;
			ief = NULL;
		}
	}

	void InputManager::initializeInputIntervalTime() {
		string strVar;
		ifstream fis;

		imperativeIntervalTime  = 0;
		declarativeIntervalTime = 0;

		strVar = SystemCompat::appendGingaFilesPrefix("mb/config/input.cfg");

		fis.open(strVar.c_str(), ifstream::in);
		if (!fis.is_open()) {
			clog << "InputManager: can't open input file:";
			clog << strVar.c_str() << endl;
			return;
		}

		while (fis.good()) {
			fis >> strVar;
			if (strVar == "imperative.inputIntervalTime" && fis.good()) {
				fis >> strVar;
				if (strVar == "=" && fis.good()) {
					fis >> strVar;
					if (strVar != "") {
						imperativeIntervalTime = util::stof(strVar);
					}
				}

			} else if (strVar == "declarative.inputIntervalTime" &&
					fis.good()) {

				fis >> strVar;
				if (strVar == "=" && fis.good()) {
					fis >> strVar;
					if (strVar != "") {
						declarativeIntervalTime = util::stof(strVar);
					}
				}
			}
		}
		fis.close();

		clog << "InputManager::initializeInputIntervalTime imperative = '";
		clog << imperativeIntervalTime << "' declarative = '";
		clog << declarativeIntervalTime << "'" << endl;
	}

	void InputManager::release() {
		map<IInputEventListener*, set<int>*>::iterator i;
		bool runDone = false;
		IInputEvent* ie;
		int mbKeyCode;

		running = false;
		if (eventBuffer != NULL) {
			eventBuffer->wakeUp();

		} else {
			return;
		}

		while (!runDone) {
			SystemCompat::uSleep(10000);
			runDone = (maxX == 0 && maxY == 0);
		}

		mbKeyCode = LocalScreenManager::getInstance()->fromGingaToMB(
				myScreen, CodeMap::KEY_QUIT);

		ie = LocalScreenManager::getInstance()->createInputEvent(
				myScreen, NULL, mbKeyCode);

		running = true;
		dispatchEvent(ie);
		running = false;

		delete ie;

		lock();

		i = eventListeners.begin();
		while (i != eventListeners.end()) {
			if (i->second != NULL) {
				delete i->second;
			}
			++i;
		}
		eventListeners.clear();

		if (!actionsToInpListeners.empty()) {
			pthread_mutex_lock(&actInpMutex);
			actionsToInpListeners.clear();
			pthread_mutex_unlock(&actInpMutex);
		}
		pthread_mutex_destroy(&actInpMutex);


		if (!applicationListeners.empty()) {
			pthread_mutex_lock(&appMutex);
			applicationListeners.clear();

			if (!actionsToAppListeners.empty()) {
				pthread_mutex_lock(&actAppMutex);
				actionsToAppListeners.clear();
				pthread_mutex_unlock(&actAppMutex);
			}

			pthread_mutex_unlock(&appMutex);
		}
		pthread_mutex_destroy(&actAppMutex);
		pthread_mutex_destroy(&appMutex);

		if (eventBuffer != NULL) {
			delete eventBuffer;
			eventBuffer = NULL;
		}

		if (!motionListeners.empty()) {
			pthread_mutex_lock(&mlMutex);
			motionListeners.clear();
			pthread_mutex_unlock(&mlMutex);
		}
		pthread_mutex_destroy(&mlMutex);

		unlock();
	}

	void InputManager::addMotionEventListener(IMotionEventListener* listener) {
		pthread_mutex_lock(&mlMutex);
		motionListeners.insert(listener);
		pthread_mutex_unlock(&mlMutex);
	}

	void InputManager::removeMotionEventListener(
			IMotionEventListener* listener) {

		set<IMotionEventListener*>::iterator i;

		pthread_mutex_lock(&mlMutex);
		i = motionListeners.find(listener);
		if (i != motionListeners.end()) {
			motionListeners.erase(i);
		}
		pthread_mutex_unlock(&mlMutex);
	}

	void InputManager::notifyMotionListeners(int x, int y, int z) {

		set<IMotionEventListener*>::iterator i;

		pthread_mutex_lock(&mlMutex);
		i = motionListeners.begin();
		while (i != motionListeners.end()) {
			(*i)->motionEventReceived(x, y, z);
			++i;
		}
		pthread_mutex_unlock(&mlMutex);
	}

	void InputManager::addInputEventListener(
		    IInputEventListener* listener, set<int>* evs) {

		LockedAction* action;
		set<int>* iEvs;
		map<IInputEventListener*, set<int>*>::iterator i;

		if (!running) {
			clog << "InputManager::addInputEventListener add listener ";
			clog << "but I am not running. I will notify you though,";
			clog << "as soon as you start me!" << endl;
		}

		if (notifying) {
			pthread_mutex_lock(&actInpMutex);
			action = new LockedAction;
			action->l      = listener;
			action->isAdd  = true;
			action->events = evs;

			actionsToInpListeners.push_back(action);
			pthread_mutex_unlock(&actInpMutex);

		} else {
			lock();
			performInputLockedActions();
			i = eventListeners.find(listener);
			if (i != eventListeners.end()) {
				iEvs = i->second;
				eventListeners.erase(i);
				if (iEvs != NULL) {
					delete iEvs;
				}
			}
			eventListeners[listener] = evs;
			unlock();
		}
	}

	void InputManager::removeInputEventListener(
		    IInputEventListener* listener) {

		LockedAction* action;
		map<IInputEventListener*, set<int>*>::iterator i;

		if (!running) {
			return;
		}

		if (notifying) {
			pthread_mutex_lock(&actInpMutex);
			action = new LockedAction;
			action->l      = listener;
			action->isAdd  = false;
			action->events = NULL;

			actionsToInpListeners.push_back(action);
			pthread_mutex_unlock(&actInpMutex);

		} else {
			lock();
			if (!running) {
				unlock();
				return;
			}
			performInputLockedActions();
			i = eventListeners.find(listener);
			if (i != eventListeners.end()) {
				if (i->second != NULL) {
					delete i->second;
				}
				eventListeners.erase(i);
			}
			unlock();
		}
	}

	void InputManager::performInputLockedActions() {
		LockedAction* action;
		IInputEventListener* listener;

		set<int>* evs;
		map<IInputEventListener*, set<int>*>::iterator i;
		vector<LockedAction*>::iterator j;

		if (!running) {
			return;
		}

		pthread_mutex_lock(&actInpMutex);
		j = actionsToInpListeners.begin();
		while (j != actionsToInpListeners.end()) {
			action   = *j;
			listener = action->l;
			evs      = action->events;

			i = eventListeners.find(listener);
			if (i != eventListeners.end()) {
				if (i->second != NULL) {
					delete i->second;
				}
				eventListeners.erase(i);
			}

			if (action->isAdd) {
				eventListeners[listener] = evs;
			}

			delete action;
			++j;
		}

		actionsToInpListeners.clear();
		pthread_mutex_unlock(&actInpMutex);
	}

	void InputManager::performApplicationLockedActions() {
		LockedAction* action;
		IInputEventListener* listener;

		set<IInputEventListener*>::iterator i;
		vector<LockedAction*>::iterator j;

		if (!running) {
			return;
		}

		pthread_mutex_lock(&actAppMutex);
		j = actionsToAppListeners.begin();
		while (j != actionsToAppListeners.end()) {
			action = *j;
			listener = action->l;

			if (action->isAdd) {
				applicationListeners.insert(listener);

			} else {
				i = applicationListeners.find(listener);
				if (i != applicationListeners.end()) {
					applicationListeners.erase(i);
				}
			}

			delete action;
			++j;
		}

		actionsToAppListeners.clear();
		pthread_mutex_unlock(&actAppMutex);
	}

	bool InputManager::dispatchEvent(IInputEvent* inputEvent) {
		map<IInputEventListener*, set<int>*>::iterator i;

		IInputEventListener* lis;
		set<int>* evs;
		int keyCode;

		if (!running) {
			return false;
		}

		notifying = true;
		lock();
		notifying = true;

		performInputLockedActions();

		if (eventListeners.empty() || inputEvent == NULL || !running) {
			unlock();
			notifying = false;

			clog << "InputManger::dispatchEvent no listeners to notify '";
			clog << inputEvent << "'" << endl;
			return true;
		}

		keyCode = inputEvent->getKeyCode(myScreen);

		if (keyCode == CodeMap::KEY_TAP) {
			clog << "InputManger::dispatchEvent code '";
			clog << keyCode << "' screen id = '" << myScreen << "'" << endl;
		}

		i = eventListeners.begin();
		while (i != eventListeners.end() && running) {
			lis = i->first;
			evs = i->second;
			if (evs != NULL) {
				if (evs->find(keyCode) != evs->end()) {
					//return false means an event with changed keySymbol
					if (!lis->userEventReceived(inputEvent)) {
						unlock();
						notifying = false;
						return false;
					}
				}

			} else if (!lis->userEventReceived(inputEvent)) {
				unlock();
				notifying = false;
				return false;
			}
			++i;
		}
		unlock();
		notifying = false;
		return true;
	}

	bool InputManager::dispatchApplicationEvent(IInputEvent* inputEvent) {
		set<IInputEventListener*>::iterator i;
		vector<LockedAction*>::iterator j;

		if (!running) {
			return false;
		}

		notifyingApp = true;
		pthread_mutex_lock(&appMutex);

		performApplicationLockedActions();

		if (applicationListeners.empty() || inputEvent == NULL) {
			pthread_mutex_unlock(&appMutex);
			notifyingApp = false;
			return true;
		}

		i = applicationListeners.begin();
		while (i != applicationListeners.end()) {
			if (*i != NULL) {
				if (!(*i)->userEventReceived(inputEvent)) {
					pthread_mutex_unlock(&appMutex);
					notifyingApp = false;
					return false;
				}
			}
			++i;
		}
		pthread_mutex_unlock(&appMutex);
		notifyingApp = false;
		return true;
	}

	void InputManager::addApplicationInputEventListener(
		    IInputEventListener* listener) {

		LockedAction* action;

		if (!running) {
			return;
		}

		if (notifyingApp) {
			pthread_mutex_lock(&actAppMutex);
			action         = new LockedAction;
			action->l      = listener;
			action->isAdd  = true;
			action->events = NULL;

			actionsToAppListeners.push_back(action);
			pthread_mutex_unlock(&actAppMutex);

		} else {
			pthread_mutex_lock(&appMutex);
			performApplicationLockedActions();
			applicationListeners.insert(listener);
			pthread_mutex_unlock(&appMutex);
		}
	}

	void InputManager::removeApplicationInputEventListener(
		    IInputEventListener* listener) {

		LockedAction* action;
		set<IInputEventListener*>::iterator i;

		if (!running) {
			return;
		}

		if (notifyingApp) {
			pthread_mutex_lock(&actAppMutex);
			action         = new LockedAction;
			action->l      = listener;
			action->isAdd  = false;
			action->events = NULL;

			actionsToAppListeners.push_back(action);
			pthread_mutex_unlock(&actAppMutex);

		} else {
			pthread_mutex_lock(&appMutex);
			performApplicationLockedActions();
			i = applicationListeners.find(listener);
			if (i != applicationListeners.end()) {
				applicationListeners.erase(i);
			}
			pthread_mutex_unlock(&appMutex);
		}
	}

	void InputManager::postInputEvent(IInputEvent* event) {
		if (!running) {
			delete event;
			return;
		}

		eventBuffer->postInputEvent(event);
	}

	void InputManager::postInputEvent(int keyCode) {
		IInputEvent* ie;
		int mbKeyCode;

		mbKeyCode = LocalScreenManager::getInstance()->fromGingaToMB(
				myScreen, keyCode);

		ie = LocalScreenManager::getInstance()->createInputEvent(
				myScreen, NULL, mbKeyCode);

		postInputEvent(ie);
	}

	void InputManager::setAxisValues(int x, int y, int z) {
		this->currentXAxis = x;
		this->currentYAxis = y;
		this->currentZAxis = z;
	}

	void InputManager::setAxisBoundaries(int x, int y, int z) {
		this->maxX = x;
		this->maxY = y;
	}

	int InputManager::getCurrentXAxisValue() {
		return currentXAxis;
	}

	int InputManager::getCurrentYAxisValue() {
		return currentYAxis;
	}

	IEventBuffer* InputManager::getEventBuffer() {
		if (eventBuffer == NULL) {
			eventBuffer = LocalScreenManager::getInstance()->
					createEventBuffer(myScreen);

			if (!running) {
				running = true;
				Thread::startThread();
			}
		}

		return eventBuffer;
	}

	void InputManager::run() {
		IInputEvent* inputEvent;

		int pLastCode         = -1;
		int lastCode          = -1;
		double pTimeStamp     = 0;
		double timeStamp      = 0;

		int mouseX, mouseY;

#if HAVE_KINECTSUPPORT
		if (running) {
			ief->createFactory(InputEventFactory::FT_KINECT, this);
		}
#endif

		while (running && eventBuffer != NULL) {
			eventBuffer->waitEvent();
			if (!running) {
				break;
			}

			inputEvent = eventBuffer->getNextEvent();
			while (inputEvent != NULL) {
				if (inputEvent->isMotionType()) {
					mouseX = currentXAxis;
					mouseY = currentYAxis;

					inputEvent->getAxisValue(
							&currentXAxis, &currentYAxis, &currentZAxis);

					if (currentXAxis == 0) {
						currentXAxis = mouseX;

					} else if (currentXAxis > maxX && maxX != 0) {
						currentXAxis = maxX;
					}

					if (currentYAxis == 0) {
						currentYAxis = mouseY;

					} else if (currentYAxis > maxY && maxY != 0) {
						currentYAxis = maxY;
					}

					/*
					clog << "InputManager::run new currentX = '";
					clog << currentXAxis << "' currentY = '";
					clog << currentYAxis << "'" << endl;*/

					delete inputEvent;
					if (eventBuffer != NULL) {
						inputEvent = eventBuffer->getNextEvent();
					}

					notifyMotionListeners(
							currentXAxis, currentYAxis, currentZAxis);

					continue;
				}

				if (!inputEvent->isApplicationType() &&
						inputEvent->getKeyCode(myScreen) == CodeMap::KEY_NULL) {

					delete inputEvent;
					inputEvent = eventBuffer->getNextEvent();
					continue;
				}

				if (inputEvent->isButtonPressType()) {
					inputEvent->setAxisValue(currentXAxis, currentYAxis, 0);

					inputEvent->getAxisValue(
							&currentXAxis,
							&currentYAxis,
							&currentZAxis);

					dispatchEvent(inputEvent);
					delete inputEvent;
					inputEvent = eventBuffer->getNextEvent();
					continue;
				}

				if (inputEvent->isPressedType() && ((getCurrentTimeMillis() -
						timeStamp) >= declarativeIntervalTime ||
								lastCode != inputEvent->getKeyCode(myScreen))) {

					lastCode  = inputEvent->getKeyCode(myScreen);
					timeStamp = getCurrentTimeMillis();
					if (!dispatchEvent(inputEvent)) {
						delete inputEvent;
						inputEvent = eventBuffer->getNextEvent();
						continue;
					}
				}

				if (inputEvent->isKeyType() ||
						inputEvent->isApplicationType()) {

					dispatchApplicationEvent(inputEvent);
				}

				delete inputEvent;
				inputEvent = eventBuffer->getNextEvent();
			}
		}

		if (running && eventBuffer == NULL) {
			clog << "InputManager::run Warning! Can't receive events: ";
			clog << "event buffer is NULL" << endl;
		}

		currentXAxis = 0;
		currentYAxis = 0;
		currentZAxis = 0;
		maxX         = 0;
		maxY         = 0;

		clog << "InputManager::run all done" << endl;
	}
}
}
}
}
}
}
