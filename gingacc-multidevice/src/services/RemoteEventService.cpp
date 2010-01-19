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

#include "../../include/services/RemoteEventService.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace multidevice {
	RemoteEventService::RemoteEventService() {
		groups = new map<int,TcpSocketService*>;
		pthread_mutex_init(&groupsMutex, NULL);
	}

	RemoteEventService::~RemoteEventService() {
		map<int,TcpSocketService*>::iterator i;

		// TODO Auto-generated destructor stub
		pthread_mutex_lock(&groupsMutex);
		if (groups != NULL) {
			i = groups->begin();
			while (i != groups->end()) {
				delete i->second;
				++i;
			}
			delete groups;
			groups = NULL;
		}
		pthread_mutex_unlock(&groupsMutex);
		pthread_mutex_destroy(&groupsMutex);
	}

	void RemoteEventService::addDeviceClass(unsigned int id) {
		pthread_mutex_lock(&groupsMutex);
		if (groups->count(id) == 0) {
			(*groups)[id] = new TcpSocketService(
					RemoteEventService::DEFAULT_PORT);;

		} else {
			cout << "RemoteEventService::addDeviceClass Warning! Trying to ";
			cout << "add the same device class '" << id << "' twice!" << endl;
		}
		pthread_mutex_unlock(&groupsMutex);
	}

	void RemoteEventService::addDevice(
			unsigned int device_class, unsigned int device_id, char* addr) {

		TcpSocketService* tss;

		pthread_mutex_lock(&groupsMutex);
		if (groups->count(device_class) == 0) {
			pthread_mutex_unlock(&groupsMutex);
			return;
		}

		tss = (*groups)[device_class];
		tss->addConnection(device_id, addr);
		pthread_mutex_unlock(&groupsMutex);
	}

	void RemoteEventService::addDocument(
			unsigned int device_class, char* name, char* body) {

		TcpSocketService* tss;

		pthread_mutex_lock(&groupsMutex);
		if (groups->count(device_class) == 0) {
			pthread_mutex_unlock(&groupsMutex);
			return;
		}

		tss = (*groups)[device_class];
		tss->postTcpCommand((char*)"ADD", 0, name, body);
		pthread_mutex_unlock(&groupsMutex);
	}

	void RemoteEventService::startDocument(
			unsigned int device_class, char* name) {

		TcpSocketService* tss;

		pthread_mutex_lock(&groupsMutex);
		if (groups->count(device_class) == 0) {
			pthread_mutex_unlock(&groupsMutex);
			return;
		}

		tss = (*groups)[device_class];
		cout << "POSTING START DOCUMENT ("<< name << ")" << endl;
		tss->postTcpCommand((char*)"START", 0, name, (char*)"");
		pthread_mutex_unlock(&groupsMutex);
	}

	void RemoteEventService::stopDocument(
			unsigned int device_class, char* name) {

		TcpSocketService* tss;

		pthread_mutex_lock(&groupsMutex);
		if (groups->count(device_class) == 0) {
			pthread_mutex_unlock(&groupsMutex);
			return;
		}

		tss = (*groups)[device_class];
		cout << "POSTING STOP DOCUMENT (" << name << ")" << endl;
		tss->postTcpCommand((char*)"STOP", 0, name, (char*)"");
		pthread_mutex_unlock(&groupsMutex);
	}
}
}
}
}
}
}
