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

#include "multidevice/services/device/PassiveDeviceService.h"
#include "multidevice/services/IDeviceDomain.h"
#ifdef _WIN32
#include <io.h>
#define O_LARGEFILE 0
#endif

#include <string.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <fcntl.h>

#include <iostream>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace multidevice {
	PassiveDeviceService::PassiveDeviceService() : DeviceService() {
		serviceClass = IDeviceDomain::CT_PASSIVE;
	}

	PassiveDeviceService::~PassiveDeviceService() {

	}

	void PassiveDeviceService::connectedToBaseDevice(unsigned int domainAddr) {
		set<IRemoteDeviceListener*>::iterator i;

		addDevice(domainAddr, IDeviceDomain::CT_BASE, 0, 0);

		pthread_mutex_lock(&lMutex);
		i = listeners->begin();
		while (i != listeners->end()) {
			(*i)->connectedToBaseDevice(domainAddr);
			++i;
		}
		pthread_mutex_unlock(&lMutex);
	}

	bool PassiveDeviceService::receiveMediaContent(
			unsigned int devAddr,
			char* stream,
			int streamSize) {

		int remoteDevClass, fd, bytesWrite;
		IRemoteDevice* dev;
		string uri;
		set<IRemoteDeviceListener*>::iterator i;
		bool hasLists;

		//clog << "PassiveDeviceService::receiveMediaContent" << endl;

		dev = getDevice(devAddr);
		pthread_mutex_lock(&lMutex);
		hasLists = !listeners->empty();
		pthread_mutex_unlock(&lMutex);

		if (dev != NULL && hasLists) {
			remoteDevClass = dev->getDeviceClass();

			#ifdef	__DARWIN_UNIX03
					i = listeners->begin();
					while (i != listeners->end()) {
						(*i)->receiveRemoteContent(
								remoteDevClass, stream, streamSize);
						++i;
					}
					return true;

			#else
				uri = "/tmp/render.jpg";
				remove((char*)(uri.c_str()));
				fd = open(
						uri.c_str(),
						O_LARGEFILE | O_WRONLY | O_CREAT,
						0644);

				if (fd > 0) {
					bytesWrite = write(fd, stream, streamSize);
					close(fd);
					if (bytesWrite == streamSize) {
						pthread_mutex_lock(&lMutex);
						i = listeners->begin();
						while (i != listeners->end()) {
							(*i)->receiveRemoteContent(remoteDevClass, uri);
							++i;
						}
						pthread_mutex_unlock(&lMutex);
						return true;

					} else {
						clog << "PassiveDeviceService::receiveMediaContent ";
						clog << "Warning! can't write '" << streamSize << "'";
						clog << " in file '" << uri << "' (" << bytesWrite;
						clog << " bytes wrote";
						clog << endl;
					}

				} else {
					clog << "PassiveDeviceService::receiveMediaContent Warning! ";
					clog << " can't create file '" << uri << "'";
					clog << endl;
				}

			#endif

		} else {
			clog << "PassiveDeviceService::receiveMediaContent Warning! ";
			clog << " can't find device '" << dev << "' or no listeners found";
			clog << endl;
		}
		return false;
	}
}
}
}
}
}
}
