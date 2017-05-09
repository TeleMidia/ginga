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

#include "ginga.h"

#include "FormatterBaseDevice.h"
#include "FormatterMediator.h"

#include "ncl/DeviceLayout.h"
using namespace ::ginga::ncl;

GINGA_FORMATTER_BEGIN

FormatterBaseDevice::FormatterBaseDevice (DeviceLayout *deviceLayout,
                                          arg_unused (string) playerId, int x, int y,
                                          int w, int h, bool useMulticast,
                                          int srvPort)
    : FormatterMultiDevice (deviceLayout, x, y, w, h,
                            useMulticast, srvPort)
{
  string layoutName = deviceLayout->getLayoutName ();

  deviceClass = DeviceDomain::CT_BASE;
  deviceLayout->addDevice ("systemScreen(1)", 0, 0, DV_QVGA_WIDTH,
                           DV_QVGA_HEIGHT);

  serialized = Ginga_Display->createWindow (0, 0, DV_QVGA_WIDTH,
                                 DV_QVGA_HEIGHT, -1.0);

  mainLayout = new NclFormatterLayout (x, y, w, h);
  mainLayout->getDeviceRegion ()->setDeviceClass (0, "");
  layoutManager[deviceClass] = mainLayout;
}

FormatterBaseDevice::~FormatterBaseDevice ()
{
  if (Ginga_Display->hasWindow (serialized))
    {
      Ginga_Display->destroyWindow (serialized);
      serialized = 0;
    }

  if (mainLayout != NULL)
    {
      mainLayout = NULL;
    }
}

bool
FormatterBaseDevice::newDeviceConnected (int newDevClass, int w, int h)
{
  return FormatterMultiDevice::newDeviceConnected (newDevClass, w, h);
}

bool
FormatterBaseDevice::receiveRemoteEvent (int remoteDevClass, int eventType,
                                         const string &eventContent)
{
  return FormatterMultiDevice::receiveRemoteEvent (remoteDevClass,
                                                   eventType, eventContent);
}


GINGA_FORMATTER_END
