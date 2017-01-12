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

#ifndef __PlayerProcess_h_
#define __PlayerProcess_h_

#include "Player.h"

#include "system/process/Process.h"
#include "system/process/IProcessListener.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::process;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace player {
	class PlayerProcess :
			public Player, public Process, public IProcessListener {

		private:
			map<string, string>* msgs;
			pthread_mutex_t msgMutex;

			bool isWaitingAns;
			pthread_mutex_t ansMutex;
			pthread_cond_t ansCond;
			int windowId;

		public:
			PlayerProcess(GingaScreenID screenId, const char* objectName);
			virtual ~PlayerProcess();

		private:
			void init(const char* objName);

		public:
			void receiveProcessSignal(int sigType, int pSig, int ppid);
			void setMrl(string mrl, bool visible=true);
			void reset();
			void rebase();

		private:
			string getAnswer(string token, int howPatient);
			void waitAnswer(int milliseconds);

		public:
			void messageReceived(string msg);

			void setNotifyContentUpdate(bool notify);
			void addListener(IPlayerListener* listener);
			void removeListener(IPlayerListener* listener);
			void notifyPlayerListeners(
					short code, string paremeter, short type, string value);

			int64_t getVPts();
			double getMediaTime();
			void setMediaTime(double newTime);
			bool setKeyHandler(bool isHandler);
			void setScope(
					string scope,
					short type,
					double begin=-1, double end=-1, double outTransDur=-1);

			bool play();
			void stop();
			void abort();
			void pause();
			void resume();

			string getPropertyValue(string name);
			void setPropertyValue(string name, string value);

			void setReferenceTimePlayer(IPlayer* player);
			void addTimeReferPlayer(IPlayer* referPlayer);
			void removeTimeReferPlayer(IPlayer* referPlayer);
			void notifyReferPlayers(int transition);
			void timebaseObjectTransitionCallback(int transition);
			void setTimeBasePlayer(IPlayer* timeBasePlayer);
			bool hasPresented();
			void setPresented(bool presented);
			bool isVisible();
			void setVisible(bool visible);
			bool immediatelyStart();
			void setImmediatelyStart(bool immediatelyStartVal);
			void forceNaturalEnd(bool forceIt);
			bool isForcedNaturalEnd();
			bool setOutWindow(GingaWindowID windowId);

			/*Exclusive for ChannelPlayer*/
			IPlayer* getSelectedPlayer();
			void setPlayerMap(map<string, IPlayer*>* objs);
			map<string, IPlayer*>* getPlayerMap();
			IPlayer* getPlayer(string objectId);
			void select(IPlayer* selObject);

			/*Exclusive for Application Players*/
			void setCurrentScope(string scopeId);

			void timeShift(string direction);
	};
}
}
}
}
}
}

typedef ::br::pucrio::telemidia::ginga::core::player::IPlayer* PlayerCreator(
		GingaScreenID screenId, const char* mrl);

typedef void PlayerDestroyer(
		::br::pucrio::telemidia::ginga::core::player::IPlayer* player);

#endif /*__PlayerProcess_h_*/
