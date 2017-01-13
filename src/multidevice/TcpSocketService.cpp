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

#include "config.h"
#include <stdio.h>
#include <sys/types.h>

#ifdef _MSC_VER
extern "C" {
# include "asprintf.h"
}
#endif

#include <pthread.h>
#include <stdio.h>

#include "TcpSocketService.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace multidevice {

TcpSocketService::TcpSocketService(unsigned int p, IRemoteDeviceListener* r) {
	res = r;
	port = p;

	connection_counter = 0;

	connections = new map<unsigned int, TCPClientConnection*>;
	Thread::mutexInit(&connMutex, NULL);
}

TcpSocketService::~TcpSocketService() {
	Thread::mutexLock(&connMutex);
	if (connections != NULL) {
		delete connections;
		connections = NULL;
	}
	Thread::mutexUnlock(&connMutex);
	Thread::mutexDestroy(&connMutex);
}

void TcpSocketService::addConnection(unsigned int deviceId,
									char* addr,
									int srvPort,
									bool isLocalConnection) {
	char* portStr;
	TCPClientConnection* tcpcc;
	//unsigned int newDevId;

	asprintf(&portStr,"%d",srvPort);

	Thread::mutexLock(&connMutex);
	if (connections != NULL && connections->count(deviceId) == 0) {

//	if (connections != NULL) {
	//(*connections)[deviceId] = new TCPClientConnection(addr, portStr);
		connection_counter++;
		tcpcc = new TCPClientConnection(
						deviceId,
						connection_counter,
						addr,
						portStr,
						(IRemoteDeviceListener*)res);

		(*connections)[deviceId] = tcpcc;

		tcpcc->startThread();

	} else if (connections != NULL) {
		clog << "TcpSocketService::warning - connection already registered";
		clog << endl;

//		if (!isLocalConnection) {
//			//TODO: maintain index when connection is created again for the same device
//			//TODO: defining a getIndex method for the TCPClientConn class
//			//configurable: stick (based on address+port), slot (like videogames)
//			//and continuous (index in not regained, keeps increasing)
//
//			connection_counter++;
//			clog << "TcpSocketService::warning - not a local connection,";
//			clog << " removing and adding it again (";
//			clog << deviceId << ")" << endl;
//
//			this->removeConnection(deviceId);
//			tcpcc = new TCPClientConnection(
//							deviceId,
//							connection_counter,
//							addr,
//							portStr,
//							(IRemoteDeviceListener*) res);
//
//			(*connections)[deviceId] = tcpcc;
//			tcpcc->startThread();
//		}
		//newDevId = (--connections->end())->first + 1;
		//(*connections)[newDevId] = new TCPClientConnection(addr, portStr);
	}

	Thread::mutexUnlock(&connMutex);

	clog << "TcpSocketService::addConnection all done" << endl;
}

void TcpSocketService::removeConnection(unsigned int deviceId) {
	TCPClientConnection *con = (*connections)[deviceId];
	con->release();
	delete con;
	(*connections)[deviceId] = NULL;
}
//TODO: create postTcpCommand with deviceId arg

void TcpSocketService::postTcpCommand(
		char* command,
		int npt,
		char* payloadDesc,
		char* payload) {

	map<unsigned int, TCPClientConnection*>::iterator i;
	char* com;
/*
	asprintf(
			&com,
			"%d %s %s %d\n%s\n",
			npt,
			command,
			payloadDesc,
			(int)strlen(payload),
			payload);
*/
	asprintf(
				&com,
				"%d %s %s %d\n",
				npt,
				command,
				payloadDesc,
				(int)strlen(payload));

	string s_com;
	s_com = string(com) + "\n" + string(payload);

	Thread::mutexLock(&connMutex);
	i = connections->begin();

	while (i != connections->end()) {
		i->second->post((char *)s_com.c_str());
		//i->second->post(com);
		++i;
	}

	Thread::mutexUnlock(&connMutex);
}

}
}
}
}
}
}
