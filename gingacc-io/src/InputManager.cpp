/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware
Ginga (Ginga-NCL).

Direitos Autorais Reservados (c) 1989-2007 PUC-Rio/Laboratorio TeleMidia

Este programa eh software livre; voce pode redistribui-lo e/ou modificah-lo sob
os termos da Licen�a Publica Geral GNU versao 2 conforme publicada pela Free
Software Foundation.

Este programa eh distribu�do na expectativa de que seja util, porem, SEM
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

#include "../include/InputManager.h"
#include "../include/IOHandler.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace io {
	InputManager::InputManager() : Thread() {
		eventListeners = new map<InputEventListener*, set<int>*>;
		toAddEventListeners = new map<InputEventListener*, set<int>*>;
		toRemoveEventListeners = new set<InputEventListener*>;
		proceduralListeners = new set<InputEventListener*>;
		eventBuffer = NULL;
		lastEventTime = 0;
		eventBuffer = new InputEventBuffer();
		running = true;
		notifying = false;

		pthread_mutex_init(&addMutex, NULL);
		pthread_mutex_init(&removeMutex, NULL);

		pthread_mutex_init(&procMutex, NULL);

		this->start();
		timeStamp = -500;
	}

	void InputManager::release() {
		running = false;
		lock();
		notifying = true;
		wclog << "InputManager::release" << endl;
		if (eventBuffer != NULL) {
			eventBuffer->wakeUp();
		}

		if (eventListeners != NULL) {
			eventListeners->clear();
			delete eventListeners;
			eventListeners = NULL;
		}

		pthread_mutex_lock(&addMutex);
		if (toAddEventListeners != NULL) {
			toAddEventListeners->clear();
			delete toAddEventListeners;
			toAddEventListeners = NULL;
		}
		pthread_mutex_unlock(&addMutex);
		pthread_mutex_destroy(&addMutex);

		pthread_mutex_lock(&removeMutex);
		if (toRemoveEventListeners != NULL) {
			toRemoveEventListeners->clear();
			delete toRemoveEventListeners;
			toRemoveEventListeners = NULL;
		}
		pthread_mutex_unlock(&removeMutex);
		pthread_mutex_destroy(&removeMutex);

		pthread_mutex_lock(&procMutex);
		if (proceduralListeners != NULL) {
			proceduralListeners->clear();
			delete proceduralListeners;
			proceduralListeners = NULL;
		}
		pthread_mutex_unlock(&procMutex);
		pthread_mutex_destroy(&procMutex);

		if (eventBuffer != NULL) {
			delete eventBuffer;
			eventBuffer = NULL;
		}
	}

	InputManager* InputManager::_instance = 0;

	InputManager* InputManager::getInstance() {
		if (InputManager::_instance == NULL) {
			InputManager::_instance = new InputManager();
		}
		return InputManager::_instance;
	}

	void InputManager::addInputEventListener(
		    InputEventListener* listener, set<int>* events) {

		map<InputEventListener*, set<int>*>::iterator i;

		if (notifying) {
			pthread_mutex_lock(&addMutex);
			(*toAddEventListeners)[listener] = events;
			pthread_mutex_unlock(&addMutex);

		} else {
			lock();
			i = eventListeners->find(listener);
			if (i != eventListeners->end()) {
				delete i->second;
				eventListeners->erase(i);
			}
			(*eventListeners)[listener] = events;
			unlock();
		}
	}

	void InputManager::removeInputEventListener(
		    InputEventListener* listener) {

		map<InputEventListener*, set<int>*>::iterator i;

		if (notifying) {
			pthread_mutex_lock(&removeMutex);
			toRemoveEventListeners->insert(listener);
			pthread_mutex_unlock(&removeMutex);

		} else {
			lock();
			i = eventListeners->find(listener);
			if (i != eventListeners->end()) {
				delete i->second;
				eventListeners->erase(i);
			}
			unlock();
		}
	}

	bool InputManager::dispatchEvent(InputEvent* inputEvent) {
		map<InputEventListener*, set<int>*>::iterator i, j;
		set<InputEventListener*>::iterator k;
		set<int>::iterator l;
		set<int>* evs;
		int keyCode;

		notifying = true;
		lock();
		pthread_mutex_lock(&removeMutex);
		k = toRemoveEventListeners->begin();
		while (k != toRemoveEventListeners->end()) {
			j = eventListeners->find(*k);
			if (j != eventListeners->end()) {
				delete j->second;
				eventListeners->erase(j);
			}
			++k;
		}
		toRemoveEventListeners->clear();
		pthread_mutex_unlock(&removeMutex);

		pthread_mutex_lock(&addMutex);
		i = toAddEventListeners->begin();
		while (i != toAddEventListeners->end()) {
			j = eventListeners->find(i->first);
			if (j != eventListeners->end()) {
				if (i->second != j->second) {
					delete j->second;
				}
				eventListeners->erase(j);
			}
			(*eventListeners)[i->first] = i->second;
			++i;
		}
		toAddEventListeners->clear();
		pthread_mutex_unlock(&addMutex);

		if (eventListeners->empty() || inputEvent == NULL) {
			cout << "InputManager::dispatchEvent Warning!";
			cout << "no listeners found or inputEvent is null"<< endl;
			unlock();
			notifying = false;
			return true;
		}

		keyCode = inputEvent->getKeyCode();
		i = eventListeners->begin();
		while (i != eventListeners->end()) {
			evs = i->second;
			if (evs != NULL) {
				l = evs->begin();
				while (l != evs->end()) {
					if (*l == keyCode) {
						//return false means an event with changed keySymbol
						if (!i->first->userEventReceived(inputEvent)) {
							unlock();
							notifying = false;
							return false;
						}
					}
					++l;
				}

			} else if (!i->first->userEventReceived(inputEvent)) {
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

	bool InputManager::dispatchProceduralEvent(InputEvent* inputEvent) {
		pthread_mutex_lock(&procMutex);
		if (proceduralListeners->empty() || inputEvent == NULL) {
			pthread_mutex_unlock(&procMutex);
			return true;
		}

		set<InputEventListener*>::iterator i;
		i = proceduralListeners->begin();
		while (i != proceduralListeners->end()) {
			if (*i != NULL) {
				if (!(*i)->userEventReceived(inputEvent)) {
					pthread_mutex_unlock(&procMutex);
					return false;
				}
			}
			++i;
		}
		pthread_mutex_unlock(&procMutex);
		return true;
	}

	void InputManager::addProceduralInputEventListener(
		    InputEventListener* listener) {

		pthread_mutex_lock(&procMutex);
		proceduralListeners->insert(listener);
		pthread_mutex_unlock(&procMutex);
	}

	void InputManager::removeProceduralInputEventListener(
		    InputEventListener* listener) {

		set<InputEventListener*>::iterator i;

		pthread_mutex_lock(&procMutex);
		i = proceduralListeners->find(listener);
		if (i != proceduralListeners->end()) {
			proceduralListeners->erase(i);
		}
		pthread_mutex_unlock(&procMutex);
	}

	void InputManager::postEvent(InputEvent* event) {
		eventBuffer->postEvent(event);
	}

	void InputManager::postEvent(int keyCode) {
		postEvent(new InputEvent(keyCode));
	}

	void InputManager::run() {
		InputEvent* inputEvent;

		while (running && eventBuffer != NULL) {
			eventBuffer->waitEvent();
			if (!running) {
				break;
			}

			inputEvent = eventBuffer->getNextEvent();
			while (inputEvent != NULL) {
				if (inputEvent->isPressedType() &&
						((getCurrentTimeMillis() - timeStamp) > 150)) {

					timeStamp = getCurrentTimeMillis();
					if (!dispatchEvent(inputEvent)) {
						delete inputEvent;
						inputEvent = eventBuffer->getNextEvent();
						continue;
					}
				}

				dispatchProceduralEvent(inputEvent);
				delete inputEvent;
				inputEvent = eventBuffer->getNextEvent();
			}
		}
	}
}
}
}
}
}
}
