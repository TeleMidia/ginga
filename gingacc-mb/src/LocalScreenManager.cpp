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

#include "mb/interface/CodeMap.h"
#include "mb/LocalScreenManager.h"
#include "mb/InputManager.h"

extern "C" {
#include "string.h"
}

#include "config.h"

#if HAVE_COMPONENTS
#include "cm/IComponentManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::cm;
#else

#if HAVE_DIRECTFB
#include "mb/interface/dfb/DFBDeviceScreen.h"
#endif //HAVE_DIRECTFB

#if HAVE_SDL
#include "mb/interface/sdl/SDLDeviceScreen.h"
#endif //HAVE_SDL

#if HAVE_TERM
#include "mb/interface/term/TermDeviceScreen.h"
#endif //HAVE_TERM

#endif //HAVE_COMPONENTS

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
#if HAVE_COMPONENTS
	static IComponentManager* cm = IComponentManager::getCMInstance();
#endif

	set<IInputEventListener*> LocalScreenManager::iListeners;
	pthread_mutex_t LocalScreenManager::ilMutex;

	set<IMotionEventListener*> LocalScreenManager::mListeners;
	pthread_mutex_t LocalScreenManager::mlMutex;

	bool LocalScreenManager::initMutex = false;

	const short LocalScreenManager::GMBST_DFLT   = 0;
	const short LocalScreenManager::GMBST_DFB    = 1;
	const short LocalScreenManager::GMBST_DX     = 2;
	const short LocalScreenManager::GMBST_SDL    = 3;
	const short LocalScreenManager::GMBST_TERM   = 4;

	const short LocalScreenManager::GMBSST_DFLT  = 0;
	const short LocalScreenManager::GMBSST_FBDEV = 1;
	const short LocalScreenManager::GMBSST_X11   = 2;
	const short LocalScreenManager::GMBSST_HWND  = 3;
	const short LocalScreenManager::GMBSST_SDL   = 4;
	const short LocalScreenManager::GMBSST_COCOA = 5;

	LocalScreenManager::LocalScreenManager() {
		Thread::mutexInit(&mapMutex);
		Thread::mutexInit(&genMutex);
		Thread::mutexInit(&sysMutex);
		Thread::mutexInit(&surMapMutex);

		sysNames["dflt" ] = GMBST_DFLT;
		sysNames["dfb"  ] = GMBST_DFB;
		sysNames["dx"   ] = GMBST_DX;
		sysNames["sdl"  ] = GMBST_SDL;
		sysNames["term" ] = GMBST_TERM;

		sortSys.push_back(GMBST_DFB);
		sortSys.push_back(GMBST_SDL);
		sortSys.push_back(GMBST_TERM);

		waitingRefreshScreen = false;
		running              = false;
		isWaiting            = false;

		surIdRefCounter			 = 1;

		Thread::condInit(&wsSignal, NULL);
		Thread::mutexInit(&wsMutex);

		clog << "LocalScreenManager::LocalScreenManager(" << this << ") ";
		clog << "all done" << endl;
	}

	LocalScreenManager::~LocalScreenManager() {
		map<GingaScreenID, IDeviceScreen*>::iterator i;

		lockScreenMap();
		i = screens.begin();
		while (i != screens.end()) {
			delete i->second;
			++i;
		}
		screens.clear();
		unlockScreenMap();
		Thread::mutexDestroy(&mapMutex);

		lock();
		unlock();
		Thread::mutexDestroy(&genMutex);

		map<GingaSurfaceID, ISurface*>::iterator j;

		Thread::mutexLock(&surMapMutex);
		j = surMap.begin();
		while (j != surMap.end()) {
			delete j->second;
			++j;
		}
		surMap.clear();
		Thread::mutexUnlock(&surMapMutex);
		Thread::mutexDestroy(&surMapMutex);


		lockSysNames();
		sysNames.clear();
		unlockSysNames();
		Thread::mutexDestroy(&sysMutex);
	}

	LocalScreenManager* LocalScreenManager::_instance = NULL;

	void LocalScreenManager::checkInitMutex() {
		if (!initMutex) {
			initMutex = true;

			Thread::mutexInit(&ilMutex);
			iListeners.clear();

			Thread::mutexInit(&mlMutex);
			mListeners.clear();
		}
	}

	void LocalScreenManager::releaseHandler() {
		if (_instance != NULL) {
			delete _instance;
			_instance = NULL;
		}
	}

	void LocalScreenManager::addIEListenerInstance(
			IInputEventListener* listener) {

		checkInitMutex();
		Thread::mutexLock(&ilMutex);
		iListeners.insert(listener);
		Thread::mutexUnlock(&ilMutex);
	}

	void LocalScreenManager::removeIEListenerInstance(
			IInputEventListener* listener) {

		set<IInputEventListener*>::iterator i;

		checkInitMutex();
		Thread::mutexLock(&ilMutex);
		i = iListeners.find(listener);
		if (i != iListeners.end()) {
			iListeners.erase(i);
		}
		Thread::mutexUnlock(&ilMutex);
	}

	bool LocalScreenManager::hasIEListenerInstance(
			IInputEventListener* listener, bool removeInstance) {

		set<IInputEventListener*>::iterator i;
		bool hasListener = false;

		checkInitMutex();
		Thread::mutexLock(&ilMutex);
		i = iListeners.find(listener);
		if (i != iListeners.end()) {
			hasListener = true;

			if (removeInstance) {
				iListeners.erase(i);
			}
		}
		Thread::mutexUnlock(&ilMutex);

		return hasListener;
	}


	void LocalScreenManager::addMEListenerInstance(
			IMotionEventListener* listener) {

		checkInitMutex();
		Thread::mutexLock(&mlMutex);
		mListeners.insert(listener);
		Thread::mutexUnlock(&mlMutex);
	}

	void LocalScreenManager::removeMEListenerInstance(
			IMotionEventListener* listener) {

		set<IMotionEventListener*>::iterator i;

		checkInitMutex();
		Thread::mutexLock(&mlMutex);
		i = mListeners.find(listener);
		if (i != mListeners.end()) {
			mListeners.erase(i);
		}
		Thread::mutexUnlock(&mlMutex);
	}

	bool LocalScreenManager::hasMEListenerInstance(
			IMotionEventListener* listener, bool removeInstance) {

		set<IMotionEventListener*>::iterator i;
		bool hasListener = false;

		checkInitMutex();
		Thread::mutexLock(&mlMutex);
		i = mListeners.find(listener);
		if (i != mListeners.end()) {
			hasListener = true;

			if (removeInstance) {
				mListeners.erase(i);
			}
		}
		Thread::mutexUnlock(&mlMutex);

		return hasListener;
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

	void LocalScreenManager::releaseScreen(GingaScreenID screenId) {
		IDeviceScreen* screen;

		if (getScreen(screenId, &screen)) {
			removeScreen(screenId);
			screen->releaseScreen();
			delete screen;
		}
	}

	void LocalScreenManager::releaseMB(GingaScreenID screenId) {
		IDeviceScreen* screen;

		if (getScreen(screenId, &screen)) {
			screen->releaseMB();
		}
	}

	void LocalScreenManager::clearWidgetPools(GingaScreenID screenId) {
		IDeviceScreen* screen;

		if (getScreen(screenId, &screen)) {
			screen->clearWidgetPools();
		}
	}

	GingaScreenID LocalScreenManager::createScreen(int argc, char** args) {
		int i;
		bool externalRenderer = false;
		bool useStdin  = false;
		string vSystem = "", vSubSystem = "", vMode = "";
		string vParent = "", vEmbed = "";
		string aSystem = "";
		GingaScreenID newScreen;

		clog << "LocalScreenManager::createScreen argv[";
		clog << argc << "]" << endl;

		lock();
		for (i = 0; i < argc; i++) {
			if ((strcmp(args[i], "--vsystem") == 0) && ((i + 1) < argc)) {
				vSystem.assign(args[i + 1]);

			} else if ((strcmp(args[i], "--vsubsystem") == 0) &&
					((i + 1) < argc)) {

				vSubSystem.assign(args[i + 1]);

			} else if ((strcmp(args[i], "--vmode") == 0) && ((i + 1) < argc)) {
				vMode.assign(args[i + 1]);

			} else if ((strcmp(args[i], "--parent") == 0) && ((i + 1) < argc)) {
				vParent.assign(args[i + 1]);

			} else if ((strcmp(args[i], "--embed") == 0 ||
					strcmp(args[i], "--wid") == 0) && ((i + 1) < argc)) {

				vEmbed.assign(args[i + 1]);

			} else if ((strcmp(args[i], "--asystem") == 0) && ((i + 1) < argc)) {
				aSystem.assign(args[i + 1]);

			} else if (strcmp(args[i], "--external-renderer") == 0) {
				externalRenderer = true;

			} else if (strcmp(args[i], "--poll-stdin") == 0) {
				useStdin = true;
			}

			clog << "LocalScreenManager::createScreen PARSER argv[";
			clog << i << "] = '" << args[i] << "'" << endl;
		}

		newScreen = createScreen(
				vSystem, vSubSystem, vMode,
				vParent, vEmbed, aSystem, externalRenderer, useStdin);

		unlock();

		return newScreen;
	}

	GingaScreenID LocalScreenManager::createScreen(
			string vSystem,
			string vSubSystem,
			string vMode,
			string vParent,
			string vEmbed,
			string aSystem,
			bool externalRenderer,
			bool useStdin) {

		IDeviceScreen* screen  = NULL;
		GingaWindowID embedWin = NULL;
		int argc               = 0;

		short sysType;
		GingaScreenID screenId;
		char* mbArgs[4];

		string params    = "";
		string paramsSfx = "";
		string mycmd     = "ginga";

		screenId = getNumOfScreens();
		sysType = getMBSystemType(vSystem);

		switch (sysType) {
			case GMBST_SDL:
				mbArgs[argc] = (char*)mycmd.c_str();
				argc++;

				if (vSubSystem != "") {
					mbArgs[argc] = (char*)"subsystem";
					argc++;

					mbArgs[argc] = (char*)vSubSystem.c_str();
					argc++;
				}

				if (vMode != "") {
					mbArgs[argc] = (char*)"mode";
					argc++;

					mbArgs[argc] = (char*)vMode.c_str();
					argc++;
				}

				if (vParent != "") {
					mbArgs[argc] = (char*)"parent";
					argc++;

					mbArgs[argc] = (char*)vParent.c_str();
					argc++;

					clog << "LocalScreenManager::createScreen parent with ";
					clog << "following data '" << vParent << "'";
					clog << endl;
				}

				if (vEmbed != "") {
					embedWin = (void*)strtoul(vEmbed.c_str(), NULL, 10);

					clog << "LocalScreenManager::createScreen embed src = ";
					clog << vEmbed << "' and dst = '" << embedWin << "'";
					clog << endl;
				}

				if (aSystem != "") {
					mbArgs[argc] = (char*)"audio";
					argc++;

					mbArgs[argc] = (char*)aSystem.c_str();
					argc++;
				}

				if (useStdin) {
					mbArgs[argc] = (char*)"poll-stdin";
					argc++;
				}

#if HAVE_COMPONENTS
				screen = ((ScreenCreator*)(cm->getObject("SDLDeviceScreen")))(
						argc, mbArgs, screenId, embedWin, externalRenderer);

#else
#if HAVE_SDL
				screen = new SDLDeviceScreen(
						argc, mbArgs, screenId, embedWin, externalRenderer);
#endif //HAVE_SDL
#endif //HAVE_COMPONENTS
				break;

			case GMBST_TERM:
#if HAVE_COMPONENTS
				screen = ((ScreenCreator*)(cm->getObject("TermDeviceScreen")))(
						0, NULL, screenId, embedWin, externalRenderer);

#else
#if HAVE_TERM
				screen = new TermDeviceScreen(
						argc, mbArgs, screenId, embedWin, externalRenderer);
#endif //HAVE_TERM
#endif //HAVE_COMPONENTS
				break;

			case GMBST_DFB:
				params = "";

				if (vSubSystem != "") {
					params = "--dfb:system=" + vSubSystem;
				}

				if (vMode != "") {
					if (params == "") {
						params = "--dfb:mode=" + vMode;

					} else {
						params = params + ",mode=" + vMode;
					}
				}

				if (vEmbed != "") {
					if (params == "") {
						params = "--dfb:x11-root-window=" + vEmbed;

					} else {
						params = params + ",x11-root-window=" + vEmbed;
					}
				}

				paramsSfx = "no-sighandler,force-windowed,quiet,no-banner,";
				paramsSfx = paramsSfx + "no-debug,fatal-level=NONE";

				if (params == "") {
					params = "--dfb:" + paramsSfx;

				} else {
					params = params + "," + paramsSfx;
				}

				mbArgs[argc] = (char*)mycmd.c_str();
				argc++;
				mbArgs[argc] = (char*)params.c_str();
				argc++;

				if (aSystem != "") {
					mbArgs[argc] = (char*)"audio";
					argc++;

					mbArgs[argc] = (char*)aSystem.c_str();
					argc++;
				}

#if HAVE_COMPONENTS
				screen = ((ScreenCreator*)(cm->getObject("DFBDeviceScreen")))(
						argc, mbArgs, screenId, embedWin, externalRenderer);

#else
#if HAVE_DIRECTFB
				screen = new DFBDeviceScreen(
						argc, mbArgs, screenId, embedWin, externalRenderer);
#endif //HAVE_DIRECTFB
#endif //HAVE_COMPONENTS
				break;

			default:
				clog << "LocalScreenManager::createScreen please reinstall";
				clog << " ginga with a multimedia library (SDL or DFB)";
				clog << endl;
				break;
		}

		addScreen(screenId, screen);

		if (screen == NULL) {
			screenId = -1;
		}

		return screenId;
	}

	string LocalScreenManager::getScreenName(GingaScreenID screenId) {
		IDeviceScreen* screen;
		string screenName = "";

		if (getScreen(screenId, &screen)) {
			screenName = screen->getScreenName();
		}

		return screenName;
	}

	GingaWindowID LocalScreenManager::getScreenUnderlyingWindow(
			GingaScreenID screenId) {

		IDeviceScreen* screen;
		GingaWindowID uWin = NULL;

		if (getScreen(screenId, &screen)) {
			uWin = screen->getScreenUnderlyingWindow();
		}

		return uWin;
	}

	short LocalScreenManager::getMBSystemType(string mbSystemName) {
		map<string, short>::iterator i;
		vector<short>::iterator j;
		bool foundit = true;
		short mbSystemType = GMBST_DFLT;

		if (mbSystemName == "sdl") {
#if HAVE_SDL
			mbSystemType = GMBST_SDL;
#endif
		} else if (mbSystemName == "dfb") {
#if HAVE_DIRECTFB
			mbSystemType = GMBST_DFB;
#endif
		} else if (mbSystemName == "term") {
#if HAVE_TERM
			mbSystemType = GMBST_TERM;
#endif
		} else if (mbSystemName != "") {
			foundit = false;
		}

		if (mbSystemType == GMBST_DFLT) {
#if HAVE_SDL
			mbSystemType = GMBST_SDL;
#elif HAVE_DIRECTFB
			mbSystemType = GMBST_DFB;
#elif HAVE_TERM
			mbSystemType = GMBST_TERM;
#else
			foundit = false;
#endif
		}

#if !HAVE_COMPONENTS
		if (mbSystemType == GMBST_DFLT) {
			mbSystemType = GMBST_SDL;
		}

		return mbSystemType;
#endif

		lockSysNames();

		if (!foundit) {
			if (mbSystemName == "") {
				string lName = "sdl";
				i = sysNames.find(lName);
				if (i != sysNames.end()) {
					mbSystemType = i->second;
					foundit = true;
				}
			}
		}

		if (!foundit || (foundit && !isAvailable(mbSystemType))) {
			foundit = false;
			j = sortSys.begin();
			while (j != sortSys.end()) {
				if (*j != mbSystemType) {
					if (isAvailable(*j)) {
						foundit = true;
						mbSystemType = *j;
						break;
					}
				}
				++j;
			}
		}

		unlockSysNames();

		if (!foundit) {
			clog << "LocalScreenManager::getMBSystemType Warning! ";
			clog << "Can't find a multimedia backend library" << endl;
			mbSystemType = -1;
		}

		return mbSystemType;
	}

	bool LocalScreenManager::isAvailable(short mbSysType) {
		bool hasSys = false;
		string screenName = "";

		clog << "LocalScreenManager::isAvailable checking if '" << mbSysType;
		clog << "' is available" << endl;

#if HAVE_COMPONENTS
		switch (mbSysType) {
			case GMBST_SDL:
				screenName = "SDLDeviceScreen";
				break;

			case GMBST_TERM:
				screenName = "TermDeviceScreen";
				break;

			case GMBST_DFB:
				screenName = "DFBDeviceScreen";
				break;
		}

		if (screenName != "") {
			hasSys = cm->isAvailable(screenName);
		}
#endif

		return hasSys;
	}

	void LocalScreenManager::lockSysNames() {
		Thread::mutexLock(&sysMutex);
	}

	void LocalScreenManager::unlockSysNames() {
		Thread::mutexUnlock(&sysMutex);
	}

	IWindow* LocalScreenManager::getIWindowFromId(
			GingaScreenID screenId, GingaWindowID winId) {

		IDeviceScreen* screen;
		IWindow* window = NULL;

		if (getScreen(screenId, &screen)) {
			window = screen->getIWindowFromId(winId);
		}

		return window;
	}

	bool LocalScreenManager::mergeIds(
			GingaScreenID screenId,
			GingaWindowID destId,
			vector<GingaWindowID>* srcIds) {

		IDeviceScreen* screen;

		if (getScreen(screenId, &screen)) {
			return screen->mergeIds(destId, srcIds);
		}

		return false;
	}

	void LocalScreenManager::blitScreen(
			GingaScreenID screenId, ISurface* destination) {

		IDeviceScreen* screen;

		if (getScreen(screenId, &screen)) {
			screen->blitScreen(destination);
		}
	}

	void LocalScreenManager::blitScreen(
			GingaScreenID screenId, string fileUri) {

		IDeviceScreen* screen;

		if (getScreen(screenId, &screen)) {
			screen->blitScreen(fileUri);
		}
	}

	void LocalScreenManager::refreshScreen(GingaScreenID screenId) {
		IDeviceScreen* screen;

		if (getScreen(screenId, &screen)) {
			screen->refreshScreen();
		}
	}


	/* interfacing output */

	GingaWindowID LocalScreenManager::createWindow(
			GingaScreenID screenId, int x, int y, int w, int h, float z) {

		IDeviceScreen* screen;
		IWindow* window = NULL;

		if (getScreen(screenId, &screen)) {
			window = screen->createWindow(x, y, w, h, z);

		} else {
			clog << "LocalScreenManager::createWindow Warning! ";
			clog << "can't find screen '" << screenId << "'" << endl;
		}

		return window->getId();
	}

	GingaWindowID LocalScreenManager::createUnderlyingSubWindow(
			GingaScreenID screenId, int x, int y, int w, int h, float z) {

		IDeviceScreen* screen;
		GingaWindowID window = NULL;

		if (getScreen(screenId, &screen)) {
			window = screen->createUnderlyingSubWindow(x, y, w, h, z);

		} else {
			clog << "LocalScreenManager::createWindow Warning! ";
			clog << "can't find screen '" << screenId << "'" << endl;
		}

		return window;
	}

	GingaWindowID LocalScreenManager::createWindowFrom(
			GingaScreenID screenId, GingaWindowID underlyingWindow) {

		IDeviceScreen* screen;
		IWindow* window = NULL;

		if (getScreen(screenId, &screen)) {
			window = screen->createWindowFrom(underlyingWindow);
		}

		return window->getId();
	}

	bool LocalScreenManager::hasWindow(
			GingaScreenID screenId, GingaWindowID winId) {

		IWindow *window;
		IDeviceScreen* screen;
		bool hasWin = false;

		window = getIWindowFromId (screenId, winId);
		if (getScreen(screenId, &screen)) {
			hasWin = screen->hasWindow(window);
		}

		return hasWin;
	}

	void LocalScreenManager::releaseWindow(
			GingaScreenID screenId, IWindow* win) {

		IDeviceScreen* screen;

		if (getScreen(screenId, &screen)) {
			screen->releaseWindow(win);
		}
	}

	GingaSurfaceID LocalScreenManager::createSurface(GingaScreenID screenId) {
		IDeviceScreen* screen;
		ISurface* surface = NULL;
		GingaSurfaceID surId = 0;

		if (getScreen(screenId, &screen)) {
			surId = surIdRefCounter++;
			surface = screen->createSurface();
			surface->setId(surId);

			Thread::mutexLock (&surMapMutex);
			surMap [surId] = surface;
			Thread::mutexUnlock (&surMapMutex);
		}

		return surId;
	}

	GingaSurfaceID LocalScreenManager::createSurface(
			GingaScreenID screenId, int w, int h) {

		IDeviceScreen* screen;
		ISurface* surface = NULL;
		GingaSurfaceID surId = 0;



		if (getScreen(screenId, &screen)) {
			surId = surIdRefCounter++;
			surface = screen->createSurface(w, h);
			surface->setId(surId);

			Thread::mutexLock(&surMapMutex);
			surMap [surId] = surface;
			Thread::mutexUnlock(&surMapMutex);
		}

		return surId;
	}

	GingaSurfaceID LocalScreenManager::createSurfaceFrom(
			GingaScreenID screenId, void* underlyingSurface) {

		IDeviceScreen* screen;
		ISurface* surface = NULL;
		GingaSurfaceID surId = 0;

		if (getScreen(screenId, &screen)) {
			surId = surIdRefCounter++;
			surface = screen->createSurfaceFrom(underlyingSurface);
			surface->setId(surId);

			Thread::mutexLock(&surMapMutex);
			surMap [surId] = surface;
			Thread::mutexUnlock(&surMapMutex);
		}

		return surId;
	}

	bool LocalScreenManager::hasSurface(
			const GingaScreenID &screenId, const GingaSurfaceID &surId) {

		ISurface *surface = NULL;
	    IDeviceScreen* screen = NULL;
		bool hasSur = false;

		surface = getISurfaceFromId(surId);
		if (surface != NULL)
		{
			if (getScreen(screenId, &screen)) {
				hasSur = screen->hasSurface(surface);
			}
		}

		return hasSur;
	}

	bool LocalScreenManager::releaseSurface(
			GingaScreenID screenId, ISurface* sur) {

		IDeviceScreen* screen;

		if (getScreen(screenId, &screen)) {
			return screen->releaseSurface(sur);
		}

		return false;
	}


	/* interfacing content */

	IContinuousMediaProvider* LocalScreenManager::createContinuousMediaProvider(
			GingaScreenID screenId,
			const char* mrl,
			bool* hasVisual,
			bool isRemote) {

		IDeviceScreen* screen;
		IContinuousMediaProvider* provider = NULL;

		if (getScreen(screenId, &screen)) {
			provider = screen->createContinuousMediaProvider(
					mrl, hasVisual, isRemote);
		}

		return provider;
	}

	void LocalScreenManager::releaseContinuousMediaProvider(
			GingaScreenID screenId, IContinuousMediaProvider* provider) {

		IDeviceScreen* screen;

		if (getScreen(screenId, &screen)) {
			screen->releaseContinuousMediaProvider(provider);
		}
	}

	IFontProvider* LocalScreenManager::createFontProvider(
			GingaScreenID screenId, const char* mrl, int fontSize) {

		IDeviceScreen* screen;
		IFontProvider* provider = NULL;

		if (getScreen(screenId, &screen)) {
			provider = screen->createFontProvider(mrl, fontSize);
		}

		return provider;
	}

	void LocalScreenManager::releaseFontProvider(
			GingaScreenID screenId, IFontProvider* provider) {

		IDeviceScreen* screen;

		if (getScreen(screenId, &screen)) {
			screen->releaseFontProvider(provider);
		}
	}

	IImageProvider* LocalScreenManager::createImageProvider(
			GingaScreenID screenId, const char* mrl) {

		IDeviceScreen* screen;
		IImageProvider* provider = NULL;

		if (getScreen(screenId, &screen)) {
			provider = screen->createImageProvider(mrl);
		}

		return provider;
	}

	void LocalScreenManager::releaseImageProvider(
			GingaScreenID screenId, IImageProvider* provider) {

		IDeviceScreen* screen;

		if (getScreen(screenId, &screen)) {
			screen->releaseImageProvider(provider);
		}
	}

	GingaSurfaceID LocalScreenManager::createRenderedSurfaceFromImageFile(
			GingaScreenID screenId, const char* mrl) {

		IDeviceScreen* screen;
		ISurface* uSur = NULL;
		GingaSurfaceID surId = 0;

		Thread::mutexLock(&surMapMutex);

		if (getScreen(screenId, &screen)) {
			surId = surIdRefCounter++;
			uSur = screen->createRenderedSurfaceFromImageFile(mrl);
			uSur->setId(surId);

			surMap [surId] = uSur;
		}

		Thread::mutexUnlock(&surMapMutex);

		return surId;
	}


	/* interfacing input */
	IInputManager* LocalScreenManager::getInputManager(GingaScreenID screenId) {
		IDeviceScreen* screen;
		IInputManager* iManager = NULL;

		if (getScreen(screenId, &screen)) {
			iManager = screen->getInputManager();
		}

		return iManager;
	}

	IEventBuffer* LocalScreenManager::createEventBuffer(GingaScreenID screenId){
		IDeviceScreen* screen;
		IEventBuffer* buffer = NULL;

		if (getScreen(screenId, &screen)) {
			buffer = screen->createEventBuffer();
		}

		return buffer;
	}

	IInputEvent* LocalScreenManager::createInputEvent(
			GingaScreenID screenId, void* event, const int symbol) {

		IDeviceScreen* screen;
		IInputEvent* iEvent = NULL;

		if (getScreen(screenId, &screen)) {
			iEvent = screen->createInputEvent(event, symbol);
		}

		return iEvent;
	}

	IInputEvent* LocalScreenManager::createApplicationEvent(
			GingaScreenID screenId, int type, void* data) {

		IDeviceScreen* screen;
		IInputEvent* iEvent = NULL;

		if (getScreen(screenId, &screen)) {
			iEvent = screen->createApplicationEvent(type, data);
		}

		return iEvent;
	}

	int LocalScreenManager::fromMBToGinga(GingaScreenID screenId, int keyCode) {
		IDeviceScreen* screen;
		int translated = CodeMap::KEY_NULL;

		if (getScreen(screenId, &screen)) {
			translated = screen->fromMBToGinga(keyCode);
		}

		return translated;
	}

	int LocalScreenManager::fromGingaToMB(GingaScreenID screenId, int keyCode) {
		IDeviceScreen* screen;
		int translated = CodeMap::KEY_NULL;

		if (getScreen(screenId, &screen)) {
			translated = screen->fromGingaToMB(keyCode);
		}

		return translated;
	}

	/* Methods created to isolate gingacc-mb */
void LocalScreenManager::addWindowCaps (
		const GingaScreenID &screenId, const GingaWindowID &winId, int caps)
	{
		IWindow *win = NULL;
		win = getIWindowFromId (screenId, winId);
		if (win != NULL)
			win->addCaps (caps);
	}

void LocalScreenManager::setWindowCaps (
		const GingaScreenID &screenId, const GingaWindowID &winId, int caps)
{
	IWindow *win = NULL;
	win = getIWindowFromId (screenId, winId);
	if (win != NULL)
		win->setCaps (caps);
}
int LocalScreenManager::getWindowCap (
		const GingaScreenID &screenId, const GingaWindowID &winId,
		const string &capName)
{
	int cap = 0;
	IWindow *win = NULL;
	win = getIWindowFromId (screenId, winId);
	if (win != NULL)
		cap = win->getCap (capName);

	return cap;
}

void LocalScreenManager::drawWindow (
		const GingaScreenID &screenId, const GingaWindowID &winId)
{
	IWindow *win = NULL;
	win = getIWindowFromId (screenId, winId);
	if (win != NULL)
		win->draw ();
}

void LocalScreenManager::setWindowBounds (
		const GingaScreenID &screenId, const GingaWindowID &winId, int x, int y,
		int w, int h)
{
	IWindow *win = NULL;
	win = getIWindowFromId (screenId, winId);
	if (win != NULL)
		win->setBounds(x, y, w, h);
}

void LocalScreenManager::showWindow (
		const GingaScreenID &screenId, const GingaWindowID &winId)
{
	IWindow *win = NULL;
	win = getIWindowFromId (screenId, winId);
	if (win != NULL)
		win->show ();
}

void LocalScreenManager::hideWindow (
		const GingaScreenID &screenId, const GingaWindowID &winId)
{
	IWindow *win = NULL;
	win = getIWindowFromId (screenId, winId);
	if (win != NULL)
		win->hide ();
}

void LocalScreenManager::raiseWindowToTop (
		const GingaScreenID &screenId, const GingaWindowID &winId)
{
	IWindow *win = NULL;
	win = getIWindowFromId (screenId, winId);
	if (win != NULL)
		win->raiseToTop();
}

void LocalScreenManager::renderWindowFrom (
		const GingaScreenID &screenId, const GingaWindowID &winId,
		const GingaSurfaceID &surId)
{
	IWindow *win = NULL;
	ISurface *surface = NULL;

	surface = getISurfaceFromId (surId);
	if (surface != NULL)
	{
		win = getIWindowFromId (screenId, winId);
		if (win != NULL)
		{
			win->renderFrom (surface);
		}
	}
}

void LocalScreenManager::setWindowBgColor (
		const GingaScreenID &screenId, const GingaWindowID &winId, int r, int g,
		int b, int alpha)
{
	IWindow *win = NULL;
	win = getIWindowFromId (screenId, winId);
	if (win != NULL)
		win->setBgColor (r, g, b, alpha);
}

void LocalScreenManager::setWindowBorder (
		const GingaScreenID &screenId, const GingaWindowID &winId, int r, int g,
		int b, int alpha, int width)
{
	IWindow *win = NULL;
	win = getIWindowFromId (screenId, winId);
	if (win != NULL)
		win->setBorder (r, g, b, alpha, width);
}

void LocalScreenManager::setWindowCurrentTransparency (
		const GingaScreenID &screenId, const GingaWindowID &winId, int transparency)
{
	IWindow *win = NULL;
	win = getIWindowFromId (screenId, winId);
	if (win != NULL)
		win->setCurrentTransparency (transparency);
}

void LocalScreenManager::setWindowColorKey (
		const GingaScreenID &screenId, const GingaWindowID &winId, int r, int g,
		int b)
{
	IWindow *win = NULL;
	win = getIWindowFromId (screenId, winId);
	if (win != NULL)
		win->setColorKey (r, g, b);
}

void LocalScreenManager::setWindowZ (
		const GingaScreenID &screenId, const GingaWindowID &winId, float z)
{
	IWindow *win = NULL;
	win = getIWindowFromId (screenId, winId);
	if (win != NULL)
		win->setZ (z);
}

void LocalScreenManager::disposeWindow (
		const GingaScreenID &screenId, const GingaWindowID &winId)
{
	IWindow *win = NULL;
	win = getIWindowFromId (screenId, winId);
	if (win != NULL)
		delete win;
}

void LocalScreenManager::setGhostWindow(
		const GingaScreenID &screenId, const GingaWindowID &winId, bool ghost)
{
	IWindow *win = NULL;
	win = getIWindowFromId (screenId, winId);
	if (win != NULL)
		win->setGhostWindow(ghost);
}

void LocalScreenManager::validateWindow (
		const GingaScreenID &screenId, const GingaWindowID &winId)
{
	IWindow *win = NULL;
	win = getIWindowFromId (screenId, winId);
	if (win != NULL)
		win->validate();
}

int LocalScreenManager::getWindowX (
		const GingaScreenID &screenId, const GingaWindowID &winId)
{
	int reply = 0;
	IWindow *win = NULL;
	win = getIWindowFromId (screenId, winId);
	if (win != NULL)
		reply = win->getX();

	return reply;
}

int LocalScreenManager::getWindowY (
		const GingaScreenID &screenId, const GingaWindowID &winId)
{
	int reply = 0;
	IWindow *win = NULL;
	win = getIWindowFromId (screenId, winId);
	if (win != NULL)
		reply = win->getY();

	return reply;
}

int LocalScreenManager::getWindowW (
		const GingaScreenID &screenId, const GingaWindowID &winId)
{
	int reply = 0;
	IWindow *win = NULL;
	win = getIWindowFromId (screenId, winId);
	if (win != NULL)
		reply = win->getW();

	return reply;
}

int LocalScreenManager::getWindowH (
		const GingaScreenID &screenId, const GingaWindowID &winId)
{
	int reply = 0;
	IWindow *win = NULL;
	win = getIWindowFromId (screenId, winId);
	if (win != NULL)
		reply = win->getH();

	return reply;
}

float LocalScreenManager::getWindowZ (
		const GingaScreenID &screenId, const GingaWindowID &winId)
{
	float reply = 0.0;
	IWindow *win = NULL;
	win = getIWindowFromId (screenId, winId);
	if (win != NULL)
		reply = win->getZ();

	return reply;
}

int LocalScreenManager::getWindowTransparencyValue(
		const GingaScreenID &screenId, const GingaWindowID &winId)
{
	int reply = 0;
	IWindow *win = NULL;
	win = getIWindowFromId (screenId, winId);
	if (win != NULL)
		reply = win->getTransparencyValue();

	return reply;
}

void LocalScreenManager::resizeWindow(
		const GingaScreenID &screenId, const GingaWindowID &winId, int width,
		int height)
{
	IWindow *win = NULL;
	win = getIWindowFromId (screenId, winId);
	if (win != NULL)
		win->resize(width, height);
}

string LocalScreenManager::getWindowDumpFileUri (
		const GingaScreenID &screenId, const GingaWindowID &winId, int quality,
		int dumpW, int dumpH)
{
	string reply = "";
	IWindow *win = NULL;
	win = getIWindowFromId (screenId, winId);
	if (win != NULL)
		reply = win->getDumpFileUri(quality, dumpW, dumpH);

	return reply;
}

void LocalScreenManager::clearWindowContent (
		const GingaScreenID &screenId, const GingaWindowID &winId)
{
	IWindow *win;
	win = getIWindowFromId (screenId, winId);
	if (win != NULL)
		win->clearContent();
}

 void LocalScreenManager::lowerWindowToBottom (
		 const GingaScreenID &screenId, const GingaWindowID &winId)
 {
		IWindow *win;
		win = getIWindowFromId (screenId, winId);
		if (win != NULL)
			win->lowerToBottom();
 }

 void LocalScreenManager::setWindowMirrorSrc (
			const GingaScreenID &screenId, const GingaWindowID &winId,
			const GingaWindowID &mirrorSrc)
 {
		IWindow *win;
		IWindow *mirrorSrcWin;

		win = getIWindowFromId (screenId, winId);
		mirrorSrcWin = getIWindowFromId (screenId, mirrorSrc);

		if (win != NULL && mirrorSrcWin != NULL)
			win->setMirrorSrc(mirrorSrcWin);
 }

 void* LocalScreenManager::getSurfaceContent(const GingaSurfaceID &surId)
 {
	 ISurface *surface = NULL;
	 void *surfaceContent = NULL;

	 surface = getISurfaceFromId(surId);
	 if (surface != NULL)
		 surfaceContent = surface->getSurfaceContent();

	 return surfaceContent;
 }

 GingaWindowID LocalScreenManager::getSurfaceParentWindow(
			const GingaSurfaceID& surId)
 {
	 ISurface *surface = NULL;
	 GingaWindowID parentWindow = 0;

	 surface = getISurfaceFromId(surId);
	 if (surface != NULL)
	 {
		 IWindow *win = (IWindow *) surface->getParentWindow();
		 if (win != NULL)
			 parentWindow = win->getId();
	 }

	 return parentWindow;
 }

 void LocalScreenManager::deleteSurface (const GingaSurfaceID &surId)
 {
	 IDeviceScreen* screen = NULL;
	 ISurface *surface = NULL;

	 surface = getISurfaceFromId(surId);
	 if (surface)
	 {
			Thread::mutexLock (&surMapMutex);
			surMap.erase(surId);
			delete surface;

			Thread::mutexUnlock (&surMapMutex);
	 }
 }

 bool LocalScreenManager::setSurfaceParentWindow (
		 const GingaScreenID &screenId,  const GingaSurfaceID &surId,
		 const GingaWindowID &winId)
 {
	 bool reply = false;
	 ISurface *surface = NULL;
	 IWindow *window = NULL;

	 surface = getISurfaceFromId(surId);
	 if (surface != NULL)
	 {
		 window = getIWindowFromId(screenId, winId);
		 if (window != NULL)
			 reply = surface->setParentWindow(window);
	 }

	 return reply;
 }

 void LocalScreenManager::clearSurfaceContent (const GingaSurfaceID &surId)
 {
	 ISurface *surface = NULL;

	 surface = getISurfaceFromId(surId);
	 if (surface != NULL)
		 surface->clearContent();
 }

 void LocalScreenManager::getSurfaceSize(const GingaSurfaceID &surId,
                                         int* width, int* height)
 {
	 ISurface *surface = NULL;
	 surface = getISurfaceFromId (surId);
	 if (surface != NULL)
		 surface->getSize(width, height);
 }

 void LocalScreenManager::addSurfaceCaps(const GingaSurfaceID &surId,
                                         const int caps)
 {
	 ISurface* surface = NULL;

	 surface = getISurfaceFromId (surId);
	 if (surface != NULL)
		 surface->addCaps(caps);
 }

 void LocalScreenManager::setSurfaceCaps(const GingaSurfaceID &surId,
                                         const int caps)
 {
	 ISurface* surface = NULL;

	 surface = getISurfaceFromId (surId);
	 if (surface != NULL)
		 surface->setCaps(caps);
 }

 int LocalScreenManager::getSurfaceCap(const GingaSurfaceID &surId,
                                        const string &cap)
 {
	 ISurface* surface = NULL;
	 int value = 0;

	 surface = getISurfaceFromId (surId);
	 if (surface != NULL)
		 value = surface->getCap(cap);

	 return value;
 }

 int LocalScreenManager::getSurfaceCaps(const GingaSurfaceID &surId)
 {
	 ISurface* surface = NULL;
	 int value = 0;

	 surface = getISurfaceFromId (surId);
	 if (surface != NULL)
		 value = surface->getCaps();

	 return value;
 }

 void LocalScreenManager::setSurfaceBgColor(
			const GingaSurfaceID &surId, int r, int g, int b, int alpha)
 {
	 ISurface *surface = NULL;

	 surface = getISurfaceFromId (surId);
	 if (surface != NULL)
		 surface->setBgColor(r, g, b, alpha);
 }

 void LocalScreenManager::setSurfaceFont(
		 const GingaSurfaceID &surId, void* font)
 {
	 ISurface *surface = NULL;

	 surface = getISurfaceFromId (surId);
	 if (surface != NULL)
		 surface->setSurfaceFont(font);
 }

 void LocalScreenManager::setColor(
		 const GingaSurfaceID &surId, int r, int g, int b, int alpha)
 {
	 ISurface *surface = NULL;

	 surface = getISurfaceFromId (surId);
	 if (surface != NULL)
		 surface->setColor (r, g, b, alpha);
 }

 void LocalScreenManager::setExternalHandler(
		 const GingaSurfaceID &surId, bool extHandler)
 {
	 ISurface *surface = NULL;

	 surface = getISurfaceFromId (surId);
	 if (surface != NULL)
		 surface->setExternalHandler(extHandler);
 }

 void LocalScreenManager::blitSurface (
			const GingaSurfaceID &surId,int x, int y, GingaSurfaceID src,
			int srcX, int srcY, int srcW, int srcH)
 {
	 ISurface *surface = NULL;
	 ISurface *surfaceSrc = NULL;

	 surface = getISurfaceFromId (surId);
	 surfaceSrc = getISurfaceFromId (src);
	 if (surface != NULL)
	 {
		 surface->blit(x, y, surfaceSrc, srcX, srcY, srcW, srcH);
	 }
 }

 void LocalScreenManager::flipSurface (const GingaSurfaceID &surId)
 {
	 ISurface *surface = NULL;

	 surface = getISurfaceFromId (surId);
	 if (surface != NULL)
		 surface->flip();
 }

 void LocalScreenManager::setSurfaceContent(const GingaSurfaceID &surId,
                                            void* surface)
 {
	 ISurface *sur = NULL;

	 sur = getISurfaceFromId (surId);
	 if (sur != NULL)
		 sur->setSurfaceContent(surface);
 }

 IColor* LocalScreenManager::getSurfaceColor(const GingaSurfaceID &surId)
 {
	 ISurface* surface = NULL;
	 IColor* color = NULL;

	 surface = getISurfaceFromId (surId);
	 if (surface != NULL)
		 color = surface->getColor();

	 return color;
 }

 bool LocalScreenManager::hasSurfaceExternalHandler(const GingaSurfaceID &surId)
 {
	 ISurface *surface = NULL;
	 bool reply = false;

	 surface = getISurfaceFromId (surId);
	 if (surface != NULL)
		 reply = surface->hasExternalHandler();

	 return reply;
 }

 ISurface* LocalScreenManager::getISurfaceFromId(const GingaSurfaceID &surId)
 {
		map<GingaSurfaceID, ISurface*>::iterator i;
		ISurface* iSur = NULL;

		Thread::mutexLock (&surMapMutex);
		i = surMap.find (surId);
		if (i != surMap.end ())
		{
			iSur = i->second;
		}
		Thread::mutexUnlock (&surMapMutex);

		return iSur;
 }


	/* private functions */

	void LocalScreenManager::addScreen(
			GingaScreenID screenId, IDeviceScreen* screen) {

		lockScreenMap();
		if (screen != NULL) {
			screens[screenId] = screen;

		} else {
			clog << "LocalScreenManager::addScreen Warning! Trying to add ";
			clog << "a NULL screen" << endl;
		}
		unlockScreenMap();
	}

	short LocalScreenManager::getNumOfScreens() {
		short numOfScreens;

		lockScreenMap();
		numOfScreens = screens.size();
		unlockScreenMap();

		return numOfScreens;
	}

	bool LocalScreenManager::getScreen(
			GingaScreenID screenId, IDeviceScreen** screen) {

		bool hasScreen = false;
		map<GingaScreenID, IDeviceScreen*>::iterator i;

		lockScreenMap();
		i = screens.find(screenId);
		if (i != screens.end()) {
			hasScreen = true;
			*screen   = i->second;
		}
		unlockScreenMap();

		return hasScreen;
	}

	bool LocalScreenManager::removeScreen(GingaScreenID screenId) {
		bool hasScreen = false;
		map<GingaScreenID, IDeviceScreen*>::iterator i;

		lockScreenMap();
		i = screens.find(screenId);
		if (i != screens.end()) {
			screens.erase(i);
		}
		unlockScreenMap();

		return hasScreen;
	}

	void LocalScreenManager::lockScreenMap() {
		Thread::mutexLock(&mapMutex);
	}

	void LocalScreenManager::unlockScreenMap() {
		Thread::mutexUnlock(&mapMutex);
	}

	void LocalScreenManager::lock() {
		Thread::mutexLock(&genMutex);
	}

	void LocalScreenManager::unlock() {
		Thread::mutexUnlock(&genMutex);
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
