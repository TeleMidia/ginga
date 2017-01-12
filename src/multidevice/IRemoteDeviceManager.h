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

#ifndef _IRemoteDeviceManager_H_
#define _IRemoteDeviceManager_H_

#include "IRemoteDeviceListener.h"

#include <set>
#include <vector>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace multidevice {
  class IRemoteDeviceManager {
	public:
		virtual ~IRemoteDeviceManager(){};
		virtual void release()=0;
		virtual void addListener(IRemoteDeviceListener* listener)=0;
		virtual void removeListener(IRemoteDeviceListener* listener)=0;
		virtual void setDeviceInfo(int deviceClass, int width, int height, string base_device_ncl_path)=0;
		virtual void postEvent(
				int devClass,
				int eventType,
				char* event,
				int eventSize)=0;

		virtual bool postMediaContent(int destDevClass, string url)=0;
  };
}
}
}
}
}
}

typedef ::br::pucrio::telemidia::ginga::core::multidevice::IRemoteDeviceManager*
		IRemoteDeviceManagerCreator(int devClass, bool useMulticast, int srvPort);

typedef void IRemoteDeviceManagerDestroyer(
		::br::pucrio::telemidia::ginga::core::multidevice::
				IRemoteDeviceManager* rdm);

#endif /*_IRemoteDeviceManager_H_*/
