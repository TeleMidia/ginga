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
#include "../../../include/IOHandler.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace io {
#if HAVE_DIRECTFB
	const short FontProvider::A_LEFT = DSTF_LEFT;
	const short FontProvider::A_CENTER = DSTF_CENTER;
	const short FontProvider::A_RIGHT = DSTF_RIGHT;

	const short FontProvider::A_TOP = DSTF_TOP;
	const short FontProvider::A_TOP_CENTER = DSTF_TOPCENTER;
	const short FontProvider::A_TOP_LEFT = DSTF_TOPLEFT;
	const short FontProvider::A_TOP_RIGHT = DSTF_TOPRIGHT;

	const short FontProvider::A_BOTTOM = DSTF_BOTTOM;
	const short FontProvider::A_BOTTOM_CENTER = DSTF_BOTTOMCENTER;
	const short FontProvider::A_BOTTOM_LEFT = DSTF_BOTTOMLEFT;
	const short FontProvider::A_BOTTOM_RIGHT = DSTF_BOTTOMRIGHT;
#endif

	FontProvider::FontProvider(char* fontUri, int heightInPixel) {
		this->renderer = IOHandler::createFont(fontUri, heightInPixel);
#if HAVE_DIRECTFB
		IDirectFBFont* f;
		f = (IDirectFBFont*)renderer;

		DFBCHECK(f->SetEncoding(f, 1));
#endif
	}

	FontProvider::~FontProvider() {
		IOHandler::releaseFont(this);
	}

	void* FontProvider::getContent() {
		return renderer;
	}

	int FontProvider::getMaxAdvance() {
		int maxAdvance = 0;
#if HAVE_DIRECTFB
		IDirectFBFont* f;
		f = (IDirectFBFont*)renderer;

		DFBCHECK(f->GetMaxAdvance(f, &maxAdvance));
#endif
		return maxAdvance;
	}

	int FontProvider::getStringWidth(char* text, int textLength) {
		int width = 0;

#if HAVE_DIRECTFB
		IDirectFBFont* f;
		f = (IDirectFBFont*)renderer;

		DFBCHECK(f->GetStringWidth(f, (const char*)text, textLength, &width));
#endif

		return width;
	}

	int FontProvider::getHeight() {
		int fontHeight = 0;

#if HAVE_DIRECTFB
		IDirectFBFont* f;
		f = (IDirectFBFont*)renderer;
		DFBCHECK(f->GetHeight(f, &fontHeight));
#endif

		return fontHeight;
	}

	void FontProvider::playOver(
			void* surface, char* text, int x, int y, short align) {

#if HAVE_DIRECTFB
		IDirectFBSurface* s;
		IDirectFBFont* f;
		f = (IDirectFBFont*)renderer;
		s = (IDirectFBSurface*)(((Surface*)surface)->getContent());

		DFBCHECK(s->DrawString(
				s, text, -1, x, y, (DFBSurfaceTextFlags)(align)));
#endif
	}
}
}
}
}
}
}
