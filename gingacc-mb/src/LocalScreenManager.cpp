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
#define	_EXP_LOCALSCREENHANDLER_API 0
#endif

#include "mb/LocalScreenManager.h"

#include "config.h"

#if HAVE_COMPSUPPORT
#include "cm/IComponentManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::cm;
#else
#ifndef _WIN32
#include "mb/interface/dfb/device/DFBDeviceScreen.h"
#else
#include "mb/interface/dx/device/DXDeviceScreen.h"
#endif
#endif

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {

#if HAVE_COMPSUPPORT
	static IComponentManager* cm = IComponentManager::getCMInstance();
#endif

	LocalScreenManager::LocalScreenManager() {
		screens = new map<GingaScreenID, IDeviceScreen*>;
		pthread_mutex_init(&scrMutex, NULL);
	}

	LocalScreenManager::~LocalScreenManager() {
		map<GingaScreenID, IDeviceScreen*>::iterator i;

		lockScreens();
		if (screens != NULL) {
			i = screens->begin();
			while (i != screens->end()) {
				delete i->second;
				++i;
			}
			delete screens;
			screens = NULL;
		}
		unlockScreens();

		pthread_mutex_destroy(&scrMutex);
	}

	LocalScreenManager* LocalScreenManager::_instance = NULL;

	void LocalScreenManager::setParentScreen(
			GingaScreenID screenId, void* parentId) {

		IDeviceScreen* screen;

		if (getScreen(screenId, &screen)) {
			screen->setParentScreen(parentId);
			cout << "LocalScreenManager::setParentDevice(" << this << ")";
			cout << " parent device ID = '" << parentId << "'" << endl;
		}
	}

	void LocalScreenManager::releaseHandler() {
		if (_instance != NULL) {
			delete _instance;
			_instance = NULL;
		}
	}

	void LocalScreenManager::setBackgroundImage(
			GingaScreenID screenId, string uri) {

		IDeviceScreen* screen;

		if (getScreen(screenId, &screen)) {
			screen->setBackgroundImage(uri);
		}
	}

	LocalScreenManager* LocalScreenManager::getInstance() {
		if (LocalScreenManager::_instance == NULL) {
			LocalScreenManager::_instance = new LocalScreenManager();
		}
		return LocalScreenManager::_instance;
	}

	int LocalScreenManager::getDeviceWidth(GingaScreenID screenId) {
		IDeviceScreen* screen;
		int width = 0;

		if (getScreen(screenId, &screen)) {
			width = screen->getWidthResolution();
		}

		return width;
	}

	int LocalScreenManager::getDeviceHeight(GingaScreenID screenId) {
		IDeviceScreen* screen;
		int height = 0;

		if (getScreen(screenId, &screen)) {
			height = screen->getHeightResolution();
		}

		return height;
	}

	void* LocalScreenManager::getGfxRoot(GingaScreenID screenId) {
		IDeviceScreen* screen;
		void* gfxRoot = NULL;

		if (getScreen(screenId, &screen)) {
			gfxRoot = screen->getGfxRoot();
		}

		return gfxRoot;
	}

	void LocalScreenManager::clearWidgetPools(GingaScreenID screenId) {
		IDeviceScreen* screen;

		if (getScreen(screenId, &screen)) {
			screen->clearWidgetPools();
		}
	}

	unsigned long LocalScreenManager::createScreen(int numArgs, char** args) {
		IDeviceScreen* screen;
		GingaScreenID screenId;
		GingaWindowID parentId = NULL;

		screenId = getNumOfScreens();

#if HAVE_COMPSUPPORT
		screen = ((ScreenCreator*)(cm->getObject("DeviceScreen")))(
				numArgs, args, parentId);

#else
#ifndef _WIN32
		screen = new DFBDeviceScreen(numArgs, args, parentId);

#else
		screen = new DXDeviceScreen(numArgs, args, parentId);
#endif
#endif

		addScreen(screenId, screen);

		return screenId;
	}

	void LocalScreenManager::mergeIds(
			GingaScreenID screenId,
			GingaWindowID destId,
			vector<GingaWindowID>* srcIds) {

		IDeviceScreen* screen;

		if (getScreen(screenId, &screen)) {
			screen->mergeIds(destId, srcIds);
		}
	}

	void* LocalScreenManager::getWindow(
			GingaScreenID screenId, GingaWindowID winId) {

		IDeviceScreen* screen;
		void* window = NULL;

		if (getScreen(screenId, &screen)) {
			window = screen->getWindow(winId);
		}

		return window;
	}

	void* LocalScreenManager::createWindow(
			GingaScreenID screenId, void* winDesc) {

		IDeviceScreen* screen;
		void* window = NULL;

		if (getScreen(screenId, &screen)) {
			window = screen->createWindow(winDesc);
		}

		return window;
	}

	void LocalScreenManager::releaseWindow(GingaScreenID screenId, void* win) {
		IDeviceScreen* screen;

		if (getScreen(screenId, &screen)) {
			screen->releaseWindow(win);
		}
	}

	void* LocalScreenManager::createSurface(
			GingaScreenID screenId, void* surfaceDesc) {

		IDeviceScreen* screen;
		void* surface = NULL;

		if (getScreen(screenId, &screen)) {
			surface = screen->createSurface(surfaceDesc);
		}

		return surface;
	}

	void LocalScreenManager::releaseSurface(GingaScreenID screenId, void* sur) {
		IDeviceScreen* screen;

		if (getScreen(screenId, &screen)) {
			screen->releaseSurface(sur);
		}
	}

	void LocalScreenManager::addScreen(
			GingaScreenID screenId, IDeviceScreen* screen) {

		lockScreens();
		(*screens)[screenId] = screen;
		unlockScreens();
	}

	short LocalScreenManager::getNumOfScreens() {
		short numOfScreens;

		lockScreens();
		numOfScreens = screens->size();
		unlockScreens();

		return numOfScreens;
	}

	bool LocalScreenManager::getScreen(
			GingaScreenID screenId, IDeviceScreen** screen) {

		bool hasScreen = false;
		map<GingaScreenID, IDeviceScreen*>::iterator i;

		lockScreens();
		i = screens->find(screenId);
		if (i != screens->end()) {
			hasScreen = true;
			*screen   = i->second;
		}
		unlockScreens();

		return hasScreen;
	}

	void LocalScreenManager::lockScreens() {
		pthread_mutex_lock(&scrMutex);
	}

	void LocalScreenManager::unlockScreens() {
		pthread_mutex_unlock(&scrMutex);
	}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::mb::
		ILocalScreenManager* createLocalScreenManager() {

	::br::pucrio::telemidia::ginga::core::mb::ILocalScreenManager* ldm = (
			::br::pucrio::telemidia::ginga::core::mb::LocalScreenManager::getInstance());

	return ldm;
}

extern "C" void destroyLocalScreenManager(
		::br::pucrio::telemidia::ginga::core::mb::ILocalScreenManager* dm) {

	dm->releaseHandler();
}
