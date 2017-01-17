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

#ifndef TCPCLIENTCONNECTION_H_
#define TCPCLIENTCONNECTION_H_

#include "system/SystemCompat.h"
#include "system/PracticalSocket.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;

#include <iostream>
#include <sstream>
#include <string.h>
using namespace std;

#include "system/Thread.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::thread;

#include "IRemoteDeviceListener.h"

BR_PUCRIO_TELEMIDIA_GINGA_CORE_MULTIDEVICE_BEGIN

	class TCPClientConnection : public Thread  {
		private:
			TCPSocket* tcpSocket;
			int sockfd;
			int counter;
			char* srv_hostname;
			char* portno;
			struct sockaddr_in server_addr;
			struct hostent *server;
			IRemoteDeviceListener *resrv;
			void run();
			bool running;
			unsigned int deviceId;
			unsigned int orderId;

		public:
			TCPClientConnection(unsigned int devid,
						unsigned int index,
					    char* hostname,
					    char* port_str,
					    IRemoteDeviceListener* srv);
			virtual ~TCPClientConnection();
			bool post(char* str);
			void release();
	};

BR_PUCRIO_TELEMIDIA_GINGA_CORE_MULTIDEVICE_END
#endif /* TCPCLIENTCONNECTION_H_ */
