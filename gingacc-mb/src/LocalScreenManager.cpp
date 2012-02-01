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

extern "C" {
#include "string.h"
}

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
		screens  = new map<GingaScreenID, IDeviceScreen*>;
		pthread_mutex_init(&scrMutex, NULL);

		sysNames = new map<string, short>;
		pthread_mutex_init(&sysMutex, NULL);

		(*sysNames)["dflt" ] = GMBST_DFLT;
		(*sysNames)["dfb"  ] = GMBST_DFB;
		(*sysNames)["dx"   ] = GMBST_DX;
		(*sysNames)["term" ] = GMBST_TERM;
		(*sysNames)["sdl"  ] = GMBST_SDL;
		(*sysNames)["fbdev"] = GMBSST_FBDEV;
		(*sysNames)["x11"  ] = GMBSST_X11;
		(*sysNames)["hwnd" ] = GMBSST_HWND;
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

		lockSysNames();
		if (sysNames != NULL) {
			delete sysNames;
			sysNames = NULL;
		}
		unlockSysNames();
		pthread_mutex_destroy(&sysMutex);
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

	GingaScreenID LocalScreenManager::createScreen(int argc, char** args) {
		int i;
		string mbSystem = "", mbSubSystem = "", mbMode = "", mbParent = "";

		for (i = 0; i < argc; i++) {
			if ((strcmp(args[i], "--system") == 0) && ((i + 1) < argc)) {
				mbSystem.assign(args[i + 1]);

			} else if ((strcmp(args[i], "--subsystem") == 0) &&
					((i + 1) < argc)) {

				mbSubSystem.assign(args[i + 1]);

			} else if ((strcmp(args[i], "--mode") == 0) && ((i + 1) < argc)) {
				mbMode.assign(args[i + 1]);

			} else if ((strcmp(args[i], "--parent") == 0) && ((i + 1) < argc)) {
				mbParent.assign(args[i + 1]);
			}
		}

		return createScreen(mbSystem, mbSubSystem, mbMode, mbParent);
	}

	GingaScreenID LocalScreenManager::createScreen(
			string mbSystem,
			string mbSubSystem,
			string mbMode,
			string mbParent) {

		IDeviceScreen* screen;
		GingaScreenID screenId;
		GingaWindowID parentId = NULL;
		short sysType, subSysType;

		screenId = getNumOfScreens();
		getMBSystemType(mbSystem, &sysType, mbSubSystem, &subSysType);

		switch (sysType) {
			case GMBST_TERM:
#if HAVE_COMPSUPPORT
				screen = ((ScreenCreator*)(cm->getObject(
						"TermDeviceScreen")))(0, NULL, screenId, parentId);

#endif
				break;

			case GMBST_DFLT:
			default:
#if HAVE_COMPSUPPORT
				screen = ((ScreenCreator*)(cm->getObject(
						"DFBDeviceScreen")))(0, NULL, screenId, parentId);

#else
#ifndef _WIN32
				screen = new DFBDeviceScreen(0, NULL, screenId, parentId);

#else
				screen = new DXDeviceScreen(0, NULL, screenId, parentId);
#endif
#endif
				break;
		}

		addScreen(screenId, screen);

		return screenId;
	}

	void LocalScreenManager::getMBSystemType(
			string mbSystemName,
			short* mbSystemType,
			string mbSubSystemName,
			short* mbSubSystemType) {

		map<string, short>::iterator i;

		*mbSystemType    = GMBST_DFLT;
		*mbSubSystemType = GMBSST_DFLT;

#if !HAVE_COMPSUPPORT
		return;
#endif

		lockSysNames();

		i = sysNames->find(mbSystemName);
		if (i != sysNames->end()) {
			*mbSystemType = i->second;
		}

		i = sysNames->find(mbSubSystemName);
		if (i != sysNames->end()) {
			*mbSubSystemType = i->second;
		}

		unlockSysNames();
	}

	void LocalScreenManager::lockSysNames() {
		pthread_mutex_lock(&sysMutex);
	}

	void LocalScreenManager::unlockSysNames() {
		pthread_mutex_unlock(&sysMutex);
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

	IWindow* LocalScreenManager::createWindow(
			GingaScreenID screenId, int x, int y, int w, int h) {

		IDeviceScreen* screen;
		IWindow* window = NULL;

		if (getScreen(screenId, &screen)) {
			window = screen->createWindow(x, y, w, h);
		}

		return window;
	}

	IWindow* LocalScreenManager::createWindowFrom(
			GingaScreenID screenId, GingaWindowID underlyingWindow) {

		IDeviceScreen* screen;
		IWindow* window = NULL;

		if (getScreen(screenId, &screen)) {
			window = screen->createWindowFrom(underlyingWindow);
		}

		return window;
	}

	void LocalScreenManager::releaseWindow(
			GingaScreenID screenId, IWindow* win) {

		IDeviceScreen* screen;

		if (getScreen(screenId, &screen)) {
			screen->releaseWindow(win);
		}
	}

	ISurface* LocalScreenManager::createSurface(GingaScreenID screenId) {
		IDeviceScreen* screen;
		ISurface* surface = NULL;

		if (getScreen(screenId, &screen)) {
			surface = screen->createSurface();
		}

		return surface;
	}

	ISurface* LocalScreenManager::createSurface(
			GingaScreenID screenId, int w, int h) {

		IDeviceScreen* screen;
		ISurface* surface = NULL;

		if (getScreen(screenId, &screen)) {
			surface = screen->createSurface(w, h);
		}

		return surface;
	}

	ISurface* LocalScreenManager::createSurfaceFrom(
			GingaScreenID screenId, void* underlyingSurface) {

		IDeviceScreen* screen;
		ISurface* surface = NULL;

		if (getScreen(screenId, &screen)) {
			surface = screen->createSurfaceFrom(underlyingSurface);
		}

		return surface;
	}

	void LocalScreenManager::releaseSurface(
			GingaScreenID screenId, ISurface* sur) {

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
