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

#include "../../../include/IOHandler.h"
#include "../../../include/Graphics.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace io {
	Surface::Surface() {
		this->sur = NULL;
		this->parent = NULL;
		this->chromaColor = NULL;
		this->caps = Window::CAPS_NONE;
	}

	Surface::Surface(void* someSurface) {
		this->sur = someSurface;
		this->parent = NULL;
		this->chromaColor = NULL;
		this->caps = Window::CAPS_NONE;
	}

	Surface::Surface(int w, int h) {
#if HAVE_DIRECTFB
		DFBSurfaceDescription surDsc;
		surDsc.width = w;
		surDsc.height = h;
		surDsc.pixelformat = DSPF_LUT8;
		surDsc.caps = (DFBSurfaceCapabilities)(DSCAPS_ALL);
		surDsc.flags = (DFBSurfaceDescriptionFlags)(
				DSDESC_CAPS | DSDESC_WIDTH |
				DSDESC_HEIGHT | DSDESC_PIXELFORMAT);

	     /*desc.flags       = DSDESC_CAPS | DSDESC_PIXELFORMAT;
	     desc.caps        = DSCAPS_PRIMARY | DSCAPS_DOUBLE;
	     desc.pixelformat = DSPF_LUT8;*/

		this->sur = IOHandler::createSurface(&surDsc);
#endif
		this->parent = NULL;
		this->chromaColor = NULL;
	}

	Surface::~Surface() {
		if (chromaColor != NULL) {
			delete chromaColor;
			chromaColor = NULL;
		}

		if (sur != NULL) {
			if (parent != NULL) {
				if (((Window*)parent)->removeChildSurface(this)) {
#if HAVE_DIRECTFB
					IDirectFBSurface* s;
					s = (IDirectFBSurface*)sur;
					DFBCHECK(s->Clear(s, 0, 0, 0, 0x00));
#endif
					IOHandler::releaseSurface(this);
					sur = NULL;
				}

			} else {
				IOHandler::releaseSurface(this);
				sur = NULL;
			}
		}
	}

	void Surface::addCaps(int caps) {
		this->caps = this->caps | caps;
	}

	void Surface::setCaps(int caps) {
		this->caps = caps;
	}

	int Surface::getCaps() {
		return this->caps;
	}

	void* Surface::getContent() {
		return sur;
	}

	void Surface::setContent(void* surface) {
		if (this->sur != NULL && surface != NULL) {
			if (parent == NULL || ((Window*)parent)->removeChildSurface(this)) {
#if HAVE_DIRECTFB
				IDirectFBSurface* s;
				s = (IDirectFBSurface*)sur;
				DFBCHECK(s->Clear(s, 0, 0, 0, 0x00));
#endif
				IOHandler::releaseSurface(this);
				sur = NULL;
			}
		}
		this->sur = surface;
	}

	bool Surface::setParent(void* parentWindow) {
		this->parent = parentWindow;
		if (parent != NULL && chromaColor != NULL) {
			((Window*)parent)->setColorKey(
				    chromaColor->getR(),
				    chromaColor->getG(),
				    chromaColor->getB());
		}

		if (this->sur == NULL && parent != NULL) {
#if HAVE_DIRECTFB
			IDirectFBWindow* wgWin;
			IDirectFBSurface* s;

			wgWin = (IDirectFBWindow*)(((Window*)parent)->getContent());

			DFBCHECK(wgWin->GetSurface(wgWin, &s));
			DFBCHECK(s->Clear(s, 0, 0, 0, 0x00));
			sur = (void*)s;
#endif
			return false;
		}

		if (parent != NULL) {
			((Window*)parent)->addChildSurface(this);
		}
		return true;
	}

	void* Surface::getParent() {
		return this->parent;
	}

	void Surface::setChromaColor(Color* color) {
		if (this->chromaColor != NULL) {
			delete this->chromaColor;
			chromaColor = NULL;
		}

		this->chromaColor = color;

		if (sur != NULL) {
#if HAVE_DIRECTFB
			IDirectFBSurface* s;
			s = (IDirectFBSurface*)sur;

			DFBCHECK(s->SetSrcColorKey(
					s,
				    chromaColor->getR(),
				    chromaColor->getG(),
				    chromaColor->getB()));

			DFBCHECK(s->SetBlittingFlags(s, DSBLIT_SRC_COLORKEY));
#endif
		}
	}

	Color* Surface::getChromaColor() {
		return this->chromaColor;
	}

	void Surface::clear() {
		if (sur == NULL) {
			return;
		}

#if HAVE_DIRECTFB
		IDirectFBSurface* s;
		s = (IDirectFBSurface*)sur;
		DFBCHECK(s->Clear(s, 0, 0, 0, 0xFF));
#endif
		if (parent != NULL) {
			((Window*)parent)->clear();
		}
	}

	void Surface::clearSurface() {
		if (sur == NULL) {
			return;
		}

#if HAVE_DIRECTFB
		IDirectFBSurface* s;
		s = (IDirectFBSurface*)sur;
		DFBCHECK(s->Clear(s, 0, 0, 0, 0xFF));
#endif
	}

	Surface* Surface::getSubSurface(int x, int y, int w, int h) {
		if (this->sur == NULL) {
			return NULL;
		}

		Surface* subSurface = NULL;

#if HAVE_DIRECTFB
		DFBRectangle rect;

		rect.x = x;
		rect.y = y;
		rect.w = w;
		rect.h = h;

		IDirectFBSurface* s;
		s = (IDirectFBSurface*)sur;
		DFBCHECK(s->GetSubSurface(s, &rect, &s));

		subSurface = new Surface(s);
		subSurface->setParent(parent);
#endif
		return subSurface;
	}

	void Surface::setBorder(Color* borderColor) {
		if (sur == NULL) {
			return;
		}

		int w, h;

#if HAVE_DIRECTFB
		IDirectFBSurface* s;
		s = (IDirectFBSurface*)sur;
		DFBCHECK(s->GetSize(s, &w, &h));

		DFBCHECK(s->SetColor(
				s,
				borderColor->getR(),
				borderColor->getG(),
				borderColor->getB(),
				0xFF));

		DFBCHECK(s->DrawRectangle(s, 0, 0, w, h));
#endif

		if (parent != NULL) {
			((Window*)parent)->validate();
		}
	}

	void Surface::setColor(Color* writeColor) {
		if (sur == NULL) {
			return;
		}

#if HAVE_DIRECTFB
		IDirectFBSurface* s;
		s = (IDirectFBSurface*)sur;

		DFBCHECK(s->SetColor(s,
				writeColor->getR(),
				writeColor->getG(),
				writeColor->getB(), 0xFF));
#endif
	}

	void Surface::setBgColor(Color* bgColor) {
		if (sur == NULL) {
			return;
		}

#if HAVE_DIRECTFB
		IDirectFBSurface* s;
		s = (IDirectFBSurface*)sur;
		DFBCHECK(s->Clear(
				s, bgColor->getR(), bgColor->getG(), bgColor->getB(), 0xFF));
#endif
	}

	void Surface::setFont(void* font) {
#if HAVE_DIRECTFB
		IDirectFBSurface* s;
		IDirectFBFont* f;

		s = (IDirectFBSurface*)sur;
		if (s != NULL) {
			f = (IDirectFBFont*)(((FontProvider*)font)->getContent());
			if (f != NULL) {
				DFBCHECK(s->SetFont(s, f));
				return;
			}
		}
		cout << "Surface::setFont Warning! Can't set font '" << f << "'";
		cout << " surface '" << s << "'" << endl;
#endif
	}

	void Surface::getSize(int* w, int* h) {
		if (sur == NULL) {
			return;
		}

#if HAVE_DIRECTFB
		IDirectFBSurface* s;
		s = (IDirectFBSurface*)sur;
		s->GetSize(s, w, h);
#endif
	}
}
}
}
}
}
}
