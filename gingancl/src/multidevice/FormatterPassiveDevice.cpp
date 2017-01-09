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

#include "gingancl/multidevice/FormatterPassiveDevice.h"
#include "gingancl/multidevice/FMDComponentSupport.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace multidevice {
	FormatterPassiveDevice::FormatterPassiveDevice(
			GingaScreenID screenId,
			IDeviceLayout* deviceLayout,
			int x, int y, int w, int h, bool useMulticast, int srvPort) : FormatterMultiDevice(
					screenId, deviceLayout, x, y, w, h, useMulticast, srvPort) {

		deviceClass = IDeviceDomain::CT_PASSIVE;
		serialized = dm->createWindow(
				myScreen, x, y, defaultWidth, defaultHeight, -1.0);

		int cap = dm->getWindowCap (myScreen, serialized, "ALPHACHANNEL");
		dm->setWindowCaps (myScreen, serialized, cap);
		dm->drawWindow (myScreen, serialized);

		if (rdm == NULL) {
			rdm = RemoteDeviceManager::getInstance();
			((RemoteDeviceManager*)rdm)->setDeviceDomain(
					new PassiveDeviceDomain(useMulticast, srvPort));
		}

		rdm->setDeviceInfo(deviceClass, w, h, "");
		rdm->addListener(this);

		im->addInputEventListener(this, NULL);

		mainLayout = new FormatterLayout(myScreen, x, y, w, h);
		layoutManager[deviceClass] = mainLayout;
	}

	FormatterPassiveDevice::~FormatterPassiveDevice() {

	}

	void FormatterPassiveDevice::connectedToBaseDevice(unsigned int domainAddr) {
		/*clog << "FormatterPassiveDevice::connectedToDomainService '";
		clog << domainAddr << "'" << endl;*/

		hasRemoteDevices = true;
	}

	bool FormatterPassiveDevice::receiveRemoteContent(
			int remoteDevClass,
			string contentUri) {

		/*clog << "FormatterPassiveDevice::receiveRemoteContent from class '";
		clog << remoteDevClass << "' and contentUri '" << contentUri << "'";
		clog << endl;*/

		renderFromUri(serialized, contentUri);
		return true;
	}

	bool FormatterPassiveDevice::userEventReceived(IInputEvent* ev) {
		string mnemonicCode;
		int currentX;
		int currentY;
		int code;

		clog << "FormatterPassiveDevice::userEventReceived" << endl;

		code = ev->getKeyCode(myScreen);
		if (code == CodeMap::KEY_F11 || code == CodeMap::KEY_F10) {
			std::abort();

		} else if (code == CodeMap::KEY_QUIT) {
			this->im = NULL;
			return true;
		}

		if (!hasRemoteDevices) {
			clog << "FormatterPassiveDevice::userEventReceived !hasRemoteDevices" << endl;
			return false;
		}

		mnemonicCode = CodeMap::getInstance()->getValue(code);

		if (ev->isButtonPressType()) {
			ev->getAxisValue(&currentX, &currentY, NULL);

			mnemonicCode = (mnemonicCode + "," +
					itos(currentX - xOffset) + "," +
					itos(currentY - yOffset));
		}

		clog << "FormatterPassiveDevice::userEventReceived posting '";
		clog << mnemonicCode << "'" << endl;

		rdm->postEvent(
				IDeviceDomain::CT_BASE,
				IDeviceDomain::FT_SELECTIONEVENT,
				(char*)(mnemonicCode.c_str()),
				(int)(mnemonicCode.length()));

		return false;
	}
}
}
}
}
}
}
