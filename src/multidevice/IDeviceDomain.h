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

#ifndef _IDeviceDomain_H_
#define _IDeviceDomain_H_

#include "IRemoteDeviceListener.h"

#include <string>
#include <set>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace multidevice {
  class IDeviceDomain {
	public:
		//CT: class types
		static const int CT_BASE              = 0;
		static const int CT_PASSIVE           = 1;
		static const int CT_ACTIVE            = 2;

		//FT: frame types
		static const int FT_CONNECTIONREQUEST = 0;
		static const int FT_ANSWERTOREQUEST   = 1;
		static const int FT_KEEPALIVE         = 2;
		static const int FT_MEDIACONTENT      = 3;
		static const int FT_SELECTIONEVENT    = 4;
		static const int FT_ATTRIBUTIONEVENT  = 5;
		static const int FT_PRESENTATIONEVENT = 6;

		virtual ~IDeviceDomain(){};

		virtual bool isConnected()=0;
		virtual void postConnectionRequestTask()=0;
		virtual bool postMediaContentTask(int destDevClass, string url)=0;

		virtual void postEventTask(
				int destDevClass,
				int frameType,
				char* payload,
				int payloadSize)=0;

		virtual void setDeviceInfo(int width, int height, string base_device_ncl_path)=0;
		virtual int getDeviceClass()=0;
		virtual void checkDomainTasks()=0;
		virtual void addDeviceListener(IRemoteDeviceListener* listener)=0;
		virtual void removeDeviceListener(IRemoteDeviceListener* listener)=0;
  };
}
}
}
}
}
}

#endif /*_IDeviceDomain_H_*/
