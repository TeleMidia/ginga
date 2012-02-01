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
#include "mb/interface/dfb/device/DFBDeviceScreen.h"
#include "mb/interface/dfb/output/DFBWindow.h"
#include "mb/interface/dfb/output/DFBSurface.h"
#include "mb/ILocalScreenManager.h"

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

const unsigned int DFBDeviceScreen::DSA_UNKNOWN = 0;
const unsigned int DFBDeviceScreen::DSA_4x3     = 1;
const unsigned int DFBDeviceScreen::DSA_16x9    = 2;

unsigned int DFBDeviceScreen::numOfDFBScreens = 0;
IDirectFB* DFBDeviceScreen::dfb = NULL;
IDirectFBDisplayLayer* DFBDeviceScreen::gfxLayer = NULL;

	DFBDeviceScreen::DFBDeviceScreen(
			int numArgs, char** args,
			GingaScreenID myId, GingaWindowID parentId) {

		DFBDisplayLayerConfig layer_config;
		DFBResult ret;

		aspect = DSA_UNKNOWN;
		hSize  = 0;
		vSize  = 0;
		hRes   = 0;
		wRes   = 0;
		id     = myId;
		numOfDFBScreens++;

		windowPool  = new set<IWindow*>;
		surfacePool = new set<ISurface*>;

		pthread_mutex_init(&winMutex, NULL);
		pthread_mutex_init(&surMutex, NULL);

		if (DFBDeviceScreen::dfb == NULL) {
			DFBCHECK(DirectFBInit(&numArgs, &args));

			if (parentId != NULL) {
				cout << "DFBDeviceScreen::DFBDeviceScreen" << endl;
				setParentScreen(parentId);
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
						"gingacc-systemio DFBDeviceScreen TestConfig error: ",
						ret);

			} else {
				DFBCHECK(gfxLayer->
						SetConfiguration(gfxLayer, &layer_config));
			}
		}
	}

	DFBDeviceScreen::~DFBDeviceScreen() {
		clearWidgetPools();
		pthread_mutex_destroy(&winMutex);
		pthread_mutex_destroy(&surMutex);

		numOfDFBScreens--;
		if (numOfDFBScreens == 0) {
			gfxLayer->Release(gfxLayer);
			dfb->Release(dfb);
			gfxLayer = NULL;
			dfb = NULL;
		}
	}

	void DFBDeviceScreen::clearWidgetPools() {
		set<IWindow*>::iterator i;
		set<ISurface*>::iterator j;

		clog << "DFBDeviceScreen::clearWidgetPools ";
		clog << "windowPool size = " << windowPool->size();
		clog << ", surfacePool size = " << surfacePool->size() << endl;

		//Releasing remaining Window objects in Window Pool
		pthread_mutex_lock(&winMutex);
		if (windowPool != NULL) {
			i = windowPool->begin();
			while (i != windowPool->end()) {
				if ((*i) != NULL) {
					delete (*i);
				}
				++i;
			}
			windowPool->clear();
		}
		pthread_mutex_unlock(&winMutex);

		//Releasing remaining Surface objects in Surface Pool
		pthread_mutex_lock(&surMutex);
		if (surfacePool != NULL) {
			for (j = surfacePool->begin(); j != surfacePool->end(); ++j) {
				if ((*j) != NULL) {
					delete (*j);
				}
			}
			surfacePool->clear();
		}
		pthread_mutex_unlock(&surMutex);
	}

	void DFBDeviceScreen::setParentScreen(GingaWindowID parentId) {
		//unsigned long xid = strtoul(strdup((const char*)devId), NULL, 10);
#if HAVE_WINDOWLESS
#ifdef DFB_CONFIG_SET_X11_ROOT_WINDOW
		dfb_config_set_x11_root_window(parentId);
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

	void DFBDeviceScreen::mergeIds(
			GingaWindowID destId, vector<GingaWindowID>* srcIds) {

		IDirectFBWindow* srcWin  = NULL;
		IDirectFBWindow* dstWin  = NULL;
		IDirectFBSurface* srcSur = NULL;
		IDirectFBSurface* dstSur = NULL;
		vector<void*>::iterator i;
		int x, y;

		dstWin = getUnderlyingWindow(destId);
		if (dstWin == NULL) {
			return;
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
			}
			++i;
		}
	}


	/* interfacing output */

	IWindow* DFBDeviceScreen::createWindow(int x, int y, int w, int h) {
		IWindow* iWin;

		pthread_mutex_lock(&winMutex);
		iWin = new DFBWindow(NULL, NULL, id, x, y, w, h);
		windowPool->insert(iWin);
		pthread_mutex_unlock(&winMutex);

		return iWin;
	}

	IWindow* DFBDeviceScreen::createWindowFrom(GingaWindowID underlyingWindow) {
		IWindow* iWin = NULL;

		if (underlyingWindow != NULL) {
			pthread_mutex_lock(&winMutex);
			iWin = new DFBWindow(NULL, NULL, id, 0, 0, 0, 0);
			windowPool->insert(iWin);
			pthread_mutex_unlock(&winMutex);
		}

		return iWin;
	}

	void DFBDeviceScreen::releaseWindow(IWindow* win) {
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

	ISurface* DFBDeviceScreen::createSurface() {
		return createSurfaceFrom(NULL);
	}

	ISurface* DFBDeviceScreen::createSurface(int w, int h) {
		ISurface* iSur = NULL;

		pthread_mutex_lock(&surMutex);
		iSur = new DFBSurface(id, w, h);
		surfacePool->insert(iSur);
		pthread_mutex_unlock(&surMutex);

		return iSur;
	}

	ISurface* DFBDeviceScreen::createSurfaceFrom(void* uSur) {
		ISurface* iSur = NULL;

		pthread_mutex_lock(&surMutex);
		if (uSur != NULL) {
			iSur = new DFBSurface(id, uSur);

		} else {
			iSur = new DFBSurface(id);
		}

		surfacePool->insert(iSur);
		pthread_mutex_unlock(&surMutex);

		return iSur;
	}

	void DFBDeviceScreen::releaseSurface(ISurface* s) {
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
	IContinuousMediaProvider* DFBDeviceScreen::createContinuousMediaProvider(
			const char* mrl, bool hasVisual, bool isRemote) {

		IContinuousMediaProvider* provider;
		string strSym;

#if HAVE_COMPSUPPORT
		if (hasVisual) {
			strSym = "DFBVideoProvider";

		} else {
			strSym = "AudioProvider";
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
		if (>hasVisual) {
			provider = new DFBVideoProvider(id, mrl);

		} else {
			provider = new FusionSoundAudioProvider(id, mrl);
		}
#endif

		provider->setLoadSymbol(strSym);
		return provider;
	}

	void DFBDeviceScreen::releaseContinuousMediaProvider(
			IContinuousMediaProvider* provider) {

		string strSym = provider->getLoadSymbol();

		delete provider;
		provider = NULL;

#if HAVE_COMPSUPPORT
		cm->releaseComponentFromObject(strSym);
#endif
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


	/* interfacing underlying multimedia system */

	void* DFBDeviceScreen::getGfxRoot() {
		return (void*)dfb;
	}

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

		IDirectFBSurface* surface;

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
				int numArgs, char** args,
				GingaScreenID myId, GingaWindowID parentId) {

	return (new ::br::pucrio::telemidia::ginga::core::mb::
			DFBDeviceScreen(numArgs, args, myId, parentId));
}

extern "C" void destroyDFBScreen(
		::br::pucrio::telemidia::ginga::core::mb::IDeviceScreen* ds) {

	delete ds;
}
