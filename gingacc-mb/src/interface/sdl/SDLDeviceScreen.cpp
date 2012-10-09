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

#include "config.h"

#include "mb/interface/sdl/SDLDeviceScreen.h"

#include "mb/interface/sdl/output/SDLWindow.h"
#include "mb/interface/sdl/output/SDLSurface.h"

#include "mb/interface/sdl/input/SDLEventBuffer.h"
#include "mb/interface/sdl/input/SDLInputEvent.h"

#include "mb/ILocalScreenManager.h"
#include "mb/InputManager.h"

extern "C" {
#include "SDL_endian.h"
#include "SDL_syswm.h"
#include <string.h>
#include <stdlib.h>
}

#if !HAVE_COMPSUPPORT
#include "mb/interface/sdl/content/audio/SDLAudioProvider.h"
#include "mb/interface/sdl/content/image/SDLImageProvider.h"
#include "mb/interface/sdl/content/text/SDLFontProvider.h"
#include "mb/interface/sdl/content/video/SDLVideoProvider.h"
#endif

#if defined(SDL_VIDEO_DRIVER_X11)
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#endif

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
#if HAVE_COMPSUPPORT
	IComponentManager* SDLDeviceScreen::cm = IComponentManager::getCMInstance();
#endif

	map<SDLDeviceScreen*, short> SDLDeviceScreen::sdlScreens;
	bool SDLDeviceScreen::hasRenderer = false;
	bool SDLDeviceScreen::hasERC      = false;
	bool SDLDeviceScreen::mutexInit   = false;

	map<int, int> SDLDeviceScreen::gingaToSDLCodeMap;
	map<int, int> SDLDeviceScreen::sdlToGingaCodeMap;
	set<SDL_Texture*> SDLDeviceScreen::uTexPool;
	set<SDL_Surface*> SDLDeviceScreen::uSurPool;
	vector<ReleaseContainer*> SDLDeviceScreen::releaseList;
	map<GingaScreenID, map<float, set<IWindow*>*>*> SDLDeviceScreen::renderMap;
	set<IContinuousMediaProvider*> SDLDeviceScreen::cmpRenderList;

	const unsigned int SDLDeviceScreen::DSA_UNKNOWN = 0;
	const unsigned int SDLDeviceScreen::DSA_4x3     = 1;
	const unsigned int SDLDeviceScreen::DSA_16x9    = 2;

	pthread_mutex_t SDLDeviceScreen::sdlMutex;
	pthread_mutex_t SDLDeviceScreen::sieMutex;
	pthread_mutex_t SDLDeviceScreen::renMutex;
	pthread_mutex_t SDLDeviceScreen::scrMutex;
	pthread_mutex_t SDLDeviceScreen::recMutex;
	pthread_mutex_t SDLDeviceScreen::winMutex;
	pthread_mutex_t SDLDeviceScreen::surMutex;
	pthread_mutex_t SDLDeviceScreen::proMutex;
	pthread_mutex_t SDLDeviceScreen::cstMutex;

	SDLDeviceScreen::SDLDeviceScreen(
			int argc, char** args,
			GingaScreenID myId, GingaWindowID embedId,
			bool externalRenderer) {

		string parentCoords = "";
		int i;

		aSystem         = "";
		aspect          = DSA_UNKNOWN;
		hSize           = 0;
		vSize           = 0;
		hRes            = 0;
		wRes            = 0;
		im              = NULL;
		id              = myId;
		uParentId       = NULL;
		uEmbedId        = embedId;
		uEmbedFocused   = false;
		mustGainFocus   = false;
		renderer        = NULL;
		mbMode          = "";
		mbSubSystem     = "";
		screen          = NULL;
		sdlId           = 0;
		backgroundLayer = NULL;
		fullScreen      = false;

		if (externalRenderer) {
			hasERC = externalRenderer;
		}

		for (i = 0; i < argc; i++) {
			if ((strcmp(args[i], "subsystem") == 0) && ((i + 1) < argc)) {
				mbSubSystem.assign(args[i + 1]);

			} else if ((strcmp(args[i], "parent") == 0) && ((i + 1) < argc)) {
				parentCoords.assign(args[i + 1]);

			} else if ((strcmp(args[i], "mode") == 0) && ((i + 1) < argc)) {
				mbMode.assign(args[i + 1]);
				if (mbMode == "fullscreen") {
					fullScreen = true;
				}

			} else if ((strcmp(args[i], "audio") == 0) && ((i + 1) < argc)) {
				aSystem.assign(args[i + 1]);
			}
		}

		if (aSystem != "" && aSystem != "sdlffmpeg") {
			clog << "SDLDeviceScreen::SDLDeviceScreen Warning! Audio ";
			clog << "system not supported: '" << aSystem << "'! Using ";
			clog << "SDL2_ffmpeg instead." << endl;
		}
		aSystem = "SDLAudioProvider";

		if (mbSubSystem == "dfb") {
			mbSubSystem = "directfb";
		}

		if (uEmbedId == NULL) {
			setEmbedFromParent(parentCoords);
		}

		waitingCreator = false;
		Thread::mutexInit(&condMutex);
		pthread_cond_init(&cond, NULL);

		checkMutexInit();

		if (!hasRenderer) {
			hasRenderer = true;

			if (!hasERC) {
				setInitScreenFlag();

				pthread_t tId;
				pthread_attr_t tattr;

				pthread_attr_init(&tattr);
				pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED);
				pthread_attr_setscope(&tattr, PTHREAD_SCOPE_SYSTEM);

				pthread_create(&tId, &tattr, SDLDeviceScreen::rendererT, this);
				pthread_detach(tId);

			} else {
				setInitScreenFlag();
				rendererT(this);
			}

		} else {
			setInitScreenFlag();

			if (hasERC) {
				rendererT(this);
			}
		}
	}

	SDLDeviceScreen::~SDLDeviceScreen() {
		map<SDLDeviceScreen*, short>::iterator i;
		map<GingaScreenID, map<float, set<IWindow*>*>*>::iterator j;
		map<float, set<IWindow*>*>::iterator k;

		waitingCreator = false;
		pthread_mutex_destroy(&condMutex);
		pthread_cond_destroy(&cond);

		Thread::mutexLock(&renMutex);
		j = renderMap.find(id);
		if (j != renderMap.end()) {
			k = j->second->begin();
			while (k != j->second->end()) {
				delete k->second;
				++k;
			}
			delete j->second;
			renderMap.erase(j);
		}
		Thread::mutexUnlock(&renMutex);

		if (im != NULL) {
			delete im;
			im = NULL;
		}

		releaseScreen();

		while (this->renderer != NULL) {
			SystemCompat::uSleep(10000);
		}

		Thread::mutexLock(&scrMutex);
		i = sdlScreens.find(this);
		if (i != sdlScreens.end()) {
			sdlScreens.erase(i);
		}

		if (sdlScreens.empty()) {
			hasRenderer = false;
			sdlQuit();
		}
		Thread::mutexUnlock(&scrMutex);

		clog << "SDLDeviceScreen::~SDLDeviceScreen all done" << endl;
	}

	void SDLDeviceScreen::checkMutexInit() {
		if (!mutexInit) {
			mutexInit = true;

			Thread::mutexInit(&sdlMutex, true);
			Thread::mutexInit(&sieMutex, true);
			Thread::mutexInit(&renMutex, true);
			Thread::mutexInit(&scrMutex, true);
			Thread::mutexInit(&recMutex, true);
			Thread::mutexInit(&winMutex, true);
			Thread::mutexInit(&surMutex, true);
			Thread::mutexInit(&proMutex, true);
			Thread::mutexInit(&cstMutex, true);
		}
	}

	void SDLDeviceScreen::lockSDL() {
		checkMutexInit();

		Thread::mutexLock(&sdlMutex);
	}

	void SDLDeviceScreen::unlockSDL() {
		checkMutexInit();

		Thread::mutexUnlock(&sdlMutex);
	}

	void SDLDeviceScreen::updateRenderMap(
			GingaScreenID screenId, IWindow* window,
			float oldZIndex, float newZIndex) {

		checkMutexInit();

		renderMapRemoveWindow(screenId, window, oldZIndex);
		renderMapInsertWindow(screenId, window, newZIndex);
	}

	void SDLDeviceScreen::releaseScreen() {
		Thread::mutexLock(&scrMutex);
		sdlScreens[this] = SPT_RELEASE;
		Thread::mutexUnlock(&scrMutex);
	}

	void SDLDeviceScreen::releaseMB() {
		int errCount = 0;
		int numSDL;

		Thread::mutexLock(&scrMutex);
		numSDL = sdlScreens.size();

		while (numSDL > 1) {
			SystemCompat::uSleep(100000);
			errCount++;

			numSDL = sdlScreens.size();

			if (errCount > 5 || numSDL <= 1) {
				break;
			}
		}
		Thread::mutexUnlock(&scrMutex);

		hasRenderer = false;
		Thread::mutexLock(&proMutex);
		cmpRenderList.clear();
		Thread::mutexUnlock(&proMutex);
	}

	void SDLDeviceScreen::clearWidgetPools() {
		Thread::mutexLock(&scrMutex);
		sdlScreens[this] = SPT_CLEAR;
		Thread::mutexUnlock(&scrMutex);
	}

	string SDLDeviceScreen::getScreenName() {
		return "sdl";
	}

	void SDLDeviceScreen::setEmbedFromParent(string parentCoords) {
		vector<string>* params;
		string spec;
		int uEmbedX, uEmbedY, uEmbedW, uEmbedH;

		clog << "SDLDeviceScreen::setEmbedFromParent: '";
		clog << parentCoords << "'";
		clog << endl;

		if (parentCoords == "") {
			return;
		}

		params = split(parentCoords, ",");
		if (params->size() == 6) {
			spec       = (*params)[0];
			uParentId  = (void*)strtoul((*params)[1].c_str(), NULL, 10);
			uEmbedX    = util::stof((*params)[2]);
			uEmbedY    = util::stof((*params)[3]);
			uEmbedW    = util::stof((*params)[4]);
			uEmbedH    = util::stof((*params)[5]);
			uEmbedId   = createUnderlyingSubWindow(
					uParentId, spec, uEmbedX, uEmbedY, uEmbedW, uEmbedH, 1.0);
		}

		delete params;
	}

	void SDLDeviceScreen::setBackgroundImage(string uri) {
		lockSDL();

		if (backgroundLayer == NULL) {
			backgroundLayer = createWindow(0, 0, wRes, hRes, 0.0);
		}

		backgroundLayer->lock();
		backgroundLayer->renderImgFile(uri);
		backgroundLayer->show();
		backgroundLayer->unlock();

		unlockSDL();
	}

	unsigned int SDLDeviceScreen::getWidthResolution() {
		/*
		 * wRes == 0 is an initial state. So pthread_cond_t
		 * is not necessary here.
		 */
		while (wRes <= 0) {
			SystemCompat::uSleep(uSleepTime);
		}

		clog << "SDLDeviceScreen::getWidthResolution returns '";
		clog << wRes << "'";
		clog << endl;

		return wRes;
	}

	void SDLDeviceScreen::setWidthResolution(unsigned int wRes) {
		this->wRes = wRes;

		lockSDL();
		if (screen != NULL) {
			SDL_SetWindowSize(screen, this->wRes, this->hRes);
		}
		unlockSDL();

		clog << "SDLDeviceScreen::setWidthResolution to '";
		clog << wRes << "'";
		clog << endl;
	}

	unsigned int SDLDeviceScreen::getHeightResolution() {
		/*
		 * hRes == 0 is an initial state. So pthread_cond_t
		 * is not necessary here.
		 */
		while (hRes <= 0) {
			SystemCompat::uSleep(uSleepTime);
		}

		clog << "SDLDeviceScreen::getHeightResolution returns '";
		clog << hRes << "'";
		clog << endl;

		return hRes;
	}

	void SDLDeviceScreen::setHeightResolution(unsigned int hRes) {
		this->hRes = hRes;

		lockSDL();
		if (screen != NULL) {
			SDL_SetWindowSize(screen, this->wRes, this->hRes);
		}
		unlockSDL();

		clog << "SDLDeviceScreen::setHeightResolution to '";
		clog << hRes << "'";
		clog << endl;
	}

	void SDLDeviceScreen::setColorKey(int r, int g, int b) {

	}

	void SDLDeviceScreen::mergeIds(
			GingaWindowID destId, vector<GingaWindowID>* srcIds) {

	}

	void SDLDeviceScreen::blitScreen(ISurface* destination) {
		SDL_Surface* dest;

		lockSDL();
		dest = (SDL_Surface*)(destination->getSurfaceContent());
		if (dest == NULL) {
			dest = createUnderlyingSurface(wRes, hRes);
			destination->setSurfaceContent(dest);
		}

		blitScreen(dest);

		unlockSDL();
	}

	void SDLDeviceScreen::blitScreen(string fileUri) {
		SDL_Surface* dest;

		lockSDL();
		dest = createUnderlyingSurface(wRes, hRes);
		blitScreen(dest);

		if (SDL_SaveBMP_RW(dest, SDL_RWFromFile(fileUri.c_str(), "wb"), 1) < 0) {
			clog << "SDLDeviceScreen::blitScreen SDL error: '";
			clog << SDL_GetError() << "'" << endl;
		}
		unlockSDL();
	}

	void SDLDeviceScreen::blitScreen(SDL_Surface* dest) {
		map<GingaScreenID, map<float, set<IWindow*>*>*>::iterator i;
		map<float, set<IWindow*>*>::iterator j;
		set<IWindow*>::iterator k;

		Thread::mutexLock(&renMutex);
		i = renderMap.find(id);
		if (i != renderMap.end()) {
			j = i->second->begin();
			while (j != i->second->end()) {
				k = j->second->begin();
				while (k != j->second->end()) {
					blitFromWindow((*k), dest);
					++k;
				}
				++j;
			}
		}
		Thread::mutexUnlock(&renMutex);
	}

	void SDLDeviceScreen::setInitScreenFlag() {
		Thread::mutexLock(&scrMutex);
		sdlScreens[this] = SPT_INIT;
		Thread::mutexUnlock(&scrMutex);
	}

	void SDLDeviceScreen::refreshScreen() {
		if (hasERC) {
			rendererT(NULL);
		}
	}


	/* interfacing output */

	IWindow* SDLDeviceScreen::createWindow(
			int x, int y, int w, int h, float z) {

		IWindow* iWin;

		Thread::mutexLock(&winMutex);

		iWin = new SDLWindow(NULL, NULL, id, x, y, w, h, z);
		windowPool.insert(iWin);
		renderMapInsertWindow(id, iWin, z);

		Thread::mutexUnlock(&winMutex);

		return iWin;
	}

	GingaWindowID SDLDeviceScreen::createUnderlyingSubWindow(
			int x, int y, int w, int h, float z) {

		GingaWindowID uWin   = NULL;
		GingaWindowID parent = NULL;

		lockSDL();
		parent = getScreenUnderlyingWindow();

		uWin = createUnderlyingSubWindow(parent, "", x, y, w, h, z);
		if (uWin != NULL) {
			initEmbed(this, uWin);
			forceInputFocus(this, uWin);
		}

		unlockSDL();

		return uWin;
	}

	GingaWindowID SDLDeviceScreen::createUnderlyingSubWindow(
			GingaWindowID parent,
			string spec,
			int x, int y, int w, int h, float z) {

		GingaWindowID uWin = NULL;

#if defined(SDL_VIDEO_DRIVER_X11)
		Display* xDisplay;
		int xScreen;
		int blackColor;

		if (spec == "") {
			xDisplay = XOpenDisplay(getenv("DISPLAY"));

		} else {
			xDisplay = XOpenDisplay(spec.c_str());
		}

		if (parent == NULL) {
			parent = (GingaWindowID)XDefaultRootWindow(xDisplay);
		}

		xScreen    = DefaultScreen(xDisplay);
		blackColor = BlackPixel(xDisplay, xScreen);

		uWin       = (GingaWindowID)XCreateSimpleWindow(
				xDisplay,               /* display */
				(Window)parent,         /* parent */
				x,                      /* x */
				y,                      /* y */
				w,                      /* w */
				h,                      /* h */
				0,                      /* border_width */
				blackColor,             /* border_color */
				blackColor);            /* background_color */

		XSync(xDisplay, 0);

		if (uWin == NULL) {
			clog << "SDLDeviceScreen::createUnderlyingSubWindow Warning! ";
			clog << "Can't create child window" << endl;
			return NULL;
		}

		XMapWindow(xDisplay, (Window)uWin);
		XClearWindow(xDisplay, (Window)uWin);
		XFlush(xDisplay);

		if (!hasRenderer) {
			XInitThreads();
		}

		clog << "SDLDeviceScreen::createUnderlyingSubWindow embed id created '";
		clog << (void*)uWin << "'";
		clog << endl;

		XSync(xDisplay, 1);

#elif defined(SDL_VIDEO_DRIVER_WINDOWS)
		//TODO: Windows - create a child window from parent window id

#elif defined(SDL_VIDEO_DRIVER_COCOA)
		//TODO: Cocoa - create a child window from parent window id
#endif

		return uWin;
	}

	GingaWindowID SDLDeviceScreen::getScreenUnderlyingWindow() {
		GingaWindowID sUWin = NULL;
		SDL_SysWMinfo info;

		lockSDL();
		if (uEmbedId != NULL) {
			sUWin = uEmbedId;

		} else {
			SDL_VERSION(&info.version);
			SDL_GetWindowWMInfo(screen, &info);

#if defined(SDL_VIDEO_DRIVER_X11)
			sUWin = (GingaWindowID)info.info.x11.window;

#elif defined(SDL_VIDEO_DRIVER_WINDOWS)
			//TODO: Windows input event configuration

#elif defined(SDL_VIDEO_DRIVER_COCOA)
			//TODO: Cocoa input event configuration
#endif
		}

		unlockSDL();

		return sUWin;
	}

	IWindow* SDLDeviceScreen::createWindowFrom(GingaWindowID underlyingWindow) {
		IWindow* iWin = NULL;

		Thread::mutexLock(&winMutex);

		if (underlyingWindow != NULL) {
			iWin = new SDLWindow(underlyingWindow, NULL, id, 0, 0, 0, 0, 0);
			windowPool.insert(iWin);
			renderMapInsertWindow(id, iWin, 2.0);
		}

		Thread::mutexUnlock(&winMutex);

		return iWin;
	}

	bool SDLDeviceScreen::hasWindow(IWindow* win) {
		set<IWindow*>::iterator i;
		bool hasWin = false;

		Thread::mutexLock(&winMutex);

		i = windowPool.find(win);
		if (i != windowPool.end()) {
			hasWin = true;
		}

		Thread::mutexUnlock(&winMutex);

		return hasWin;
	}

	void SDLDeviceScreen::releaseWindow(IWindow* win) {
		set<IWindow*>::iterator i;
		SDLWindow* iWin;
		SDL_Texture* uTex = NULL;
		bool uTexOwn;

		Thread::mutexLock(&winMutex);
		i = windowPool.find(win);
		if (i != windowPool.end()) {
			iWin = (SDLWindow*)(*i);

			renderMapRemoveWindow(id, iWin, iWin->getZ());
			windowPool.erase(i);

			uTex = iWin->getTexture(NULL);
			uTexOwn = iWin->isTextureOwner(uTex);

			iWin->clearContent();
			iWin->setTexture(NULL);

			Thread::mutexUnlock(&winMutex);

			if (uTexOwn) {
				createReleaseContainer(NULL, uTex, NULL);
			}

		} else {
			Thread::mutexUnlock(&winMutex);
		}
	}

	ISurface* SDLDeviceScreen::createSurface() {
		return createSurfaceFrom(NULL);
	}

	ISurface* SDLDeviceScreen::createSurface(int w, int h) {
		ISurface* iSur    = NULL;
		SDL_Surface* uSur = NULL;

		lockSDL();

		uSur = createUnderlyingSurface(w, h);

		iSur = new SDLSurface(id, uSur);

		unlockSDL();

		Thread::mutexLock(&surMutex);
		surfacePool.insert(iSur);
		Thread::mutexUnlock(&surMutex);

		return iSur;
	}

	ISurface* SDLDeviceScreen::createSurfaceFrom(void* uSur) {
		ISurface* iSur = NULL;

		lockSDL();
		if (uSur != NULL) {
			iSur = new SDLSurface(id, uSur);

		} else {
			iSur = new SDLSurface(id);
		}
		unlockSDL();

		Thread::mutexLock(&surMutex);
		surfacePool.insert(iSur);
		Thread::mutexUnlock(&surMutex);

		return iSur;
	}

	bool SDLDeviceScreen::hasSurface(ISurface* s) {
		set<ISurface*>::iterator i;
		bool hasSur = false;

		Thread::mutexLock(&surMutex);
		i = surfacePool.find(s);
		if (i != surfacePool.end()) {
			hasSur = true;
		}
		Thread::mutexUnlock(&surMutex);

		return hasSur;
	}

	bool SDLDeviceScreen::releaseSurface(ISurface* s) {
		set<ISurface*>::iterator i;
		SDL_Surface* uSur = NULL;
		bool released = false;

		Thread::mutexLock(&surMutex);
		i = surfacePool.find(s);
		if (i != surfacePool.end()) {
			surfacePool.erase(i);
			released = true;
		}
		Thread::mutexUnlock(&surMutex);

		return released;
	}


	/* interfacing content */
	IContinuousMediaProvider* SDLDeviceScreen::createContinuousMediaProvider(
			const char* mrl, bool* hasVisual, bool isRemote) {

		IContinuousMediaProvider* provider;
		string strSym;
		bool providerHasVisual;

		lockSDL();
		if (*hasVisual) {
			strSym = "SDLVideoProvider";

		} else {
			strSym = aSystem;
		}

#if HAVE_COMPSUPPORT
		provider = ((CMPCreator*)(cm->getObject(strSym)))(id, mrl);
		provider->setLoadSymbol(strSym);
#else
		if (*hasVisual) {
			provider = new SDLVideoProvider(id, mrl);

		} else {
			provider = new SDLAudioProvider(id, mrl);
		}
#endif

		providerHasVisual = provider->getHasVisual();
		if (*hasVisual != providerHasVisual) {
			*hasVisual = providerHasVisual;

			clog << "SDLDeviceScreen::createContinuousMediaProvider ";
			clog << "mime has visual = '" << *hasVisual << "' and ";
			clog << "content has visual = '" << providerHasVisual << "'! ";
			clog << "Trying to recreate provider with new data";
			clog << endl;
			delete provider;

			provider = createContinuousMediaProvider(mrl, hasVisual, isRemote);
		}

		unlockSDL();

		Thread::mutexLock(&proMutex);
		cmpPool.insert(provider);
		Thread::mutexUnlock(&proMutex);

		return provider;
	}

	void SDLDeviceScreen::releaseContinuousMediaProvider(
			IContinuousMediaProvider* provider) {

		set<IContinuousMediaProvider*>::iterator i;
		IContinuousMediaProvider* cmp;

		Thread::mutexLock(&proMutex);
		i = cmpPool.find(provider);
		if (i != cmpPool.end()) {
			cmp = (*i);
			cmpPool.erase(i);
			cmp->stop();

			Thread::mutexUnlock(&proMutex);
			createReleaseContainer(NULL, NULL, cmp);

		} else {
			Thread::mutexUnlock(&proMutex);
		}
	}

	IFontProvider* SDLDeviceScreen::createFontProvider(
			const char* mrl, int fontSize) {

		IFontProvider* provider = NULL;

		lockSDL();

#if HAVE_COMPSUPPORT
		provider = ((FontProviderCreator*)(cm->getObject("SDLFontProvider")))(
				id, mrl, fontSize);

#else
		provider = new SDLFontProvider(id, mrl, fontSize);
#endif

		unlockSDL();

		Thread::mutexLock(&proMutex);
		dmpPool.insert(provider);
		Thread::mutexUnlock(&proMutex);

		return provider;
	}

	void SDLDeviceScreen::releaseFontProvider(IFontProvider* provider) {
		set<IDiscreteMediaProvider*>::iterator i;
		IDiscreteMediaProvider* dmp;

		Thread::mutexLock(&proMutex);
		i = dmpPool.find(provider);
		if (i != dmpPool.end()) {
			dmp = (*i);
			dmpPool.erase(i);

			Thread::mutexUnlock(&proMutex);
			createReleaseContainer(NULL, NULL, dmp);

		} else {
			Thread::mutexUnlock(&proMutex);
		}
	}

	IImageProvider* SDLDeviceScreen::createImageProvider(const char* mrl) {
		IImageProvider* provider = NULL;

		lockSDL();

#if HAVE_COMPSUPPORT
		provider = ((ImageProviderCreator*)(cm->getObject(
				"SDLImageProvider")))(id, mrl);
#else
		provider = new SDLImageProvider(id, mrl);
#endif

		unlockSDL();

		Thread::mutexLock(&proMutex);
		dmpPool.insert(provider);
		Thread::mutexUnlock(&proMutex);

		return provider;
	}

	void SDLDeviceScreen::releaseImageProvider(IImageProvider* provider) {
		set<IDiscreteMediaProvider*>::iterator i;
		IDiscreteMediaProvider* dmp;

		Thread::mutexLock(&proMutex);
		i = dmpPool.find(provider);
		if (i != dmpPool.end()) {
			dmp = (*i);
			dmpPool.erase(i);

			Thread::mutexUnlock(&proMutex);
			createReleaseContainer(NULL, NULL, dmp);

		} else {
			Thread::mutexUnlock(&proMutex);
		}
	}

	ISurface* SDLDeviceScreen::createRenderedSurfaceFromImageFile(
			const char* mrl) {

		ISurface* iSur           = NULL;
		IImageProvider* provider = NULL;

		if (fileExists(mrl)) {
			provider = createImageProvider(mrl);
			if (provider != NULL) {
				iSur = createSurfaceFrom(NULL);
				provider->playOver(iSur);

				releaseImageProvider(provider);
			}

		} else {
			clog << "SDLDeviceScreen::createRenderedSurfaceFromImageFile ";
			clog << "Warning! '" << mrl << "' file not found" << endl;
		}

		return iSur;
	}

	void SDLDeviceScreen::addCMPToRendererList(IContinuousMediaProvider* cmp) {
		checkMutexInit();

		Thread::mutexLock(&proMutex);
		cmpRenderList.insert(cmp);
		Thread::mutexUnlock(&proMutex);
	}

	void SDLDeviceScreen::removeCMPToRendererList(
			IContinuousMediaProvider* cmp) {

		set<IContinuousMediaProvider*>::iterator i;

		checkMutexInit();

		Thread::mutexLock(&proMutex);
		i = cmpRenderList.find(cmp);
		if (i != cmpRenderList.end()) {
			cmpRenderList.erase(i);
		}
		Thread::mutexUnlock(&proMutex);
	}

	void SDLDeviceScreen::createReleaseContainer(
			SDL_Surface* uSur,
			SDL_Texture* uTex,
			IMediaProvider* iDec) {

		ReleaseContainer* rc;

		checkMutexInit();

		Thread::mutexLock(&recMutex);

		rc = new ReleaseContainer;
		rc->iDec = iDec;
		rc->uSur = uSur;
		rc->uTex = uTex;

		releaseList.push_back(rc);

		Thread::mutexUnlock(&recMutex);
	}

	void SDLDeviceScreen::checkSDLInit() {
		Uint32 subsystem_init = SDL_WasInit(0);

		if (subsystem_init == 0) {
			SDL_Init((Uint32)(
					SDL_INIT_AUDIO |
					SDL_INIT_VIDEO |
					SDL_INIT_TIMER |
					SDL_INIT_NOPARACHUTE));

		} else {
			if ((subsystem_init & SDL_INIT_AUDIO) == 0) {
				SDL_InitSubSystem(SDL_INIT_AUDIO);
			}

			if ((subsystem_init & SDL_INIT_VIDEO) == 0) {
				SDL_InitSubSystem(SDL_INIT_VIDEO);
			}

			if ((subsystem_init & SDL_INIT_TIMER) == 0) {
				SDL_InitSubSystem(SDL_INIT_TIMER);
			}
		}
	}

	void SDLDeviceScreen::sdlQuit() {
		SDL_Quit();
		clog << "SDLDeviceScreen::sdlQuit all done!" << endl;
	}

	void* SDLDeviceScreen::rendererT(void* ptr) {
		map<SDLDeviceScreen*, short>::iterator i;
		SDLDeviceScreen* s;
		SDL_Event event;
		int elapsedTime, decRate;
		bool shiftOn = false;
		bool capsOn  = false;
		SDLEventBuffer* eventBuffer = NULL;
		double lastRender = 0;

		checkSDLInit();

		while (hasRenderer) {
			elapsedTime = getCurrentTimeMillis();

			while (SDL_PollEvent(&event)) {
				if (event.type == SDL_KEYDOWN) {
					if (event.key.keysym.sym == SDLK_LSHIFT ||
							event.key.keysym.sym == SDLK_RSHIFT) {

						shiftOn = true;
					}

				} else  if (event.type == SDL_KEYUP) {
					if (event.key.keysym.sym == SDLK_CAPSLOCK) {
						capsOn = !capsOn;

					} else if (event.key.keysym.sym == SDLK_LSHIFT ||
							event.key.keysym.sym == SDLK_RSHIFT) {

						shiftOn = false;
					}
				}

				Thread::mutexLock(&scrMutex);
				i = sdlScreens.begin();
				while (i != sdlScreens.end()) {
					s = i->first;

			    	if (s->uEmbedId != NULL) {
						if (event.type == SDL_WINDOWEVENT &&
								event.window.windowID == s->sdlId) {

							switch (event.window.event) {
								case SDL_WINDOWEVENT_SHOWN:
									clog << "SDLDeviceScreen::rendererT ";
									clog << "Window '" << event.window.windowID;
									clog << "' shown" << endl;
									break;

								case SDL_WINDOWEVENT_HIDDEN:
									clog << "SDLDeviceScreen::rendererT ";
									clog << "Window '" << event.window.windowID;
									clog << "' hidden" << endl;
									break;

								case SDL_WINDOWEVENT_EXPOSED:
									clog << "SDLDeviceScreen::rendererT ";
									clog << "Window '" << event.window.windowID;
									clog << "' exposed" << endl;

									s->mustGainFocus = true;
									break;

								case SDL_WINDOWEVENT_MOVED:
									clog << "SDLDeviceScreen::rendererT ";
									clog << "Window '" << event.window.windowID;
									clog << "' moved to '";
									clog << event.window.data1;
									clog << "," << event.window.data2 << "'";
									clog << endl;
									break;

								case SDL_WINDOWEVENT_RESIZED:
									clog << "SDLDeviceScreen::rendererT ";
									clog << "Window '" << event.window.windowID;
									clog << "' resized to '";
									clog << event.window.data1;
									clog << "," << event.window.data2 << "'";
									clog << endl;

									s->wRes = event.window.data1;
									s->hRes = event.window.data2;

									if (s->im != NULL) {
										s->im->setAxisBoundaries(
												s->wRes, s->hRes, 0);
									}

									break;

								case SDL_WINDOWEVENT_MINIMIZED:
									clog << "SDLDeviceScreen::rendererT ";
									clog << "Window '" << event.window.windowID;
									clog << "' minimized" << endl;
									break;

								case SDL_WINDOWEVENT_MAXIMIZED:
									clog << "SDLDeviceScreen::rendererT ";
									clog << "Window '" << event.window.windowID;
									clog << "' maximized" << endl;
									break;

								case SDL_WINDOWEVENT_RESTORED:
									clog << "SDLDeviceScreen::rendererT ";
									clog << "Window '" << event.window.windowID;
									clog << "' restored to '";
									clog << event.window.data1;
									clog << ", " << event.window.data2 << "'";
									clog << endl;
									break;

								case SDL_WINDOWEVENT_ENTER:
									clog << "SDLDeviceScreen::rendererT ";
									clog << "Window '" << event.window.windowID;
									clog << "' Mouse entered" << endl;

									s->mustGainFocus = true;
									break;

								case SDL_WINDOWEVENT_LEAVE:
									clog << "SDLDeviceScreen::rendererT ";
									clog << "Window '" << event.window.windowID;
									clog << "' Mouse left" << endl;
									break;

								case SDL_WINDOWEVENT_FOCUS_GAINED:
									clog << "SDLDeviceScreen::rendererT ";
									clog << "Window '" << event.window.windowID;
									clog << "' gained keyboard focus" << endl;

									s->uEmbedFocused = true;
									break;

								case SDL_WINDOWEVENT_FOCUS_LOST:
									clog << "SDLDeviceScreen::rendererT ";
									clog << "Window '" << event.window.windowID;
									clog << "' lost keyboard focus" << endl;

									s->uEmbedFocused = false;
									break;

								case SDL_WINDOWEVENT_CLOSE:
									clog << "SDLDeviceScreen::rendererT ";
									clog << "Window '" << event.window.windowID;
									clog << "' closed" << endl;
									break;

								default:
									clog << "SDLDeviceScreen::rendererT ";
									clog << "Window '" << event.window.windowID;
									clog << "' got unknown event '";
									clog << event.window.event << "'" << endl;
									break;
							}
						}
			    	}

			    	if (event.type == SDL_QUIT) {
#if !defined(SDL_VIDEO_DRIVER_WINDOWS)
						/*
						 * TODO:
						 *      1) send a notification to NCL player
						 *      2) check which screen sent SDL_QUIT.
						 */

			    		Thread::mutexUnlock(&scrMutex);
						releaseAll();
						sdlQuit();
						exit(0);
						return NULL;
#else
						exit(0);
#endif

					} else if (s->im != NULL) {
						eventBuffer = (SDLEventBuffer*)(
								s->im->getEventBuffer());

						if (((SDLEventBuffer::checkEvent(s->sdlId, event) &&
								s->uEmbedId == NULL) || checkEventFocus(s))) {

							eventBuffer->feed(event, capsOn, shiftOn);
						}
					}
					++i;
				}

				Thread::mutexUnlock(&scrMutex);
	    	}

			Thread::mutexLock(&scrMutex);
			i = sdlScreens.begin();
			while (i != sdlScreens.end()) {
				s = i->first;

				switch (i->second) {
					case SPT_NONE:
						decRate = refreshCMP(s);

						if (lastRender == 0) {
							refreshWin(s);
							lastRender = getCurrentTimeMillis() * 1000;

						} else if ((getCurrentTimeMillis() * 1000) -
								lastRender > uSleepTime) {

							lastRender = 0;
						}

						if (s->mustGainFocus) {
							if (!s->uEmbedFocused) {
								forceInputFocus(s, s->uEmbedId);
							}
							s->mustGainFocus = false;
						}

						refreshRC(s);
						++i;
						break;

					case SPT_INIT:
						initScreen(s);
						sdlScreens[s] = SPT_NONE;
						i = sdlScreens.begin();
						break;

					case SPT_CLEAR:
						clearScreen(s);
						sdlScreens[s] = SPT_NONE;
						i = sdlScreens.begin();
						break;

					case SPT_RELEASE:
						releaseScreen(s);
						sdlScreens.erase(i);
						i = sdlScreens.begin();
						break;

					default:
						i = sdlScreens.end();
						break;
				}
			}
			Thread::mutexUnlock(&scrMutex);

			if (hasERC) {
				break;

			} else {
#if defined(SDL_VIDEO_DRIVER_WINDOWS)
				/*
				 * TODO: we have to set windows to sleep less than 20ms
				 *       (we're sleeping 2ms on linux)
				 */
				Sleep(0);

#else
				SystemCompat::uSleep(2000);
#endif
			}
		}

		return NULL;
	}

	void SDLDeviceScreen::refreshRC(SDLDeviceScreen* s) {
		vector<ReleaseContainer*>::iterator i;
		ReleaseContainer* rc;
		IMediaProvider* dec;
		SDL_Surface* sur;
		SDL_Texture* tex;
		IContinuousMediaProvider* cmp;
		IDiscreteMediaProvider* dmp;
		string strSym = "";

		set<IDiscreteMediaProvider*>::iterator j;

		Thread::mutexLock(&recMutex);

		if (s->releaseList.empty()) {
			Thread::mutexUnlock(&recMutex);
			return;
		}

		i = s->releaseList.begin();
		while (i != s->releaseList.end()) {
			rc = (*i);

			dec = rc->iDec;
			sur = rc->uSur;
			tex = rc->uTex;

			delete rc;

			s->releaseList.erase(i);
			Thread::mutexUnlock(&recMutex);

			if (sur != NULL) {
				releaseUnderlyingSurface(sur);
			}

			if (tex != NULL) {
				releaseTexture(tex);
			}

			if (dec != NULL) {
				strSym = "";

				cmp = dynamic_cast<IContinuousMediaProvider*>(dec);

				if (cmp != NULL) {
					strSym = cmp->getLoadSymbol();
					delete cmp;

				} else {
					dmp = dynamic_cast<IDiscreteMediaProvider*>(dec);

					if (dmp != NULL) {
						Thread::mutexLock(&proMutex);
						j = s->dmpPool.find(dmp);
						if (j != s->dmpPool.end()) {
							s->dmpPool.erase(j);
						}
						Thread::mutexUnlock(&proMutex);

						strSym = dmp->getLoadSymbol();
						delete dmp;
					}
				}

#if HAVE_COMPSUPPORT
				if (strSym != "") {
					cm->releaseComponentFromObject(strSym);
				}
#endif
			}

			Thread::mutexLock(&recMutex);
			i = s->releaseList.begin();
		}

		s->releaseList.clear();
		Thread::mutexUnlock(&recMutex);
	}

	int SDLDeviceScreen::refreshCMP(SDLDeviceScreen* s) {
		set<IContinuousMediaProvider*>::iterator i;
		set<IContinuousMediaProvider*>::iterator j;

		int size;

		Thread::mutexLock(&proMutex);
		size = cmpRenderList.size();
		i = cmpRenderList.begin();
		while (i != cmpRenderList.end()) {
			j = s->cmpPool.find(*i);
			if (j != s->cmpPool.end()) {
				if ((*i)->getHasVisual()) {
					if ((*i)->getProviderContent() == NULL) {
						initCMP(s, (*i));

					} else {
						(*i)->refreshDR(NULL);
					}
				}
			}
			++i;
		}
		Thread::mutexUnlock(&proMutex);

		return size;
	}

	void SDLDeviceScreen::refreshWin(SDLDeviceScreen* s) {
		SDL_Texture* uTex;
		SDLWindow* win;
		bool ownTex = false;

		map<GingaScreenID, map<float, set<IWindow*>*>*>::iterator i;
		map<float, set<IWindow*>*>::iterator j;
		set<IWindow*>::iterator k;

		Thread::mutexLock(&renMutex);
		lockSDL();
		if (s->renderer != NULL && !renderMap.empty()) {
			SDL_RenderClear(s->renderer);
			unlockSDL();

			i = renderMap.find(s->id);
			if (i != renderMap.end()) {
				j = i->second->begin();
				while (j != i->second->end()) {
					k = j->second->begin();
					while (k != j->second->end()) {
						win = (SDLWindow*)(*k);

						if (win->isVisible() && !win->isGhostWindow()) {
							uTex = win->getTexture(s->renderer);
							if (uTex != NULL) {
								drawWindow(s->renderer, uTex, win);
							}

							win->rendered();
						}

						++k;
					}
					++j;
				}
			}

			lockSDL();
			SDL_RenderPresent(s->renderer);
		}
		unlockSDL();
		Thread::mutexUnlock(&renMutex);
	}

	void SDLDeviceScreen::initEmbed(SDLDeviceScreen* s, GingaWindowID uWin) {
		SDL_SysWMinfo info;

		lockSDL();
		SDL_VERSION(&info.version);
		SDL_GetWindowWMInfo(s->screen, &info);

#if defined(SDL_VIDEO_DRIVER_X11)
		XSetWindowAttributes attributes;

		if (info.info.x11.display == NULL) {
			info.info.x11.display = XOpenDisplay(getenv("DISPLAY"));
		}

		if (info.info.x11.display != NULL) {
			attributes.event_mask = (
					FocusChangeMask       |
					EnterWindowMask       |
					LeaveWindowMask       |
					ExposureMask          |
					ButtonPressMask       |
					ButtonReleaseMask     |
					PointerMotionMask     |
					KeyPressMask          |
					KeyReleaseMask        |
					PropertyChangeMask    |
					StructureNotifyMask   |
					KeymapStateMask);

			attributes.override_redirect = False;

			XChangeWindowAttributes(
					info.info.x11.display,
					(Window)uWin,
					(CWEventMask | CWOverrideRedirect),
					&attributes);

			XFlush(info.info.x11.display);

			XSync(info.info.x11.display, 1);

			clog << "SDLDeviceScreen::initEmbed set attributes for '";
			clog << uWin << "'" << endl;

		} else {
			clog << "SDLDeviceScreen::initEmbed Warning! ";
			clog << "Can't set input event mask for embedded ";
			clog << "window '" << uWin << "'" << endl;
		}

#elif defined(SDL_VIDEO_DRIVER_WINDOWS)
		//TODO: Windows input event configuration

#elif defined(SDL_VIDEO_DRIVER_COCOA)
		//TODO: Cocoa input event configuration
#endif

		unlockSDL();
	}

	void SDLDeviceScreen::forceInputFocus(
			SDLDeviceScreen* s, GingaWindowID uWin) {

		lockSDL();

#if defined(SDL_VIDEO_DRIVER_X11)
		Window focusedWindow;
		int revert;
		SDL_SysWMinfo info;

		SDL_VERSION(&info.version);
		SDL_GetWindowWMInfo(s->screen, &info);

		if (info.info.x11.display == NULL) {
			info.info.x11.display = XOpenDisplay(getenv("DISPLAY"));
		}

		if (info.info.x11.display != NULL) {
			XGetInputFocus(
					info.info.x11.display,
					&focusedWindow,
					&revert);

			if (focusedWindow != (Window)uWin) {
				XSetInputFocus(
						info.info.x11.display,
						(Window)uWin,
						RevertToParent,
						CurrentTime);

			    /*XSelectInput(
						info.info.x11.display,
						(Window)s->uEmbedId,
						FocusChangeMask       |
						EnterWindowMask       |
						LeaveWindowMask       |
						ExposureMask          |
						ButtonPressMask       |
						ButtonReleaseMask     |
						PointerMotionMask     |
						KeyPressMask          |
						KeyReleaseMask        |
						PropertyChangeMask    |
						StructureNotifyMask   |
						KeymapStateMask);*/

				XCirculateSubwindowsUp(
						info.info.x11.display,
						(Window)uWin);

				XFlush(info.info.x11.display);

				XSync(info.info.x11.display, 1);

				clog << "SDLDeviceScreen::forceInputFocus set input for '";
				clog << uWin << "'" << endl;
			}

		} else {
			clog << "SDLDeviceScreen::forceInputFocus Warning! ";
			clog << "Can't set input event mask for embedded ";
			clog << "window '" << uWin << "'" << endl;
		}

#elif defined(SDL_VIDEO_DRIVER_WINDOWS)
		//TODO: Windows input event configuration

#elif defined(SDL_VIDEO_DRIVER_COCOA)
		//TODO: Cocoa input event configuration
#endif

		unlockSDL();
	}

	void SDLDeviceScreen::initScreen(SDLDeviceScreen* s) {
		SDL_Rect rect;
		int i, numOfDrivers, x, y;
		string title = "";

		lockSDL();

		SDL_GetDisplayBounds(0, &rect);

		if (s->mbSubSystem != "") {
			numOfDrivers = SDL_GetNumVideoDrivers();
			for (i = 0; i < numOfDrivers; i++) {
				if (strcmp(
						SDL_GetVideoDriver(i), s->mbSubSystem.c_str()) == 0) {

					SDL_VideoInit(s->mbSubSystem.c_str());
					break;
				}
			}
		}

		if (s->uEmbedId != NULL) {
			s->screen = SDL_CreateWindowFrom(s->uEmbedId);
			if (s->screen != NULL) {
				SDL_GetWindowSize(s->screen, &s->wRes, &s->hRes);
				s->sdlId = SDL_GetWindowID(s->screen);

				initEmbed(s, s->uEmbedId);
			}

		} else {
			title.assign((char*)VERSION);
			title = "Ginga v" + title;

			if (s->mbMode != "" && s->mbMode.find("x") != std::string::npos) {
				s->wRes = (int)util::stof(
						s->mbMode.substr(0, s->mbMode.find_first_of("x")));

				s->hRes = (int)util::stof(
						s->mbMode.substr(
								s->mbMode.find_first_of("x") + 1,
								(s->mbMode.length() -
										(s->mbMode.find_first_of("x")) + 1)));

			} else {
				s->wRes = 1280;
				s->hRes = 720;
			}

			if (s->wRes <= 0 || s->wRes > rect.w) {
				s->wRes = 0.9 * rect.w;
			}

			if (s->hRes <= 0 || s->hRes > rect.h) {
				s->hRes = 0.9 * rect.h;
			}

			x = (rect.w - s->wRes) / 2;
			y = (rect.h - s->hRes) / 2;

			if (s->fullScreen) {
				s->screen = SDL_CreateWindow(
						title.c_str(),
						x, y, s->wRes, s->hRes,
						SDL_WINDOW_FULLSCREEN);

			} else {
				s->screen = SDL_CreateWindow(
						title.c_str(),
						x, y, s->wRes, s->hRes,
						0);
			}

			s->sdlId = SDL_GetWindowID(s->screen);
		}

		if (s->screen != NULL) {
			s->renderer = SDL_CreateRenderer(
					s->screen, -1, SDL_RENDERER_ACCELERATED);

			if (s->renderer != NULL) {
				clog << "SDLDeviceScreen::initScreen renderer ";
				clog << "accelerated by hardware was created";
				clog << endl;

			} else {
				clog << "SDLDeviceScreen::initScreen software renderer ";
				clog << "was created";
				clog << endl;

				s->renderer = SDL_CreateRenderer(
						s->screen, -1, SDL_RENDERER_SOFTWARE);
			}
		}

		initCodeMaps();
		s->im = new InputManager(s->id);
		s->im->setAxisBoundaries(s->wRes, s->hRes, 0);

		unlockSDL();
	}

	void SDLDeviceScreen::clearScreen(SDLDeviceScreen* s) {
		IWindow* iWin;
		ISurface* iSur;
		IContinuousMediaProvider* iCmp;
		IDiscreteMediaProvider* iDmp;

		set<IWindow*>::iterator i;
		set<ISurface*>::iterator j;
		set<IContinuousMediaProvider*>::iterator k;
		set<IDiscreteMediaProvider*>::iterator l;

		Thread::mutexLock(&winMutex);

		//Releasing remaining Window objects in Window Pool
		if (!s->windowPool.empty()) {
			i = s->windowPool.begin();
			while (i != s->windowPool.end()) {
				iWin = (*i);
				if (iWin != NULL) {
					delete iWin;
				}
				++i;
			}
			s->windowPool.clear();
		}
		Thread::mutexUnlock(&winMutex);

		Thread::mutexLock(&surMutex);
		//Releasing remaining Surface objects in Surface Pool
		if (!s->surfacePool.empty()) {
			j = s->surfacePool.begin();
			while (j != s->surfacePool.end()) {
				iSur = (*j);
				if (iSur != NULL) {
					delete iSur;
				}
				++j;
			}
			s->surfacePool.clear();
		}

		Thread::mutexUnlock(&surMutex);

		Thread::mutexLock(&proMutex);

		//Releasing remaining CMP objects in CMP Pool
		if (!s->cmpPool.empty()) {
			k = s->cmpPool.begin();
			while (k != s->cmpPool.end()) {
				iCmp = (*k);

				if (iCmp != NULL) {
					iCmp->stop();
					delete iCmp;
				}
				++k;
			}
			s->cmpPool.clear();
		}

		//Releasing remaining DMP objects in DMP Pool
		if (!s->dmpPool.empty()) {
			l = s->dmpPool.begin();
			while (l != s->dmpPool.end()) {
				iDmp = *l;

				if (iDmp != NULL) {
					delete iDmp;
				}
				++l;
			}
			s->dmpPool.clear();
		}

		Thread::mutexUnlock(&proMutex);
	}

	void SDLDeviceScreen::releaseScreen(SDLDeviceScreen* s) {
		lockSDL();

		clearScreen(s);

		if (s->uEmbedId == NULL) {
			if (s->screen != NULL) {
				SDL_HideWindow(s->screen);
			}

			if (s->renderer != NULL) {
				SDL_DestroyRenderer(s->renderer);
				s->renderer = NULL;
			}

			if (s->screen != NULL) {
				SDL_DestroyWindow(s->screen);
				s->screen = NULL;
			}
		}

		unlockSDL();
	}

	void SDLDeviceScreen::releaseAll() {
		map<SDLDeviceScreen*, short>::iterator i;

		Thread::mutexLock(&scrMutex);

		i = sdlScreens.begin();
		while (i != sdlScreens.begin()) {
			releaseScreen(i->first);
			++i;
		}
		sdlScreens.clear();

		Thread::mutexUnlock(&scrMutex);
	}

	void SDLDeviceScreen::initCMP(
			SDLDeviceScreen* s, IContinuousMediaProvider* cmp) {

		SDL_Texture* texture;
		int w, h;

		cmp->getOriginalResolution(&w, &h);

		/*clog << "SDLDeviceScreen::initCMP creating texture with w = '";
		clog << w << "' and h = '" << h << "'" << endl;*/

		texture = createTexture(s->renderer, w, h);
		cmp->setProviderContent((void*)texture);
	}

	void SDLDeviceScreen::blitFromWindow(IWindow* iWin, SDL_Surface* dest) {
		SDL_Surface* tmpSur;
		SDL_Texture* tmpTex;
		SDL_Rect rect;

		bool freeSurface = false;

		lockSDL();
		iWin->lock();
		tmpTex = ((SDLWindow*)iWin)->getTexture(NULL);
		if (tmpTex != NULL) {
			tmpSur = createUnderlyingSurfaceFromTexture(tmpTex);
			freeSurface = true;

		} else {
			tmpSur = (SDL_Surface*)(iWin->getContent());
		}

		if (tmpSur != NULL) {
			rect.x = iWin->getX();
			rect.y = iWin->getY();
			rect.w = iWin->getW();
			rect.h = iWin->getH();

			if (SDL_UpperBlitScaled(tmpSur, NULL, dest, &rect) < 0) {
				clog << "SDLDeviceScreen::blitFromWIndow SDL error: '";
				clog << SDL_GetError() << "'" << endl;
			}
		}

		if (freeSurface) {
			freeSurface = false;
			releaseUnderlyingSurface(tmpSur);
		}

		iWin->unlock();
		unlockSDL();
	}


	/* interfacing input */

	IInputManager* SDLDeviceScreen::getInputManager() {
		/*
		 * im == NULL is an initial state. So pthread_cond_t
		 * is not necessary here.
		 */
		while (im == NULL) {
			SystemCompat::uSleep(1000000 / SDS_FPS);
		}
		return im;
	}

	IEventBuffer* SDLDeviceScreen::createEventBuffer() {
		return new SDLEventBuffer(id);
	}

	IInputEvent* SDLDeviceScreen::createInputEvent(
			void* event, const int symbol) {

		IInputEvent* ie = NULL;

		lockSDL();
		if (event != NULL) {
			ie = new SDLInputEvent(*(SDL_Event*)event);
		}

		if (symbol >= 0) {
			ie = new SDLInputEvent(symbol);
		}

		unlockSDL();

		return ie;
	}

	IInputEvent* SDLDeviceScreen::createApplicationEvent(int type, void* data) {
		return new SDLInputEvent(type, data);
	}

	int SDLDeviceScreen::fromMBToGinga(int keyCode) {
		map<int, int>::iterator i;
		int translated;

		checkMutexInit();

		Thread::mutexLock(&sieMutex);

		translated = CodeMap::KEY_NULL;
		i = sdlToGingaCodeMap.find(keyCode);
		if (i != sdlToGingaCodeMap.end()) {
			translated = i->second;
		}

		Thread::mutexUnlock(&sieMutex);

		return translated;
	}

	int SDLDeviceScreen::fromGingaToMB(int keyCode) {
		map<int, int>::iterator i;
		int translated = CodeMap::KEY_NULL;

		checkMutexInit();

		Thread::mutexLock(&sieMutex);

		i = gingaToSDLCodeMap.find(keyCode);
		if (i != gingaToSDLCodeMap.end()) {
			translated = i->second;
		}

		Thread::mutexUnlock(&sieMutex);

		return translated;
	}


	/* interfacing underlying multimedia system */

	void* SDLDeviceScreen::getGfxRoot() {
		return renderer;
	}

	/* libgingaccmbsdl internal use*/

	/* input */
	void SDLDeviceScreen::initCodeMaps() {
		checkMutexInit();

		Thread::mutexLock(&sieMutex);
		if (!gingaToSDLCodeMap.empty()) {
			Thread::mutexUnlock(&sieMutex);
			return;
		}

		gingaToSDLCodeMap[CodeMap::KEY_QUIT]              = SDL_QUIT;
		gingaToSDLCodeMap[CodeMap::KEY_NULL]              = SDLK_UNKNOWN;
		gingaToSDLCodeMap[CodeMap::KEY_0]                 = SDLK_0;
		gingaToSDLCodeMap[CodeMap::KEY_1]                 = SDLK_1;
		gingaToSDLCodeMap[CodeMap::KEY_2]                 = SDLK_2;
		gingaToSDLCodeMap[CodeMap::KEY_3]                 = SDLK_3;
		gingaToSDLCodeMap[CodeMap::KEY_4]                 = SDLK_4;
		gingaToSDLCodeMap[CodeMap::KEY_5]                 = SDLK_5;
		gingaToSDLCodeMap[CodeMap::KEY_6]                 = SDLK_6;
		gingaToSDLCodeMap[CodeMap::KEY_7]                 = SDLK_7;
		gingaToSDLCodeMap[CodeMap::KEY_8]                 = SDLK_8;
		gingaToSDLCodeMap[CodeMap::KEY_9]                 = SDLK_9;

		gingaToSDLCodeMap[CodeMap::KEY_SMALL_A]           = SDLK_a;
		gingaToSDLCodeMap[CodeMap::KEY_SMALL_B]           = SDLK_b;
		gingaToSDLCodeMap[CodeMap::KEY_SMALL_C]           = SDLK_c;
		gingaToSDLCodeMap[CodeMap::KEY_SMALL_D]           = SDLK_d;
		gingaToSDLCodeMap[CodeMap::KEY_SMALL_E]           = SDLK_e;
		gingaToSDLCodeMap[CodeMap::KEY_SMALL_F]           = SDLK_f;
		gingaToSDLCodeMap[CodeMap::KEY_SMALL_G]           = SDLK_g;
		gingaToSDLCodeMap[CodeMap::KEY_SMALL_H]           = SDLK_h;
		gingaToSDLCodeMap[CodeMap::KEY_SMALL_I]           = SDLK_i;
		gingaToSDLCodeMap[CodeMap::KEY_SMALL_J]           = SDLK_j;
		gingaToSDLCodeMap[CodeMap::KEY_SMALL_K]           = SDLK_k;
		gingaToSDLCodeMap[CodeMap::KEY_SMALL_L]           = SDLK_l;
		gingaToSDLCodeMap[CodeMap::KEY_SMALL_M]           = SDLK_m;
		gingaToSDLCodeMap[CodeMap::KEY_SMALL_N]           = SDLK_n;
		gingaToSDLCodeMap[CodeMap::KEY_SMALL_O]           = SDLK_o;
		gingaToSDLCodeMap[CodeMap::KEY_SMALL_P]           = SDLK_p;
		gingaToSDLCodeMap[CodeMap::KEY_SMALL_Q]           = SDLK_q;
		gingaToSDLCodeMap[CodeMap::KEY_SMALL_R]           = SDLK_r;
		gingaToSDLCodeMap[CodeMap::KEY_SMALL_S]           = SDLK_s;
		gingaToSDLCodeMap[CodeMap::KEY_SMALL_T]           = SDLK_t;
		gingaToSDLCodeMap[CodeMap::KEY_SMALL_U]           = SDLK_u;
		gingaToSDLCodeMap[CodeMap::KEY_SMALL_V]           = SDLK_v;
		gingaToSDLCodeMap[CodeMap::KEY_SMALL_W]           = SDLK_w;
		gingaToSDLCodeMap[CodeMap::KEY_SMALL_X]           = SDLK_x;
		gingaToSDLCodeMap[CodeMap::KEY_SMALL_Y]           = SDLK_y;
		gingaToSDLCodeMap[CodeMap::KEY_SMALL_Z]           = SDLK_z;

		gingaToSDLCodeMap[CodeMap::KEY_CAPITAL_A]         = SDLK_a + 5000;
		gingaToSDLCodeMap[CodeMap::KEY_CAPITAL_B]         = SDLK_b + 5000;
		gingaToSDLCodeMap[CodeMap::KEY_CAPITAL_C]         = SDLK_c + 5000;
		gingaToSDLCodeMap[CodeMap::KEY_CAPITAL_D]         = SDLK_d + 5000;
		gingaToSDLCodeMap[CodeMap::KEY_CAPITAL_E]         = SDLK_e + 5000;
		gingaToSDLCodeMap[CodeMap::KEY_CAPITAL_F]         = SDLK_f + 5000;
		gingaToSDLCodeMap[CodeMap::KEY_CAPITAL_G]         = SDLK_g + 5000;
		gingaToSDLCodeMap[CodeMap::KEY_CAPITAL_H]         = SDLK_h + 5000;
		gingaToSDLCodeMap[CodeMap::KEY_CAPITAL_I]         = SDLK_i + 5000;
		gingaToSDLCodeMap[CodeMap::KEY_CAPITAL_J]         = SDLK_j + 5000;
		gingaToSDLCodeMap[CodeMap::KEY_CAPITAL_K]         = SDLK_k + 5000;
		gingaToSDLCodeMap[CodeMap::KEY_CAPITAL_L]         = SDLK_l + 5000;
		gingaToSDLCodeMap[CodeMap::KEY_CAPITAL_M]         = SDLK_m + 5000;
		gingaToSDLCodeMap[CodeMap::KEY_CAPITAL_N]         = SDLK_n + 5000;
		gingaToSDLCodeMap[CodeMap::KEY_CAPITAL_O]         = SDLK_o + 5000;
		gingaToSDLCodeMap[CodeMap::KEY_CAPITAL_P]         = SDLK_p + 5000;
		gingaToSDLCodeMap[CodeMap::KEY_CAPITAL_Q]         = SDLK_q + 5000;
		gingaToSDLCodeMap[CodeMap::KEY_CAPITAL_R]         = SDLK_r + 5000;
		gingaToSDLCodeMap[CodeMap::KEY_CAPITAL_S]         = SDLK_s + 5000;
		gingaToSDLCodeMap[CodeMap::KEY_CAPITAL_T]         = SDLK_t + 5000;
		gingaToSDLCodeMap[CodeMap::KEY_CAPITAL_U]         = SDLK_u + 5000;
		gingaToSDLCodeMap[CodeMap::KEY_CAPITAL_V]         = SDLK_v + 5000;
		gingaToSDLCodeMap[CodeMap::KEY_CAPITAL_W]         = SDLK_w + 5000;
		gingaToSDLCodeMap[CodeMap::KEY_CAPITAL_X]         = SDLK_x + 5000;
		gingaToSDLCodeMap[CodeMap::KEY_CAPITAL_Y]         = SDLK_y + 5000;
		gingaToSDLCodeMap[CodeMap::KEY_CAPITAL_Z]         = SDLK_z + 5000;

		gingaToSDLCodeMap[CodeMap::KEY_PAGE_DOWN]         = SDLK_PAGEDOWN;
		gingaToSDLCodeMap[CodeMap::KEY_PAGE_UP]           = SDLK_PAGEUP;

		gingaToSDLCodeMap[CodeMap::KEY_F1]                = SDLK_F1;
		gingaToSDLCodeMap[CodeMap::KEY_F2]                = SDLK_F2;
		gingaToSDLCodeMap[CodeMap::KEY_F3]                = SDLK_F3;
		gingaToSDLCodeMap[CodeMap::KEY_F4]                = SDLK_F4;
		gingaToSDLCodeMap[CodeMap::KEY_F5]                = SDLK_F5;
		gingaToSDLCodeMap[CodeMap::KEY_F6]                = SDLK_F6;
		gingaToSDLCodeMap[CodeMap::KEY_F7]                = SDLK_F7;
		gingaToSDLCodeMap[CodeMap::KEY_F8]                = SDLK_F8;
		gingaToSDLCodeMap[CodeMap::KEY_F9]                = SDLK_F9;
		gingaToSDLCodeMap[CodeMap::KEY_F10]               = SDLK_F10;
		gingaToSDLCodeMap[CodeMap::KEY_F11]               = SDLK_F11;
		gingaToSDLCodeMap[CodeMap::KEY_F12]               = SDLK_F12;

		gingaToSDLCodeMap[CodeMap::KEY_PLUS_SIGN]         = SDLK_PLUS;
		gingaToSDLCodeMap[CodeMap::KEY_MINUS_SIGN]        = SDLK_MINUS;

		gingaToSDLCodeMap[CodeMap::KEY_ASTERISK]          = SDLK_ASTERISK;
		gingaToSDLCodeMap[CodeMap::KEY_NUMBER_SIGN]       = SDLK_HASH;

		gingaToSDLCodeMap[CodeMap::KEY_PERIOD]            = SDLK_PERIOD;

		gingaToSDLCodeMap[CodeMap::KEY_SUPER]             = SDLK_CAPSLOCK;
		gingaToSDLCodeMap[CodeMap::KEY_PRINTSCREEN]       = SDLK_PRINTSCREEN;
		gingaToSDLCodeMap[CodeMap::KEY_MENU]              = SDLK_MENU;
		gingaToSDLCodeMap[CodeMap::KEY_INFO]              = SDLK_F14;
		gingaToSDLCodeMap[CodeMap::KEY_EPG]               = SDLK_QUESTION;

		gingaToSDLCodeMap[CodeMap::KEY_CURSOR_DOWN]       = SDLK_DOWN;
		gingaToSDLCodeMap[CodeMap::KEY_CURSOR_LEFT]       = SDLK_LEFT;
		gingaToSDLCodeMap[CodeMap::KEY_CURSOR_RIGHT]      = SDLK_RIGHT;
		gingaToSDLCodeMap[CodeMap::KEY_CURSOR_UP]         = SDLK_UP;

		gingaToSDLCodeMap[CodeMap::KEY_CHANNEL_DOWN]      = SDLK_F15;
		gingaToSDLCodeMap[CodeMap::KEY_CHANNEL_UP]        = SDLK_F16;

		gingaToSDLCodeMap[CodeMap::KEY_VOLUME_DOWN]       = SDLK_VOLUMEDOWN;
		gingaToSDLCodeMap[CodeMap::KEY_VOLUME_UP]         = SDLK_VOLUMEUP;

		gingaToSDLCodeMap[CodeMap::KEY_ENTER]             = SDLK_RETURN;
		gingaToSDLCodeMap[CodeMap::KEY_OK]                = SDLK_RETURN2;

		gingaToSDLCodeMap[CodeMap::KEY_RED]               = SDLK_F17;
		gingaToSDLCodeMap[CodeMap::KEY_GREEN]             = SDLK_F18;
		gingaToSDLCodeMap[CodeMap::KEY_YELLOW]            = SDLK_F19;
		gingaToSDLCodeMap[CodeMap::KEY_BLUE]              = SDLK_F20;

		gingaToSDLCodeMap[CodeMap::KEY_SPACE]             = SDLK_SPACE;
		gingaToSDLCodeMap[CodeMap::KEY_BACKSPACE]         = SDLK_BACKSPACE;
		gingaToSDLCodeMap[CodeMap::KEY_BACK]              = SDLK_AC_BACK;
		gingaToSDLCodeMap[CodeMap::KEY_ESCAPE]            = SDLK_ESCAPE;
		gingaToSDLCodeMap[CodeMap::KEY_EXIT]              = SDLK_OUT;

		gingaToSDLCodeMap[CodeMap::KEY_POWER]             = SDLK_POWER;
		gingaToSDLCodeMap[CodeMap::KEY_REWIND]            = SDLK_F21;
		gingaToSDLCodeMap[CodeMap::KEY_STOP]              = SDLK_STOP;
		gingaToSDLCodeMap[CodeMap::KEY_EJECT]             = SDLK_EJECT;
		gingaToSDLCodeMap[CodeMap::KEY_PLAY]              = SDLK_EXECUTE;
		gingaToSDLCodeMap[CodeMap::KEY_RECORD]            = SDLK_F22;
		gingaToSDLCodeMap[CodeMap::KEY_PAUSE]             = SDLK_PAUSE;

		gingaToSDLCodeMap[CodeMap::KEY_GREATER_THAN_SIGN] = SDLK_GREATER;
		gingaToSDLCodeMap[CodeMap::KEY_LESS_THAN_SIGN]    = SDLK_LESS;

		gingaToSDLCodeMap[CodeMap::KEY_TAB]               = SDLK_TAB;
		gingaToSDLCodeMap[CodeMap::KEY_TAP]               = SDLK_F23;

        map<int, int>::iterator i;
        i = gingaToSDLCodeMap.begin();
        while (i != gingaToSDLCodeMap.end()) {
		    sdlToGingaCodeMap[i->second] = i->first;
		    ++i;
        }

        Thread::mutexUnlock(&sieMutex);
	}

	bool SDLDeviceScreen::checkEventFocus(SDLDeviceScreen* s) {
		bool hasFocus = false;

		if (s->uEmbedId != NULL) {
			/*
			 * NOTE: in this case, the focus is defined by the application
			 *       that is embedding Ginga, through keyHandler specs.
			 */
			hasFocus = true;
		}

		return hasFocus;
	}


	/* output */
	void SDLDeviceScreen::renderMapInsertWindow(
			GingaScreenID screenId, IWindow* iWin, float z) {

		map<GingaScreenID, map<float, set<IWindow*>*>*>::iterator i;
		map<float, set<IWindow*>*>::iterator j;

		map<float, set<IWindow*>*>* sortedMap;
		set<IWindow*>* windows;

		checkMutexInit();

		Thread::mutexLock(&renMutex);
		i = renderMap.find(screenId);
		if (i != renderMap.end()) {
			sortedMap = i->second;

		} else {
			sortedMap = new map<float, set<IWindow*>*>;
			renderMap[screenId] = sortedMap;
		}

		j = sortedMap->find(z);
		if (j != sortedMap->end()) {
			windows = j->second;
		} else {
			windows = new set<IWindow*>;
			(*sortedMap)[z] = windows;
		}

		windows->insert(iWin);
		Thread::mutexUnlock(&renMutex);
	}

	void SDLDeviceScreen::renderMapRemoveWindow(
			GingaScreenID screenId, IWindow* iWin, float z) {

		map<GingaScreenID, map<float, set<IWindow*>*>*>::iterator i;
		map<float, set<IWindow*>*>::iterator j;
		set<IWindow*>::iterator k;

		map<float, set<IWindow*>*>* sortedMap;
		set<IWindow*>* windows;

		checkMutexInit();

		Thread::mutexLock(&renMutex);
		i = renderMap.find(screenId);
		if (i != renderMap.end()) {
			sortedMap = i->second;
			j = sortedMap->find(z);
			if (j != sortedMap->end()) {
				windows = j->second;
				k = windows->find(iWin);
				if (k != windows->end()) {
					windows->erase(k);
				}
			}
		}
		Thread::mutexUnlock(&renMutex);
	}

	/*void SDLDeviceScreen::updateWindowState(
			GingaScreenID screenId, IWindow* win, short state) {

		map<GingaScreenID, vector<IWindow*>*>::iterator i;
		vector<IWindow*>* wins;

		Thread::mutexLock(&wrMutex);
		i = windowRenderMap.find(screenId);
		if (i != windowRenderMap.end()) {
			wins = i->second;
			updateWindowList(wins, win, state);

		} else {
			wins = new vector<IWindow*>;
			wins->push_back(win);
			windowRenderMap[screenId] = wins;
		}


		Thread::mutexUnlock(&wrMutex);
	}

	void SDLDeviceScreen::updateWindowList(
			vector<IWindow*>* windows, IWindow* win, short state) {

		switch (state) {
			case SUW_SHOW:
				windows->push_back(win);
				break;

			case SUW_HIDE:
				removeFromWindowList(windows, win);
				break;

			case SUW_RAISETOTOP:
				removeFromWindowList(windows, win);
				windows->push_back(win);
				break;

			case SUW_LOWERTOBOTTOM:
				removeFromWindowList(windows, win);
				windows->insert(windows->begin(), win);
				break;

			default:
				break;
		}
	}*/

	void SDLDeviceScreen::removeFromWindowList(
			vector<IWindow*>* windows, IWindow* win) {

		vector<IWindow*>::iterator i;

		i = windows->begin();
		while (i != windows->end()) {
			if ((*i) == win) {
				windows->erase(i);
				i = windows->begin();
			} else {
				++i;
			}
		}
	}

	SDL_Window* SDLDeviceScreen::getUnderlyingWindow(GingaWindowID winId) {
		SDL_Window* window = NULL;
		Uint32 wid;

		checkMutexInit();

		lockSDL();

		wid    = (Uint32)(unsigned long)winId;
		window = SDL_GetWindowFromID(wid);

		if (window == NULL) {
			clog << "SDLDeviceScreen::getUnderlyingWindow ";
			clog << "can't find id '" << wid;
			clog << "'" << endl;
		}

		unlockSDL();

		return window;
	}

	bool SDLDeviceScreen::drawWindow(
			SDL_Renderer* renderer,
			SDL_Texture* texture,
			IWindow* iWin) {

		SDL_Rect rect;
		IColor* bgColor;
		Uint8 rr, rg, rb, ra;
		int i, r, g, b, a, bw;
		int alpha = 0;

		bool drawing = false;

		DrawData* dd;
		SDL_Rect dr;
		vector<DrawData*>* drawData;
		vector<DrawData*>::iterator it;

		lockSDL();

	    if (iWin != NULL) {
	    	/* getting renderer previous state */
	    	SDL_GetRenderDrawColor(renderer, &rr, &rg, &rb, &ra);

	    	rect.x = iWin->getX();
	    	rect.y = iWin->getY();
	    	rect.w = iWin->getW();
	    	rect.h = iWin->getH();

	    	alpha = iWin->getTransparencyValue();
	    	if (texture != NULL) {
	    		SDL_SetTextureAlphaMod(texture, 255 - alpha);
	    	}

	    	/* setting window background */
	    	bgColor = iWin->getBgColor();
	    	if (bgColor != NULL) {
	    		drawing = true;
	    		if (alpha == 0) {
	    			alpha = 255 - bgColor->getAlpha();
	    		}

	    		r = bgColor->getR();
	    		g = bgColor->getG();
	    		b = bgColor->getB();

	    		SDL_SetRenderDrawColor(
	    				renderer,
	    				bgColor->getR(),
	    				bgColor->getG(),
	    				bgColor->getB(),
	    				255 - alpha);

	    		if (SDL_RenderFillRect(renderer, &rect) < 0) {
	    	        clog << "SDLDeviceScreen::drawWindow ";
	    	        clog << "Warning! Can't use render to fill rect ";
	    	        clog << SDL_GetError();
	    	        clog << endl;
	    		}
	    	}

	    	/* geometric figures (lua only) */
	    	drawData = ((SDLWindow*)iWin)->createDrawDataList();
	    	if (drawData != NULL) {
	    		drawing = true;
	    		it = drawData->begin();
	    		while (it != drawData->end()) {
	    			dd = (*it);
		    		SDL_SetRenderDrawColor(
		    				renderer, dd->r, dd->g, dd->b, dd->a);

	    			switch (dd->dataType) {
						case SDLWindow::DDT_LINE:
							if ((dd->coord1 < rect.x) ||
									(dd->coord2 < rect.y) ||
									(dd->coord1 > rect.w) ||
									(dd->coord2 > rect.h) ||
									(dd->coord3 > rect.w) ||
									(dd->coord4 > rect.h)) {

								clog << "SDLDeviceScreen::drawWindow Warning!";
								clog << " Invalid line coords: " << endl;
								clog << dd->coord1 << ", ";
								clog << dd->coord2 << ", ";
								clog << dd->coord3 << ", ";
								clog << dd->coord4 << "'";
								clog << endl;
								clog << "Window rect coords: " << endl;
								clog << rect.x << ", ";
								clog << rect.y << ", ";
								clog << rect.w << ", ";
								clog << rect.h << "'";
								clog << endl;
								break;
							}

							if (SDL_RenderDrawLine(
									renderer,
									dd->coord1 + rect.x,
									dd->coord2 + rect.y,
									dd->coord3 + rect.x,
									dd->coord4 + rect.y) < 0) {

				    	        clog << "SDLDeviceScreen::drawWindow ";
				    	        clog << "Warning! Can't draw line ";
				    	        clog << SDL_GetError();
				    	        clog << endl;
							}

							break;

						case SDLWindow::DDT_RECT:
							dr.x = dd->coord1 + rect.x;
							dr.y = dd->coord2 + rect.y;
							dr.w = dd->coord3;
							dr.h = dd->coord4;

							if ((dr.x > + rect.x + rect.w) ||
									(dr.y >  + rect.y + rect.h) ||
									(dd->coord1 + dr.w > rect.w) ||
									(dd->coord2 + dr.h > rect.h)) {

								clog << "SDLDeviceScreen::drawWindow Warning!";
								clog << " Invalid rect coords: " << endl;
								clog << dr.x << ", ";
								clog << dr.y << ", ";
								clog << dr.w << ", ";
								clog << dr.h << "'";
								clog << endl;
								clog << "Window rect coords: " << endl;
								clog << rect.x << ", ";
								clog << rect.y << ", ";
								clog << rect.w << ", ";
								clog << rect.h << "'";
								clog << endl;
								break;
							}

							if (dd->dataType == SDLWindow::DDT_RECT) {
								if (SDL_RenderDrawRect(renderer, &dr) < 0) {
					    	        clog << "SDLDeviceScreen::drawWindow ";
					    	        clog << "Warning! Can't draw rect ";
					    	        clog << SDL_GetError();
					    	        clog << endl;
								}

							} else {
								if (SDL_RenderFillRect(renderer, &dr) < 0) {
					    	        clog << "SDLDeviceScreen::drawWindow ";
					    	        clog << "Warning! Can't fill rect ";
					    	        clog << SDL_GetError();
					    	        clog << endl;
								}
							}
							break;
	    			}
	    			++it;
	    		}
	    		delete drawData;
	    	}

	    	/* window rendering */
	    	if (hasTexture(texture)) {
	    		void* pixels;
	    		int tpitch[3];
	    		bool locked;

	    		//trying to lock texture
	    		locked = SDL_LockTexture(
	    				texture, NULL, &pixels, &tpitch[0]) == 0;

				drawing = true;
				if (SDL_RenderCopy(renderer, texture, NULL, &rect) < 0) {
					clog << "SDLDeviceScreen::drawWindow Warning! ";
					clog << "can't perform render copy " << SDL_GetError();
					clog << endl;
				}

				if (locked) {
					SDL_UnlockTexture(texture);
				}
	    	}

	    	/* window border */
	    	iWin->getBorder(&r, &g, &b, &a, &bw);
	    	if (bw != 0) {
	    		SDL_SetRenderDrawColor(renderer, r, g, b, a);

				i = 0;
				while (i != bw) {
					rect.x = iWin->getX() - i;
					rect.y = iWin->getY() - i;
					rect.w = iWin->getW() + 2*i;
					rect.h = iWin->getH() + 2*i;

					if (SDL_RenderDrawRect(renderer, &rect) < 0) {
						clog << "SDLDeviceScreen::drawWindow SDL error: '";
						clog << SDL_GetError() << "'" << endl;
					}

					if (bw < 0) {
						i--;
					} else {
						i++;
					}
				}
	    	}

	    	/* setting renderer previous state */
	    	SDL_SetRenderDrawColor(renderer, rr, rg, rb, ra);

	    } else {
	        clog << "SDLDeviceScreen::drawWindow Warning! ";
	        clog << "NULL interface window";
	        clog << endl;
	    }

	    unlockSDL();

	    return (drawing);
	}

	SDL_Texture* SDLDeviceScreen::createTextureFromSurface(
			SDL_Renderer* renderer, SDL_Surface* surface) {

		SDL_Texture* texture = NULL;

		checkMutexInit();

		lockSDL();
		Thread::mutexLock(&surMutex);

		if (SDLDeviceScreen::hasUnderlyingSurface(surface)) {
			texture = SDL_CreateTextureFromSurface(renderer, surface);
			if (texture == NULL) {
				clog << "SDLDeviceScreen::createTextureFromSurface Warning! ";
				clog << "Couldn't create texture: " << SDL_GetError();
				clog << endl;

			} else {
				uTexPool.insert(texture);

				/* allowing alpha */
				SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
			}
		}

		Thread::mutexUnlock(&surMutex);
		unlockSDL();

		return texture;
	}

	SDL_Texture* SDLDeviceScreen::createTexture(
			SDL_Renderer* renderer, int w, int h) {

		SDL_Texture* texture;

		lockSDL();

		texture = SDL_CreateTexture(
				renderer,
				SDL_PIXELFORMAT_RGB24,
				SDL_TEXTUREACCESS_STREAMING,
				w, h);

		if (texture != NULL) {
			uTexPool.insert(texture);

			/* allowing alpha */
			SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

		} else {
			clog << "SDLDeviceScreen::createTexture SDL error: '";
			clog << SDL_GetError() << "'" << endl;
		}
	    unlockSDL();

	    return texture;
	}

	bool SDLDeviceScreen::hasTexture(SDL_Texture* uTex) {
		set<SDL_Texture*>::iterator i;
		bool hasIt = false;

		checkMutexInit();

		lockSDL();
		i = uTexPool.find(uTex);
		if (i != uTexPool.end()) {
			hasIt = true;
		}
		unlockSDL();

		return hasIt;
	}

	void SDLDeviceScreen::releaseTexture(SDL_Texture* texture) {
		set<SDL_Texture*>::iterator i;

		checkMutexInit();

		lockSDL();
		i = uTexPool.find(texture);
		if (i != uTexPool.end()) {
			uTexPool.erase(i);
			SDL_DestroyTexture(texture);
		}
		unlockSDL();
	}

	void SDLDeviceScreen::addUnderlyingSurface(SDL_Surface* uSur) {
		checkMutexInit();

		Thread::mutexLock(&surMutex);
		uSurPool.insert(uSur);
		Thread::mutexUnlock(&surMutex);
	}

	SDL_Surface* SDLDeviceScreen::createUnderlyingSurface(
			int width, int height) {

		SDL_Surface* newUSur = NULL;
		Uint32 rmask, gmask, bmask, amask;

		checkMutexInit();

		lockSDL();

		getRGBAMask(24, &rmask, &gmask, &bmask, &amask);
		newUSur = SDL_CreateRGBSurface(
				0, width, height, 24, rmask, gmask, bmask, amask);

		unlockSDL();

		Thread::mutexLock(&surMutex);
		if (newUSur != NULL) {
			uSurPool.insert(newUSur);

		} else {
			clog << "SDLDeviceScreen::createUnderlyingSurface SDL error: '";
			clog << SDL_GetError() << "'" << endl;
		}
		Thread::mutexUnlock(&surMutex);

		return newUSur;
	}

	SDL_Surface* SDLDeviceScreen::createUnderlyingSurfaceFromTexture(
			SDL_Texture* texture) {

		SDL_Surface* uSur = NULL;
		void* pixels;
		int tpitch[3];
		Uint32 rmask, gmask, bmask, amask, format;
		int textureAccess, w, h;
		bool locked;

		lockSDL();

		//trying to lock texture
		locked = SDL_LockTexture(texture, NULL, &pixels, &tpitch[0]) == 0;

		SDL_QueryTexture(texture, &format, &textureAccess, &w, &h);
		getRGBAMask(24, &rmask, &gmask, &bmask, &amask);

		uSur = SDL_CreateRGBSurfaceFrom(
				pixels, w, h, 24, tpitch[0], rmask, gmask, bmask, amask);

		if (locked) {
			SDL_UnlockTexture(texture);
		}

		unlockSDL();

		Thread::mutexLock(&surMutex);
		if (uSur != NULL) {
			uSurPool.insert(uSur);
		}
		Thread::mutexUnlock(&surMutex);

		return uSur;
	}

	bool SDLDeviceScreen::hasUnderlyingSurface(SDL_Surface* uSur) {
		set<SDL_Surface*>::iterator i;
		bool hasIt = false;

		checkMutexInit();

		Thread::mutexLock(&surMutex);
		i = uSurPool.find(uSur);
		if (i != uSurPool.end()) {
			hasIt = true;
		}
		Thread::mutexUnlock(&surMutex);

		return hasIt;
	}

	void SDLDeviceScreen::releaseUnderlyingSurface(SDL_Surface* uSur) {
		set<SDL_Surface*>::iterator i;

		checkMutexInit();

		lockSDL();
		Thread::mutexLock(&surMutex);

		i = uSurPool.find(uSur);
		if (i != uSurPool.end()) {
			uSurPool.erase(i);

			SDL_FreeSurface(uSur);
		}

		Thread::mutexUnlock(&surMutex);
		unlockSDL();
	}

	void SDLDeviceScreen::getRGBAMask(
			int depth,
			Uint32* rmask,
			Uint32* gmask,
			Uint32* bmask,
			Uint32* amask) {

		switch (depth) {
			case 32:
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
				*rmask = 0xff000000;
				*gmask = 0x00ff0000;
				*bmask = 0x0000ff00;
				*amask = 0x000000ff;
#else
				*rmask = 0x000000ff;
				*gmask = 0x0000ff00;
				*bmask = 0x00ff0000;
				*amask = 0xff000000;
#endif
				break;

			case 24:
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
				*rmask = 0x00ff0000;
				*gmask = 0x0000ff00;
				*bmask = 0x000000ff;
				*amask = 0x00000000;
#else
				*rmask = 0x000000ff;
				*gmask = 0x0000ff00;
				*bmask = 0x00ff0000;
				*amask = 0x00000000;
#endif
				break;
		}
	}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::mb::IDeviceScreen*
		createSDLScreen(
				int numArgs, char** args,
				GingaScreenID myId, GingaWindowID embedId,
				bool externalRenderer) {

	return (new ::br::pucrio::telemidia::ginga::core::mb::
			SDLDeviceScreen(numArgs, args, myId, embedId, externalRenderer));
}

extern "C" void destroySDLScreen(
		::br::pucrio::telemidia::ginga::core::mb::IDeviceScreen* ds) {

	delete ds;
}
