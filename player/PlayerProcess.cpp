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

#include "player/PlayerProcess.h"

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
	PlayerProcess::PlayerProcess(const char* objName) :
			Player(""), Process(NULL) {

		msgs = new map<string, string>;

		Thread::mutexInit(&msgMutex, NULL);

		isWaitingAns = false;
		Thread::mutexInit(&ansMutex, NULL);
		Thread::condInit(&ansCond, NULL);

		init(objName);
		setProcessListener(this);
	}

	PlayerProcess::~PlayerProcess() {
		clog << "PlayerProcess::~PlayerProcess(" << this;
		clog << ") => '" << mrl << "'" << endl;

		reader = false;

		if (isWaitingAns) {
			Thread::condSignal(&ansCond);
		}

		isWaitingAns = false;
		Thread::mutexDestroy(&ansMutex);
		Thread::condDestroy(&ansCond);

		Thread::mutexLock(&msgMutex);
		if (msgs != NULL) {
			delete msgs;
			msgs = NULL;
		}

		Thread::mutexUnlock(&msgMutex);
		Thread::mutexDestroy(&msgMutex);
	}

	void PlayerProcess::init(const char* objName) {
		setProcessInfo(
				"/usr/etc/ginga/tools/loaders/players",
				objName);

		run();
		checkCom();
	}

	void PlayerProcess::receiveProcessSignal(int sigType, int pSig, int ppid) {
		notifyPlayerListeners(PL_NOTIFY_STOP, itos(pSig), TYPE_SIGNAL);
	}

	void PlayerProcess::setMrl(string mrl, bool visible) {
		string strbool = "true";
		if (!visible) {
			strbool = "false";
		}

		clog << "PlayerProcess::setMrl '" << mrl << "'" << endl;

		sendMsg("createplayer," + mrl + "," + strbool + "::;::");
		Player::setMrl(mrl, visible);
	}

	void PlayerProcess::reset() {
		Process::release();

		init(objName.c_str());
		setProcessListener(this);
		setMrl(mrl, visible);
		setOutWindow(windowId);
	}

	void PlayerProcess::rebase() {
		if (status == PLAY) {
			play();

		} else if (status == PAUSE) {
			play();
			pause();
		}
	}

	string PlayerProcess::getAnswer(string token, int howPatient) {
		string ans = "";
		map<string, string>::iterator i;

		clog << "PlayerProcess::getAnswer '" << token << "'" << endl;
		while (reader) {
			Thread::mutexLock(&msgMutex);
			i = msgs->find(token);
			if (i != msgs->end()) {
				ans = i->second;
				msgs->erase(i);
				Thread::mutexUnlock(&msgMutex);
				return ans;

			} else {
				Thread::mutexUnlock(&msgMutex);
				waitAnswer(howPatient);
			}
		}

		return ans;
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

		Thread::mutexLock(&ansMutex);
		isWaitingAns = true;
		pthread_cond_timedwait(
				&ansCond, &ansMutex, (const struct timespec*)(&timeOut));

		isWaitingAns = false;
		Thread::mutexUnlock(&ansMutex);
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

			Thread::mutexLock(&msgMutex);
			(*msgs)[key] = value;
			Thread::mutexUnlock(&msgMutex);
			if (isWaitingAns) {
				Thread::condSignal(&ansCond);
			}

		} else if (msg.find(",") != std::string::npos) {
			vMsg = split(msg, ",");
			if ((*vMsg)[0] == "updatestatus" && vMsg->size() == 4) {
				if ((*vMsg)[2] == "NULL") {
					(*vMsg)[2] = "";
				}
				notifyPlayerListeners(stof((*vMsg)[1]), (*vMsg)[2], stof((*vMsg)[3]));
			}
		}
	}

	void PlayerProcess::setNotifyContentUpdate(bool notify) {
		Player::setNotifyContentUpdate(notify);
	}

	void PlayerProcess::addListener(IPlayerListener* listener) {
		Player::addListener(listener);
	}

	void PlayerProcess::removeListener(IPlayerListener* listener) {
		Player::removeListener(listener);
	}

	void PlayerProcess::notifyPlayerListeners(
			short code, string parameter, short type, string value) {

		Player::notifyPlayerListeners(code, parameter, type, value);
	}

	int64_t PlayerProcess::getVPts() {
		string ans;

		sendMsg("getvpts::;::");

		ans = getAnswer("vpts", 1000);
		if (ans == "") {
			return 0.0;
		}

		return stof(ans);
	}

	double PlayerProcess::getMediaTime() {
		return Player::getMediaTime();
	}

	void PlayerProcess::setMediaTime(double newTime) {
		if (status == PLAY) {
			sendMsg("setmediatime," + itos(newTime + 0.25) + "::;::");

		} else {
			sendMsg("setmediatime," + itos(newTime) + "::;::");
		}
		Player::setMediaTime(newTime);
	}

	bool PlayerProcess::setKeyHandler(bool isHandler) {
		string auxstr = "true";
		if (!isHandler) {
			auxstr = "false";
		}

		sendMsg("setkeyhandler," + auxstr + "::;::");
		auxstr = getAnswer("iskeyhandler", 1000);
		if (auxstr == "") {
			return false;
		}

		return auxstr == "true";
	}

	void PlayerProcess::setScope(
			string scope,
			short type,
			double begin, double end, double outTransDur) {

		sendMsg(
				"setscope," +
				scope       + "," +
				itos(type)  + "," +
				itos(begin) + "," +
				itos(end)   + "::;::");

		Player::setScope(scope, type, begin, end, outTransDur);
	}

	void PlayerProcess::play() {
		sendMsg("play::;::");
		Player::play();
	}

	void PlayerProcess::stop() {
		sendMsg("stop::;::");
		Player::stop();
	}

	void PlayerProcess::abort() {
		sendMsg("abort::;::");
		Player::abort();
	}

	void PlayerProcess::pause() {
		sendMsg("pause::;::");
		Player::pause();
	}

	void PlayerProcess::resume() {
		sendMsg("resume::;::");
		Player::resume();
	}

	string PlayerProcess::getPropertyValue(string name) {
		string ans;

		sendMsg("getpropertyvalue," + name + "::;::");

		ans = getAnswer("propertyvalue" + name, 1000);
		if (ans == "") {
			return "";
		}

		return ans;
	}

	void PlayerProcess::setPropertyValue(string name, string value) {
		string msg = "setpropertyvalue," + name + "," + value;

		sendMsg(msg + "::;::");
	}

	void PlayerProcess::setReferenceTimePlayer(IPlayer* player) {
		Player::setReferenceTimePlayer(player);
	}

	void PlayerProcess::addTimeReferPlayer(IPlayer* referPlayer) {
		Player::addTimeReferPlayer(referPlayer);
	}

	void PlayerProcess::removeTimeReferPlayer(IPlayer* referPlayer) {
		Player::removeTimeReferPlayer(referPlayer);
	}

	void PlayerProcess::notifyReferPlayers(int transition) {
		Player::notifyReferPlayers(transition);
	}

	void PlayerProcess::timebaseObjectTransitionCallback(int transition) {
		Player::timebaseObjectTransitionCallback(transition);
	}

	void PlayerProcess::setTimeBasePlayer(IPlayer* timeBasePlayer) {
		Player::setTimeBasePlayer(timeBasePlayer);
	}

	bool PlayerProcess::hasPresented() {
		return Player::hasPresented();
	}

	void PlayerProcess::setPresented(bool presented) {
		Player::setPresented(presented);
	}

	bool PlayerProcess::isVisible() {
		return Player::isVisible();
	}

	void PlayerProcess::setVisible(bool visible) {
		Player::setVisible(visible);
	}

	bool PlayerProcess::immediatelyStart() {
		return Player::immediatelyStart();
	}

	void PlayerProcess::setImmediatelyStart(bool immediatelyStartVal) {
		Player::setImmediatelyStart(immediatelyStartVal);
	}

	void PlayerProcess::forceNaturalEnd(bool forceIt) {
		Player::forceNaturalEnd(forceIt);
	}

	bool PlayerProcess::isForcedNaturalEnd() {
		return Player::isForcedNaturalEnd();
	}

	bool PlayerProcess::setOutWindow(GingaWindowID windowId) {
		this->windowId = windowId;
		sendMsg("setoutwindow," + itos((unsigned long)windowId) + "::;::");
		return true;
	}

	IPlayer* PlayerProcess::getSelectedPlayer() {
		return NULL;
	}

	void PlayerProcess::setPlayerMap(map<string, IPlayer*>* objs) {

	}

	map<string, IPlayer*>* PlayerProcess::getPlayerMap() {
		return NULL;
	}

	IPlayer* PlayerProcess::getPlayer(string objectId) {
		return NULL;
	}

	void PlayerProcess::select(IPlayer* selObject) {

	}

	void PlayerProcess::setCurrentScope(string scopeId) {
		sendMsg("setcurrentscope," + scopeId + "::;::");
	}

	void PlayerProcess::timeShift(string direction) {
		sendMsg("timeshift," + direction + "::;::");
	}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::player::IPlayer*
createPlayerProcess(const char* objectName) {
	return new ::br::pucrio::telemidia::ginga::core::player::PlayerProcess(
			objectName);
}

extern "C" void destroyPlayerProcess(
		::br::pucrio::telemidia::ginga::core::player::IPlayer* player) {

	delete player;
}
