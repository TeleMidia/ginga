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

#ifdef _WIN32
#define	_EXP_LOCALDEVICEMANAGER_API 0
#endif

#include "mb/LocalDeviceManager.h"

#include "config.h"

#if HAVE_COMPSUPPORT
#include "cm/IComponentManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::cm;
#else
#ifndef _WIN32
#include "../../include/mb/interface/dfb/device/DFBDeviceScreen.h"
#else
#include "../../include/mb/interface/dx/device/DXDeviceScreen.h"
#endif
#endif

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	unsigned long LocalDeviceManager::parentId = 0;
	int LocalDeviceManager::numArgs = 0;
	char** LocalDeviceManager::args = NULL;
#if HAVE_COMPSUPPORT
	static IComponentManager* cm = IComponentManager::getCMInstance();
#endif

	LocalDeviceManager::LocalDeviceManager() {
		screenManager = NULL;
	}

	LocalDeviceManager::~LocalDeviceManager() {
		if (screenManager != NULL) {
			delete screenManager;
			screenManager = NULL;
		}
	}

	LocalDeviceManager* LocalDeviceManager::_instance = NULL;

	void LocalDeviceManager::setParentDevice(void* devId) {
		cout << "LocalDeviceManager::setParentDevice(" << this << ")";
		cout << " parent device ID = '" << devId << "'" << endl;
		parentId = (unsigned long)devId;
	}

	void LocalDeviceManager::release() {
		if (_instance != NULL) {
			delete _instance;
			_instance = NULL;
		}
		//TODO: delete devices
	}

	void LocalDeviceManager::setBackgroundImage(string uri) {
		if (screenManager != NULL) {
			screenManager->setBackgroundImage(0, uri);
		}
	}

	void LocalDeviceManager::setParameters(int numArgs, char** args) {
		LocalDeviceManager::numArgs = numArgs;
		LocalDeviceManager::args = args;
	}

	LocalDeviceManager* LocalDeviceManager::getInstance() {
		if (LocalDeviceManager::_instance == NULL) {
			LocalDeviceManager::_instance = new LocalDeviceManager();
		}
		return LocalDeviceManager::_instance;
	}

	int LocalDeviceManager::getDeviceWidth(
			unsigned int deviceNumber, unsigned int screenNumber) {

		if (screenManager == NULL) {
			return 0;
		}

		return screenManager->getWidthResolution(screenNumber);
	}

	int LocalDeviceManager::getDeviceHeight(
			unsigned int deviceNumber, unsigned int screenNumber) {

		if (screenManager == NULL) {
			return 0;
		}

		return screenManager->getHeightResolution(screenNumber);
	}

	void* LocalDeviceManager::getGfxRoot(unsigned int deviceNumber) {
		if (screenManager == NULL) {
			return 0;
		}

		return screenManager->getGfxRoot();
	}

	void LocalDeviceManager::clearWidgetPools() {
		if (screenManager != NULL) {
			//screenManager->clearWidgets();
		}
	}

	int LocalDeviceManager::createDevice(string description) {
		unsigned int deviceNumber;
		IDeviceScreen* scr = NULL;

#if HAVE_COMPSUPPORT
		cout << "LocalDeviceManager::createDevice(" << this << ") with";
		cout << " parentId = '" << parentId << "'" << endl;

		screenManager = ((ScreenCreator*)(cm->getObject("DeviceScreen")))(
				this->numArgs, this->args, (void*)parentId);

#else
#ifndef _WIN32
		screenManager = new DFBDeviceScreen(
				this->numArgs, this->args, parentId);

#else
		screenManager = new DXDeviceScreen(
				this->numArgs, this->args, parentId);
#endif
#endif

		return 0;
	}

	void LocalDeviceManager::mergeIds(
			int destId,
			vector<int>* srcIds,
			unsigned int deviceNumber,
			unsigned int screenNumber) {

		if (screenManager == NULL) {
			return;
		}

		screenManager->mergeIds(destId, srcIds);
	}

	void* LocalDeviceManager::getWindow(
			int winId, unsigned int deviceNumber, unsigned int screenNumber) {

		if (screenManager == NULL) {
			return NULL;
		}

		return screenManager->getWindow(winId);
	}

	void* LocalDeviceManager::createWindow(
			void* winDesc,
			unsigned int deviceNumber, unsigned int screenNumber) {

		if (screenManager == NULL) {
			return NULL;
		}

		return screenManager->createWindow(screenNumber, winDesc);
	}

	void LocalDeviceManager::releaseWindow(
			void* win, unsigned int deviceNumber, unsigned int screenNumber) {

		if (screenManager == NULL) {
			return;
		}

		return screenManager->releaseWindow(win);
	}

	void* LocalDeviceManager::createSurface(
			void* surfaceDesc,
			unsigned int deviceNumber, unsigned int screenNumber) {

		if (screenManager == 0) {
			return NULL;
		}

		return screenManager->createSurface(surfaceDesc);
	}

	void LocalDeviceManager::releaseSurface(
			void* sur,
			unsigned int deviceNumber, unsigned int screenNumber) {

		if (screenManager == NULL) {
			return;
		}

		return screenManager->releaseSurface(sur);
	}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::mb::
		ILocalDeviceManager* createLocalDeviceManager() {

	::br::pucrio::telemidia::ginga::core::mb::ILocalDeviceManager* ldm = (
			::br::pucrio::telemidia::ginga::core::mb::LocalDeviceManager::getInstance());

	return ldm;
}

extern "C" void destroyLocalDeviceManager(
		::br::pucrio::telemidia::ginga::core::mb::ILocalDeviceManager* dm) {

	dm->release();
}
