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

	SDLSurface::~SDLSurface() {
		bool mySurface = false;

		LocalScreenManager::getInstance()->releaseSurface(myScreen, this);

		releaseChromaColor();
		releaseBorderColor();
		releaseBgColor();
		releaseSurfaceColor();

		releaseFont();

		pthread_mutex_lock(&sMutex);
		if (owner && sur != NULL) {
			SDLDeviceScreen::createReleaseContainer(sur, NULL, NULL);
			sur = NULL;
		}
		pthread_mutex_unlock(&sMutex);
		pthread_mutex_destroy(&sMutex);

		if (LocalScreenManager::getInstance()->hasWindow(myScreen, parent)) {
			if (parent->removeChildSurface(this)) {
				mySurface = true;
			}
		}

		releaseDrawData();
		pthread_mutex_lock(&ddMutex);
		this->drawData.clear();
		pthread_mutex_unlock(&ddMutex);
		pthread_mutex_destroy(&ddMutex);
	}

	void SDLSurface::fill() {
		int r = 0, g = 0, b = 0, alpha = 0;

		pendingFill = false;
		if (sur != NULL) {
			if (bgColor != NULL) {
				r     = bgColor->getR();
				g     = bgColor->getG();
				b     = bgColor->getB();
				alpha = bgColor->getAlpha();
			}

			SDL_FillRect(sur, NULL, SDL_MapRGBA(sur->format, r, g, b, alpha));
		}
	}

	void SDLSurface::releaseChromaColor() {
		if (this->chromaColor != NULL) {
			delete this->chromaColor;
			chromaColor = NULL;
		}
	}

	void SDLSurface::releaseBorderColor() {
		if (this->borderColor != NULL) {
			delete this->borderColor;
			this->borderColor = NULL;
		}
	}

	void SDLSurface::releaseBgColor() {
		if (this->bgColor != NULL) {
			delete this->bgColor;
			this->bgColor = NULL;
		}
	}

	void SDLSurface::releaseSurfaceColor() {
		if (this->surfaceColor != NULL) {
			delete this->surfaceColor;
			this->surfaceColor = NULL;
		}
	}

	void SDLSurface::releaseFont() {
		LocalScreenManager::getInstance()->releaseFontProvider(myScreen, iFont);
		iFont = NULL;
	}

	void SDLSurface::releaseDrawData() {
		vector<DrawData*>::iterator i;

		pthread_mutex_lock(&ddMutex);
		if (!drawData.empty()) {
			i = drawData.begin();
			while (i != drawData.end()) {
				delete (*i);
				++i;
			}
			drawData.clear();
		}
		pthread_mutex_unlock(&ddMutex);
	}

	void SDLSurface::initialize(GingaScreenID screenId) {
		this->myScreen      = screenId;
		this->sur           = NULL;
		this->iFont         = NULL;
		this->parent        = NULL;
		this->chromaColor   = NULL;
		this->borderColor   = NULL;
		this->bgColor       = NULL;
		this->surfaceColor  = NULL;
		this->caps          = 0;
		this->hasExtHandler = false;
		this->owner         = false;
		this->pendingFill   = false;

		this->drawData.clear();

		pthread_mutex_init(&ddMutex, NULL);
		pthread_mutex_init(&sMutex, NULL);
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
		return 0;
	}

	int SDLSurface::getCaps() {
		return this->caps;
	}

	void* SDLSurface::getSurfaceContent() {
		return sur;
	}

	void SDLSurface::setSurfaceContent(void* surface) {
		if (this->sur != NULL && surface != NULL && this->sur != surface) {
//			if (parent == NULL || (parent)->removeChildSurface(this)) {
			if (owner) {
				SDLDeviceScreen::createReleaseContainer(sur, NULL, NULL);
				sur = NULL;
			}
		}

		this->owner = false;
		this->sur   = (SDL_Surface*)surface;
	}

	bool SDLSurface::setParent(void* parentWindow) {
		this->parent = (IWindow*)parentWindow;

		if (parent != NULL) {
			if (chromaColor != NULL) {
				parent->setColorKey(
						chromaColor->getR(),
						chromaColor->getG(),
						chromaColor->getB());
			}
		}

		if (parent != NULL) {
			parent->addChildSurface(this);
		}

		return true;
	}

	void* SDLSurface::getParent() {
		if (LocalScreenManager::getInstance()->hasWindow(myScreen, parent)) {
			return this->parent;

		} else {
			return NULL;
		}
	}

	void SDLSurface::clearContent() {
		clearSurface();

		if (parent != NULL) {
			parent->clearContent();
		}
	}

	void SDLSurface::clearSurface() {

		releaseDrawData();

		if (sur == NULL) {
			clog << "DFBSurface::clearContent Warning! ";
			clog << "Can't clear content: ";
			clog << "internal surface is NULL" << endl;
			return;
		}

		fill();
	}

	vector<DrawData*>* SDLSurface::createDrawDataList() {
		vector<DrawData*>* cloneDD = NULL;

		pthread_mutex_lock(&ddMutex);
		if (!drawData.empty()) {
			cloneDD = new vector<DrawData*>(drawData);
		}
		pthread_mutex_unlock(&ddMutex);

		return cloneDD;
	}

	void SDLSurface::pushDrawData(int c1, int c2, int c3, int c4, short type) {
		DrawData* dd;

		if (surfaceColor != NULL) {
			pthread_mutex_lock(&ddMutex);
			dd = new DrawData;
			dd->coord1   = c1;
			dd->coord2   = c2;
			dd->coord3   = c3;
			dd->coord4   = c4;
			dd->dataType = type;
			dd->r        = surfaceColor->getR();
			dd->g        = surfaceColor->getG();
			dd->b        = surfaceColor->getB();
			dd->a        = surfaceColor->getAlpha();

			clog << "SDLSurface::pushDrawData current size = '";
			clog << drawData.size() << "'" << endl;

			drawData.push_back(dd);
			pthread_mutex_unlock(&ddMutex);
		}
	}

	void SDLSurface::drawLine(int x1, int y1, int x2, int y2) {
		clog << "SDLSurface::drawLine '";
		clog << x1 << ", " << y1 << ", " << x2 << ", " << y2 << "'";
		clog << endl;

		pushDrawData(x1, y1, x2, y2, SDLWindow::DDT_LINE);
	}

	void SDLSurface::drawRectangle(int x, int y, int w, int h) {
		clog << "SDLSurface::drawRectangle '";
		clog << x << ", " << y << ", " << w << ", " << h << "'";
		clog << endl;

		pushDrawData(x, y, w, h, SDLWindow::DDT_RECT);
	}

	void SDLSurface::fillRectangle(int x, int y, int w, int h) {
		SDL_Rect rect;
		int r, g, b;

		clog << "SDLSurface::fillRectangle '";
		clog << x << ", " << y << ", " << w << ", " << h << "'";
		clog << endl;

		createSurface();

		if (sur != NULL && surfaceColor != NULL) {
			rect.x = x;
			rect.y = y;
			rect.w = w;
			rect.h = h;

			r = surfaceColor->getR();
			g = surfaceColor->getG();
			b = surfaceColor->getB();

			SDL_FillRect(sur, &rect, SDL_MapRGB(sur->format, r, g, b));
		}

		//pushDrawData(x, y, w, h, SDLWindow::DDT_FILL_RECT);
	}

	void SDLSurface::drawString(int x, int y, const char* txt) {
		if (iFont != NULL && txt != NULL) {
			if (x < 0) {
				x = 0;
			}

			if (y < 0) {
				y = 0;
			}

			if (pendingFill) {
				fill();
			}

			iFont->playOver(this, txt, x, y, 0);
		}
	}

	void SDLSurface::setChromaColor(int r, int g, int b, int alpha) {
		releaseChromaColor();

		this->chromaColor = new Color(r, g, b, alpha);

		if (sur != NULL) {
			SDL_SetColorKey(sur, SDL_TRUE, SDL_MapRGB(sur->format, r, g, b));
		}

		if (parent != NULL) {
			parent->setColorKey(r, g, b);
		}
	}

	IColor* SDLSurface::getChromaColor() {
		return this->chromaColor;
	}

	void SDLSurface::setBorderColor(int r, int g, int b, int alpha) {
		releaseBorderColor();

		this->borderColor = new Color(r, g, b, alpha);
	}

	IColor* SDLSurface::getBorderColor() {
		return borderColor;
	}

	void SDLSurface::setBgColor(int r, int g, int b, int alpha) {
		releaseBgColor();

		this->bgColor = new Color(r, g, b, alpha);
		pendingFill = true;
	}

	IColor* SDLSurface::getBgColor() {
		return bgColor;
	}

	void SDLSurface::setColor(int r, int g, int b, int alpha) {
		releaseSurfaceColor();

		this->surfaceColor = new Color(r, g, b, alpha);
	}

	IColor* SDLSurface::getColor() {
		return surfaceColor;
	}

	void SDLSurface::setSurfaceFont(void* font) {
		if (iFont != font) {
			releaseFont();
		}

		iFont = (IFontProvider*)font;
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

	void SDLSurface::createSurface() {
		unsigned int r, g, b, a;

		if (sur == NULL && parent != NULL) {
			sur = (SDL_Surface*)(parent->getContent());
			if (sur == NULL) {
				this->owner = true;
				pthread_mutex_lock(&sMutex);
				SDLDeviceScreen::getRGBAMask(24, &r, &g, &b, &a);

				sur = SDL_CreateRGBSurface(
						0,
						parent->getW(),
						parent->getH(),
						24,
						r, g, b, a);

				((SDLWindow*)parent)->setRenderedSurface(sur);
				pthread_mutex_unlock(&sMutex);

			} else {
				this->owner = false;
			}
		}
	}

	void SDLSurface::blit(
			int x, int y, ISurface* src,
			int srcX, int srcY, int srcW, int srcH) {

		SDL_Rect srcRect;
		SDL_Rect* s = NULL;
		SDL_Rect dstRect;
		SDL_Surface* uSur;

		createSurface();
		if (sur != NULL) {
			uSur = (SDL_Surface*)(src->getSurfaceContent());

			if (uSur != NULL) {
				if (srcX >= 0) {
					srcRect.x = srcX;
					srcRect.y = srcY;
					srcRect.w = srcW;
					srcRect.h = srcH;

					s = &srcRect;
				}

				dstRect.x = x;
				dstRect.y = y;

				if (srcW > 0) {
					dstRect.w = srcW;
					dstRect.h = srcH;

				} else {
					dstRect.w = uSur->w;
					dstRect.h = uSur->h;
				}

				if (pendingFill) {
					fill();
				}
				SDL_UpperBlit(uSur, s, sur, &dstRect);
			}

		} else {
			clog << "SDLSurface::blit(" << this << ") Warning! ";
			clog << "Can't blit: ";
			clog << "underlying surface is NULL. Destination ISurface ";
			clog << "address would be '" << src << "'" << endl;
		}
	}

	void SDLSurface::getStringExtents(const char* text, int* w, int* h) {
		if (iFont != NULL) {
			iFont->getStringExtents(text, w, h);

		} else {
			clog << "SDLSurface::getStringExtends Warning! ";
			clog << "Can't get string info: ";
			clog << "font provider is NULL" << endl;
		}
	}

	void SDLSurface::setClip(int x, int y, int w, int h) {
		SDL_Rect rect;

		if (sur != NULL) {
			rect.x = x;
			rect.y = y;
			rect.w = w;
			rect.h = h;
			SDL_SetClipRect(sur, &rect);

		} else {
			clog << "SDLSurface::setClip Warning! NULL underlying surface";
			clog << endl;
		}
	}

	void SDLSurface::getSize(int* w, int* h) {
		if (sur != NULL) {
			*w = sur->w;
			*h = sur->h;

		} else if (parent != NULL) {
			*w = parent->getW();
			*h = parent->getH();

		} else {
			clog << "SDLSurface::getSize Warning! NULL underlying surface and";
			clog << " parent";
			clog << endl;
		}
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
