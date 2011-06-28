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

#include "system/io/interface/content/image/DFBImageProvider.h"
#include "system/io/interface/output/dfb/DFBWindow.h"
#include "system/io/interface/output/dfb/DFBSurface.h"
#include "system/io/LocalDeviceManager.h"

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

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace system {
namespace io {
	DFBImageProvider::DFBImageProvider(const char* mrl) {
		IDirectFB* dfb = NULL;
		decoder = NULL;

		if (mrl != NULL) {
			dfb = (IDirectFB*)(LocalDeviceManager::getInstance()->getGfxRoot());
			DFBCHECK(dfb->CreateImageProvider(dfb, mrl, &decoder));
		}
	}

	DFBImageProvider::~DFBImageProvider() {
		decoder->Release(decoder);
		decoder = NULL;
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
		ISurface* renderedSurface = NULL;

		IDirectFBImageProvider* ip;
		ip = (IDirectFBImageProvider*)decoder;

		DFBImageDescription imgDsc;
		IDirectFBSurface* destination = NULL;
		DFBSurfaceDescription surDsc;
		IColor* chromaKey = NULL;

		//IDirectFBSurface* source;
		if ((ip->GetImageDescription(ip, &imgDsc) == DFB_OK) &&
			 (ip->GetSurfaceDescription(ip, &surDsc) == DFB_OK)) {

			destination = (IDirectFBSurface*)(
					LocalDeviceManager::getInstance()->createSurface(&surDsc));

			renderedSurface = new DFBSurface(destination);

			if (imgDsc.caps & DICAPS_ALPHACHANNEL) {
				/*cout << "ImagePlayer::ImagePlayer(" << mrl << ")";
				cout << " setted alphachannel: ";*/

				renderedSurface->setCaps(DWCAPS_ALPHACHANNEL);

				//alpha channel of gif does not exists anymore, it turn into
				//black src color key
				if (isGif) {
					chromaKey = new Color(0, 0, 0);
					renderedSurface->setChromaColor(chromaKey);
					//outputDisplay->setColorKey(0, 0, 0);
					//cout << "black color cause it is a gif image" << endl;
				}

				//cout << " trying to blit image alpha channel" << endl;

				DFBCHECK(destination->SetBlittingFlags(destination,
					 (DFBSurfaceBlittingFlags)(DSBLIT_BLEND_ALPHACHANNEL)));

				/*cout << "ImagePlayer::ImagePlayer(" << mrl << ")";
				cout << " setted alpha: '";
				cout << (((int)(imgDsc.colorkey_r & 0xFF)) & 0xFF);
				cout << ", " << (((int)(imgDsc.colorkey_g)) & 0xFF);
				cout << ", " << (((int)(imgDsc.colorkey_b)) & 0xFF);
				cout << "'" << endl;*/
			}

			if (imgDsc.caps & DICAPS_COLORKEY) {
				chromaKey = new Color(
					    imgDsc.colorkey_r,
					    imgDsc.colorkey_g,
					    imgDsc.colorkey_b);

				DFBCHECK(destination->SetBlittingFlags(destination,
					    (DFBSurfaceBlittingFlags)(
					    DSBLIT_BLEND_ALPHACHANNEL |
					    DSBLIT_SRC_COLORKEY)));

				renderedSurface->setChromaColor(chromaKey);
				/*cout << "ImagePlayer::ImagePlayer(" << mrl << ")";
				cout << " setted colorkey: '";
				cout << (((int)(imgDsc.colorkey_r & 0xFF)) & 0xFF);
				cout << ", " << (((int)(imgDsc.colorkey_g)) & 0xFF);
				cout << ", " << (((int)(imgDsc.colorkey_b)) & 0xFF);
				cout << "'" << endl;*/
			}

			if (imgDsc.caps & DICAPS_NONE) {
				DFBCHECK(destination->SetBlittingFlags(destination,
					    (DFBSurfaceBlittingFlags)DSBLIT_NOFX));

				renderedSurface->setCaps(DWCAPS_NONE);
				/*cout << "ImagePlayer::ImagePlayer(" << mrl << ")";
				cout << " NOFX" << endl;*/
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
			wclog << "Warning! VideoPlayer::frameCB" << endl;
		}
		return NULL;
	}*/
}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::system::io::IImageProvider*
		createImageProvider(const char* mrl) {

	return (new ::br::pucrio::telemidia::ginga::core::system::io::
			DFBImageProvider(mrl));
}

extern "C" void destroyImageProvider(
		::br::pucrio::telemidia::ginga::core::system::io::IImageProvider* ip) {

	delete ip;
}
