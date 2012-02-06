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

extern "C" {
#include <string.h>
#include <stdlib.h>
}

#ifndef Uint32
typedef uint32_t Uint32;
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

bool SDLDeviceScreen::initSDL = false;

unsigned int SDLDeviceScreen::numOfSDLScreens = 0;

const unsigned int SDLDeviceScreen::DSA_UNKNOWN = 0;
const unsigned int SDLDeviceScreen::DSA_4x3     = 1;
const unsigned int SDLDeviceScreen::DSA_16x9    = 2;

	SDLDeviceScreen::SDLDeviceScreen(
			int argc, char** args,
			GingaScreenID myId, GingaWindowID parentId) {

		SDL_Rect rect;
		string mbMode = "", mbSubSystem = "";
		int i, numOfDrivers;

		aspect      = DSA_UNKNOWN;
		hSize       = 0;
		vSize       = 0;
		hRes        = 0;
		wRes        = 0;
		im          = NULL;
		id          = myId;
		uId         = parentId;

		windowPool  = new set<IWindow*>;
		surfacePool = new set<ISurface*>;

		numOfSDLScreens++;

		pthread_mutex_init(&winMutex, NULL);
		pthread_mutex_init(&surMutex, NULL);

		if (!SDLDeviceScreen::initSDL) {
			SDLDeviceScreen::initSDL = true;
			SDL_Init((Uint32)(
					SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_TIMER));
		}

		SDL_GetDisplayBounds(0, &rect);

		for (i = 0; i < argc; i++) {
			if ((strcmp(args[i], "subsystem") == 0) && ((i + 1) < argc)) {
				mbSubSystem.assign(args[i + 1]);

			} else if ((strcmp(args[i], "mode") == 0) && ((i + 1) < argc)) {
				mbMode.assign(args[i + 1]);
			}
		}

		if (mbSubSystem != "") {
			if (mbSubSystem == "dfb") {
				mbSubSystem = "directfb";
			}

			numOfDrivers = SDL_GetNumVideoDrivers();
			for (i = 0; i < numOfDrivers; i++) {
				if (strcmp(SDL_GetVideoDriver(i), mbSubSystem.c_str()) == 0) {
					SDL_VideoInit(mbSubSystem.c_str());
					break;
				}
			}
		}

		if (uId != NULL) {
			cout << "SDLDeviceScreen::SDLDeviceScreen" << endl;
			screen = SDL_CreateWindowFrom(uId);
			SDL_GetWindowSize(screen, &wRes, &hRes);

		} else {
			int x, y;
			string title = "";

			title.assign((char*)VERSION);
			title = "Ginga v" + title;

			if (mbMode == "" && mbMode.find("x") != std::string::npos) {
				wRes = (int)stof(mbMode.substr(0, mbMode.find_first_of("x")));
				hRes = (int)stof(mbMode.substr(
						mbMode.find_first_of("x") + 1,
						mbMode.length() - (mbMode.find_first_of("x")) + 1));

			} else {
				wRes = rect.w;
				hRes = rect.h;
			}

			if (wRes <= 0 || wRes > rect.w) {
				wRes = 0.9 * rect.w;
			}

			if (hRes <= 0 || hRes > rect.h) {
				hRes = 0.9 * rect.h;
			}

			x = (rect.w - wRes) / 2;
			y = (rect.h - hRes) / 2;

			screen = SDL_CreateWindow(title.c_str(), x, y, wRes, hRes, 0);
		}
	}

	SDLDeviceScreen::~SDLDeviceScreen() {
		pthread_mutex_destroy(&winMutex);
		pthread_mutex_destroy(&surMutex);
	}

	void SDLDeviceScreen::releaseScreen() {
		clearWidgetPools();

		if (uId == NULL) {
			SDL_DestroyWindow(screen);
			screen = NULL;
		}
	}

	void SDLDeviceScreen::releaseMB() {
		int errCount = 0;

		while (numOfSDLScreens > 1) {
			::usleep(100000);
			errCount++;
			if (errCount > 5) {
				cout << "SDLDeviceScreen::releaseMB Warning! call to releaseMB";
				cout << " with undeleted SDLDeviceScreen. Please solve your ";
				cout << "program." << endl;
				break;
			}
		}

		SDL_Quit();
	}

	void SDLDeviceScreen::clearWidgetPools() {
		IWindow* iWin;
		ISurface* iSur;

		set<IWindow*>::iterator i;
		set<ISurface*>::iterator j;

		clog << "SDLDeviceScreen::clearWidgetPools ";
		clog << "windowPool size = " << windowPool->size();
		clog << ", surfacePool size = " << surfacePool->size() << endl;

		//Releasing remaining Window objects in Window Pool
		pthread_mutex_lock(&winMutex);
		if (windowPool != NULL) {
			i = windowPool->begin();
			while (i != windowPool->end()) {
				iWin = (*i);

				windowPool->erase(i);
				i = windowPool->begin();

				if (iWin != NULL) {
					pthread_mutex_unlock(&winMutex);
					delete iWin;
					pthread_mutex_lock(&winMutex);
				}
			}
		}
		pthread_mutex_unlock(&winMutex);

		//Releasing remaining Surface objects in Surface Pool
		pthread_mutex_lock(&surMutex);
		if (surfacePool != NULL) {
			j = surfacePool->begin();
			while (j != surfacePool->end()) {
				iSur = (*j);

				surfacePool->erase(j);
				j = surfacePool->begin();

				if (iSur != NULL) {
					pthread_mutex_unlock(&surMutex);
					delete iSur;
					pthread_mutex_lock(&surMutex);
				}
			}
			surfacePool->clear();
		}
		pthread_mutex_unlock(&surMutex);
	}

	void SDLDeviceScreen::setParentScreen(GingaWindowID parentId) {

	}

	void SDLDeviceScreen::setBackgroundImage(string uri) {

	}

	unsigned int SDLDeviceScreen::getWidthResolution() {
		return wRes;
	}

	void SDLDeviceScreen::setWidthResolution(unsigned int wRes) {
		this->wRes = wRes;
	}

	unsigned int SDLDeviceScreen::getHeightResolution() {
		return hRes;
	}

	void SDLDeviceScreen::setHeightResolution(unsigned int hRes) {
		this->hRes = hRes;
	}

	void SDLDeviceScreen::setColorKey(int r, int g, int b) {

	}

	void SDLDeviceScreen::mergeIds(
			GingaWindowID destId, vector<GingaWindowID>* srcIds) {

		SDL_Window* srcWin  = NULL;
		SDL_Window* dstWin  = NULL;
		SDL_Surface* srcSur = NULL;
		SDL_Surface* dstSur = NULL;
		vector<void*>::iterator i;
		int x, y;

		dstWin = getUnderlyingWindow(destId);
		if (dstWin == NULL) {
			return;
		}

		/*dstWin->GetSurface(dstWin, &dstSur);
		SDLCHECK(dstSur->SetBlittingFlags(
				dstSur, (SDLSurfaceBlittingFlags)DSBLIT_BLEND_ALPHACHANNEL));*/

		i = srcIds->begin();
		while (i != srcIds->end()) {
			srcWin = getUnderlyingWindow(*i);
			if (srcWin != NULL) {
				/*srcWin->GetPosition(srcWin, &x, &y);
				srcWin->GetSurface(srcWin, &srcSur);
				SDLCHECK(dstSur->Blit(dstSur, srcSur, NULL, x, y));*/
			}
			++i;
		}
	}


	/* interfacing output */

	IWindow* SDLDeviceScreen::createWindow(int x, int y, int w, int h) {
		IWindow* iWin;

		pthread_mutex_lock(&winMutex);
		iWin = new SDLWindow(NULL, NULL, id, x, y, w, h);
		windowPool->insert(iWin);
		pthread_mutex_unlock(&winMutex);

		return iWin;
	}

	IWindow* SDLDeviceScreen::createWindowFrom(GingaWindowID underlyingWindow) {
		IWindow* iWin = NULL;

		if (underlyingWindow != NULL) {
			pthread_mutex_lock(&winMutex);
			iWin = new SDLWindow(NULL, NULL, id, 0, 0, 0, 0);
			windowPool->insert(iWin);
			pthread_mutex_unlock(&winMutex);
		}

		return iWin;
	}

	void SDLDeviceScreen::releaseWindow(IWindow* win) {
		set<IWindow*>::iterator i;

		pthread_mutex_lock(&winMutex);
		if (windowPool != NULL) {
			i = windowPool->find(win);
			if (i != windowPool->end()) {
				windowPool->erase(i);
				pthread_mutex_unlock(&winMutex);

			} else {
				pthread_mutex_unlock(&winMutex);
			}

		} else {
			pthread_mutex_unlock(&winMutex);
		}
	}

	ISurface* SDLDeviceScreen::createSurface() {
		return createSurfaceFrom(NULL);
	}

	ISurface* SDLDeviceScreen::createSurface(int w, int h) {
		ISurface* iSur = NULL;

		pthread_mutex_lock(&surMutex);
		iSur = new SDLSurface(id, w, h);
		surfacePool->insert(iSur);
		pthread_mutex_unlock(&surMutex);

		return iSur;
	}

	ISurface* SDLDeviceScreen::createSurfaceFrom(void* uSur) {
		ISurface* iSur = NULL;

		pthread_mutex_lock(&surMutex);
		if (uSur != NULL) {
			iSur = new SDLSurface(id, uSur);

		} else {
			iSur = new SDLSurface(id);
		}

		surfacePool->insert(iSur);
		pthread_mutex_unlock(&surMutex);

		return iSur;
	}

	void SDLDeviceScreen::releaseSurface(ISurface* s) {
		set<ISurface*>::iterator i;

		pthread_mutex_lock(&surMutex);
		if (surfacePool != NULL) {
			i = surfacePool->find(s);
			if (i != surfacePool->end()) {
				surfacePool->erase(i);
				pthread_mutex_unlock(&surMutex);

			} else {
				pthread_mutex_unlock(&surMutex);
			}

		} else {
			pthread_mutex_unlock(&surMutex);
		}
	}


	/* interfacing content */
	IContinuousMediaProvider* SDLDeviceScreen::createContinuousMediaProvider(
			const char* mrl, bool hasVisual, bool isRemote) {

		IContinuousMediaProvider* provider;
		string strSym;

		if (hasVisual) {
			strSym = "SDLVideoProvider";

		} else {
			strSym = "SDLAudioProvider";
		}

		if (isRemote) {
			strSym = "SDL_FFmpegVideoProvider";
		}

		provider = ((CMPCreator*)(cm->getObject(strSym)))(id, mrl);

		provider->setLoadSymbol(strSym);
		return provider;
	}

	void SDLDeviceScreen::releaseContinuousMediaProvider(
			IContinuousMediaProvider* provider) {

		string strSym = provider->getLoadSymbol();

		delete provider;
		provider = NULL;

#if HAVE_COMPSUPPORT
		cm->releaseComponentFromObject(strSym);
#endif
	}

	IFontProvider* SDLDeviceScreen::createFontProvider(
			const char* mrl, int fontSize) {

		IFontProvider* provider = NULL;

#if HAVE_COMPSUPPORT
		provider = ((FontProviderCreator*)(cm->getObject("SDLFontProvider")))(
				id, mrl, fontSize);

#else
		provider = new SDLFontProvider(id, mrl, fontSize);
#endif

		return provider;
	}

	void SDLDeviceScreen::releaseFontProvider(IFontProvider* provider) {
		delete provider;
		provider = NULL;

#if HAVE_COMPSUPPORT
		cm->releaseComponentFromObject("SDLFontProvider");
#endif
	}

	IImageProvider* SDLDeviceScreen::createImageProvider(const char* mrl) {
		IImageProvider* provider = NULL;

#if HAVE_COMPSUPPORT
		provider = ((ImageProviderCreator*)(cm->getObject(
				"SDLImageProvider")))(id, mrl);
#else
		provider = new SDLImageProvider(id, mrl);
#endif

		return provider;
	}

	void SDLDeviceScreen::releaseImageProvider(IImageProvider* provider) {
		delete provider;
		provider = NULL;

#if HAVE_COMPSUPPORT
		cm->releaseComponentFromObject("SDLImageProvider");
#endif
	}

	ISurface* SDLDeviceScreen::createRenderedSurfaceFromImageFile(
			const char* mrl) {

		ISurface* iSur           = NULL;
		IImageProvider* provider = NULL;
		string strMrl            = "";

		provider = createImageProvider(mrl);

		if (provider != NULL) {
			strMrl.assign(mrl);
			if (strMrl.length() > 4 &&
					strMrl.substr(strMrl.length() - 4, 4) == ".gif") {

				iSur = provider->prepare(true);

			} else {
				iSur = provider->prepare(false);
			}
		}

		delete provider;

		return iSur;
	}


	/* interfacing input */

	IInputManager* SDLDeviceScreen::getInputManager() {
		return im;
	}

	void SDLDeviceScreen::setInputManager(IInputManager* im) {
		this->im = im;
	}

	IEventBuffer* SDLDeviceScreen::createEventBuffer() {
		return new SDLEventBuffer(id);
	}

	IInputEvent* SDLDeviceScreen::createInputEvent(
			void* event, const int symbol) {

		if (event != NULL) {
			return new SDLInputEvent(event);
		}

		if (symbol >= 0) {
			return new SDLInputEvent(symbol);
		}

		return NULL;
	}

	IInputEvent* SDLDeviceScreen::createUserEvent(int type, void* data) {
		return NULL;//new SDLInputEvent(DFEC_USER, type, data);
	}

	int SDLDeviceScreen::fromMBToGinga(int keyCode) {
		return 0;
	}

	int SDLDeviceScreen::fromGingaToMB(int keyCode) {
		return 0;
	}


	/* interfacing underlying multimedia system */

	void* SDLDeviceScreen::getGfxRoot() {
		return NULL;
	}

	SDL_Window* SDLDeviceScreen::getUnderlyingWindow(GingaWindowID winId) {
		SDL_Window* window = NULL;
		Uint32 wid;

		wid    = (Uint32)(unsigned long)winId;
		window = SDL_GetWindowFromID(wid);

		if (window == NULL) {
			clog << "SDLDeviceScreen::getUnderlyingWindow ";
			clog << "can't find id '" << wid;
			clog << "'" << endl;
		}

		return window;
	}

	SDL_Window* SDLDeviceScreen::createUnderlyingWindow(
			void* desc) {

		SDL_Window* window = NULL;

		return window;
	}

	void SDLDeviceScreen::releaseUnderlyingWindow(SDL_Window* uWin) {
		SDL_DestroyWindow(uWin);
	}

	SDL_Surface* SDLDeviceScreen::createUnderlyingSurface(
			void* desc) {

		SDL_Surface* surface = NULL;

		return surface;
	}

	void SDLDeviceScreen::releaseUnderlyingSurface(SDL_Surface* uSur) {

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
				GingaScreenID myId, GingaWindowID parentId) {

	return (new ::br::pucrio::telemidia::ginga::core::mb::
			SDLDeviceScreen(numArgs, args, myId, parentId));
}

extern "C" void destroySDLScreen(
		::br::pucrio::telemidia::ginga::core::mb::IDeviceScreen* ds) {

	delete ds;
}
