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

#include "mb/interface/dfb/DFBDeviceScreen.h"

#include "mb/interface/dfb/output/DFBWindow.h"
#include "mb/interface/dfb/output/DFBSurface.h"

#include "mb/interface/dfb/input/DFBEventBuffer.h"
#include "mb/interface/dfb/input/DFBGInputEvent.h"

#include "mb/interface/CodeMap.h"
#include "mb/ILocalScreenManager.h"
#include "mb/InputManager.h"

#if !HAVE_COMPSUPPORT
#include "mb/interface/dfb/content/audio/DFBAudioProvider.h"

#if HAVE_FUSIONSOUND
#include "mb/interface/dfb/content/audio/FusionSoundAudioProvider.h"
#endif //HAVE_FUSIONSOUND

#include "mb/interface/dfb/content/image/DFBImageProvider.h"
#include "mb/interface/dfb/content/text/DFBFontProvider.h"
#include "mb/interface/dfb/content/video/DFBVideoProvider.h"
#endif //HAVE_COMPSUPPORT

#include <string.h>
#include <stdlib.h>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
#if HAVE_COMPSUPPORT
	IComponentManager* DFBDeviceScreen::cm = IComponentManager::getCMInstance();
#endif

	const unsigned int DFBDeviceScreen::DSA_UNKNOWN   = 0;
	const unsigned int DFBDeviceScreen::DSA_4x3       = 1;
	const unsigned int DFBDeviceScreen::DSA_16x9      = 2;

	pthread_mutex_t DFBDeviceScreen::ieMutex;
	map<int, int> DFBDeviceScreen::gingaToDFBCodeMap;
	map<int, int> DFBDeviceScreen::dfbToGingaCodeMap;

	unsigned int DFBDeviceScreen::numOfDFBScreens     = 0;
	IDirectFB* DFBDeviceScreen::dfb                   = NULL;
	IDirectFBDisplayLayer* DFBDeviceScreen::gfxLayer  = NULL;

	DFBDeviceScreen::DFBDeviceScreen(
			int argc, char** argv,
			GingaScreenID myId, GingaWindowID embedId,
			bool externalRenderer) {

		DFBDisplayLayerConfig layer_config;
		DFBResult ret;
		int i;

		numOfDFBScreens++;

		aSystem     = "";
		aspect      = DSA_UNKNOWN;
		hSize       = 0;
		vSize       = 0;
		hRes        = 0;
		wRes        = 0;
		im          = NULL;
		id          = myId;

		Thread::mutexInit(&winMutex);
		Thread::mutexInit(&surMutex);
		Thread::mutexInit(&cmpMutex);

		for (i = 0; i < argc; i++) {
			if ((strcmp(argv[i], "audio") == 0) && ((i + 1) < argc)) {
				aSystem.assign(argv[i + 1]);
			}
		}

		if (aSystem == "xine") {
			aSystem = "DFBAudioProvider";

		} else if (aSystem == "fusionsound") {
			aSystem = "FusionSoundAudioProvider";

		} else {
			if (aSystem != "") {
				cout << "DFBDeviceScreen::DFBDeviceScreen Warning! Not ";
				cout << "supported audio system: '" << aSystem << "'! Using ";
				cout << "FusionSound instead." << endl;
			}
			aSystem = "FusionSoundAudioProvider";
		}

		if (DFBDeviceScreen::dfb == NULL) {
			DFBCHECK(DirectFBInit(&argc, &argv));

			if (embedId != NULL) {
				setUnderlyingRootWindow(embedId);
			}

			DFBCHECK(DirectFBCreate(&dfb));

			if (gfxLayer == NULL) {
				DFBCHECK(dfb->GetDisplayLayer(
						dfb, DLID_PRIMARY, &gfxLayer));
			}

			ret = gfxLayer->SetCooperativeLevel(gfxLayer, DLSCL_ADMINISTRATIVE);
			if (ret) {
				DirectFBError(
						"DFBDeviceScreen cooperative level error: ",
						ret);
			}

			DFBCHECK(gfxLayer->GetConfiguration(gfxLayer, &layer_config));

			wRes = layer_config.width;
			hRes = layer_config.height;

			/*if ((ret = gfxLayer->SetCooperativeLevel(
			  gfxLayer, DLSCL_EXCLUSIVE)) != DFB_OK)
			  DirectFBError("IDirectFBScreen::SetCooperativeLevel",
			  ret);*/

			layer_config.flags = (DFBDisplayLayerConfigFlags)
					(DLCONF_BUFFERMODE | DLCONF_OPTIONS);

			layer_config.buffermode = DLBM_BACKVIDEO;
			layer_config.options = DLOP_ALPHACHANNEL;

			DFBDisplayLayerConfigFlags failed_flags;
			if ((ret = gfxLayer->TestConfiguration(
					gfxLayer, &layer_config, &failed_flags)) != DFB_OK) {

				DirectFBError(
						"gingacc-mb DFBDeviceScreen TestConfig error: ", ret);

			} else {
				DFBCHECK(gfxLayer->
						SetConfiguration(gfxLayer, &layer_config));
			}
		}

		initCodeMaps();
	}

	DFBDeviceScreen::~DFBDeviceScreen() {
		numOfDFBScreens--;

		releaseScreen();

		pthread_mutex_destroy(&winMutex);
		pthread_mutex_destroy(&surMutex);
		pthread_mutex_destroy(&cmpMutex);

		windowPool.clear();
		surfacePool.clear();
		cmpPool.clear();
	}

	void DFBDeviceScreen::releaseScreen() {
		clearWidgetPools();
	}

	void DFBDeviceScreen::releaseMB() {
		int errCount = 0;

		while (numOfDFBScreens > 1) {
			SystemCompat::uSleep(100000);
			errCount++;
			if (errCount > 5 || numOfDFBScreens <= 1) {
				break;
			}
		}

		if (gfxLayer != NULL) {
			gfxLayer->Release(gfxLayer);
			gfxLayer = NULL;
		}

		if (dfb != NULL) {
			dfb->Release(dfb);
			dfb = NULL;
		}
	}

	void DFBDeviceScreen::clearWidgetPools() {
		IWindow* iWin;
		ISurface* iSur;
		IContinuousMediaProvider* iCmp;

		set<IWindow*>::iterator i;
		set<ISurface*>::iterator j;
		set<IContinuousMediaProvider*>::iterator k;

		clog << "DFBDeviceScreen::clearWidgetPools ";
		clog << "windowPool size = " << windowPool.size();
		clog << ", surfacePool size = " << surfacePool.size();
		clog << endl;

		//Releasing remaining Window objects in Window Pool
		Thread::mutexLock(&winMutex);
		i = windowPool.begin();
		while (i != windowPool.end()) {
			iWin = (*i);

			windowPool.erase(i);
			i = windowPool.begin();

			if (iWin != NULL) {
				Thread::mutexUnlock(&winMutex);
				delete iWin;
				Thread::mutexLock(&winMutex);
			}
		}
		Thread::mutexUnlock(&winMutex);

		//Releasing remaining Surface objects in Surface Pool
		Thread::mutexLock(&surMutex);
		j = surfacePool.begin();
		while (j != surfacePool.end()) {
			iSur = (*j);

			surfacePool.erase(j);
			j = surfacePool.begin();

			if (iSur != NULL) {
				Thread::mutexUnlock(&surMutex);
				delete iSur;
				Thread::mutexLock(&surMutex);
			}
		}
		surfacePool.clear();
		Thread::mutexUnlock(&surMutex);

		//Releasing remaining CMP objects in CMP Pool
		Thread::mutexLock(&cmpMutex);
		k = cmpPool.begin();
		while (k != cmpPool.end()) {
			iCmp = (*k);

			cmpPool.erase(k);
			if (iCmp != NULL) {
				Thread::mutexUnlock(&cmpMutex);
				delete iCmp;
				Thread::mutexLock(&cmpMutex);
			}
			k = cmpPool.begin();
		}
		cmpPool.clear();
		Thread::mutexUnlock(&cmpMutex);
	}

	string DFBDeviceScreen::getScreenName() {
		return "dfb";
	}

	void DFBDeviceScreen::setUnderlyingRootWindow(GingaWindowID embedId) {
		//unsigned long xid = strtoul(strdup((const char*)devId), NULL, 10);
#if HAVE_WINDOWLESS
#ifdef DFB_CONFIG_SET_X11_ROOT_WINDOW
		dfb_config_set_x11_root_window(embedId);
#endif //DFB_CONFIG_SET_X11_ROOT_WINDOW
#endif //HAVE_WINDOWLESS
	}

	void DFBDeviceScreen::setBackgroundImage(string uri) {
		DFBResult               ret;
		DFBSurfaceDescription   desc;
		IDirectFBSurface       *surface;
		IDirectFBImageProvider *provider;

		ret = dfb->CreateImageProvider(dfb, uri.c_str(), &provider);
		if (ret) {
			DirectFBError("IDirectFB::CreateImageProvider() failed", ret);
			return;
		}

		ret = provider->GetSurfaceDescription(provider, &desc);
		if (ret) {
			DirectFBError(
					"DFBDeviceScreen::setBackgroundImage surdsc failed", ret);

			provider->Release(provider);
			return;
		}

		desc.flags = (DFBSurfaceDescriptionFlags)(desc.flags | DSDESC_CAPS);
		desc.caps  = DSCAPS_SHARED;

		ret = dfb->CreateSurface(dfb, &desc, &surface);
		if (ret) {
			DirectFBError(
					"DFBDeviceScreen::setBackgroundImage sur failed",
					ret);

			provider->Release(provider);
			return;
		}

		ret = provider->RenderTo(provider, surface, NULL);
		if (ret) {
			DirectFBError(
					"DFBDeviceScreen::setBackgroundImage renderto failed",
					ret);

			surface->Release(surface);
			provider->Release(provider);
			return;
		}

		ret = gfxLayer->SetBackgroundImage(gfxLayer, surface);
		if (ret) {
			DirectFBError(
					"DFBDeviceScreen::setBackgroundImage bg failed",
					ret);

			surface->Release(surface);
			provider->Release(provider);
			return;
		}

		ret = gfxLayer->SetBackgroundMode(gfxLayer, DLBM_IMAGE);
		if (ret) {
			DirectFBError(
					"DFBDeviceScreen::setBackgroundImage bgm failed",
					ret);
		}

		surface->Release(surface);
		provider->Release(provider);

		clog << endl << endl;
		clog << "DFBScreen::setBackgroundImage '" << uri << "'" << endl;
		clog << endl << endl;
	}

	unsigned int DFBDeviceScreen::getWidthResolution() {
		return wRes;
	}

	void DFBDeviceScreen::setWidthResolution(unsigned int wRes) {
		this->wRes = wRes;
	}

	unsigned int DFBDeviceScreen::getHeightResolution() {
		return hRes;
	}

	void DFBDeviceScreen::setHeightResolution(unsigned int hRes) {
		this->hRes = hRes;
	}

	void DFBDeviceScreen::setColorKey(int r, int g, int b) {
		if (gfxLayer != NULL) {
			DFBCHECK(gfxLayer->SetSrcColorKey(gfxLayer, r, g, b));
		}
	}

	bool DFBDeviceScreen::mergeIds(
			GingaWindowID destId, vector<GingaWindowID>* srcIds) {

		IDirectFBWindow* srcWin  = NULL;
		IDirectFBWindow* dstWin  = NULL;
		IDirectFBSurface* srcSur = NULL;
		IDirectFBSurface* dstSur = NULL;
		vector<void*>::iterator i;
		int x, y;
		bool merged = false;

		dstWin = getUnderlyingWindow(destId);
		if (dstWin == NULL) {
			return merged;
		}

		dstWin->GetSurface(dstWin, &dstSur);
		DFBCHECK(dstSur->SetBlittingFlags(
				dstSur, (DFBSurfaceBlittingFlags)DSBLIT_BLEND_ALPHACHANNEL));

		i = srcIds->begin();
		while (i != srcIds->end()) {
			srcWin = getUnderlyingWindow(*i);
			if (srcWin != NULL) {
				srcWin->GetPosition(srcWin, &x, &y);
				srcWin->GetSurface(srcWin, &srcSur);
				DFBCHECK(dstSur->Blit(dstSur, srcSur, NULL, x, y));

				merged = true;
			}
			++i;
		}

		return merged;
	}

	void DFBDeviceScreen::blitScreen(ISurface* destination) {

	}

	void DFBDeviceScreen::blitScreen(string fileUri) {

	}


	/* interfacing output */

	IWindow* DFBDeviceScreen::createWindow(
			int x, int y, int w, int h, float z) {

		IWindow* iWin;

		Thread::mutexLock(&winMutex);
		iWin = new DFBWindow(NULL, NULL, id, x, y, w, h);
		windowPool.insert(iWin);
		Thread::mutexUnlock(&winMutex);

		return iWin;
	}

	IWindow* DFBDeviceScreen::createWindowFrom(GingaWindowID underlyingWindow) {
		IWindow* iWin = NULL;

		if (underlyingWindow != NULL) {
			Thread::mutexLock(&winMutex);
			iWin = new DFBWindow(NULL, NULL, id, 0, 0, 0, 0);
			windowPool.insert(iWin);
			Thread::mutexUnlock(&winMutex);
		}

		return iWin;
	}

	bool DFBDeviceScreen::hasWindow(IWindow* win) {
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

	void DFBDeviceScreen::releaseWindow(IWindow* win) {
		set<IWindow*>::iterator i;

		Thread::mutexLock(&winMutex);
		i = windowPool.find(win);
		if (i != windowPool.end()) {
			windowPool.erase(i);
		}
		Thread::mutexUnlock(&winMutex);
	}

	ISurface* DFBDeviceScreen::createSurface() {
		return createSurfaceFrom(NULL);
	}

	ISurface* DFBDeviceScreen::createSurface(int w, int h) {
		ISurface* iSur = NULL;

		Thread::mutexLock(&surMutex);
		iSur = new DFBSurface(id, w, h);
		surfacePool.insert(iSur);
		Thread::mutexUnlock(&surMutex);

		return iSur;
	}

	ISurface* DFBDeviceScreen::createSurfaceFrom(void* uSur) {
		ISurface* iSur = NULL;

		Thread::mutexLock(&surMutex);
		if (uSur != NULL) {
			iSur = new DFBSurface(id, uSur);

		} else {
			iSur = new DFBSurface(id);
		}

		surfacePool.insert(iSur);
		Thread::mutexUnlock(&surMutex);

		return iSur;
	}

	bool DFBDeviceScreen::hasSurface(ISurface* s) {
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

	bool DFBDeviceScreen::releaseSurface(ISurface* s) {
		set<ISurface*>::iterator i;
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
	IContinuousMediaProvider* DFBDeviceScreen::createContinuousMediaProvider(
			const char* mrl, bool* hasVisual, bool isRemote) {

		IContinuousMediaProvider* provider;
		string strSym;

		Thread::mutexLock(&cmpMutex);

#if HAVE_COMPSUPPORT
		if (*hasVisual) {
			strSym = "DFBVideoProvider";

		} else {
			strSym = aSystem;
		}

		if (isRemote) {
#if HAVE_XINEPROVIDER
			strSym = "XineVideoProvider";

#elif HAVE_FFMPEGPROVIDER
			strSym = "FFmpegVideoProvider";
#endif
		}

		provider = ((CMPCreator*)(cm->getObject(strSym)))(id, mrl);

#else //!HAVE_COMPSUPPORT
		if (*hasVisual) {
			provider = new DFBVideoProvider(id, mrl);

		} else {
			if (aSystem == "DFBAudioProvider") {
				provider = new DFBAudioProvider(id, mrl);
			} else {
				provider = new FusionSoundAudioProvider(id, mrl);
			}
		}
#endif

		provider->setLoadSymbol(strSym);

		cmpPool.insert(provider);

		Thread::mutexUnlock(&cmpMutex);

		return provider;
	}

	void DFBDeviceScreen::releaseContinuousMediaProvider(
			IContinuousMediaProvider* provider) {

		set<IContinuousMediaProvider*>::iterator i;
		string strSym;

		Thread::mutexLock(&cmpMutex);
		i = cmpPool.find(provider);
		if (i != cmpPool.end()) {
			cmpPool.erase(i);
			strSym = provider->getLoadSymbol();
			provider->stop();

			delete provider;
			provider = NULL;

#if HAVE_COMPSUPPORT
			cm->releaseComponentFromObject(strSym);
#endif
		}
		Thread::mutexUnlock(&cmpMutex);
	}

	IFontProvider* DFBDeviceScreen::createFontProvider(
			const char* mrl, int fontSize) {

		IFontProvider* provider = NULL;

#if HAVE_COMPSUPPORT
		provider = ((FontProviderCreator*)(cm->getObject("DFBFontProvider")))(
				id, mrl, fontSize);

#else
		provider = new DFBFontProvider(id, mrl, fontSize);
#endif

		return provider;
	}

	void DFBDeviceScreen::releaseFontProvider(IFontProvider* provider) {
		delete provider;
		provider = NULL;

#if HAVE_COMPSUPPORT
		cm->releaseComponentFromObject("DFBFontProvider");
#endif
	}

	IImageProvider* DFBDeviceScreen::createImageProvider(const char* mrl) {
		IImageProvider* provider = NULL;

#if HAVE_COMPSUPPORT
		provider = ((ImageProviderCreator*)(cm->getObject(
				"DFBImageProvider")))(id, mrl);
#else
		provider = new DFBImageProvider(id, mrl);
#endif

		return provider;
	}

	void DFBDeviceScreen::releaseImageProvider(IImageProvider* provider) {
		delete provider;
		provider = NULL;

#if HAVE_COMPSUPPORT
		cm->releaseComponentFromObject("DFBImageProvider");
#endif
	}

	ISurface* DFBDeviceScreen::createRenderedSurfaceFromImageFile(
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
			clog << "DFBDeviceScreen::createRenderedSurfaceFromImageFile ";
			clog << "Warning! '" << mrl << "' file not found" << endl;
		}

		return iSur;
	}


	/* interfacing input */

	IInputManager* DFBDeviceScreen::getInputManager() {
		if (im == NULL) {
			im = new InputManager(id);

			im->setAxisBoundaries(wRes, hRes, 0);
			im->getEventBuffer();
		}
		return im;
	}

	IEventBuffer* DFBDeviceScreen::createEventBuffer() {
		return new DFBEventBuffer(id);
	}

	IInputEvent* DFBDeviceScreen::createInputEvent(
			void* event, const int symbol) {

		if (event != NULL) {
			return new DFBGInputEvent(event);
		}

		if (symbol >= 0) {
			return new DFBGInputEvent(symbol);
		}

		return NULL;
	}

	IInputEvent* DFBDeviceScreen::createApplicationEvent(int type, void* data) {
		return new DFBGInputEvent(type, data);
	}

	int DFBDeviceScreen::fromMBToGinga(int keyCode) {
		map<int, int>::iterator i;
		int translated = CodeMap::KEY_NULL;

		Thread::mutexLock(&ieMutex);
		i = dfbToGingaCodeMap.find(keyCode);
		if (i != dfbToGingaCodeMap.end()) {
			translated = i->second;
		}
		Thread::mutexUnlock(&ieMutex);

		return translated;
	}

	int DFBDeviceScreen::fromGingaToMB(int keyCode) {
		map<int, int>::iterator i;
		int translated = CodeMap::KEY_NULL;

		Thread::mutexLock(&ieMutex);
		i = gingaToDFBCodeMap.find(keyCode);
		if (i != gingaToDFBCodeMap.end()) {
			translated = i->second;
		}
		Thread::mutexUnlock(&ieMutex);

		return translated;
	}


	/* interfacing underlying multimedia system */

	void* DFBDeviceScreen::getGfxRoot() {
		return (void*)dfb;
	}

	/* libgingaccmbdfb internal use*/

	/* input */
	void DFBDeviceScreen::initCodeMaps() {
		if (!gingaToDFBCodeMap.empty()) {
			return;
		}

		Thread::mutexInit(&ieMutex);

		gingaToDFBCodeMap[CodeMap::KEY_NULL]              = DIKS_CUSTOM1;
		gingaToDFBCodeMap[CodeMap::KEY_NULL]              = DIKS_NULL;
		gingaToDFBCodeMap[CodeMap::KEY_0]                 = DIKS_0;
		gingaToDFBCodeMap[CodeMap::KEY_1]                 = DIKS_1;
		gingaToDFBCodeMap[CodeMap::KEY_2]                 = DIKS_2;
		gingaToDFBCodeMap[CodeMap::KEY_3]                 = DIKS_3;
		gingaToDFBCodeMap[CodeMap::KEY_4]                 = DIKS_4;
		gingaToDFBCodeMap[CodeMap::KEY_5]                 = DIKS_5;
		gingaToDFBCodeMap[CodeMap::KEY_6]                 = DIKS_6;
		gingaToDFBCodeMap[CodeMap::KEY_7]                 = DIKS_7;
		gingaToDFBCodeMap[CodeMap::KEY_8]                 = DIKS_8;
		gingaToDFBCodeMap[CodeMap::KEY_9]                 = DIKS_9;

		gingaToDFBCodeMap[CodeMap::KEY_SMALL_A]           = DIKS_SMALL_A;
		gingaToDFBCodeMap[CodeMap::KEY_SMALL_B]           = DIKS_SMALL_B;
		gingaToDFBCodeMap[CodeMap::KEY_SMALL_C]           = DIKS_SMALL_C;
		gingaToDFBCodeMap[CodeMap::KEY_SMALL_D]           = DIKS_SMALL_D;
		gingaToDFBCodeMap[CodeMap::KEY_SMALL_E]           = DIKS_SMALL_E;
		gingaToDFBCodeMap[CodeMap::KEY_SMALL_F]           = DIKS_SMALL_F;
		gingaToDFBCodeMap[CodeMap::KEY_SMALL_G]           = DIKS_SMALL_G;
		gingaToDFBCodeMap[CodeMap::KEY_SMALL_H]           = DIKS_SMALL_H;
		gingaToDFBCodeMap[CodeMap::KEY_SMALL_I]           = DIKS_SMALL_I;
		gingaToDFBCodeMap[CodeMap::KEY_SMALL_J]           = DIKS_SMALL_J;
		gingaToDFBCodeMap[CodeMap::KEY_SMALL_K]           = DIKS_SMALL_K;
		gingaToDFBCodeMap[CodeMap::KEY_SMALL_L]           = DIKS_SMALL_L;
		gingaToDFBCodeMap[CodeMap::KEY_SMALL_M]           = DIKS_SMALL_M;
		gingaToDFBCodeMap[CodeMap::KEY_SMALL_N]           = DIKS_SMALL_N;
		gingaToDFBCodeMap[CodeMap::KEY_SMALL_O]           = DIKS_SMALL_O;
		gingaToDFBCodeMap[CodeMap::KEY_SMALL_P]           = DIKS_SMALL_P;
		gingaToDFBCodeMap[CodeMap::KEY_SMALL_Q]           = DIKS_SMALL_Q;
		gingaToDFBCodeMap[CodeMap::KEY_SMALL_R]           = DIKS_SMALL_R;
		gingaToDFBCodeMap[CodeMap::KEY_SMALL_S]           = DIKS_SMALL_S;
		gingaToDFBCodeMap[CodeMap::KEY_SMALL_T]           = DIKS_SMALL_T;
		gingaToDFBCodeMap[CodeMap::KEY_SMALL_U]           = DIKS_SMALL_U;
		gingaToDFBCodeMap[CodeMap::KEY_SMALL_V]           = DIKS_SMALL_V;
		gingaToDFBCodeMap[CodeMap::KEY_SMALL_W]           = DIKS_SMALL_W;
		gingaToDFBCodeMap[CodeMap::KEY_SMALL_X]           = DIKS_SMALL_X;
		gingaToDFBCodeMap[CodeMap::KEY_SMALL_Y]           = DIKS_SMALL_Y;
		gingaToDFBCodeMap[CodeMap::KEY_SMALL_Z]           = DIKS_SMALL_Z;

		gingaToDFBCodeMap[CodeMap::KEY_CAPITAL_A]         = DIKS_CAPITAL_A;
		gingaToDFBCodeMap[CodeMap::KEY_CAPITAL_B]         = DIKS_CAPITAL_B;
		gingaToDFBCodeMap[CodeMap::KEY_CAPITAL_C]         = DIKS_CAPITAL_C;
		gingaToDFBCodeMap[CodeMap::KEY_CAPITAL_D]         = DIKS_CAPITAL_D;
		gingaToDFBCodeMap[CodeMap::KEY_CAPITAL_E]         = DIKS_CAPITAL_E;
		gingaToDFBCodeMap[CodeMap::KEY_CAPITAL_F]         = DIKS_CAPITAL_F;
		gingaToDFBCodeMap[CodeMap::KEY_CAPITAL_G]         = DIKS_CAPITAL_G;
		gingaToDFBCodeMap[CodeMap::KEY_CAPITAL_H]         = DIKS_CAPITAL_H;
		gingaToDFBCodeMap[CodeMap::KEY_CAPITAL_I]         = DIKS_CAPITAL_I;
		gingaToDFBCodeMap[CodeMap::KEY_CAPITAL_J]         = DIKS_CAPITAL_J;
		gingaToDFBCodeMap[CodeMap::KEY_CAPITAL_K]         = DIKS_CAPITAL_K;
		gingaToDFBCodeMap[CodeMap::KEY_CAPITAL_L]         = DIKS_CAPITAL_L;
		gingaToDFBCodeMap[CodeMap::KEY_CAPITAL_M]         = DIKS_CAPITAL_M;
		gingaToDFBCodeMap[CodeMap::KEY_CAPITAL_N]         = DIKS_CAPITAL_N;
		gingaToDFBCodeMap[CodeMap::KEY_CAPITAL_O]         = DIKS_CAPITAL_O;
		gingaToDFBCodeMap[CodeMap::KEY_CAPITAL_P]         = DIKS_CAPITAL_P;
		gingaToDFBCodeMap[CodeMap::KEY_CAPITAL_Q]         = DIKS_CAPITAL_Q;
		gingaToDFBCodeMap[CodeMap::KEY_CAPITAL_R]         = DIKS_CAPITAL_R;
		gingaToDFBCodeMap[CodeMap::KEY_CAPITAL_S]         = DIKS_CAPITAL_S;
		gingaToDFBCodeMap[CodeMap::KEY_CAPITAL_T]         = DIKS_CAPITAL_T;
		gingaToDFBCodeMap[CodeMap::KEY_CAPITAL_U]         = DIKS_CAPITAL_U;
		gingaToDFBCodeMap[CodeMap::KEY_CAPITAL_V]         = DIKS_CAPITAL_V;
		gingaToDFBCodeMap[CodeMap::KEY_CAPITAL_W]         = DIKS_CAPITAL_W;
		gingaToDFBCodeMap[CodeMap::KEY_CAPITAL_X]         = DIKS_CAPITAL_X;
		gingaToDFBCodeMap[CodeMap::KEY_CAPITAL_Y]         = DIKS_CAPITAL_Y;
		gingaToDFBCodeMap[CodeMap::KEY_CAPITAL_Z]         = DIKS_CAPITAL_Z;

		gingaToDFBCodeMap[CodeMap::KEY_PAGE_DOWN]         = DIKS_PAGE_DOWN;
		gingaToDFBCodeMap[CodeMap::KEY_PAGE_UP]           = DIKS_PAGE_UP;

		gingaToDFBCodeMap[CodeMap::KEY_F1]                = DIKS_F1;
		gingaToDFBCodeMap[CodeMap::KEY_F2]                = DIKS_F2;
		gingaToDFBCodeMap[CodeMap::KEY_F3]                = DIKS_F3;
		gingaToDFBCodeMap[CodeMap::KEY_F4]                = DIKS_F4;
		gingaToDFBCodeMap[CodeMap::KEY_F5]                = DIKS_F5;
		gingaToDFBCodeMap[CodeMap::KEY_F6]                = DIKS_F6;
		gingaToDFBCodeMap[CodeMap::KEY_F7]                = DIKS_F7;
		gingaToDFBCodeMap[CodeMap::KEY_F8]                = DIKS_F8;
		gingaToDFBCodeMap[CodeMap::KEY_F9]                = DIKS_F9;
		gingaToDFBCodeMap[CodeMap::KEY_F10]               = DIKS_F10;
		gingaToDFBCodeMap[CodeMap::KEY_F11]               = DIKS_F11;
		gingaToDFBCodeMap[CodeMap::KEY_F12]               = DIKS_F12;

		gingaToDFBCodeMap[CodeMap::KEY_PLUS_SIGN]         = DIKS_PLUS_SIGN;
		gingaToDFBCodeMap[CodeMap::KEY_MINUS_SIGN]        = DIKS_MINUS_SIGN;

		gingaToDFBCodeMap[CodeMap::KEY_ASTERISK]          = DIKS_ASTERISK;
		gingaToDFBCodeMap[CodeMap::KEY_NUMBER_SIGN]       = DIKS_NUMBER_SIGN;

		gingaToDFBCodeMap[CodeMap::KEY_PERIOD]            = DIKS_PERIOD;

		gingaToDFBCodeMap[CodeMap::KEY_SUPER]             = DIKS_SUPER;
		gingaToDFBCodeMap[CodeMap::KEY_PRINTSCREEN]       = DIKS_PRINT;
		gingaToDFBCodeMap[CodeMap::KEY_MENU]              = DIKS_MENU;
		gingaToDFBCodeMap[CodeMap::KEY_INFO]              = DIKS_INFO;
		gingaToDFBCodeMap[CodeMap::KEY_EPG]               = DIKS_EPG;

		gingaToDFBCodeMap[CodeMap::KEY_CURSOR_DOWN]       = DIKS_CURSOR_DOWN;
		gingaToDFBCodeMap[CodeMap::KEY_CURSOR_LEFT]       = DIKS_CURSOR_LEFT;
		gingaToDFBCodeMap[CodeMap::KEY_CURSOR_RIGHT]      = DIKS_CURSOR_RIGHT;
		gingaToDFBCodeMap[CodeMap::KEY_CURSOR_UP]         = DIKS_CURSOR_UP;

		gingaToDFBCodeMap[CodeMap::KEY_CHANNEL_DOWN]      = DIKS_CHANNEL_DOWN;
		gingaToDFBCodeMap[CodeMap::KEY_CHANNEL_UP]        = DIKS_CHANNEL_UP;

		gingaToDFBCodeMap[CodeMap::KEY_VOLUME_DOWN]       = DIKS_VOLUME_DOWN;
		gingaToDFBCodeMap[CodeMap::KEY_VOLUME_UP]         = DIKS_VOLUME_UP;

		gingaToDFBCodeMap[CodeMap::KEY_ENTER]             = DIKS_ENTER;
		gingaToDFBCodeMap[CodeMap::KEY_OK]                = DIKS_OK;

		gingaToDFBCodeMap[CodeMap::KEY_RED]               = DIKS_RED;
		gingaToDFBCodeMap[CodeMap::KEY_GREEN]             = DIKS_GREEN;
		gingaToDFBCodeMap[CodeMap::KEY_YELLOW]            = DIKS_YELLOW;
		gingaToDFBCodeMap[CodeMap::KEY_BLUE]              = DIKS_BLUE;

		gingaToDFBCodeMap[CodeMap::KEY_SPACE]             = DIKS_SPACE;
		gingaToDFBCodeMap[CodeMap::KEY_BACKSPACE]         = DIKS_BACKSPACE;
		gingaToDFBCodeMap[CodeMap::KEY_BACK]              = DIKS_BACK;
		gingaToDFBCodeMap[CodeMap::KEY_ESCAPE]            = DIKS_ESCAPE;
		gingaToDFBCodeMap[CodeMap::KEY_EXIT]              = DIKS_EXIT;

		gingaToDFBCodeMap[CodeMap::KEY_POWER]             = DIKS_POWER;
		gingaToDFBCodeMap[CodeMap::KEY_REWIND]            = DIKS_REWIND;
		gingaToDFBCodeMap[CodeMap::KEY_STOP]              = DIKS_STOP;
		gingaToDFBCodeMap[CodeMap::KEY_EJECT]             = DIKS_EJECT;
		gingaToDFBCodeMap[CodeMap::KEY_PLAY]              = DIKS_PLAY;
		gingaToDFBCodeMap[CodeMap::KEY_RECORD]            = DIKS_RECORD;
		gingaToDFBCodeMap[CodeMap::KEY_PAUSE]             = DIKS_PAUSE;

		gingaToDFBCodeMap[CodeMap::KEY_GREATER_THAN_SIGN] = DIKS_GREATER_THAN_SIGN;
		gingaToDFBCodeMap[CodeMap::KEY_LESS_THAN_SIGN]    = DIKS_LESS_THAN_SIGN;

		gingaToDFBCodeMap[CodeMap::KEY_TAB]               = DIKS_TAB;
		gingaToDFBCodeMap[CodeMap::KEY_TAP]               = DIKS_CUSTOM0;

        map<int, int>::iterator i;
        i = gingaToDFBCodeMap.begin();
        while (i != gingaToDFBCodeMap.end()) {
		    dfbToGingaCodeMap[i->second] = i->first;
		    ++i;
        }
	}

	/* output */
	IDirectFBWindow* DFBDeviceScreen::getUnderlyingWindow(GingaWindowID winId) {
		IDirectFBWindow* window = NULL;
		DFBWindowID wid;

		if (gfxLayer != NULL) {
			wid = (DFBWindowID)(unsigned long)winId;
			if (gfxLayer->GetWindow(
					gfxLayer,
					wid,
					&window) != DFB_OK) {

				clog << "DFBDeviceScreen::getUnderlyingWindow ";
				clog << "can't find id '" << wid;
				clog << "'" << endl;
				window = NULL;
			}
		}

		return window;
	}

	IDirectFBWindow* DFBDeviceScreen::createUnderlyingWindow(
			DFBWindowDescription* desc) {

		IDirectFBWindow* window = NULL;

		if (gfxLayer != NULL) {
			DFBCHECK(gfxLayer->CreateWindow(
					gfxLayer, (const DFBWindowDescription*)desc, &window));
		}

		return window;
	}

	void DFBDeviceScreen::releaseUnderlyingWindow(IDirectFBWindow* uWin) {
		uWin->Destroy(uWin);
		uWin->Release(uWin);
		uWin = NULL;
	}

	IDirectFBSurface* DFBDeviceScreen::createUnderlyingSurface(
			DFBSurfaceDescription* desc) {

		IDirectFBSurface* surface = NULL;

		if (dfb != NULL) {
			DFBCHECK(dfb->CreateSurface(
					dfb, (const DFBSurfaceDescription*)desc, &surface));
		}

		return surface;
	}

	void DFBDeviceScreen::releaseUnderlyingSurface(IDirectFBSurface* uSur) {
		uSur->Clear(uSur, 0, 0, 0, 0x00);
		uSur->Release(uSur);
		uSur = NULL;
	}

	/*
	static DFBEnumerationResult
	Graphics::display_layer_callback(DFBDisplayLayerID   id,
	                        DFBDisplayLayerDescription  desc,
	                        void                       *arg) {
		int i;
		bool acceptVideo = false;
		bool acceptGraphics = false;
		for (i=0; layer_types[i].type; i++) {
			if (desc.caps & layer_types[i].type) {
				if (layer_types[i].name == "VIDEO") {
					acceptVideo = true;
				}
				if (layer_types[i].name == "GRAPHICS") {
					acceptGraphics = true;
				}
			}
		}

		if (acceptVideo && acceptGraphics) {
			DFBResult ret;
			IDirectFB* dfb = NULL;
			dfb = lite_get_dfb_interface();
			ret = dfb->GetDisplayLayer(dfb, id, (IDirectFBDisplayLayer**)
				    (arg));

			if (ret) {
				DirectFBError(
					    "Graphics DirectFB::GetDisplayLayer() failed",
					    ret);

				return (DFBEnumerationResult)ret;
			}
		}

		return (DFBEnumerationResult)DFB_OK;
	}
	*/
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::mb::IDeviceScreen*
		createDFBScreen(
				int argc, char** argv,
				GingaScreenID myId, GingaWindowID embedId,
				bool externalRenderer) {

	return (new ::br::pucrio::telemidia::ginga::core::mb::
			DFBDeviceScreen(argc, argv, myId, embedId, externalRenderer));
}

extern "C" void destroyDFBScreen(
		::br::pucrio::telemidia::ginga::core::mb::IDeviceScreen* ds) {

	delete ds;
}
