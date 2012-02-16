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

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
#if HAVE_COMPSUPPORT
	IComponentManager* SDLDeviceScreen::cm = IComponentManager::getCMInstance();
#endif

	map<SDLDeviceScreen*, bool> SDLDeviceScreen::sdlScreens;
	pthread_mutex_t SDLDeviceScreen::sMutex;
	bool SDLDeviceScreen::hasRenderer                 = false;

	pthread_mutex_t SDLDeviceScreen::ieMutex;
	map<int, int>* SDLDeviceScreen::gingaToSDLCodeMap = NULL;
	map<int, int>* SDLDeviceScreen::sdlToGingaCodeMap = NULL;

	const unsigned int SDLDeviceScreen::DSA_UNKNOWN = 0;
	const unsigned int SDLDeviceScreen::DSA_4x3     = 1;
	const unsigned int SDLDeviceScreen::DSA_16x9    = 2;

	SDLDeviceScreen::SDLDeviceScreen(
			int argc, char** args,
			GingaScreenID myId, GingaWindowID parentId) {

		int i;

		aspect      = DSA_UNKNOWN;
		hSize       = 0;
		vSize       = 0;
		hRes        = 0;
		wRes        = 0;
		im          = NULL;
		id          = myId;
		uId         = parentId;
		renderer    = NULL;
		windowPool  = new set<IWindow*>;
		surfacePool = new set<ISurface*>;
		cmpPool     = new map<IContinuousMediaProvider*, bool>;
		dmpPool     = new set<IDiscreteMediaProvider*>;
		mbMode      = "";
		mbSubSystem = "";

		for (i = 0; i < argc; i++) {
			if ((strcmp(args[i], "subsystem") == 0) && ((i + 1) < argc)) {
				mbSubSystem.assign(args[i + 1]);

			} else if ((strcmp(args[i], "mode") == 0) && ((i + 1) < argc)) {
				mbMode.assign(args[i + 1]);
			}
		}

		if (mbSubSystem == "dfb") {
			mbSubSystem = "directfb";
		}

		waitingCreator = false;
		pthread_mutex_init(&cMutex, NULL);
		pthread_cond_init(&cond, NULL);

		uSur        = NULL;
		uSurPending = false;
		pthread_mutex_init(&uSurMutex, NULL);

		pthread_mutex_init(&winMutex, NULL);
		pthread_mutex_init(&surMutex, NULL);
		pthread_mutex_init(&cmpMutex, NULL);
		pthread_mutex_init(&dmpMutex, NULL);

		if (!hasRenderer) {
			hasRenderer = true;
			pthread_mutex_init(&sMutex, NULL);
			initCodeMaps();

			pthread_t tId;
			pthread_attr_t tattr;

			pthread_attr_init(&tattr);
			pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED);
			pthread_attr_setscope(&tattr, PTHREAD_SCOPE_SYSTEM);

			pthread_create(&tId, &tattr, SDLDeviceScreen::rendererT, this);
			pthread_detach(tId);
		}

		pthread_mutex_lock(&sMutex);
		sdlScreens[this] = false;
		pthread_mutex_unlock(&sMutex);
	}

	SDLDeviceScreen::~SDLDeviceScreen() {
		map<SDLDeviceScreen*, bool>::iterator i;

		waitingCreator = false;
		pthread_mutex_destroy(&cMutex);
		pthread_cond_destroy(&cond);

		uSur        = NULL;
		uSurPending = false;
		pthread_mutex_destroy(&uSurMutex);

		pthread_mutex_lock(&sMutex);
		i = sdlScreens.find(this);
		if (i != sdlScreens.end()) {
			sdlScreens.erase(i);
		}
		pthread_mutex_unlock(&sMutex);

		releaseScreen();

		pthread_mutex_destroy(&winMutex);
		pthread_mutex_destroy(&surMutex);
		pthread_mutex_destroy(&cmpMutex);

		if (windowPool != NULL) {
			delete windowPool;
			windowPool = NULL;
		}

		if (surfacePool != NULL) {
			delete surfacePool;
			surfacePool = NULL;
		}

		if (cmpPool != NULL) {
			delete cmpPool;
			cmpPool = NULL;
		}
	}

	void SDLDeviceScreen::releaseScreen() {
		clearWidgetPools();

		if (renderer != NULL) {
			SDL_DestroyRenderer(renderer);
			renderer = NULL;
		}

		if (uId == NULL && screen != NULL) {
			SDL_DestroyWindow(screen);
			screen = NULL;
		}
	}

	void SDLDeviceScreen::releaseMB() {
		int errCount = 0;
		int numSDL;

		pthread_mutex_lock(&sMutex);
		numSDL = sdlScreens.size();
		pthread_mutex_unlock(&sMutex);

		while (numSDL > 1) {
			::usleep(100000);
			errCount++;

			pthread_mutex_lock(&sMutex);
			numSDL = sdlScreens.size();
			pthread_mutex_unlock(&sMutex);

			if (errCount > 5 || numSDL <= 1) {
				break;
			}
		}

		SDL_Quit();
	}

	void SDLDeviceScreen::clearWidgetPools() {
		IWindow* iWin;
		ISurface* iSur;
		IContinuousMediaProvider* iCmp;
		IDiscreteMediaProvider* iDmp;

		set<IWindow*>::iterator i;
		set<ISurface*>::iterator j;
		map<IContinuousMediaProvider*, bool>::iterator k;
		set<IDiscreteMediaProvider*>::iterator l;

		clog << "SDLDeviceScreen::clearWidgetPools ";
		clog << "windowPool size = " << windowPool->size();
		clog << ", surfacePool size = " << surfacePool->size();
		clog << endl;

		//Releasing remaining Window objects in Window Pool
		pthread_mutex_lock(&winMutex);
		if (windowPool != NULL) {
			i = windowPool->begin();
			while (i != windowPool->end()) {
				iWin = (*i);

				windowPool->erase(i);
				if (iWin != NULL) {
					pthread_mutex_unlock(&winMutex);
					delete iWin;
					pthread_mutex_lock(&winMutex);
				}
				i = windowPool->begin();
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
				if (iSur != NULL) {
					pthread_mutex_unlock(&surMutex);
					delete iSur;
					pthread_mutex_lock(&surMutex);
				}
				j = surfacePool->begin();
			}
			surfacePool->clear();
		}
		pthread_mutex_unlock(&surMutex);

		//Releasing remaining CMP objects in CMP Pool
		pthread_mutex_lock(&cmpMutex);
		if (cmpPool != NULL) {
			k = cmpPool->begin();
			while (k != cmpPool->end()) {
				iCmp = k->first;

				cmpPool->erase(k);
				if (iCmp != NULL) {
					pthread_mutex_unlock(&cmpMutex);
					delete iCmp;
					pthread_mutex_lock(&cmpMutex);
				}
				k = cmpPool->begin();
			}
			cmpPool->clear();
		}
		pthread_mutex_unlock(&cmpMutex);

		//Releasing remaining DMP objects in DMP Pool
		pthread_mutex_lock(&dmpMutex);
		if (dmpPool != NULL) {
			l = dmpPool->begin();
			while (l != dmpPool->end()) {
				iDmp = *l;

				dmpPool->erase(l);
				if (iDmp != NULL) {
					pthread_mutex_unlock(&dmpMutex);
					delete iDmp;
					pthread_mutex_lock(&dmpMutex);
				}
				l = dmpPool->begin();
			}
			dmpPool->clear();
		}
		pthread_mutex_unlock(&dmpMutex);
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

		if (screen != NULL) {
			SDL_SetWindowSize(screen, this->wRes, this->hRes);
		}
	}

	unsigned int SDLDeviceScreen::getHeightResolution() {
		return hRes;
	}

	void SDLDeviceScreen::setHeightResolution(unsigned int hRes) {
		this->hRes = hRes;

		if (screen != NULL) {
			SDL_SetWindowSize(screen, this->wRes, this->hRes);
		}
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
			iWin = new SDLWindow(underlyingWindow, NULL, id, 0, 0, 0, 0);
			windowPool->insert(iWin);
			pthread_mutex_unlock(&winMutex);
		}

		return iWin;
	}

	bool SDLDeviceScreen::hasWindow(IWindow* win) {
		set<IWindow*>::iterator i;
		bool hasWin = false;

		pthread_mutex_lock(&winMutex);
		if (windowPool != NULL) {
			i = windowPool->find(win);
			if (i != windowPool->end()) {
				hasWin = true;
				pthread_mutex_unlock(&winMutex);

			} else {
				pthread_mutex_unlock(&winMutex);
			}

		} else {
			pthread_mutex_unlock(&winMutex);
		}

		return hasWin;
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
		ISurface* iSur    = NULL;
		SDL_Surface* uSur = NULL;

		uSur = createUnderlyingSurface(w, h);

		pthread_mutex_lock(&surMutex);
		iSur = new SDLSurface(id, uSur);
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

	bool SDLDeviceScreen::hasSurface(ISurface* s) {
		set<ISurface*>::iterator i;
		bool hasSur = false;

		pthread_mutex_lock(&surMutex);
		if (surfacePool != NULL) {
			i = surfacePool->find(s);
			if (i != surfacePool->end()) {
				hasSur = true;
				pthread_mutex_unlock(&surMutex);

			} else {
				pthread_mutex_unlock(&surMutex);
			}

		} else {
			pthread_mutex_unlock(&surMutex);
		}

		return hasSur;
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

	bool SDLDeviceScreen::refreshScreen() {
		/*vector<IWindow*> renderList;
		vector<IWindow*>::iterator i;
		set<IContinuousMediaProvider*>::iterator j;
		SDL_Texture* texture;
		SDL_Rect rect;


		pthread_mutex_lock(&cmpMutex);
		j = cmpPool->begin();
		while (j != cmpPool->end()) {
			(*j)->refreshDR();
			++j;
		}
		pthread_mutex_unlock(&cmpMutex);

		pthread_mutex_lock(&winMutex);
		SDL_RenderClear(renderer);
		if (getRenderList(&renderList)) {
			i = renderList.begin();
			while (i != renderList.end()) {
				rect.x  = (*i)->getX();
				rect.y  = (*i)->getY();
				rect.w  = (*i)->getW();
				rect.h  = (*i)->getH();
				texture = (SDL_Texture*)((*i)->getContent());

				SDL_RenderCopy(renderer, texture, NULL, &rect);
				++i;
			}
			SDL_RenderPresent(renderer);
		}
		pthread_mutex_unlock(&winMutex);*/

		return true;
	}


	/* interfacing content */
	IContinuousMediaProvider* SDLDeviceScreen::createContinuousMediaProvider(
			const char* mrl, bool hasVisual, bool isRemote) {

		IContinuousMediaProvider* provider;
		string strSym;

		pthread_mutex_lock(&cmpMutex);
		if (hasVisual) {
			strSym = "SDLVideoProvider";

		} else {
			strSym = "SDLAudioProvider";
		}

		provider = ((CMPCreator*)(cm->getObject(strSym)))(id, mrl);
		provider->setLoadSymbol(strSym);

		(*cmpPool)[provider] = false;

		pthread_mutex_unlock(&cmpMutex);
		return provider;
	}

	void SDLDeviceScreen::releaseContinuousMediaProvider(
			IContinuousMediaProvider* provider) {

		map<IContinuousMediaProvider*, bool>::iterator i;
		string strSym;

		pthread_mutex_lock(&cmpMutex);
		i = cmpPool->find(provider);
		if (i != cmpPool->end()) {
			cmpPool->erase(i);
			strSym = provider->getLoadSymbol();
			provider->stop();

			delete provider;
			provider = NULL;

#if HAVE_COMPSUPPORT
			cm->releaseComponentFromObject(strSym);
#endif
		}
		pthread_mutex_unlock(&cmpMutex);
	}

	IFontProvider* SDLDeviceScreen::createFontProvider(
			const char* mrl, int fontSize) {

		IFontProvider* provider = NULL;

		pthread_mutex_lock(&cmpMutex);

#if HAVE_COMPSUPPORT
		provider = ((FontProviderCreator*)(cm->getObject("SDLFontProvider")))(
				id, mrl, fontSize);

#else
		provider = new SDLFontProvider(id, mrl, fontSize);
#endif

		dmpPool->insert(provider);
		pthread_mutex_unlock(&cmpMutex);

		return provider;
	}

	void SDLDeviceScreen::releaseFontProvider(IFontProvider* provider) {
		set<IDiscreteMediaProvider*>::iterator i;

		pthread_mutex_lock(&dmpMutex);
		i = dmpPool->find(provider);
		if (i != dmpPool->end()) {
			dmpPool->erase(i);

			delete provider;
			provider = NULL;

#if HAVE_COMPSUPPORT
			cm->releaseComponentFromObject("SDLFontProvider");
#endif
		}
		pthread_mutex_unlock(&dmpMutex);
	}

	IImageProvider* SDLDeviceScreen::createImageProvider(const char* mrl) {
		IImageProvider* provider = NULL;

		pthread_mutex_lock(&dmpMutex);
#if HAVE_COMPSUPPORT
		provider = ((ImageProviderCreator*)(cm->getObject(
				"SDLImageProvider")))(id, mrl);
#else
		provider = new SDLImageProvider(id, mrl);
#endif

		dmpPool->insert(provider);
		pthread_mutex_unlock(&dmpMutex);

		return provider;
	}

	void SDLDeviceScreen::releaseImageProvider(IImageProvider* provider) {
		set<IDiscreteMediaProvider*>::iterator i;

		pthread_mutex_lock(&dmpMutex);
		i = dmpPool->find(provider);
		if (i != dmpPool->end()) {
			dmpPool->erase(i);

			delete provider;
			provider = NULL;

#if HAVE_COMPSUPPORT
			cm->releaseComponentFromObject("SDLImageProvider");
#endif
		}

		pthread_mutex_unlock(&dmpMutex);
	}

	ISurface* SDLDeviceScreen::createRenderedSurfaceFromImageFile(
			const char* mrl) {

		ISurface* iSur           = NULL;
		IImageProvider* provider = NULL;

		provider = createImageProvider(mrl);
		if (provider != NULL) {
			iSur = createSurfaceFrom(NULL);
			provider->playOver(iSur);
		}

		releaseImageProvider(provider);

		return iSur;
	}

	bool SDLDeviceScreen::checkTasks(SDLDeviceScreen* s) {
		Uint32 rmask, gmask, bmask, amask;

		if (!s->uSurPending) {
			return false;
		}

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		rmask = 0xff000000;
		gmask = 0x00ff0000;
		bmask = 0x0000ff00;
		amask = 0x000000ff;
#else
		rmask = 0x000000ff;
		gmask = 0x0000ff00;
		bmask = 0x00ff0000;
		amask = 0xff000000;
#endif

		clog << "SDLDeviceScreen::checkTasks creating surface with w = '";
		clog << s->uSurW << "' and h = '" << s->uSurH << endl;

		std::flush(cout);

		s->uSur = SDL_CreateRGBSurface(0, s->uSurW, s->uSurH, 24, 0, 0, 0, 0);

		//s->uSur = SDL_CreateRGBSurface(
		//		0, s->uSurW, s->uSurH, 32, rmask, gmask, bmask, amask);

		s->uSurPending = false;
		s->surfaceCreator();

		return true;
	}

	void SDLDeviceScreen::refreshCMP(SDLDeviceScreen* s) {
		map<IContinuousMediaProvider*, bool>::iterator i;

		pthread_mutex_lock(&s->cmpMutex);
		i = s->cmpPool->begin();
		while (i != s->cmpPool->end()) {
			if (i->second) {
				i->first->refreshDR();
				++i;

			} else {
				initCMP(s, i->first);
				(*s->cmpPool)[i->first] = true;
				i = s->cmpPool->begin();
			}
		}
		pthread_mutex_unlock(&s->cmpMutex);
	}

	void SDLDeviceScreen::refreshDMP(SDLDeviceScreen* s) {
		set<IDiscreteMediaProvider*>::iterator i;
		IDiscreteMediaProvider* dmp;
		ISurface* surface;

		pthread_mutex_lock(&s->dmpMutex);
		i = s->dmpPool->begin();
		while (i != s->dmpPool->end()) {
			dmp = (*i);
			surface = (ISurface*)(dmp->getContent());
			if (surface != NULL) {
				dmp->ntsPlayOver(surface);
			}
			++i;
		}
		pthread_mutex_unlock(&s->dmpMutex);
	}

	void SDLDeviceScreen::refreshWin(SDLDeviceScreen* s) {
		SDL_Surface* uSur;
		SDL_Texture* uTex;
		bool ownTex = false;
		SDL_Rect rect;
		SDLWindow* win;

		vector<IWindow*> renderList;
		vector<IWindow*>::iterator i;

		pthread_mutex_lock(&s->winMutex);
		SDL_RenderClear(s->renderer);
		if (s->getRenderList(&renderList)) {
			i = renderList.begin();
			while (i != renderList.end()) {
				win    = (SDLWindow*)(*i);
				rect.x = win->getX();
				rect.y = win->getY();
				rect.w = win->getW();
				rect.h = win->getH();
				uSur   = (SDL_Surface*)(win->getContent());

				if (uSur != NULL) {
					ownTex = false;
					uTex   = createTexture(s->renderer, uSur);

				} else {
					ownTex = true;
					uTex   = win->getTexture();
				}

				if (uTex != NULL) {
					SDL_RenderCopy(s->renderer, uTex, NULL, &rect);
					if (!ownTex) {
						releaseTexture(uTex);
						ownTex = false;
					}
					uTex = NULL;
				}
				++i;
			}
			SDL_RenderPresent(s->renderer);
		}
		pthread_mutex_unlock(&s->winMutex);
	}

	void* SDLDeviceScreen::rendererT(void* ptr) {
		map<SDLDeviceScreen*, bool>::iterator i;
		int sleepTime, elapsedTime;
		bool hasTasks;

		sleepTime = (int)(1000000/30);

		SDL_Init((Uint32)(
				SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE));

		while (hasRenderer) {
			elapsedTime = getCurrentTimeMillis();
			pthread_mutex_lock(&sMutex);
			i = sdlScreens.begin();
			while (i != sdlScreens.end()) {
				if (i->second) {
					do {

						hasTasks = checkTasks(i->first);

					} while(hasTasks);

					refreshCMP(i->first);
					refreshDMP(i->first);
					refreshWin(i->first);

					++i;

				} else {
					initScreen(i->first);
					sdlScreens[i->first] = true;
					i = sdlScreens.begin();
				}
			}
			pthread_mutex_unlock(&sMutex);

			elapsedTime = (getCurrentTimeMillis() - elapsedTime) * 1000;

			if (elapsedTime < sleepTime) {
				::usleep(sleepTime - elapsedTime);
			}
		}

		return NULL;
	}

	void SDLDeviceScreen::initScreen(SDLDeviceScreen* s) {
		SDL_Rect rect;
		int i, numOfDrivers, x, y;
		string title = "";

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

		if (s->uId != NULL) {
			s->screen = SDL_CreateWindowFrom(s->uId);
			if (s->screen != NULL) {
				SDL_GetWindowSize(s->screen, &s->wRes, &s->hRes);
			}

		} else {
			title.assign((char*)VERSION);
			title = "Ginga v" + title;

			if (s->mbMode != "" && s->mbMode.find("x") != std::string::npos) {
				s->wRes = (int)stof(
						s->mbMode.substr(0, s->mbMode.find_first_of("x")));

				s->hRes = (int)stof(
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

			s->screen = SDL_CreateWindow(
					title.c_str(), x, y, s->wRes, s->hRes, 0);
		}

		if (s->screen != NULL) {
			s->renderer = SDL_CreateRenderer(
					s->screen, -1, SDL_RENDERER_ACCELERATED);

			if (s->renderer == NULL) {
				s->renderer = SDL_CreateRenderer(
						s->screen, -1, SDL_RENDERER_SOFTWARE);
			}
		}
	}

	void SDLDeviceScreen::initCMP(
			SDLDeviceScreen* s, IContinuousMediaProvider* cmp) {

		SDL_Texture* texture;
		int w, h;

		cmp->getOriginalResolution(&w, &h);

		clog << "SDLDeviceScreen::initCMP creating texture with w = '";
		clog << w << "' and h = '" << h << "'" << endl;

		texture = SDL_CreateTexture(
				s->renderer,
				SDL_PIXELFORMAT_RGB24,
				SDL_TEXTUREACCESS_STREAMING,
				w,
				h);

		cmp->setContent((void*)texture);
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
			return new SDLInputEvent(*(SDL_Event*)event);
		}

		if (symbol >= 0) {
			return new SDLInputEvent(symbol);
		}

		return NULL;
	}

	IInputEvent* SDLDeviceScreen::createApplicationEvent(int type, void* data) {
		return new SDLInputEvent(type, data);
	}

	int SDLDeviceScreen::fromMBToGinga(int keyCode) {
		map<int, int>::iterator i;
		int translated = CodeMap::KEY_NULL;

		pthread_mutex_lock(&ieMutex);
		i = sdlToGingaCodeMap->find(keyCode);
		if (i != sdlToGingaCodeMap->end()) {
			translated = i->second;
		}
		pthread_mutex_unlock(&ieMutex);

		return translated;
	}

	int SDLDeviceScreen::fromGingaToMB(int keyCode) {
		map<int, int>::iterator i;
		int translated = CodeMap::KEY_NULL;

		pthread_mutex_lock(&ieMutex);
		i = gingaToSDLCodeMap->find(keyCode);
		if (i != gingaToSDLCodeMap->end()) {
			translated = i->second;
		}
		pthread_mutex_unlock(&ieMutex);

		return translated;
	}


	/* interfacing underlying multimedia system */

	void* SDLDeviceScreen::getGfxRoot() {
		return renderer;
	}

	/* libgingaccmbdfb internal use*/

	/* input */
	void SDLDeviceScreen::initCodeMaps() {
		if (gingaToSDLCodeMap != NULL) {
			return;
		}

		gingaToSDLCodeMap = new map<int, int>;
		sdlToGingaCodeMap = new map<int, int>;
		pthread_mutex_init(&ieMutex, NULL);

		(*gingaToSDLCodeMap)[CodeMap::KEY_NULL]              = SDLK_UNKNOWN;
		(*gingaToSDLCodeMap)[CodeMap::KEY_0]                 = SDLK_0;
		(*gingaToSDLCodeMap)[CodeMap::KEY_1]                 = SDLK_1;
		(*gingaToSDLCodeMap)[CodeMap::KEY_2]                 = SDLK_2;
		(*gingaToSDLCodeMap)[CodeMap::KEY_3]                 = SDLK_3;
		(*gingaToSDLCodeMap)[CodeMap::KEY_4]                 = SDLK_4;
		(*gingaToSDLCodeMap)[CodeMap::KEY_5]                 = SDLK_5;
		(*gingaToSDLCodeMap)[CodeMap::KEY_6]                 = SDLK_6;
		(*gingaToSDLCodeMap)[CodeMap::KEY_7]                 = SDLK_7;
		(*gingaToSDLCodeMap)[CodeMap::KEY_8]                 = SDLK_8;
		(*gingaToSDLCodeMap)[CodeMap::KEY_9]                 = SDLK_9;

		(*gingaToSDLCodeMap)[CodeMap::KEY_SMALL_A]           = SDLK_a;
		(*gingaToSDLCodeMap)[CodeMap::KEY_SMALL_B]           = SDLK_b;
		(*gingaToSDLCodeMap)[CodeMap::KEY_SMALL_C]           = SDLK_c;
		(*gingaToSDLCodeMap)[CodeMap::KEY_SMALL_D]           = SDLK_d;
		(*gingaToSDLCodeMap)[CodeMap::KEY_SMALL_E]           = SDLK_e;
		(*gingaToSDLCodeMap)[CodeMap::KEY_SMALL_F]           = SDLK_f;
		(*gingaToSDLCodeMap)[CodeMap::KEY_SMALL_G]           = SDLK_g;
		(*gingaToSDLCodeMap)[CodeMap::KEY_SMALL_H]           = SDLK_h;
		(*gingaToSDLCodeMap)[CodeMap::KEY_SMALL_I]           = SDLK_i;
		(*gingaToSDLCodeMap)[CodeMap::KEY_SMALL_J]           = SDLK_j;
		(*gingaToSDLCodeMap)[CodeMap::KEY_SMALL_K]           = SDLK_k;
		(*gingaToSDLCodeMap)[CodeMap::KEY_SMALL_L]           = SDLK_l;
		(*gingaToSDLCodeMap)[CodeMap::KEY_SMALL_M]           = SDLK_m;
		(*gingaToSDLCodeMap)[CodeMap::KEY_SMALL_N]           = SDLK_n;
		(*gingaToSDLCodeMap)[CodeMap::KEY_SMALL_O]           = SDLK_o;
		(*gingaToSDLCodeMap)[CodeMap::KEY_SMALL_P]           = SDLK_p;
		(*gingaToSDLCodeMap)[CodeMap::KEY_SMALL_Q]           = SDLK_q;
		(*gingaToSDLCodeMap)[CodeMap::KEY_SMALL_R]           = SDLK_r;
		(*gingaToSDLCodeMap)[CodeMap::KEY_SMALL_S]           = SDLK_s;
		(*gingaToSDLCodeMap)[CodeMap::KEY_SMALL_T]           = SDLK_t;
		(*gingaToSDLCodeMap)[CodeMap::KEY_SMALL_U]           = SDLK_u;
		(*gingaToSDLCodeMap)[CodeMap::KEY_SMALL_V]           = SDLK_v;
		(*gingaToSDLCodeMap)[CodeMap::KEY_SMALL_W]           = SDLK_w;
		(*gingaToSDLCodeMap)[CodeMap::KEY_SMALL_X]           = SDLK_x;
		(*gingaToSDLCodeMap)[CodeMap::KEY_SMALL_Y]           = SDLK_y;
		(*gingaToSDLCodeMap)[CodeMap::KEY_SMALL_Z]           = SDLK_z;

		/*(*gingaToSDLCodeMap)[CodeMap::KEY_CAPITAL_A]         = SDLK_a;
		(*gingaToSDLCodeMap)[CodeMap::KEY_CAPITAL_B]         = SDLK_b;
		(*gingaToSDLCodeMap)[CodeMap::KEY_CAPITAL_C]         = SDLK_c;
		(*gingaToSDLCodeMap)[CodeMap::KEY_CAPITAL_D]         = SDLK_d;
		(*gingaToSDLCodeMap)[CodeMap::KEY_CAPITAL_E]         = SDLK_e;
		(*gingaToSDLCodeMap)[CodeMap::KEY_CAPITAL_F]         = SDLK_f;
		(*gingaToSDLCodeMap)[CodeMap::KEY_CAPITAL_G]         = SDLK_g;
		(*gingaToSDLCodeMap)[CodeMap::KEY_CAPITAL_H]         = SDLK_h;
		(*gingaToSDLCodeMap)[CodeMap::KEY_CAPITAL_I]         = SDLK_i;
		(*gingaToSDLCodeMap)[CodeMap::KEY_CAPITAL_J]         = SDLK_j;
		(*gingaToSDLCodeMap)[CodeMap::KEY_CAPITAL_K]         = SDLK_k;
		(*gingaToSDLCodeMap)[CodeMap::KEY_CAPITAL_L]         = SDLK_l;
		(*gingaToSDLCodeMap)[CodeMap::KEY_CAPITAL_M]         = SDLK_m;
		(*gingaToSDLCodeMap)[CodeMap::KEY_CAPITAL_N]         = SDLK_n;
		(*gingaToSDLCodeMap)[CodeMap::KEY_CAPITAL_O]         = SDLK_o;
		(*gingaToSDLCodeMap)[CodeMap::KEY_CAPITAL_P]         = SDLK_p;
		(*gingaToSDLCodeMap)[CodeMap::KEY_CAPITAL_Q]         = SDLK_q;
		(*gingaToSDLCodeMap)[CodeMap::KEY_CAPITAL_R]         = SDLK_r;
		(*gingaToSDLCodeMap)[CodeMap::KEY_CAPITAL_S]         = SDLK_s;
		(*gingaToSDLCodeMap)[CodeMap::KEY_CAPITAL_T]         = SDLK_t;
		(*gingaToSDLCodeMap)[CodeMap::KEY_CAPITAL_U]         = SDLK_u;
		(*gingaToSDLCodeMap)[CodeMap::KEY_CAPITAL_V]         = SDLK_v;
		(*gingaToSDLCodeMap)[CodeMap::KEY_CAPITAL_W]         = SDLK_w;
		(*gingaToSDLCodeMap)[CodeMap::KEY_CAPITAL_X]         = SDLK_x;
		(*gingaToSDLCodeMap)[CodeMap::KEY_CAPITAL_Y]         = SDLK_y;
		(*gingaToSDLCodeMap)[CodeMap::KEY_CAPITAL_Z]         = SDLK_z;*/

		(*gingaToSDLCodeMap)[CodeMap::KEY_PAGE_DOWN]         = SDLK_PAGEDOWN;
		(*gingaToSDLCodeMap)[CodeMap::KEY_PAGE_UP]           = SDLK_PAGEUP;

		(*gingaToSDLCodeMap)[CodeMap::KEY_F1]                = SDLK_F1;
		(*gingaToSDLCodeMap)[CodeMap::KEY_F2]                = SDLK_F2;
		(*gingaToSDLCodeMap)[CodeMap::KEY_F3]                = SDLK_F3;
		(*gingaToSDLCodeMap)[CodeMap::KEY_F4]                = SDLK_F4;
		(*gingaToSDLCodeMap)[CodeMap::KEY_F5]                = SDLK_F5;
		(*gingaToSDLCodeMap)[CodeMap::KEY_F6]                = SDLK_F6;
		(*gingaToSDLCodeMap)[CodeMap::KEY_F7]                = SDLK_F7;
		(*gingaToSDLCodeMap)[CodeMap::KEY_F8]                = SDLK_F8;
		(*gingaToSDLCodeMap)[CodeMap::KEY_F9]                = SDLK_F9;
		(*gingaToSDLCodeMap)[CodeMap::KEY_F10]               = SDLK_F10;
		(*gingaToSDLCodeMap)[CodeMap::KEY_F11]               = SDLK_F11;
		(*gingaToSDLCodeMap)[CodeMap::KEY_F12]               = SDLK_F12;

		(*gingaToSDLCodeMap)[CodeMap::KEY_PLUS_SIGN]         = SDLK_PLUS;
		(*gingaToSDLCodeMap)[CodeMap::KEY_MINUS_SIGN]        = SDLK_MINUS;

		(*gingaToSDLCodeMap)[CodeMap::KEY_ASTERISK]          = SDLK_ASTERISK;
		(*gingaToSDLCodeMap)[CodeMap::KEY_NUMBER_SIGN]       = SDLK_HASH;

		(*gingaToSDLCodeMap)[CodeMap::KEY_PERIOD]            = SDLK_PERIOD;

		(*gingaToSDLCodeMap)[CodeMap::KEY_SUPER]             = SDLK_F13;
		(*gingaToSDLCodeMap)[CodeMap::KEY_PRINTSCREEN]       = SDLK_PRINTSCREEN;
		(*gingaToSDLCodeMap)[CodeMap::KEY_MENU]              = SDLK_MENU;
		(*gingaToSDLCodeMap)[CodeMap::KEY_INFO]              = SDLK_F14;
		(*gingaToSDLCodeMap)[CodeMap::KEY_EPG]               = SDLK_QUESTION;

		(*gingaToSDLCodeMap)[CodeMap::KEY_CURSOR_DOWN]       = SDLK_DOWN;
		(*gingaToSDLCodeMap)[CodeMap::KEY_CURSOR_LEFT]       = SDLK_LEFT;
		(*gingaToSDLCodeMap)[CodeMap::KEY_CURSOR_RIGHT]      = SDLK_RIGHT;
		(*gingaToSDLCodeMap)[CodeMap::KEY_CURSOR_UP]         = SDLK_UP;

		(*gingaToSDLCodeMap)[CodeMap::KEY_CHANNEL_DOWN]      = SDLK_F15;
		(*gingaToSDLCodeMap)[CodeMap::KEY_CHANNEL_UP]        = SDLK_F16;

		(*gingaToSDLCodeMap)[CodeMap::KEY_VOLUME_DOWN]       = SDLK_VOLUMEDOWN;
		(*gingaToSDLCodeMap)[CodeMap::KEY_VOLUME_UP]         = SDLK_VOLUMEUP;

		(*gingaToSDLCodeMap)[CodeMap::KEY_ENTER]             = SDLK_RETURN;
		(*gingaToSDLCodeMap)[CodeMap::KEY_OK]                = SDLK_RETURN2;

		(*gingaToSDLCodeMap)[CodeMap::KEY_RED]               = SDLK_F17;
		(*gingaToSDLCodeMap)[CodeMap::KEY_GREEN]             = SDLK_F18;
		(*gingaToSDLCodeMap)[CodeMap::KEY_YELLOW]            = SDLK_F19;
		(*gingaToSDLCodeMap)[CodeMap::KEY_BLUE]              = SDLK_F20;

		(*gingaToSDLCodeMap)[CodeMap::KEY_BACKSPACE]         = SDLK_BACKSPACE;
		(*gingaToSDLCodeMap)[CodeMap::KEY_BACK]              = SDLK_AC_BACK;
		(*gingaToSDLCodeMap)[CodeMap::KEY_ESCAPE]            = SDLK_ESCAPE;
		(*gingaToSDLCodeMap)[CodeMap::KEY_EXIT]              = SDLK_OUT;

		(*gingaToSDLCodeMap)[CodeMap::KEY_POWER]             = SDLK_POWER;
		(*gingaToSDLCodeMap)[CodeMap::KEY_REWIND]            = SDLK_F21;
		(*gingaToSDLCodeMap)[CodeMap::KEY_STOP]              = SDLK_STOP;
		(*gingaToSDLCodeMap)[CodeMap::KEY_EJECT]             = SDLK_EJECT;
		(*gingaToSDLCodeMap)[CodeMap::KEY_PLAY]              = SDLK_EXECUTE;
		(*gingaToSDLCodeMap)[CodeMap::KEY_RECORD]            = SDLK_F22;
		(*gingaToSDLCodeMap)[CodeMap::KEY_PAUSE]             = SDLK_PAUSE;

		(*gingaToSDLCodeMap)[CodeMap::KEY_GREATER_THAN_SIGN] = SDLK_GREATER;
		(*gingaToSDLCodeMap)[CodeMap::KEY_LESS_THAN_SIGN]    = SDLK_LESS;

		(*gingaToSDLCodeMap)[CodeMap::KEY_TAP]               = SDLK_F23;

        map<int, int>::iterator i;
        i = gingaToSDLCodeMap->begin();
        while (i != gingaToSDLCodeMap->end()) {
		    (*sdlToGingaCodeMap)[i->second] = i->first;
		    ++i;
        }
	}

	/* output */
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

	SDL_Texture* SDLDeviceScreen::createTexture(
			SDL_Renderer* renderer, SDL_Surface* surface) {

		SDL_Texture* texture;

	    texture = SDL_CreateTextureFromSurface(renderer, surface);
	    if (!texture) {
	        clog << "SDLDeviceScreen::createTexture";
	        clog << "Couldn't create texture: " << SDL_GetError();
	        clog << endl;
	        return NULL;
	    }

	    /* allowing alpha */
	    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

		return texture;
	}

	void SDLDeviceScreen::releaseTexture(SDL_Texture* texture) {
		SDL_DestroyTexture(texture);
	}

	SDL_Surface* SDLDeviceScreen::createUnderlyingSurface(
			int width, int height) {

		pthread_mutex_lock(&uSurMutex);
		SDL_Surface* newUSur = NULL;

		uSurW       = width;
		uSurH       = height;
		uSur        = NULL;
		uSurPending = true;
		waitSurfaceCreator();

		newUSur = uSur;
		pthread_mutex_unlock(&uSurMutex);

		return newUSur;
	}

	void SDLDeviceScreen::releaseUnderlyingSurface(SDL_Surface* uSur) {
		SDL_FreeSurface(uSur);
	}

	bool SDLDeviceScreen::getRenderList(vector<IWindow*>* renderList) {
		IWindow* win;
		set<IWindow*>::iterator i;

		if (windowPool != NULL) {
			i = windowPool->begin();
			while (i != windowPool->end()) {
				win = *i;
				if (win->isVisible() && (win->getContent() != NULL ||
						((SDLWindow*)win)->getTexture() != NULL)) {

					renderList->push_back(win);
				}
				++i;
			}
		}

		return !renderList->empty();
	}

	void SDLDeviceScreen::waitSurfaceCreator() {
		waitingCreator = true;
		pthread_mutex_lock(&cMutex);
		pthread_cond_wait(&cond, &cMutex);
		waitingCreator = false;
		pthread_mutex_unlock(&cMutex);
	}

	bool SDLDeviceScreen::surfaceCreator() {
		if (waitingCreator) {
			pthread_cond_signal(&cond);
			return true;
		}
		return false;
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
