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

#ifndef NETWORKINTERFACE_H_
#define NETWORKINTERFACE_H_

#include "util/functions.h"
using namespace br::pucrio::telemidia::util;

#include "INetworkInterface.h"

#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tuning {
	class NetworkInterface : public INetworkInterface {
		private:
			int id;    //NetworkInterface number
			string name;  //eth, isdb-t, dvb-c, ...
			string protocol; //none, rtp, ip, udp, ...
			string address;  //frequency, ip+sock, ...
			IDataProvider* provider;
			bool tuned;

		public:
			NetworkInterface(
					int networkId,
					string name, string protocol, string addr);

			virtual ~NetworkInterface();

			short getCaps();
			int getId();
			string getName();
			string getProtocol();
			string getAddress();

			void attachFilter(IFrontendFilter* filter);
			void removeFilter(IFrontendFilter* filter);

			void setDataProvider(IDataProvider* provider);

		private:
			virtual bool createProvider();

		public:
			bool hasSignal();
			IDataProvider* tune();
			bool changeChannel(int factor);
			bool setChannel(string channelValue);
			bool getSTCValue(uint64_t* stc, int* valueType);
			IChannel* getCurrentChannel();
			int createPesFilter(int pid, int pesType, bool compositeFiler);
			string getPesFilterOutput();

			char* receiveData(int* len);

			void close();
	};
}
}
}
}
}
}

#endif /*NETWORKINTERFACE_H_*/
