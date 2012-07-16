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

#include <stdio.h>
#include <sys/types.h>

#ifdef _MSC_VER
extern "C" {
# include "asprintf.h"
}
#else
# include <sys/socket.h>
# include <netinet/in.h>
# include <netdb.h>
#endif

#include <pthread.h>
#include <stdio.h>

#include "multidevice/services/network/TcpSocketService.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace multidevice {

TcpSocketService::TcpSocketService(unsigned int p, IRemoteDeviceListener* r) {
	res = r;
	port = p;

	connections = new map<unsigned int, TCPClientConnection*>;
	pthread_mutex_init(&connMutex, NULL);
}

TcpSocketService::~TcpSocketService() {
	pthread_mutex_lock(&connMutex);
	if (connections != NULL) {
		delete connections;
		connections = NULL;
	}
	pthread_mutex_unlock(&connMutex);
	pthread_mutex_destroy(&connMutex);
}

void TcpSocketService::addConnection(unsigned int deviceId, char* addr) {
	char* portStr;
	TCPClientConnection* tcpcc;
	//unsigned int newDevId;

	asprintf(&portStr,"%d",port);
	pthread_mutex_lock(&connMutex);
	if (connections != NULL && connections->count(deviceId) == 0) {

//	if (connections != NULL) {
	//(*connections)[deviceId] = new TCPClientConnection(addr, portStr);
		tcpcc = new TCPClientConnection(
						deviceId,
						addr,
						portStr,
						(IRemoteDeviceListener*)res);
		(*connections)[deviceId] = tcpcc;
		tcpcc->startThread();

	} else if (connections != NULL) {
		clog << "TcpSocketService::warning - connection already registered";
		clog << endl;

		clog << "TcpSocketService::warning - removing and adding it again (";
		clog << deviceId << ")" << endl;

		this->removeConnection(deviceId);
		tcpcc = new TCPClientConnection(
						deviceId,
						addr,
						portStr,
						(IRemoteDeviceListener*) res);

		(*connections)[deviceId] = tcpcc;	
		tcpcc->startThread();
		//newDevId = (--connections->end())->first + 1;
		//(*connections)[newDevId] = new TCPClientConnection(addr, portStr);
	}

	pthread_mutex_unlock(&connMutex);

	clog << "TcpSocketService::addConnection all done" << endl;
}

void TcpSocketService::removeConnection(unsigned int deviceId) {
	TCPClientConnection *con = (*connections)[deviceId];
	delete con;
	(*connections)[deviceId] = NULL;
}

void TcpSocketService::postTcpCommand(
		char* command,
		int npt,
		char* payloadDesc,
		char* payload) {

	map<unsigned int, TCPClientConnection*>::iterator i;
	char* com;

	asprintf(
			&com,
			"%d %s %s %d\n%s\n",
			npt,
			command,
			payloadDesc,
			(int)strlen(payload),
			payload);

	//clog << "TcpSocketService::postTcpCommand = ";
	//clog << com << endl;

	pthread_mutex_lock(&connMutex);
	i = connections->begin();

	while (i != connections->end()) {
		i->second->post(com);
		++i;
	}

	pthread_mutex_unlock(&connMutex);
}

}
}
}
}
}
}
