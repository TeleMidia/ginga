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

#include "system/io/LocalDeviceManager.h"
#include "system/io/interface/content/text/DFBFontProvider.h"

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
	static int dfpRefs = 0;
#endif //DFBTM_PATCH

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace system {
namespace io {
	const short DFBFontProvider::A_LEFT = DSTF_LEFT;
	const short DFBFontProvider::A_CENTER = DSTF_CENTER;
	const short DFBFontProvider::A_RIGHT = DSTF_RIGHT;

	const short DFBFontProvider::A_TOP = DSTF_TOP;
	const short DFBFontProvider::A_TOP_CENTER = DSTF_TOPCENTER;
	const short DFBFontProvider::A_TOP_LEFT = DSTF_TOPLEFT;
	const short DFBFontProvider::A_TOP_RIGHT = DSTF_TOPRIGHT;

	const short DFBFontProvider::A_BOTTOM = DSTF_BOTTOM;
	const short DFBFontProvider::A_BOTTOM_CENTER = DSTF_BOTTOMCENTER;
	const short DFBFontProvider::A_BOTTOM_LEFT = DSTF_BOTTOMLEFT;
	const short DFBFontProvider::A_BOTTOM_RIGHT = DSTF_BOTTOMRIGHT;

	DFBFontProvider::DFBFontProvider(const char* fontUri, int heightInPixel) {
		IDirectFB* dfb;
		DFBFontDescription desc;

#if DFBTM_PATCH
		dfpRefs++;
#endif //DFBTM_PATCH

		string aux = "";

		font = NULL;

#ifdef _WIN32
		aux = getUserDocAndSetPath().append("\\config\\decker.ttf");
#else
		aux = "/usr/local/share/directfb-examples/fonts/decker.ttf";
#endif

		if (strcmp(fontUri, "") == 0 || !fileExists(fontUri)) {
			if (!fileExists(aux)) {
				clog << "DFBFontProvider Warning! File not found: '";
				clog << fontUri << "' and '";
				clog << aux << "'" << endl;
				aux = "";
			}

		} else {
			aux = fontUri;
		}

		if (heightInPixel < 1) {
			aux = "";
		}

		if (aux != "") {
			dfb = (IDirectFB*)(LocalDeviceManager::getInstance()->getGfxRoot());
			desc.flags = (DFBFontDescriptionFlags)(
					DFDESC_HEIGHT | DFDESC_ATTRIBUTES);

			desc.height = heightInPixel;
			desc.attributes = (DFBFontAttributes)0;

			DFBCHECK(dfb->CreateFont(dfb, aux.c_str(), &desc, &font));
		}
	}

	DFBFontProvider::~DFBFontProvider() {
		if (font != NULL) {
			font->Release(font);
			font = NULL;
		}

#if DFBTM_PATCH
		dfpRefs--;

		if (dfpRefs == 0) {
			DirectReleaseInterface("IDirectFBFont");
		}
#endif //DFBTM_PATCH
	}

	void* DFBFontProvider::getContent() {
		return font;
	}

	int DFBFontProvider::getMaxAdvance() {
		int maxAdvance = 0;

		if (font != NULL) {
			DFBCHECK(font->GetMaxAdvance(font, &maxAdvance));
		}

		return maxAdvance;
	}

	int DFBFontProvider::getStringWidth(const char* text, int textLength) {
		int width = 0;

		if (font != NULL) {
			DFBCHECK(font->GetStringWidth(font, text, textLength, &width));
		}

		return width;
	}

	int DFBFontProvider::getHeight() {
		int fontHeight = 0;

		if (font != NULL) {
			DFBCHECK(font->GetHeight(font, &fontHeight));
		}
		return fontHeight;
	}

	void DFBFontProvider::playOver(
			void* surface, const char* text, int x, int y, short align) {

		IDirectFBSurface* s;
		s = (IDirectFBSurface*)(((ISurface*)surface)->getContent());
		DFBCHECK(s->DrawString(
				s, text, -1, x, y, (DFBSurfaceTextFlags)(align)));
	}
}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::system::io::IFontProvider*
		createFontProvider(const char* fontUri, int heightInPixel) {

	return (new ::br::pucrio::telemidia::ginga::core::system::io::
			DFBFontProvider(fontUri, heightInPixel));
}

extern "C" void destroyFontProvider(
		::br::pucrio::telemidia::ginga::core::system::io::IFontProvider* fp) {

	delete fp;
}
