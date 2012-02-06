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

#include "mb/LocalScreenManager.h"
#include "mb/interface/sdl/content/text/SDLFontProvider.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	/*const short SDLFontProvider::A_LEFT = DSTF_LEFT;
	const short SDLFontProvider::A_CENTER = DSTF_CENTER;
	const short SDLFontProvider::A_RIGHT = DSTF_RIGHT;

	const short SDLFontProvider::A_TOP = DSTF_TOP;
	const short SDLFontProvider::A_TOP_CENTER = DSTF_TOPCENTER;
	const short SDLFontProvider::A_TOP_LEFT = DSTF_TOPLEFT;
	const short SDLFontProvider::A_TOP_RIGHT = DSTF_TOPRIGHT;

	const short SDLFontProvider::A_BOTTOM = DSTF_BOTTOM;
	const short SDLFontProvider::A_BOTTOM_CENTER = DSTF_BOTTOMCENTER;
	const short SDLFontProvider::A_BOTTOM_LEFT = DSTF_BOTTOMLEFT;
	const short SDLFontProvider::A_BOTTOM_RIGHT = DSTF_BOTTOMRIGHT;*/

	SDLFontProvider::SDLFontProvider(
			GingaScreenID screenId, const char* fontUri, int heightInPixel) {

	}

	SDLFontProvider::~SDLFontProvider() {

	}

	void* SDLFontProvider::getContent() {
		return NULL;
	}

	int SDLFontProvider::getMaxAdvance() {
		return 0;
	}

	int SDLFontProvider::getStringWidth(const char* text, int textLength) {
		return 0;
	}

	int SDLFontProvider::getHeight() {
		int fontHeight = 0;

		return fontHeight;
	}

	void SDLFontProvider::playOver(
			void* surface, const char* text, int x, int y, short align) {

	}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::mb::IFontProvider*
		createSDLFontProvider(
				GingaScreenID screenId,
				const char* fontUri,
				int heightInPixel) {

	return (new ::br::pucrio::telemidia::ginga::core::mb::
			SDLFontProvider(screenId, fontUri, heightInPixel));
}

extern "C" void destroySDLFontProvider(
		::br::pucrio::telemidia::ginga::core::mb::IFontProvider* fp) {

	delete fp;
}
