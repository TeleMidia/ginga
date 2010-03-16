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

#include "../include/PlayerProcess.h"

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include <iostream>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace player {
	PlayerProcess::PlayerProcess(const char* objName) : Process(NULL) {
		msgs = new map<string, string>;

		pthread_mutex_init(&msgMutex, NULL);

		isWaitingAns = false;
		pthread_mutex_init(&ansMutex, NULL);
		pthread_cond_init(&ansCond, NULL);

		setProcessInfo("/usr/local/etc/ginga/tools/loaders/players", objName);
		run();
		checkCom();
	}

	PlayerProcess::~PlayerProcess() {
		reader = false;

		if (isWaitingAns) {
			pthread_cond_signal(&ansCond);
		}

		isWaitingAns = false;
		pthread_mutex_destroy(&ansMutex);
		pthread_cond_destroy(&ansCond);

		pthread_mutex_lock(&msgMutex);
		if (msgs != NULL) {
			delete msgs;
			msgs = NULL;
		}

		pthread_mutex_unlock(&msgMutex);
		pthread_mutex_destroy(&msgMutex);
	}

	void PlayerProcess::createPlayer(string mrl, bool visible) {
		string strbool = "true";
		if (!visible) {
			strbool = "false";
		}

		sendMsg("createplayer," + mrl + "," + strbool + "::;::");
	}

	void PlayerProcess::waitAnswer(int milliseconds) {
		int res;
		struct timeval time;
		struct timespec timeOut;
		long int micro;

		gettimeofday(&time, NULL);
		timeOut.tv_sec = time.tv_sec + (long int)(milliseconds / 1000);
		micro = ((milliseconds%1000) * 1000) + time.tv_usec;
		if (micro > 1000000) {
			timeOut.tv_sec++;
			micro -= 1000000;
		}

		timeOut.tv_nsec = micro * 1000;

		pthread_mutex_lock(&ansMutex);
		isWaitingAns = true;
		pthread_cond_timedwait(
				&ansCond, &ansMutex, (const struct timespec*)(&timeOut));

		isWaitingAns = false;
		pthread_mutex_unlock(&ansMutex);
	}

	void PlayerProcess::setGhost(bool isGhost) {
		string strbool = "true";
		if (!isGhost) {
			strbool = "false";
		}

		sendMsg("setghost," + strbool + "::;::");
	}

	void PlayerProcess::messageReceived(string msg) {
		string key, value;
		vector<string>* vMsg;
		vector<string>::iterator i;

		if (msg.find("=") != std::string::npos) {
			key   = msg.substr(0, msg.find_first_of("="));
			value = msg.substr(
					msg.find_first_of("=") + 1,
					msg.length() - msg.find_first_of("=") + 1);

			pthread_mutex_lock(&msgMutex);
			(*msgs)[key] = value;
			pthread_mutex_unlock(&msgMutex);
			if (isWaitingAns) {
				pthread_cond_signal(&ansCond);
			}

		} else {
			vMsg = split(msg, ",");
			if ((*vMsg)[0] == "updatestatus" && vMsg->size() == 4) {
				notifyListeners(stof((*vMsg)[1]),(*vMsg)[2], stof((*vMsg)[3]));
			}
		}
	}

	void PlayerProcess::setNotifyContentUpdate(bool notify) {
		string strbool = "true";
		if (!notify) {
			strbool = "false";
		}

		sendMsg("setnotifycontentupdate," + strbool + "::;::");
	}

	void PlayerProcess::addListener(IPlayerListener* listener) {

	}

	void PlayerProcess::removeListener(IPlayerListener* listener) {

	}

	void PlayerProcess::notifyListeners(
			short code, string paremeter, short type) {

	}

	void PlayerProcess::setSurface(io::ISurface* surface) {

	}

	io::ISurface* PlayerProcess::getSurface() {

	}

	int64_t PlayerProcess::getVPts() {

	}

	double PlayerProcess::getMediaTime() {

	}

	void PlayerProcess::setMediaTime(double newTime) {

	}

	bool PlayerProcess::setKeyHandler(bool isHandler) {

	}

	void PlayerProcess::setScope(
			string scope, short type, double begin, double end) {

		sendMsg(
				"setscope," +
				itos(type)  + "," +
				itos(begin) + "," +
				itos(end)   + "::;::");
	}

	void PlayerProcess::play() {
		sendMsg("play::;::");
	}

	void PlayerProcess::stop() {
		sendMsg("stop::;::");
	}

	void PlayerProcess::abort() {
		sendMsg("abort::;::");
	}

	void PlayerProcess::pause() {
		sendMsg("pause::;::");
	}

	void PlayerProcess::resume() {
		sendMsg("resume::;::");
	}

	string PlayerProcess::getPropertyValue(string name) {

	}

	void PlayerProcess::setPropertyValue(
			string name, string value, double duration, double by) {

	}

	void PlayerProcess::setReferenceTimePlayer(IPlayer* player) {

	}

	void PlayerProcess::addTimeReferPlayer(IPlayer* referPlayer) {

	}

	void PlayerProcess::removeTimeReferPlayer(IPlayer* referPlayer) {

	}

	void PlayerProcess::notifyReferPlayers(int transition) {

	}

	void PlayerProcess::timebaseObjectTransitionCallback(int transition) {

	}

	void PlayerProcess::setTimeBasePlayer(IPlayer* timeBasePlayer) {

	}

	bool PlayerProcess::hasPresented() {

	}

	void PlayerProcess::setPresented(bool presented) {

	}

	bool PlayerProcess::isVisible() {

	}

	void PlayerProcess::setVisible(bool visible) {

	}

	bool PlayerProcess::immediatelyStart() {

	}

	void PlayerProcess::setImmediatelyStart(bool immediatelyStartVal) {

	}

	void PlayerProcess::forceNaturalEnd() {

	}

	bool PlayerProcess::isForcedNaturalEnd() {

	}

	bool PlayerProcess::setOutWindow(int windowId) {
		sendMsg("setoutwindow," + itos(windowId) + "::;::");
	}

	IPlayer* PlayerProcess::getSelectedPlayer() {

	}

	void PlayerProcess::setPlayerMap(map<string, IPlayer*>* objs) {

	}

	map<string, IPlayer*>* PlayerProcess::getPlayerMap() {

	}

	IPlayer* PlayerProcess::getPlayer(string objectId) {

	}

	void PlayerProcess::select(IPlayer* selObject) {

	}

	void PlayerProcess::setCurrentScope(string scopeId) {

	}

	void PlayerProcess::timeShift(string direction) {

	}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::player::IPlayer*
createPlayerProcess(const char* objectName, bool hasVisual) {
	return new ::br::pucrio::telemidia::ginga::core::player::PlayerProcess(
			objectName);
}

extern "C" void destroyPlayerProcess(
		::br::pucrio::telemidia::ginga::core::player::IPlayer* player) {

	delete player;
}
