/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware
Ginga (Ginga-NCL).

Direitos Autorais Reservados (c) 1989-2007 PUC-Rio/Laboratorio TeleMidia

Este programa eh software livre; voce pode redistribui-lo e/ou modificah-lo sob
os termos da Licen�a Publica Geral GNU versao 2 conforme publicada pela Free
Software Foundation.

Este programa eh distribu�do na expectativa de que seja util, porem, SEM
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

#include "../../../include/Graphics.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace io {
#if HAVE_DIRECTFB
	set<IDirectFBWindow*>* Graphics::windowPool = (
			new set<IDirectFBWindow*>);

	set<IDirectFBSurface*>* Graphics::surfacePool = (
			new set<IDirectFBSurface*>);

	IDirectFB* Graphics::dfb = NULL;
	IDirectFBDisplayLayer* Graphics::gfxLayer = NULL;
#endif

#if HAVE_FUSIONSOUND
	IFusionSound* Graphics::sound = NULL;
#endif

	int Graphics::screenWidth = 0;
	int Graphics::screenHeight = 0;
	const string Graphics::version = "";

	/*
	static DFBEnumerationResult
	Graphics::display_layer_callback( DFBDisplayLayerID   id,
	                        DFBDisplayLayerDescription  desc,
	                        void                       *arg ) {
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
	bool Graphics::isInitialized() {
#if HAVE_DIRECTFB
		return (dfb != NULL);
#else
		return false;
#endif
	}

	void Graphics::releasePoolObjects() {
#if HAVE_DIRECTFB
		cout << "windowPool size = " << windowPool->size() << endl;
		cout << "surfacePool size = " << surfacePool->size() << endl;

		//Releasing still Window objects in Window Pool
		set<IDirectFBWindow*>::iterator w;
		for (w = windowPool->begin(); w != windowPool->end(); ++w) {
			if ((*w) != NULL) {
#if DFB_VER == 120
				(*w)->Release(*w);
#else
				DFBCHECK((*w)->Release(*w));
#endif
			}
		}
		windowPool->clear();

		//Releasing still Surface objects in Surface Pool
		set<IDirectFBSurface*>::iterator s;
		for (s = surfacePool->begin(); s != surfacePool->end(); ++s) {
			if ((*s) != NULL) {
#if DFB_VER == 120
				(*s)->Release(*s);
#else
				DFBCHECK((*s)->Release(*s));
#endif
			}
		}
		surfacePool->clear();
#endif
	}

	void Graphics::initialize() {
#if HAVE_DIRECTFB
		/*cout << "MAJOR = " << directfb_major_version << endl;
		cout << "MINOR = " << directfb_minor_version << endl;
		cout << "MICRO = " << directfb_micro_version << endl;*/

		gfxLayer = NULL;
		DFBCHECK(DirectFBInit(NULL, NULL));
		DFBCHECK(DirectFBCreate( &dfb ));

#if HAVE_FUSIONSOUND
		FusionSoundInit(NULL, NULL);
		FusionSoundCreate( &sound );
#endif /*HAVE_FUSIONSOUND*/

		DFBResult ret;

		if (gfxLayer == NULL) {
			DFBCHECK(dfb->GetDisplayLayer(
					dfb, DLID_PRIMARY, &gfxLayer));
		}

		DFBDisplayLayerConfig layer_config;
		DFBCHECK(gfxLayer->GetConfiguration(
				gfxLayer, &layer_config));

		screenWidth = layer_config.width;
		screenHeight = layer_config.height;

		/*if ((ret = gfxLayer->SetCooperativeLevel(
		  gfxLayer, DLSCL_EXCLUSIVE )) != DFB_OK)
		  DirectFBError("IDirectFBScreen::SetCooperativeLevel",
		  ret);*/

		layer_config.flags = (DFBDisplayLayerConfigFlags)
			    ( DLCONF_BUFFERMODE | DLCONF_OPTIONS );

		layer_config.buffermode = DLBM_BACKVIDEO;
		layer_config.options = DLOP_ALPHACHANNEL;

		DFBDisplayLayerConfigFlags failed_flags;
		if ((ret = gfxLayer->TestConfiguration(
			    gfxLayer, &layer_config, &failed_flags )) != DFB_OK) {

			DirectFBError(
				    "Graphics IDirectFBScreen::TestConfiguration",
				    ret );

		} else {
			DFBCHECK(gfxLayer->
				    SetConfiguration(gfxLayer, &layer_config));
		}
#endif
	}

	void Graphics::release() {
#if HAVE_DIRECTFB
#if DFB_VER == 120
		gfxLayer->Release(gfxLayer);
		dfb->Release(dfb);
#else
		DFBCHECK(gfxLayer->Release(gfxLayer));
		DFBCHECK(dfb->Release(dfb));
#endif /*DFB_VER*/
		dfb = NULL;
#endif /*HAVE_DIRECTFB*/
#if HAVE_FUSIONSOUND
		if (sound)
			sound->Release( sound );
#endif
	}

	int Graphics::getDeviceWidth() {
		return screenWidth;
	}

	int Graphics::getDeviceHeight() {
		return screenHeight;
	}

	void Graphics::setLayerColorKey(int r, int g, int b) {
#if HAVE_DIRECTFB
		DFBCHECK(gfxLayer->SetSrcColorKey(gfxLayer, r, g, b));
#endif
	}

	void* Graphics::createWindow(void* desc) {
#if HAVE_DIRECTFB
		IDirectFBWindow* window = NULL;

		DFBCHECK(gfxLayer->CreateWindow(
				gfxLayer, (const DFBWindowDescription*)desc, &window));

		windowPool->insert(window);

		return (void*)window;
#else
		return NULL;
#endif
	}

	void Graphics::releaseWindow(void* win) {
#if HAVE_DIRECTFB
		set<IDirectFBWindow*>::iterator i;
		IDirectFBWindow* w;
		w = (IDirectFBWindow*)win;

		i = windowPool->find(w);
		if (i != windowPool->end()) {
			windowPool->erase(i);

		} else {
			return;
		}
#if DFB_VER == 120
		w->Release(w);
#else
		DFBCHECK(w->Release(w));
#endif /*DFB_VER*/
		win = NULL;
		w = NULL;
#endif /*HAVE_DIRECTFB*/
	}

	void* Graphics::createSurface(void* desc) {
#if HAVE_DIRECTFB
		IDirectFBSurface* surface;
		DFBCHECK(dfb->CreateSurface(
				dfb, (const DFBSurfaceDescription*)desc, &surface));

		surfacePool->insert(surface);
		return (void*)surface;
#else
		return NULL;
#endif
	}

	void Graphics::releaseSurface(void* sur) {
#if HAVE_DIRECTFB
		set<IDirectFBSurface*>::iterator i;
		IDirectFBSurface* s;
		s = (IDirectFBSurface*)sur;

		i = surfacePool->find(s);
		if (i != surfacePool->end()) {
			surfacePool->erase(i);

		} else {
			return;
		}
#if DFB_VER == 120
		s->Release(s);
#else
		DFBCHECK(s->Release(s));
#endif /*DFB_VER*/
		s = NULL;
		sur = NULL;
#endif /*HAVE_DIRECTFB*/
	}

	void* Graphics::getRoot() {
#if HAVE_DIRECTFB
		return (void*)dfb;
#else
		return NULL;
#endif
	}

	void* Graphics::getFSRoot() {
#if HAVE_FUSIONSOUND
		return (void*)sound;
#else
		return NULL;
#endif
	}
}
}
}
}
}
}
