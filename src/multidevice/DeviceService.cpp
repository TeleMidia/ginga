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
#include "DeviceService.h"

#include <iostream>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace multidevice {
	DeviceService::DeviceService() {
		devices      = new map<unsigned int, RemoteDevice*>;
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

	RemoteDevice* DeviceService::getDevice(unsigned int devAddr) {
		map<unsigned int, RemoteDevice*>::iterator i;
		RemoteDevice* remoteDev;

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
		RemoteDevice* device = getDevice(deviceAddress);

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
