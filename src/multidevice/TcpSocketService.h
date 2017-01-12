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

#ifndef TCPSOCKETSERVICE_H_
#define TCPSOCKETSERVICE_H_

#include <map>
#include <string.h>
using namespace std;

#include "IRemoteDeviceListener.h"
#include "TcpClientConnection.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace multidevice {
	class TcpSocketService {
		private:
			map<unsigned int, TCPClientConnection*>* connections;
			unsigned int port;
			int connection_counter;
			pthread_mutex_t connMutex;
			IRemoteDeviceListener* res;

		public:
			TcpSocketService(unsigned int p, IRemoteDeviceListener* r);
			virtual ~TcpSocketService();
			void addConnection(unsigned int deviceId, char* addr, int srvPort, bool isLocalConnection);
			void removeConnection(unsigned int deviceId);
			void postTcpCommand(
					char* command,
					int npt,
					char* payloadDesc,
					char* payload);
	};

}
}
}
}
}
}

#endif /* TCPSOCKETSERVICE_H_ */
