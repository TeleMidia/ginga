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

		myScreen = screenId;
		decoder  = NULL;

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

	void* DFBImageProvider::getContent() {
		return decoder;
	}

	void DFBImageProvider::playOver(ISurface* surface) {
		IDirectFBImageProvider* ip;
		ip = (IDirectFBImageProvider*)decoder;

		IDirectFBSurface* s;
		s = (IDirectFBSurface*)(surface->getContent());
		DFBCHECK(ip->RenderTo(ip, s, NULL));
	}

	ISurface* DFBImageProvider::prepare(bool isGif) {
		ISurface* renderedSurface  = NULL;
		IDirectFBImageProvider* ip = NULL;

		if (decoder == NULL) {
			return NULL;
		}

		ip = (IDirectFBImageProvider*)decoder;

		DFBImageDescription imgDsc;
		IDirectFBSurface* destination = NULL;
		DFBSurfaceDescription surDsc;
		IColor* chromaKey = NULL;

		//IDirectFBSurface* source;
		if ((ip->GetImageDescription(ip, &imgDsc) == DFB_OK) &&
			 (ip->GetSurfaceDescription(ip, &surDsc) == DFB_OK)) {

			destination = (IDirectFBSurface*)(
					DFBDeviceScreen::createUnderlyingSurface(&surDsc));

			renderedSurface = new DFBSurface(myScreen, destination);

			if (imgDsc.caps & DICAPS_ALPHACHANNEL) {
				/*clog << "ImagePlayer::ImagePlayer(" << mrl << ")";
				clog << " setted alphachannel: ";*/

				renderedSurface->setCaps(DWCAPS_ALPHACHANNEL);

				//alpha channel of gif does not exists anymore, it turn into
				//black src color key
				if (isGif) {
					renderedSurface->setChromaColor(0, 0, 0, 0xFF);
					//outputDisplay->setColorKey(0, 0, 0);
					//clog << "black color cause it is a gif image" << endl;
				}

				//clog << " trying to blit image alpha channel" << endl;

				DFBCHECK(destination->SetBlittingFlags(destination,
					 (DFBSurfaceBlittingFlags)(DSBLIT_BLEND_ALPHACHANNEL)));

				/*clog << "ImagePlayer::ImagePlayer(" << mrl << ")";
				clog << " setted alpha: '";
				clog << (((int)(imgDsc.colorkey_r & 0xFF)) & 0xFF);
				clog << ", " << (((int)(imgDsc.colorkey_g)) & 0xFF);
				clog << ", " << (((int)(imgDsc.colorkey_b)) & 0xFF);
				clog << "'" << endl;*/
			}

			if (imgDsc.caps & DICAPS_COLORKEY) {
				DFBCHECK(destination->SetBlittingFlags(destination,
					    (DFBSurfaceBlittingFlags)(
					    DSBLIT_BLEND_ALPHACHANNEL |
					    DSBLIT_SRC_COLORKEY)));

				renderedSurface->setChromaColor(
						imgDsc.colorkey_r,
					    imgDsc.colorkey_g,
					    imgDsc.colorkey_b,
					    0xFF);

				/*clog << "ImagePlayer::ImagePlayer(" << mrl << ")";
				clog << " setted colorkey: '";
				clog << (((int)(imgDsc.colorkey_r & 0xFF)) & 0xFF);
				clog << ", " << (((int)(imgDsc.colorkey_g)) & 0xFF);
				clog << ", " << (((int)(imgDsc.colorkey_b)) & 0xFF);
				clog << "'" << endl;*/
			}

			if (imgDsc.caps & DICAPS_NONE) {
				DFBCHECK(destination->SetBlittingFlags(destination,
					    (DFBSurfaceBlittingFlags)DSBLIT_NOFX));

				renderedSurface->setCaps(DWCAPS_NONE);
				/*clog << "ImagePlayer::ImagePlayer(" << mrl << ")";
				clog << " NOFX" << endl;*/
			}
		}

		if (destination != NULL && renderedSurface != NULL) {
			DFBCHECK(ip->RenderTo(
					ip,
					(IDirectFBSurface*)(renderedSurface->getContent()), NULL));
		}
		return renderedSurface;
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
