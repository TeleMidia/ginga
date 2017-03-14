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
#include "multidev/ActiveDeviceDomain.h"
#include "multidev/BaseDeviceDomain.h"
#include "FormatterMediator.h"

#include "mb/Key.h"

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
  set<int> *evs;
  string layoutName = deviceLayout->getLayoutName ();

  deviceClass = DeviceDomain::CT_BASE;
  deviceLayout->addDevice ("systemScreen(1)", 0, 0, DV_QVGA_WIDTH,
                           DV_QVGA_HEIGHT);

  serialized = Ginga_Display->createWindow (0, 0, DV_QVGA_WIDTH,
                                 DV_QVGA_HEIGHT, -1.0);

  evs = new set<int>;
  evs->insert (Key::KEY_TAP);

  //im->addInputEventListener (this, evs);

  int caps = serialized->getCap ("ALPHACHANNEL");
  serialized->setCaps (caps);
  serialized->draw ();

#if defined WITH_MULTIDEVICE && WITH_MULTIDEVICE
  if (rdm == NULL)
    {
      rdm = RemoteDeviceManager::getInstance ();
      if (!(layoutName.compare ("systemScreen(2)") == 0))
        ((RemoteDeviceManager *)rdm)
            ->setDeviceDomain (
                new BaseDeviceDomain (useMulticast, srvPort));
      else
        ((RemoteDeviceManager *)rdm)
            ->setDeviceDomain (
                new ActiveDeviceDomain (useMulticast, srvPort));
    }

  rdm->setDeviceInfo (deviceClass, w, h, playerId);
  rdm->addListener (this);
#endif // WITH_MULTIDEVICE

  mainLayout = new NclFormatterLayout (x, y, w, h);
  mainLayout->getDeviceRegion ()->setDeviceClass (0, "");
  layoutManager[deviceClass] = mainLayout;
}

FormatterBaseDevice::~FormatterBaseDevice ()
{
 /* if (im != NULL)
    {
      im->removeInputEventListener (this);
    }  */

  if (rdm != NULL)
    {
#if defined WITH_MULTIDEVICE && WITH_MULTIDEVICE
      rdm->removeListener (this);
#endif
    }

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
/*
bool
FormatterBaseDevice::userEventReceived (InputEvent *ev)
{
  int currentX;
  int currentY;
  int code;

  code = ev->getKeyCode ();

  if (code == Key::KEY_TAP)
    {
      ev->getAxisValue (&currentX, &currentY, NULL);
      tapObject (deviceClass, currentX, currentY);
    }
  else if (code == Key::KEY_QUIT)
    {
      clog << "FormatterBaseDevice::userEventReceived setting im as NULL";
      clog << endl;
    //  this->im = NULL;
    }

  return true;
} */

GINGA_FORMATTER_END
