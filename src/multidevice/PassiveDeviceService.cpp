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
#include "PassiveDeviceService.h"
#include "DeviceDomain.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <iostream>
using namespace std;

BR_PUCRIO_TELEMIDIA_GINGA_CORE_MULTIDEVICE_BEGIN

	PassiveDeviceService::PassiveDeviceService() : DeviceService() {
		serviceClass = DeviceDomain::CT_PASSIVE;
	}

	PassiveDeviceService::~PassiveDeviceService() {

	}

	void PassiveDeviceService::connectedToBaseDevice(unsigned int domainAddr) {
		set<IRemoteDeviceListener*>::iterator i;

		addDevice(domainAddr, DeviceDomain::CT_BASE, 0, 0);

		Thread::mutexLock(&lMutex);
		i = listeners->begin();
		while (i != listeners->end()) {
			(*i)->connectedToBaseDevice(domainAddr);
			++i;
		}
		clog << "PassiveDeviceService::connectedToBaseDevice("<<domainAddr<<") "<<endl;
		Thread::mutexUnlock(&lMutex);
	}

	bool PassiveDeviceService::receiveMediaContent(
			unsigned int devAddr,
			char* stream,
			int streamSize) {

		int remoteDevClass, bytesWrite;
		FILE* fd;
		RemoteDevice* dev;
		string uri;
		set<IRemoteDeviceListener*>::iterator i;
		bool hasLists;

		clog << "PassiveDeviceService::receiveMediaContent" << endl;

		dev = getDevice(devAddr);
		Thread::mutexLock(&lMutex);
		hasLists = !listeners->empty();
		Thread::mutexUnlock(&lMutex);

		if (dev != NULL && hasLists) {
			remoteDevClass = dev->getDeviceClass();

				uri = string (g_get_tmp_dir ()) + "/render.jpg";
				remove((char*)(uri.c_str()));
				fd = fopen(uri.c_str(), "w+b");

				if (fd != NULL) {
					bytesWrite = fwrite(stream, 1, streamSize, fd);
					fclose(fd);
					if (bytesWrite == streamSize) {
						Thread::mutexLock(&lMutex);
						i = listeners->begin();
						while (i != listeners->end()) {
							(*i)->receiveRemoteContent(remoteDevClass, uri);
							++i;
						}
						Thread::mutexUnlock(&lMutex);
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

		} else {
			clog << "PassiveDeviceService::receiveMediaContent Warning! ";
			clog << " can't find device '" << dev << "' or no listeners found";
			clog << endl;
		}
		return false;
	}

BR_PUCRIO_TELEMIDIA_GINGA_CORE_MULTIDEVICE_END
