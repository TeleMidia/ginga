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
#include "BaseDeviceService.h"
#include "DeviceDomain.h"

BR_PUCRIO_TELEMIDIA_GINGA_CORE_MULTIDEVICE_BEGIN

	BaseDeviceService::BaseDeviceService() : DeviceService() {
		serviceClass = DeviceDomain::CT_BASE;
	}

	BaseDeviceService::~BaseDeviceService() {

	}

	void BaseDeviceService::newDeviceConnected(unsigned int devAddr) {
		RemoteDevice* dev;
		int newDevClass, w, h;
		set<IRemoteDeviceListener*>::iterator i;

		dev = getDevice(devAddr);
		if (dev != NULL) {
			dev->getDeviceResolution(&w, &h);
			newDevClass = dev->getDeviceClass();

			Thread::mutexLock(&lMutex);
			i = listeners->begin();
			while (i != listeners->end()) {
				(*i)->newDeviceConnected(newDevClass, w, h);
				++i;
			}
			Thread::mutexUnlock(&lMutex);
		}
	}

	bool BaseDeviceService::receiveEvent(
			unsigned int devAddr,
			int eventType,
			char* stream,
			int streamSize) {

		string strStream = "";
		RemoteDevice* dev;
		int devClass;
		set<IRemoteDeviceListener*>::iterator it;
		bool hasLists;

		dev = getDevice(devAddr);

		Thread::mutexLock(&lMutex);
		hasLists = !listeners->empty();
		Thread::mutexUnlock(&lMutex);

		if (dev != NULL && hasLists) {
			strStream.append(stream, streamSize);

			/*clog << "BaseDeviceService::receiveEvent '" << strStream << "'";
			clog << endl;*/
			devClass = dev->getDeviceClass();

			Thread::mutexLock(&lMutex);
			it = listeners->begin();
			while (it != listeners->end()) {
				(*it)->receiveRemoteEvent(devClass, eventType, strStream);
				++it;
			}
			Thread::mutexUnlock(&lMutex);

			return true;
		}

		return false;
	}

BR_PUCRIO_TELEMIDIA_GINGA_CORE_MULTIDEVICE_END
