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
#include "ActiveDeviceService.h"
#include "IDeviceDomain.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace multidevice {
	ActiveDeviceService::ActiveDeviceService() : BaseDeviceService() {
		serviceClass = IDeviceDomain::CT_ACTIVE;
	}

	ActiveDeviceService::~ActiveDeviceService() {

	}

	void ActiveDeviceService::connectedToBaseDevice(unsigned int domainAddr) {
		set<IRemoteDeviceListener*>::iterator i;

		addDevice(domainAddr, IDeviceDomain::CT_BASE, 0, 0);

		Thread::mutexLock(&lMutex);
		i = listeners->begin();
		while (i != listeners->end()) {
			(*i)->connectedToBaseDevice(domainAddr);
			++i;
		}
		Thread::mutexUnlock(&lMutex);
	}

	bool ActiveDeviceService::receiveMediaContent(
			unsigned int devAddr,
			char* stream,
			int streamSize) {

		int remoteDevClass;
		IRemoteDevice* dev;
		string uri;
		set<IRemoteDeviceListener*>::iterator i;
		//INCLSectionProcessor* nsp = NULL;
		bool hasLists;

		//clog << "ActiveDeviceService::receiveMediaContent" << endl;

		dev = getDevice(devAddr);
		Thread::mutexLock(&lMutex);
		hasLists = !listeners->empty();
		Thread::mutexUnlock(&lMutex);

		if (dev != NULL && hasLists) {
			remoteDevClass = dev->getDeviceClass();

		}

		clog << "ActiveDeviceService::receiveMediaContent Warning! ";
		clog << " can't find device '" << dev << "' or no listeners found";
		clog << endl;

		return false;
	}
}
}
}
}
}
}
