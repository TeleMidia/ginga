/* Copyright (C) 1989-2017 PUC-Rio/Laboratorio TeleMidia

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

#ifndef _RemoteDeviceManager_H_
#define _RemoteDeviceManager_H_

#include "system/Thread.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::thread;

#include "IRemoteDeviceManager.h"
#include "BaseDeviceDomain.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace multidevice {
  class RemoteDeviceManager : public IRemoteDeviceManager, public Thread {
	private:
		bool connecting;
		bool running;
		bool released;
		IDeviceDomain* domainService;
		static RemoteDeviceManager* _instance;

		RemoteDeviceManager();
		~RemoteDeviceManager();

	public:
		void release();
		static RemoteDeviceManager* getInstance();

		void setDeviceDomain(IDeviceDomain* domain);
		void setDeviceInfo(int deviceClass, int width, int height, string base_device_ncl_path);
		int getDeviceClass();
		void addListener(IRemoteDeviceListener* listener);
		void removeListener(IRemoteDeviceListener* listener);
		void postEvent(int devClass, int eventType, char* event, int eventSize);
		bool postMediaContent(int destDevClass, string url);

	private:
		void run();
  };
}
}
}
}
}
}

#endif /*_RemoteDeviceManager_H_*/
