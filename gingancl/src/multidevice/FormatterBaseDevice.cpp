/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware
Ginga (Ginga-NCL).

Direitos Autorais Reservados (c) 1989-2007 PUC-Rio/Laboratorio TeleMidia

Este programa eh software livre; voce pode redistribui-lo e/ou modificah-lo sob
os termos da Licenca Publica Geral GNU versao 2 conforme publicada pela Free
Software Foundation.

Este programa eh distribuido na expectativa de que seja util, porem, SEM
NENHUMA GARANTIA; nem mesmo a garantia implicita de COMERCIABILIDADE OU
ADEQUACAO A UMA FINALIDADE ESPECIFICA. Consulte a Licenca Publica Geral do
GNU versao 2 para mais detalhes.

Voce deve ter recebido uma copia da Licenca Publica Geral do GNU versao 2 junto
com este programa; se nao, escreva para a Free Software Foundation, Inc., no
endereco 59 Temple Street, Suite 330, Boston, MA 02111-1307 USA.

Para maiores informacoes:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
******************************************************************************
This file is part of the declarative environment of middleware Ginga (Ginga-NCL)

Copyright: 1989-2007 PUC-RIO/LABORATORIO TELEMIDIA, All Rights Reserved.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License version 2 as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License version 2 for more
details.

You should have received a copy of the GNU General Public License version 2
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

For further information contact:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
*******************************************************************************/

#include "gingancl/multidevice/FormatterBaseDevice.h"
#include "gingancl/multidevice/FMDComponentSupport.h"

#include "mb/interface/CodeMap.h"

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
#if HAVE_COMPONENTS
			rdm = ((IRemoteDeviceManagerCreator*)(cm->getObject(
					"RemoteDeviceManager")))(deviceClass, useMulticast, srvPort);
#else //!HAVE_COMPONENTS
			rdm = RemoteDeviceManager::getInstance();
			if (!active_dev)
				((RemoteDeviceManager*)rdm)->setDeviceDomain(
						new BaseDeviceDomain(useMulticast, srvPort));
			else 
				((RemoteDeviceManager*)rdm)->setDeviceDomain(
						new ActiveDeviceDomain(useMulticast, srvPort));
#endif //HAVE_COMPONENTS
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
			delete serialized;
			serialized = NULL;
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
