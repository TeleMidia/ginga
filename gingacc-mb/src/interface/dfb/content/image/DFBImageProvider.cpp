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

#include "util/Color.h"

#include "mb/interface/dfb/content/image/DFBImageProvider.h"
#include "mb/interface/dfb/output/DFBWindow.h"
#include "mb/interface/dfb/output/DFBSurface.h"
#include "mb/interface/dfb/DFBDeviceScreen.h"
#include "mb/LocalScreenManager.h"

/* macro for a safe call to DirectFB functions */
#ifndef DFBCHECK
#define DFBCHECK(x...)                                            \
{                                                                 \
	DFBResult err = x;                                            \
	if (err != DFB_OK) {                                          \
		fprintf( stderr, "%s <%d>: \n\t", __FILE__, __LINE__ );   \
		DirectFBError( #x, err );                                 \
	}                                                             \
}
#endif /*DFBCHECK*/

#if DFBTM_PATCH
	static int dipRefs = 0;
#endif

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	DFBImageProvider::DFBImageProvider(
			GingaScreenID screenId, const char* mrl) {

		IDirectFB* dfb;

#if DFBTM_PATCH
		dipRefs++;
#endif

		this->mrl = "";
		myScreen  = screenId;
		decoder   = NULL;

		this->mrl.assign(mrl);

		if (mrl != NULL) {
			dfb = (IDirectFB*)(LocalScreenManager::getInstance()->getGfxRoot(
					myScreen));

			if (dfb != NULL) {
				DFBCHECK(dfb->CreateImageProvider(dfb, mrl, &decoder));
			}
		}
	}

	DFBImageProvider::~DFBImageProvider() {
		if (decoder != NULL) {
			decoder->Release(decoder);
			decoder = NULL;
		}

#if DFBTM_PATCH
		dipRefs--;

		if (dipRefs == 0) {
			DirectReleaseInterface("IDirectFBImageProvider");
		}
#endif //DFBTM_PATCH
	}

	void* DFBImageProvider::getProviderContent() {
		return decoder;
	}

	void DFBImageProvider::playOver(ISurface* surface) {
		IDirectFBSurface* s;
		s = (IDirectFBSurface*)(surface->getSurfaceContent());

		if (s == NULL) {
			s = getPerfectUnderlyingSurface(surface);
			surface->setSurfaceContent(s);
		}

		if (decoder != NULL) {
			decoder->RenderTo(decoder, s, NULL);
		}
	}

	IDirectFBSurface* DFBImageProvider::getPerfectUnderlyingSurface(
			ISurface* surface) {

		DFBImageDescription imgDsc;
		IDirectFBSurface* destination = NULL;
		DFBSurfaceDescription surDsc;
		IColor* chromaKey = NULL;

		if (decoder == NULL) {
			return NULL;
		}

		//IDirectFBSurface* source;
		if ((decoder->GetImageDescription(decoder, &imgDsc) == DFB_OK) &&
			 (decoder->GetSurfaceDescription(decoder, &surDsc) == DFB_OK)) {

			destination = (IDirectFBSurface*)(
					DFBDeviceScreen::createUnderlyingSurface(&surDsc));

			if (imgDsc.caps & DICAPS_ALPHACHANNEL) {
				surface->setCaps(DWCAPS_ALPHACHANNEL);

				if (mrl.find(".gif") != std::string::npos) {
					surface->setChromaColor(0, 0, 0, 0xFF);
				}

				destination->SetBlittingFlags(
						destination,
						(DFBSurfaceBlittingFlags)(DSBLIT_BLEND_ALPHACHANNEL));
			}

			if (imgDsc.caps & DICAPS_COLORKEY) {
				destination->SetBlittingFlags(
						destination,
					    (DFBSurfaceBlittingFlags)(
					    		DSBLIT_BLEND_ALPHACHANNEL |
					    		DSBLIT_SRC_COLORKEY));

				surface->setChromaColor(
						imgDsc.colorkey_r,
					    imgDsc.colorkey_g,
					    imgDsc.colorkey_b,
					    0xFF);
			}

			if (imgDsc.caps & DICAPS_NONE) {
				destination->SetBlittingFlags(
						destination,
						(DFBSurfaceBlittingFlags)DSBLIT_NOFX);

				surface->setCaps(DWCAPS_NONE);
			}
		}

		return destination;
	}

	bool DFBImageProvider::releaseAll() {
		return false;
	}

	/*void* DFBImageProvider::frameCB(void *cdata) {
		int dw;
		int dh;

		try {
			IDirectFBSurface* s = NULL;
			IDirectFBWindow* win = NULL;
			Window* w = NULL;

			w = ((ImageFrame*)cdata)->getDestination();
			win = w->getContent();
			win->GetSize(win, &dw, &dh);

			s = w->getContentSurface();

			s->SetBlittingFlags(s, DSBLIT_BLEND_ALPHACHANNEL);

			if (dw != ((ImageFrame*)cdata)->getWidth() ||
				    dh != ((ImageFrame*)cdata)->getHeight()) {

				s->StretchBlit(
					    s,
					    s,
					    NULL,
					    NULL);

			} else {
				s->Blit(s, s, NULL, 0, 0);
			}

			s->Flip(s, NULL, (DFBSurfaceFlipFlags) DSFLIP_BLIT);

		} catch (...) {
			clog << "Warning! VideoPlayer::frameCB" << endl;
		}
		return NULL;
	}*/
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::mb::IImageProvider*
		createDFBImageProvider(GingaScreenID screenId, const char* mrl) {

	return (new ::br::pucrio::telemidia::ginga::core::mb::
			DFBImageProvider(screenId, mrl));
}

extern "C" void destroyDFBImageProvider(
		::br::pucrio::telemidia::ginga::core::mb::IImageProvider* ip) {

	delete ip;
}
