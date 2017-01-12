/* Copyright (C) 2006-2017 PUC-Rio/Laboratorio TeleMidia

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

#ifndef __TUNER_H__
#define __TUNER_H__

#include "system/Thread.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::thread;

#include "system/SystemCompat.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;

#include "mb/IInputEventListener.h"
using namespace ::br::pucrio::telemidia::ginga::core::mb;

#include "mb/CodeMap.h"
using namespace ::br::pucrio::telemidia::ginga::core::mb;

#include "ITuner.h"
#include "Channel.h"
#include "NetworkInterface.h"
#include "ITunerListener.h"
#include "IProviderListener.h"

#include <fstream>
#include <string>
#include <iostream>
#include <cstdio>
#include <set>
#include <map>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tuning {
	class Tuner : public ITuner, public IInputEventListener, public ITProviderListener, public Thread {
		private:
			bool receiving;
			ITunerListener* listener;
			ITunerListener* loopListener;
			map<int, INetworkInterface*> interfaces;
			int currentInterface;
			bool firstTune;
			string currentSpec;
			GingaScreenID screenId;

		public:
			Tuner(
					GingaScreenID screenId,
					string network = "",
					string protocol = "",
					string address = "");

			virtual ~Tuner();

			void setLoopListener(ITunerListener* loopListener);
			bool userEventReceived(IInputEvent* ev);

		private:
			void clearInterfaces();
			void receiveSignal(short signalCode);
			void initializeInterface(string niSpec);
			void initializeInterfaces();
			void createInterface(
					string network, string protocol, string address);

			bool listenInterface(INetworkInterface* nInterface);
			void receiveInterface(INetworkInterface* nInterface);

		public:
			void setSpec(string ni, string ch);
			void tune();
			INetworkInterface* getCurrentInterface();
			void channelUp();
			void channelDown();
			void changeChannel(int factor);
			bool hasSignal();

		public:
			void setTunerListener(ITunerListener* listener);

		private:
			void notifyData(char* buff, unsigned int val);
			void notifyStatus(short newStatus, IChannel* channel);
			void waitForListeners();
			virtual void run();
	};
}
}
}
}
}
}

#endif //__TUNER_H__
