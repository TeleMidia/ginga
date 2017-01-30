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

#ifndef I_REMOTE_DEVICE_LISTENER_H
#define I_REMOTE_DEVICE_LISTENER_H

#include "ginga.h"

GINGA_MULTIDEV_BEGIN

class IRemoteDeviceListener
{
public:
  virtual ~IRemoteDeviceListener (){};
  virtual bool newDeviceConnected (int newDevClass, int w, int h) = 0;
  virtual void connectedToBaseDevice (unsigned int domainAddr) = 0;
  virtual bool receiveRemoteEvent (int remoteDevClass, int eventType,
                                   const string &eventContent)
      = 0;
  virtual bool receiveRemoteContent (int remoteDevClass,
                                     const string &contentUri)
      = 0;
  virtual bool receiveRemoteContent (int remoteDevClass, char *stream,
                                     int streamSize)
      = 0;
  virtual bool receiveRemoteContentInfo (const string &contentId,
                                         const string &contentUri)
      = 0;
};

GINGA_MULTIDEV_END

#endif /* I_REMOTE_DEVICE_LISTENER_H */
