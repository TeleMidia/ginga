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

#ifndef _BaseDeviceService_H_
#define _BaseDeviceService_H_

#include "RemoteDevice.h"
#include "IRemoteDeviceListener.h"
#include "DeviceDomain.h"
#include "DeviceService.h"

GINGA_MULTIDEVICE_BEGIN

class BaseDeviceService : public DeviceService
{
public:
  BaseDeviceService ();
  virtual ~BaseDeviceService ();

  virtual void newDeviceConnected (unsigned int devAddr);
  virtual void connectedToBaseDevice (unsigned int domainAddr){};

  virtual bool receiveEvent (unsigned int devAddr, int eventType,
                             char *stream, int streamSize);

  virtual bool
  receiveMediaContent (unsigned int devAddr, char *stream, int streamSize)
  {
    return false;
  };
};

GINGA_MULTIDEVICE_END

#endif /*_BaseDeviceService_H_*/
