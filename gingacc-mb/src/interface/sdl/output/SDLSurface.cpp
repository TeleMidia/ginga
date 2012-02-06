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

#include <stdlib.h>
#include "mb/interface/sdl/output/SDLSurface.h"
#include "mb/interface/Matrix.h"
#include "mb/interface/sdl/output/SDLWindow.h"
#include "mb/interface/sdl/SDLDeviceScreen.h"
#include "mb/interface/IFontProvider.h"
#include "mb/LocalScreenManager.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	SDLSurface::SDLSurface(GingaScreenID screenId) {
		initialize(screenId);
	}

	SDLSurface::SDLSurface(GingaScreenID screenId, void* underlyingSurface) {
		initialize(screenId);

		this->sur = (SDL_Surface*)underlyingSurface;
	}

	SDLSurface::SDLSurface(GingaScreenID screenId, int w, int h) {
		/*SDLSurfaceDescription surDsc;

		initialize(screenId);

		surDsc.width = w;
		surDsc.height = h;
		surDsc.pixelformat = DSPF_LUT8;
		surDsc.caps = (SDLSurfaceCapabilities)(DSCAPS_ALL);
		surDsc.flags = (SDLSurfaceDescriptionFlags)(
				DSDESC_CAPS | DSDESC_WIDTH |
				DSDESC_HEIGHT | DSDESC_PIXELFORMAT);

		this->sur = SDLDeviceScreen::createUnderlyingSurface(&surDsc);*/
	}

	SDLSurface::~SDLSurface() {
		if (chromaColor != NULL) {
			delete chromaColor;
			chromaColor = NULL;
		}

		LocalScreenManager::getInstance()->releaseSurface(myScreen, this);
	}

	void SDLSurface::initialize(GingaScreenID screenId) {
		this->myScreen      = screenId;
		this->sur           = NULL;
		this->parent        = NULL;
		this->chromaColor   = NULL;
		this->caps          = 0;
		this->hasExtHandler = false;
	}

	void SDLSurface::write(int x, int y, int w, int h, int pitch, char* buff) {

	}

	void SDLSurface::setExternalHandler(bool extHandler) {
		this->hasExtHandler = extHandler;
	}

	bool SDLSurface::hasExternalHandler() {
		return this->hasExtHandler;
	}

	void SDLSurface::addCaps(int caps) {
		this->caps = this->caps | caps;
	}

	void SDLSurface::setCaps(int caps) {
		this->caps = caps;
	}

	int SDLSurface::getCap(string cap) {
		/*if (cap == "ALL") {
			return DWCAPS_ALL;
		} else if (cap == "NOSTRUCTURE") {
			return DWCAPS_NODECORATION;
		} else if (cap == "ALPHACHANNEL") {
			return DWCAPS_ALPHACHANNEL;
		} else {
			return DWCAPS_NONE;
		}*/
		return 0;
	}

	int SDLSurface::getCaps() {
		return this->caps;
	}

	void* SDLSurface::getContent() {
		return sur;
	}

	void SDLSurface::setContent(void* surface) {
		/*if (this->sur != NULL && surface != NULL) {
			if (parent == NULL || (parent)->removeChildSurface(this)) {
				SDLDeviceScreen::releaseUnderlyingSurface(sur);
				sur = NULL;
			}
		}
		this->sur = (IDirectFBSurface*)surface;*/
	}

	bool SDLSurface::setParent(void* parentWindow) {
		/*this->parent = (IWindow*)parentWindow;
		if (parent != NULL && chromaColor != NULL) {
			parent->setColorKey(
				    chromaColor->getR(),
				    chromaColor->getG(),
				    chromaColor->getB());
		}

		if (this->sur == NULL && parent != NULL) {
			IDirectFBWindow* wgWin;

			wgWin = (IDirectFBWindow*)(parent->getContent());
			SDLCHECK(wgWin->GetSurface(wgWin, &sur));
			SDLCHECK(sur->Clear(sur, 0, 0, 0, 0x00));
			parent->setReleaseListener(this);
			return false;
		}

		if (parent != NULL) {
			parent->addChildSurface(this);
		}*/
		return true;
	}

	void* SDLSurface::getParent() {
		return this->parent;
	}

	void SDLSurface::setChromaColor(IColor* color) {
		if (this->chromaColor != NULL) {
			delete this->chromaColor;
			chromaColor = NULL;
		}

		this->chromaColor = color;

		/*if (sur != NULL) {
			SDLCHECK(sur->SetSrcColorKey(
					sur,
				    chromaColor->getR(),
				    chromaColor->getG(),
				    chromaColor->getB()));

			SDLCHECK(sur->SetBlittingFlags(sur,
					(SDLSurfaceBlittingFlags)(
							DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_SRC_COLORKEY)));

		} else {
			clog << "SDLSurface::setChromaColor Warning! ";
			clog << "Can't set chroma color: ";
			clog << "internal surface is NULL" << endl;
		}*/
	}

	IColor* SDLSurface::getChromaColor() {
		return this->chromaColor;
	}

	void SDLSurface::clearContent() {
		if (sur == NULL) {
			clog << "SDLSurface::clearContent Warning! ";
			clog << "Can't clear content: ";
			clog << "internal surface is NULL" << endl;
			return;
		}

		//sur->Clear(sur, 0, 0, 0, 0xFF);
		if (parent != NULL) {
			parent->clearContent();
		}
	}

	void SDLSurface::clearSurface() {
		if (sur == NULL) {
			clog << "SDLSurface::clearSurface Warning! ";
			clog << "Can't clear surface: ";
			clog << "internal surface is NULL" << endl;
			return;
		}

		//sur->Clear(sur, 0, 0, 0, 0xFF);
	}

	ISurface* SDLSurface::getSubSurface(int x, int y, int w, int h) {
		/*IDirectFBSurface* s = NULL;
		ISurface* subSurface = NULL;
		SDLRectangle rect;

		if (this->sur == NULL) {
			clog << "SDLSurface::getSubSurface Warning! ";
			clog << "Can't get sub surface: ";
			clog << "internal surface is NULL" << endl;
			return NULL;
		}

		rect.x = x;
		rect.y = y;
		rect.w = w;
		rect.h = h;

		SDLCHECK(sur->GetSubSurface(sur, &rect, &s));

		subSurface = LocalScreenManager::getInstance()->createSurfaceFrom(
				myScreen, s);

		subSurface->setParent(parent);
		return subSurface;*/
		return NULL;
	}

	void SDLSurface::drawLine(int x1, int y1, int x2, int y2) {

	}

	void SDLSurface::drawRectangle(int x, int y, int w, int h) {

	}

	void SDLSurface::fillRectangle(int x, int y, int w, int h) {

	}

	void SDLSurface::drawString(int x, int y, const char* txt) {

	}

	void SDLSurface::setBorder(IColor* borderColor) {

	}

	void SDLSurface::setColor(IColor* writeColor) {

	}

	void SDLSurface::setBgColor(IColor* bgColor) {

	}

	void SDLSurface::setFont(void* font) {

	}

	void SDLSurface::flip() {

	}

	void SDLSurface::scale(double x, double y) {
		int width, height;

		if (sur == NULL) {
			clog << "SDLSurface::scale Warning! ";
			clog << "Can't scale surface: ";
			clog << "internal surface is NULL" << endl;
			return;
		}

		/*Scale the matrix*/
		matrix_t matrix;
		Matrix::initTranslate(&matrix, width/2, height/2);				
		Matrix::scale(&matrix, x, y);

		Matrix::setMatrix(&matrix, this);
	}

	void SDLSurface::blit(
			int x, int y, ISurface* src,
			int srcX, int srcY, int srcW, int srcH) {


	}

	void SDLSurface::getStringExtents(const char* text, int* w, int* h) {
		/*SDLRectangle rect;
		IDirectFBFont* font;

		if (sur != NULL) {
			SDLCHECK( sur->GetFont(sur, &font) );
			SDLCHECK( font->GetStringExtents(font, text, -1, &rect, NULL) );
			*w = rect.w;
			*h = rect.h;

		} else {
			clog << "SDLSurface::getStringExtends Warning! ";
			clog << "Can't get string info: ";
			clog << "internal surface is NULL" << endl;
		}*/
	}

	void SDLSurface::setClip(int x, int y, int w, int h) {

	}

	void SDLSurface::getSize(int* w, int* h) {

	}

	string SDLSurface::getDumpFileUri() {
		string uri = "";

		/*if (sur == NULL) {
			clog << "SDLSurface::getDumpFileUri Warning! ";
			clog << "Can't dump surface bitmap: ";
			clog << "internal surface is NULL" << endl;

			uri = "";

		} else {
			uri = "/tmp/dump_0000";
			remove((char*)((uri + ".ppm").c_str()));
			remove((char*)((uri + ".pgm").c_str()));
			sur->Dump(sur, "/tmp", "dump");
		}*/

		return uri + ".ppm";
	}

	void SDLSurface::setMatrix(void* matrix) {
		/*if (sur != NULL) {
			sur->SetMatrix(sur, (const int*)matrix);
		}*/
	}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::mb::ISurface*
		createSDLSurface(GingaScreenID screenId, void* sur, int w, int h) {

	if (sur != NULL) {
		return (new ::br::pucrio::telemidia::ginga::core::mb::
				SDLSurface(screenId, sur));

	} else if (w != 0 && h != 0) {
		return (new ::br::pucrio::telemidia::ginga::core::mb::
				SDLSurface(screenId, w, h));

	} else {
		return (new ::br::pucrio::telemidia::ginga::core::mb::
				SDLSurface(screenId));
	}
}

extern "C" void destroySDLSurface(
		::br::pucrio::telemidia::ginga::core::mb::ISurface* s) {

	delete s;
}
