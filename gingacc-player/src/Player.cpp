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

#include "player/Player.h"

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "player/PlayersComponentSupport.h"

#include <iostream>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace player {
	Player::Player(string mrl) {
		pthread_mutex_init(&listM, NULL);
		pthread_mutex_init(&lockedListM, NULL);
		pthread_mutex_init(&referM, NULL);

		this->mrl                 = mrl;
		this->listeners           = NULL;
		this->lockedListeners     = NULL;
		this->referredPlayers     = NULL;
		this->outputWindow        = NULL;
		this->surface             = NULL;

		this->notifying           = false;
		this->presented           = false;
		this->visible             = true;
		this->immediatelyStartVar = false;
		this->status              = STOP;
		this->forcedNaturalEnd    = false;
		this->scope               = "";
		this->scopeType           = -1;
		this->scopeInitTime       = -1;
		this->scopeEndTime        = -1;
		this->properties          = new map<string, string>;
		this->notifyContentUpdate = false;
	}

	Player::~Player() {
		this->status = STOP;
		pthread_mutex_lock(&listM);
		if (listeners != NULL) {
			listeners->clear();
			delete listeners;
			listeners = NULL;
		}

		pthread_mutex_lock(&lockedListM);
		if (lockedListeners != NULL) {
			lockedListeners->clear();
			delete lockedListeners;
			lockedListeners = NULL;
		}

		if (outputWindow != NULL) {
			outputWindow->revertContent();
			delete outputWindow;
			outputWindow = NULL;
		}

		if (surface != NULL) {
			delete surface;
			surface = NULL;
		}

		pthread_mutex_lock(&referM);
		if (referredPlayers != NULL) {
			referredPlayers->clear();
			delete referredPlayers;
			referredPlayers = NULL;
		}

		if (properties != NULL) {
			delete properties;
			properties = NULL;
		}

		pthread_mutex_unlock(&referM);
		pthread_mutex_unlock(&lockedListM);
		pthread_mutex_unlock(&listM);

		pthread_mutex_destroy(&referM);
		pthread_mutex_destroy(&lockedListM);
		pthread_mutex_destroy(&listM);
   	}

	void Player::setMrl(string mrl, bool visible) {
		this->mrl     = mrl;
		this->visible = visible;
	}

	void Player::setNotifyContentUpdate(bool notify) {
		this->notifyContentUpdate = notify;
	}

	void Player::addListener(IPlayerListener* listener) {
		LockedPlayerListener* lpl = NULL;

		if (notifying) {
			pthread_mutex_lock(&lockedListM);
			lpl        = new LockedPlayerListener;
			lpl->isAdd = true;
			lpl->l     = listener;

			if (lockedListeners == NULL) {
				lockedListeners = new vector<LockedPlayerListener*>;
			}
			lockedListeners->push_back(lpl);
			pthread_mutex_unlock(&lockedListM);

		} else {
			pthread_mutex_lock(&listM);
			if (listeners == NULL) {
				this->listeners = new set<IPlayerListener*>;
			}
			listeners->insert(listener);
			pthread_mutex_unlock(&listM);
		}
	}

	void Player::removeListener(IPlayerListener* listener) {
		LockedPlayerListener* lpl = NULL;
		set<IPlayerListener*>::iterator i;

		if (notifying) {
			pthread_mutex_lock(&lockedListM);
			lpl        = new LockedPlayerListener;
			lpl->isAdd = false;
			lpl->l     = listener;
			if (lockedListeners == NULL) {
				lockedListeners = new vector<LockedPlayerListener*>;
			}
			lockedListeners->push_back(lpl);
			pthread_mutex_unlock(&lockedListM);

		} else {
			pthread_mutex_lock(&listM);
			if (listeners != NULL) {
				i = listeners->find(listener);
				if (i != listeners->end()) {
					listeners->erase(i);
				}
			}
			pthread_mutex_unlock(&listM);
		}
	}

	void Player::performLockedListenersRequest() {
		LockedPlayerListener* lpl;
		vector<LockedPlayerListener*>::iterator i;
		IPlayerListener* listener;
		set<IPlayerListener*>::iterator j;

		pthread_mutex_lock(&lockedListM);
		if (lockedListeners != NULL && listeners != NULL) {
			i = lockedListeners->begin();
			while (i != lockedListeners->end()) {
				lpl      = *i;
				listener = lpl->l;

				if (lpl->isAdd) {
					listeners->insert(listener);

				} else {
					j = listeners->find(listener);
					if (j != listeners->end()) {
						listeners->erase(j);
					}
				}

				delete lpl;
				++i;
			}
			lockedListeners->clear();
		}
		pthread_mutex_unlock(&lockedListM);
	}

	void Player::notifyListeners(short code, string parameter, short type) {
		set<IPlayerListener*>::iterator i, j;

		notifying = true;
		pthread_mutex_lock(&listM);
		notifying = true;

		performLockedListenersRequest();

		if (listeners == NULL || listeners->empty()) {
			if (code == PL_NOTIFY_STOP) {
				presented = true;
			}
			pthread_mutex_unlock(&listM);
			notifying = false;
			return;
		}

		if (code == PL_NOTIFY_NCLEDIT) {
			i = listeners->begin();
			while (i != listeners->end()) {
				if ((*i) != NULL) {
					(*i)->updateStatus(code, parameter, type);
				}
				++i;
			}

		} else {
			if (code == PL_NOTIFY_STOP) {
				presented = true;
			}

			i = listeners->begin();
			while (i != listeners->end()) {
				if ((*i) != NULL) {
					(*i)->updateStatus(code, parameter, type);
				}
				++i;
			}
		}

		pthread_mutex_unlock(&listM);
		notifying = false;
	}

	void Player::setSurface(ISurface* surface) {
		if (this->surface != NULL) {
			delete this->surface;
			this->surface = NULL;
		}
		this->surface = surface;
	}

	ISurface* Player::getSurface() {
		return surface;
	}

	void Player::setMediaTime(double newTime) {
		this->elapsedTime = newTime;
	}

	double Player::getMediaTime() {
		double mediaTime;
		mediaTime = 0;

		if (status == PAUSE) {
			mediaTime = elapsedTime;

		} else {
			mediaTime = elapsedTime + getCurrentTimeMillis() -
				    initTime - elapsedPause;
		}

		return (mediaTime / 1000);
	}

	bool Player::setKeyHandler(bool isHandler) {
		if (isHandler) {
#ifndef _WIN32
			::usleep(200000);
#else
			Sleep(200);
#endif
		}
		return false;
	}

	void Player::setScope(
			string scope, short type, double initTime, double endTime) {

		clog << "Player::setScope '" << scope << "'" << endl;
		this->scope         = scope;
		this->scopeType     = type;
		this->scopeInitTime = initTime;
		this->scopeEndTime  = endTime;
	}

	void Player::play() {
		this->forcedNaturalEnd = false;
		this->status = PLAY;
		if (scopeInitTime > 0) {
			elapsedTime = scopeInitTime * 1000;

		} else {
			elapsedTime = 0;
		}
		elapsedPause = 0;
		initTime = getCurrentTimeMillis();
	}

	void Player::stop() {
		this->status = STOP;
	}

	void Player::abort() {
		stop();
	}

	void Player::pause() {
		pauseTime = getCurrentTimeMillis();
		elapsedTime = elapsedTime + (pauseTime - initTime);
		this->status = PAUSE;
	}

	void Player::resume() {
		initTime = getCurrentTimeMillis();
		elapsedPause = elapsedPause + (initTime - pauseTime);
		this->status = PLAY;
	}

	string Player::getPropertyValue(string name) {
		if (properties->count(name) != 0) {
			return (*properties)[name];
		}

		return "";
	}

	void Player::setPropertyValue(string name, string value) {
		vector<string>* params = NULL;

		if (outputWindow != NULL) {
			if (name == "bounds") {
				params = split(value, ",");
				if (params->size() == 4) {
					outputWindow->setBounds(
							util::stof((*params)[0]),
							util::stof((*params)[1]),
							util::stof((*params)[2]),
							util::stof((*params)[3]));
				}
				delete params;

			} else if (name == "location") {
				params = split(value, ",");
				if (params->size() == 2) {
					outputWindow->moveTo(
							util::stof((*params)[0]), util::stof((*params)[1]));
				}
				delete params;

			} else if (name == "size") {
				params = split(value, ",");
				if (params->size() == 2) {
					outputWindow->resize(
							util::stof((*params)[0]), util::stof((*params)[1]));
				}
				delete params;

			} else if (name == "left") {
				outputWindow->setX(util::stof(value));

			} else if (name == "top") {
				outputWindow->setY(util::stof(value));

			} else if (name == "width") {
				outputWindow->setW(util::stof(value));

			} else if (name == "height") {
				outputWindow->setH(util::stof(value));

			} else if (name == "transparency") {
				outputWindow->setCurrentTransparency(util::stof(value));
			}
		}
		(*properties)[name] = value;
	}

	void Player::addTimeReferPlayer(IPlayer* referPlayer) {
		pthread_mutex_lock(&referM);
		if (referredPlayers == NULL) {
			referredPlayers = new set<IPlayer*>;
		}

		referredPlayers->insert(referPlayer);
		pthread_mutex_unlock(&referM);
	}

	void Player::removeTimeReferPlayer(IPlayer* referPlayer) {
		if (referredPlayers == NULL) {
			return;
		}

		pthread_mutex_lock(&referM);
		set<IPlayer*>::iterator i;

		i = referredPlayers->find(referPlayer);
		if (i != referredPlayers->end()) {
			referredPlayers->erase(i);
			pthread_mutex_unlock(&referM);
			return;
		}
		pthread_mutex_unlock(&referM);
	}

	void Player::notifyReferPlayers(int transition) {
		set<IPlayer*>::iterator i;

		pthread_mutex_lock(&referM);
		if (referredPlayers == NULL) {
			pthread_mutex_unlock(&referM);
			return;
		}

		i = referredPlayers->begin();
		while (i != referredPlayers->end()) {
			(*i)->timebaseObjectTransitionCallback(transition);
			++i;
		}
		pthread_mutex_unlock(&referM);
	}

	void Player::timebaseObjectTransitionCallback(int transition) {
		if (transition == PL_NOTIFY_STOP) {
			setReferenceTimePlayer(NULL);
			stop();
		}
	}

	void Player::setTimeBasePlayer(IPlayer* timeBasePlayer) {
		clog << "Player::setTimeBasePlayer" << endl;
		if (timeBasePlayer != NULL) {
			this->timeBasePlayer = timeBasePlayer;
			this->timeBasePlayer->addTimeReferPlayer(this);
		}
	}

	bool Player::hasPresented() {
		return presented;
	}

	void Player::setPresented(bool presented) {
		this->presented = presented;
	}

	bool Player::isVisible() {
		return this->visible;
	}

	void Player::setVisible(bool visible) {
		this->visible = visible;
	}

	bool Player::immediatelyStart() {
		return immediatelyStartVar;
	}

	void Player::setImmediatelyStart(bool immediattelyStartVal) {
		this->immediatelyStartVar = immediattelyStartVal;
	}

	void Player::checkScopeTime() {
		pthread_t threadId_;

		if (scopeInitTime >= 0 && scopeEndTime >= scopeInitTime) {
			pthread_create(
					&threadId_, 0, Player::scopeTimeHandler, (void*)this);

			pthread_detach(threadId_);
		}
	}

	void* Player::scopeTimeHandler(void* ptr) {
		double expectedSleepTime;
		Player* p = (Player*)ptr;

		expectedSleepTime = (
				p->scopeEndTime -
				(p->scopeInitTime + (p->getMediaTime() / 1000)));

		clog << "Player::scopeTimeHandler expectedSleepTime = '";
		clog << expectedSleepTime << "'" << endl;

		if (expectedSleepTime > 0) {
#ifndef _WIN32
			::usleep(expectedSleepTime * 1000000);
#else
			Sleep(expectedSleepTime * 1000);
#endif
		}

		p->forceNaturalEnd(true);

		return ptr;
	}

	void Player::forceNaturalEnd(bool forceIt) {
		forcedNaturalEnd = forceIt;
		if (forceIt) {
			notifyListeners(PL_NOTIFY_STOP);
		}
	}

	bool Player::isForcedNaturalEnd() {
		if (mrl == "") {
			return false;
		}
		return forcedNaturalEnd;
	}

	bool Player::setOutWindow(int windowId) {
#if HAVE_MULTIPROCESS
		ISurface* renderedSurface;

		if (windowId < 0 && outputWindow != NULL) {
			outputWindow->revertContent();

		} else if (outputWindow == NULL) {
#if HAVE_COMPSUPPORT
			outputWindow = ((WindowCreator*)(cm->getObject("Window")))(
					windowId, -1, -1, -1, -1);

#else
			outputWindow = new DFBWindow(windowId);
#endif
		}

		if (outputWindow == NULL || outputWindow->getId() != windowId) {
			return false;
		}

		renderedSurface = getSurface();
		if (renderedSurface != NULL) {
			if (renderedSurface->setParent((void*)outputWindow)) {
				outputWindow->renderFrom(renderedSurface);
			}

		} else {
			return false;
		}
#endif
		return true;
	}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::player::IPlayer* createPlayer(
		const char* mrl, bool hasVisual) {

	return (new ::br::pucrio::telemidia::ginga::core::player::Player(""));
}

extern "C" void destroyPlayer(
		::br::pucrio::telemidia::ginga::core::player::IPlayer* player) {

	delete player;
}
