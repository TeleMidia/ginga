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

#include "multidevice/services/device/DeviceService.h"

#include <iostream>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace multidevice {
	DeviceService::DeviceService() {
		devices      = new map<unsigned int, IRemoteDevice*>;
		listeners    = new set<IRemoteDeviceListener*>;

		Thread::mutexInit(&lMutex, NULL);
		Thread::mutexInit(&dMutex, NULL);
		serviceClass = -1;
	}

	DeviceService::~DeviceService() {
		Thread::mutexLock(&dMutex);
		if (devices != NULL) {
			delete devices;
			devices = NULL;
		}
		Thread::mutexUnlock(&dMutex);
		Thread::mutexDestroy(&dMutex);

		Thread::mutexLock(&lMutex);
		if (listeners != NULL) {
			delete listeners;
			listeners = NULL;
		}
		Thread::mutexUnlock(&lMutex);
		Thread::mutexDestroy(&lMutex);
	}

	IRemoteDevice* DeviceService::getDevice(unsigned int devAddr) {
		map<unsigned int, IRemoteDevice*>::iterator i;
		IRemoteDevice* remoteDev;

		Thread::mutexLock(&dMutex);
		i = devices->find(devAddr);
		if (i != devices->end()) {
			remoteDev = i->second;
			Thread::mutexUnlock(&dMutex);
			return remoteDev;
		}

		Thread::mutexUnlock(&dMutex);
		return NULL;
	}

	void DeviceService::addListener(IRemoteDeviceListener* listener) {
		Thread::mutexLock(&lMutex);
		listeners->insert(listener);
		Thread::mutexUnlock(&lMutex);
	}

	void DeviceService::removeListener(IRemoteDeviceListener* listener) {
		set<IRemoteDeviceListener*>::iterator i;

		Thread::mutexLock(&lMutex);
		i = listeners->find(listener);
		if (i != listeners->end()) {
			listeners->erase(i);
		}
		Thread::mutexUnlock(&lMutex);
	}

	bool DeviceService::addDevice(
			unsigned int deviceAddress,
			int newDevClass,
			int width,
			int height) {

		int w, h;
		IRemoteDevice* device = getDevice(deviceAddress);

		if (device == NULL) {
			clog << "DeviceService::addDevice() new RemoteDevice() " << endl;
			device = new RemoteDevice(deviceAddress, newDevClass);
			device->setDeviceResolution(width, height);

			Thread::mutexLock(&dMutex);
			(*devices)[deviceAddress] = device;
			Thread::mutexUnlock(&dMutex);

		} else {
			device->getDeviceResolution(&w, &h);
			if (device->getDeviceClass() != newDevClass ||
					w != width || h != height) {

				return false;
			}
		}

		//clog << "DeviceService::addDevice '" << deviceAddress << "'" << endl;
		return true;
	}

	bool DeviceService::hasDevices() {
		bool hasDev;

		Thread::mutexLock(&dMutex);
		hasDev = !devices->empty();
		Thread::mutexUnlock(&dMutex);

		return hasDev;
	}
}
}
}
}
}
}
