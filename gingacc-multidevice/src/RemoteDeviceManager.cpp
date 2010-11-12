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

#include "multidevice/RemoteDeviceManager.h"

#include "multidevice/services/ActiveDeviceDomain.h"
#include "multidevice/services/PassiveDeviceDomain.h"
#include "multidevice/services/BaseDeviceDomain.h"

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include <iostream>
using namespace std;

using namespace ::br::pucrio::telemidia::ginga::core::multidevice;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace multidevice {
	RemoteDeviceManager* RemoteDeviceManager::_instance = NULL;
	RemoteDeviceManager::RemoteDeviceManager() : Thread() {
		domainService = NULL;
		connecting    = false;
		running       = false;
	}

	RemoteDeviceManager::~RemoteDeviceManager() {
		running = false;

		lock();
		connecting = false;

		if (domainService != NULL) {
			delete domainService;
			domainService = NULL;
		}

		unlock();
	}

	void RemoteDeviceManager::release() {
		running = false;
		if (_instance != NULL) {
			delete _instance;
			_instance = NULL;
		}
	}

	RemoteDeviceManager* RemoteDeviceManager::getInstance() {
		if (_instance == NULL) {
			_instance = new RemoteDeviceManager();
		}

		return _instance;
	}

	void RemoteDeviceManager::setDeviceDomain(IDeviceDomain* domain) {
		domainService = domain;
	}

	void RemoteDeviceManager::setDeviceInfo(
			int deviceClass, int width, int height) {

		if (domainService != NULL) {
			domainService->setDeviceInfo(width, height);
			if (!running) {
				start();
			}
		}
	}

	int RemoteDeviceManager::getDeviceClass() {
		if (domainService == NULL) {
			return -1;
		}
		return domainService->getDeviceClass();
	}

	void RemoteDeviceManager::addListener(IRemoteDeviceListener* listener) {
		if (domainService != NULL) {
			domainService->addDeviceListener(listener);
		}
	}

	void RemoteDeviceManager::removeListener(IRemoteDeviceListener* listener) {
		if (domainService != NULL) {
			domainService->removeDeviceListener(listener);
		}
	}

	void RemoteDeviceManager::postEvent(
			int devClass,
			int eventType,
			char* event,
			int eventSize) {

		domainService->postEventTask(devClass, eventType, event, eventSize);
	}

	bool RemoteDeviceManager::postMediaContent(int destDevClass, string url) {
		return domainService->postMediaContentTask(destDevClass, url);
	}

	void RemoteDeviceManager::postNclMetadata(
			int devClass, vector<StreamData*>* streams) {

		domainService->postNclMetadata(devClass, streams);
	}

	void RemoteDeviceManager::run() {
		double rdmTimer;

		rdmTimer   = 0;
		connecting = true;
		running    = true;

		lock();
		if (!running) {
			unlock();
			return;
		}

		while (running) {
			/*cout << "RemoteDeviceManager::run postConnectionRequestTask";
			cout << endl;*/

			//TODO: correct this code urgently
			if (domainService != NULL) {
				domainService->checkDomainTasks();
				domainService->postConnectionRequestTask();
			}

			/*if (connecting) {
				if (domainService->isConnected()) {
					cout << "RemoteDeviceManager::run connected" << endl;
					connecting = false;

				} else if (rdmTimer == 0 ||
						((getCurrentTimeMillis() - rdmTimer) > 3000)) {

					rdmTimer = getCurrentTimeMillis();
					domainService->postConnectionRequestTask();
				}
			}*/
#ifndef _WIN32
			::usleep(25000);
#else
			Sleep(2500);
#endif
		}
		unlock();
	}
}
}
}
}
}
}

extern "C" IRemoteDeviceManager* createRemoteDeviceManager(int devClass) {
	IDeviceDomain* domain = NULL;

	RemoteDeviceManager* rdm = RemoteDeviceManager::getInstance();

	if (devClass == IDeviceDomain::CT_BASE) {
		domain = new BaseDeviceDomain();

	} else if (devClass == IDeviceDomain::CT_PASSIVE) {
		domain = new PassiveDeviceDomain();

	} else if (devClass == IDeviceDomain::CT_ACTIVE) {
		domain = new ActiveDeviceDomain();
	}

	rdm->setDeviceDomain(domain);
	return rdm;
}

extern "C" void destroyRemoteDeviceManager(
		::br::pucrio::telemidia::ginga::core::multidevice::
				IRemoteDeviceManager* rdm) {

	rdm->release();
}
