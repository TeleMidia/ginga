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

#include "FormatterBaseDevice.h"
#include "FMDComponentSupport.h"

#include "mb/CodeMap.h"

#include "ncl/layout/DeviceLayout.h"
using namespace ::br::pucrio::telemidia::ncl::layout;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace multidevice {
	FormatterBaseDevice::FormatterBaseDevice(
			GingaScreenID screenId,
			IDeviceLayout* deviceLayout,
			string playerId,
			int x, int y, int w, int h, bool useMulticast, int srvPort) : FormatterMultiDevice(
					screenId, deviceLayout, x, y, w, h, useMulticast, srvPort) {

		set<int>* evs;
		string layoutName = deviceLayout->getLayoutName();

		bool active_dev = (layoutName.compare("systemScreen(2)")==0);
		deviceClass = IDeviceDomain::CT_BASE;
		deviceLayout->addDevice(
				"systemScreen(1)", 0, 0, DV_QVGA_WIDTH, DV_QVGA_HEIGHT);

		// ATS BEGIN
//		deviceLayout->addDevice("systemScreen(2)", 0, 0, 320, 480);
		// ATS END

		serialized = dm->createWindow(
				myScreen, 0, 0, DV_QVGA_WIDTH, DV_QVGA_HEIGHT, -1.0);

		evs = new set<int>;
		evs->insert(CodeMap::KEY_TAP);

		im->addInputEventListener(this, evs);

		int caps = dm->getWindowCap (myScreen, serialized, "ALPHACHANNEL");
		dm->setWindowCaps (myScreen, serialized, caps);
		dm->drawWindow (myScreen, serialized);

#if HAVE_MULTIDEVICE
		//if ((deviceClass == IDeviceDomain::CT_BASE)&&(rdm==NULL)) {
		if (rdm == NULL) {
			rdm = RemoteDeviceManager::getInstance();
			if (!active_dev)
				((RemoteDeviceManager*)rdm)->setDeviceDomain(
						new BaseDeviceDomain(useMulticast, srvPort));
			else
				((RemoteDeviceManager*)rdm)->setDeviceDomain(
						new ActiveDeviceDomain(useMulticast, srvPort));
		}

		rdm->setDeviceInfo(deviceClass, w, h, playerId);
		rdm->addListener(this);
#endif //HAVE_MULTIDEVICE

		mainLayout = new FormatterLayout(myScreen, x, y, w, h);

		//mainLayout->getDeviceRegion()->setDeviceClass(0, "");
		mainLayout->getDeviceRegion()->setDeviceClass(0, "");

		layoutManager[deviceClass] = mainLayout;
	}

	FormatterBaseDevice::~FormatterBaseDevice() {
		if (im != NULL) {
			im->removeInputEventListener(this);
		}

		if (rdm != NULL) {
#if HAVE_MULTIDEVICE
			rdm->removeListener(this);
#endif //HAVE_MULTIDEVICE
		}

		if (dm->hasWindow(myScreen, serialized)) {
			dm->deleteWindow(myScreen, serialized);
			serialized = 0;
		}

		if (mainLayout != NULL) {
			mainLayout = NULL;
		}
	}

	bool FormatterBaseDevice::newDeviceConnected(
			int newDevClass,
			int w,
			int h) {

		return FormatterMultiDevice::newDeviceConnected(newDevClass, w, h);
	}

	bool FormatterBaseDevice::receiveRemoteEvent(
			int remoteDevClass,
			int eventType,
			string eventContent) {

		return FormatterMultiDevice::receiveRemoteEvent(
				remoteDevClass, eventType, eventContent);
	}

	bool FormatterBaseDevice::userEventReceived(IInputEvent* ev) {
		string mnemonicCode;
		int currentX;
		int currentY;
		int code;

		code = ev->getKeyCode(myScreen);

		if (code == CodeMap::KEY_TAP) {
			ev->getAxisValue(&currentX, &currentY, NULL);
			tapObject(deviceClass, currentX, currentY);

		} else if (code == CodeMap::KEY_QUIT) {
			clog << "FormatterBaseDevice::userEventReceived setting im as NULL";
			clog << endl;
			this->im = NULL;
		}

		return true;
	}
}
}
}
}
}
}
