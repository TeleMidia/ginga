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

#ifndef _IDeviceService_H_
#define _IDeviceService_H_

#include "IRemoteDeviceListener.h"

#include <map>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace multidevice {
  class IDeviceService {
	public:
		virtual ~IDeviceService(){};
		virtual void addListener(IRemoteDeviceListener* listener)=0;
		virtual void removeListener(IRemoteDeviceListener* listener)=0;

		virtual bool addDevice(
				unsigned int deviceAddress,
				int newDevClass,
				int width,
				int height)=0;

		virtual bool hasDevices()=0;

		virtual void newDeviceConnected(unsigned int devAddr)=0;
		virtual void connectedToBaseDevice(unsigned int domainAddr)=0;

		virtual bool receiveEvent(
				unsigned int devAddr,
				int eventType,
				char* stream,
				int streamSize)=0;

		virtual bool receiveMediaContent(
				unsigned int devAddr,
				char* stream,
				int streamSize)=0;
  };
}
}
}
}
}
}

#endif /*_BaseDeviceService_H_*/
