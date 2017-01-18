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
#include "FormatterPassiveDevice.h"

#include "multidevice/ActiveDeviceDomain.h"
#include "multidevice/PassiveDeviceDomain.h"
#include "multidevice/BaseDeviceDomain.h"
#include "gingancl/FormatterMediator.h"
using namespace ::ginga::formatter;

#include "gingancl/FormatterMediator.h"
using namespace ::ginga::formatter;

GINGA_FORMATTER_BEGIN

FormatterPassiveDevice::FormatterPassiveDevice (GingaScreenID screenId,
                                                DeviceLayout *deviceLayout,
                                                int x, int y, int w, int h,
                                                bool useMulticast,
                                                int srvPort)
    : FormatterMultiDevice (screenId, deviceLayout, x, y, w, h,
                            useMulticast, srvPort)
{

  deviceClass = DeviceDomain::CT_PASSIVE;
  serialized = dm->createWindow (myScreen, x, y, defaultWidth,
                                 defaultHeight, -1.0);

  int cap = dm->getWindowCap (myScreen, serialized, "ALPHACHANNEL");
  dm->setWindowCaps (myScreen, serialized, cap);
  dm->drawWindow (myScreen, serialized);

  if (rdm == NULL)
    {
      rdm = RemoteDeviceManager::getInstance ();
      ((RemoteDeviceManager *)rdm)
          ->setDeviceDomain (
              new PassiveDeviceDomain (useMulticast, srvPort));
    }

  rdm->setDeviceInfo (deviceClass, w, h, "");
  rdm->addListener (this);

  im->addInputEventListener (this, NULL);

  mainLayout = new NclFormatterLayout (myScreen, x, y, w, h);
  layoutManager[deviceClass] = mainLayout;
}

FormatterPassiveDevice::~FormatterPassiveDevice () {}

void
FormatterPassiveDevice::connectedToBaseDevice (unsigned int domainAddr)
{
  /*clog << "FormatterPassiveDevice::connectedToDomainService '";
  clog << domainAddr << "'" << endl;*/

  hasRemoteDevices = true;
}

bool
FormatterPassiveDevice::receiveRemoteContent (int remoteDevClass,
                                              string contentUri)
{

  /*clog << "FormatterPassiveDevice::receiveRemoteContent from class '";
  clog << remoteDevClass << "' and contentUri '" << contentUri << "'";
  clog << endl;*/

  renderFromUri (serialized, contentUri);
  return true;
}

bool
FormatterPassiveDevice::userEventReceived (SDLInputEvent *ev)
{
  string mnemonicCode;
  int currentX;
  int currentY;
  int code;

  clog << "FormatterPassiveDevice::userEventReceived" << endl;

  code = ev->getKeyCode (myScreen);
  if (code == CodeMap::KEY_F11 || code == CodeMap::KEY_F10)
    {
      std::abort ();
    }
  else if (code == CodeMap::KEY_QUIT)
    {
      this->im = NULL;
      return true;
    }

  if (!hasRemoteDevices)
    {
      clog << "FormatterPassiveDevice::userEventReceived !hasRemoteDevices"
           << endl;
      return false;
    }

  mnemonicCode = CodeMap::getInstance ()->getValue (code);

  if (ev->isButtonPressType ())
    {
      ev->getAxisValue (&currentX, &currentY, NULL);

      mnemonicCode = (mnemonicCode + "," + itos (currentX - xOffset) + ","
                      + itos (currentY - yOffset));
    }

  clog << "FormatterPassiveDevice::userEventReceived posting '";
  clog << mnemonicCode << "'" << endl;

  rdm->postEvent (DeviceDomain::CT_BASE, DeviceDomain::FT_SELECTIONEVENT,
                  (char *)(mnemonicCode.c_str ()),
                  (int)(mnemonicCode.length ()));

  return false;
}

GINGA_FORMATTER_END
