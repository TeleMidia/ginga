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
                                          int w, int h)

    : FormatterMultiDevice (deviceLayout, w, h)
{
  string layoutName = deviceLayout->getLayoutName ();

  deviceClass = DeviceDomain::CT_BASE;
  deviceLayout->addDevice ("systemScreen(1)", DV_QVGA_WIDTH,
                           DV_QVGA_HEIGHT);

  mainLayout = new NclFormatterLayout (w, h);
  mainLayout->getDeviceRegion ()->setDeviceClass (0, "");
  layoutManager[deviceClass] = mainLayout;
}

FormatterBaseDevice::~FormatterBaseDevice ()
{
  if (mainLayout != NULL)
    {
      mainLayout = NULL;
    }
}

GINGA_FORMATTER_END
