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

#ifndef FORMATTER_BASE_DEVICE_H
#define FORMATTER_BASE_DEVICE_H

#include "ginga.h"
#include "FormatterMultiDevice.h"

GINGA_FORMATTER_BEGIN

class FormatterBaseDevice : public FormatterMultiDevice
{
public:
  FormatterBaseDevice (DeviceLayout *deviceLayout,
                       string playerId, int x, int y, int w, int h,
                       bool useMulticast, int srvPort);
  virtual ~FormatterBaseDevice ();

protected:
  bool newDeviceConnected (int newDevClass, int w, int h);
  void connectedToBaseDevice (arg_unused (unsigned int domainAddr)){};
  bool receiveRemoteEvent (int remoteDevClass, int eventType,
                           string eventContent);
  bool
  receiveRemoteContent (arg_unused (int remoteDevClass),
                        arg_unused (char *stream),
                        arg_unused (int streamSize))
  {
    return false;
  };
  bool
  receiveRemoteContent (arg_unused (int remoteDevClass),
                        arg_unused (string contentUri))
  {
    return false;
  };
  bool userEventReceived (SDLInputEvent *ev);
};

GINGA_FORMATTER_END

#endif /* FORMATTER_BASE_DEVICE_H */
