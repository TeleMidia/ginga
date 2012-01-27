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
#include "mb/ILocalScreenManager.h"

#include <string.h>
#include <stdlib.h>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {

const unsigned int DFBDeviceScreen::DSA_UNKNOWN = 0;
const unsigned int DFBDeviceScreen::DSA_4x3     = 1;
const unsigned int DFBDeviceScreen::DSA_16x9    = 2;

unsigned int DFBDeviceScreen::numOfDFBScreens = 0;
IDirectFB* DFBDeviceScreen::dfb = NULL;
IDirectFBDisplayLayer* DFBDeviceScreen::gfxLayer = NULL;

	DFBDeviceScreen::DFBDeviceScreen(
			int numArgs, char** args, GingaWindowID parentId) {

		DFBDisplayLayerConfig layer_config;
		DFBResult ret;

		aspect = DSA_UNKNOWN;
		hSize  = 0;
		vSize  = 0;
		hRes   = 0;
		wRes   = 0;
		numOfDFBScreens++;

		windowPool  = new set<IDirectFBWindow*>;
		surfacePool = new set<IDirectFBSurface*>;

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
		set<IDirectFBWindow*>::iterator w;
		set<IDirectFBSurface*>::iterator s;

		clog << "DFBDeviceScreen::clearWidgetPools ";
		clog << "windowPool size = " << windowPool->size();
		clog << ", surfacePool size = " << surfacePool->size() << endl;

		//Releasing still Window objects in Window Pool
		pthread_mutex_lock(&winMutex);
		if (windowPool != NULL) {
			w = windowPool->begin();
			while (w != windowPool->end()) {
				if ((*w) != NULL) {
					(*w)->Release(*w);
				}
				++w;
			}
			windowPool->clear();
		}
		pthread_mutex_unlock(&winMutex);

		//Releasing still Surface objects in Surface Pool
		pthread_mutex_lock(&surMutex);
		for (s = surfacePool->begin(); s != surfacePool->end(); ++s) {
			if ((*s) != NULL) {
				(*s)->Release(*s);
			}
		}
		surfacePool->clear();
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

		dstWin = (IDirectFBWindow*)getWindow(destId);
		if (dstWin == NULL) {
			return;
		}

		dstWin->GetSurface(dstWin, &dstSur);
		DFBCHECK(dstSur->SetBlittingFlags(
				dstSur, (DFBSurfaceBlittingFlags)DSBLIT_BLEND_ALPHACHANNEL));

		i = srcIds->begin();
		while (i != srcIds->end()) {
			srcWin = (IDirectFBWindow*)getWindow(*i);
			if (srcWin != NULL) {
				srcWin->GetPosition(srcWin, &x, &y);
				srcWin->GetSurface(srcWin, &srcSur);
				DFBCHECK(dstSur->Blit(dstSur, srcSur, NULL, x, y));
			}
			++i;
		}
	}

	void* DFBDeviceScreen::getWindow(GingaWindowID winId) {
		IDirectFBWindow* window = NULL;
		DFBWindowID wid;

		if (gfxLayer != NULL) {
			wid = (DFBWindowID)(unsigned long)winId;
			if (gfxLayer->GetWindow(
					gfxLayer,
					wid,
					&window) != DFB_OK) {

				clog << "DFBDeviceScreen::getWindow can't find id '" << wid;
				clog << "'" << endl;
				window = NULL;
			}
		}

		return (void*)window;
	}

	void* DFBDeviceScreen::createWindow(void* desc) {
		IDirectFBWindow* window = NULL;

		if (gfxLayer != NULL) {
			DFBCHECK(gfxLayer->CreateWindow(
					gfxLayer, (const DFBWindowDescription*)desc, &window));

			pthread_mutex_lock(&winMutex);
			windowPool->insert(window);
			pthread_mutex_unlock(&winMutex);
		}

		return (void*)window;
	}

	void DFBDeviceScreen::releaseWindow(void* win) {
		set<IDirectFBWindow*>::iterator i;
		IDirectFBWindow* w;
		w = (IDirectFBWindow*)win;

		pthread_mutex_lock(&winMutex);
		if (windowPool != NULL) {
			i = windowPool->find(w);
			if (i != windowPool->end()) {
				windowPool->erase(i);
				pthread_mutex_unlock(&winMutex);

			} else {
				pthread_mutex_unlock(&winMutex);
			}

		} else {
			pthread_mutex_unlock(&winMutex);
		}

		w->Destroy(w);
		w->Release(w);
		win = NULL;
		w = NULL;
	}

	void* DFBDeviceScreen::createSurface(void* desc) {
		IDirectFBSurface* surface;

		if (dfb != NULL) {
			DFBCHECK(dfb->CreateSurface(
					dfb, (const DFBSurfaceDescription*)desc, &surface));
		}

		pthread_mutex_lock(&surMutex);
		surfacePool->insert(surface);
		pthread_mutex_unlock(&surMutex);
		return (void*)surface;
	}

	void DFBDeviceScreen::releaseSurface(void* sur) {
		set<IDirectFBSurface*>::iterator i;
		IDirectFBSurface* s;
		s = (IDirectFBSurface*)sur;

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

		s->Clear(s, 0, 0, 0, 0);
		s->Release(s);
		s = NULL;
		sur = NULL;
	}

	void* DFBDeviceScreen::getGfxRoot() {
		return (void*)dfb;
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
		createDFBScreen(int numArgs, char** args, GingaWindowID parentId) {

	return (new ::br::pucrio::telemidia::ginga::core::mb::
			DFBDeviceScreen(numArgs, args, parentId));
}

extern "C" void destroyDFBScreen(
		::br::pucrio::telemidia::ginga::core::mb::IDeviceScreen* ds) {

	delete ds;
}
