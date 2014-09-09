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
	GingaSurfaceID SDLSurface::refIdCounter = 1;

	SDLSurface::SDLSurface(GingaScreenID screenId) {
		initialize(screenId, refIdCounter++);
	}

	SDLSurface::SDLSurface(GingaScreenID screenId, void* underlyingSurface) {
		initialize(screenId, refIdCounter++);

		this->sur = (SDL_Surface*)underlyingSurface;
	}

	SDLSurface::~SDLSurface() {
		isDeleting = true;
		Thread::mutexLock(&sMutex);
		Thread::mutexLock(&pMutex);

		if (!LocalScreenManager::getInstance()->releaseSurface(
				myScreen, this)) {

			clog << "SDLSurface::~SDLSurface Warning! Can't find ISur" << endl;
		}

		releaseChromaColor();
		releaseBorderColor();
		releaseBgColor();
		releaseSurfaceColor();

		releaseFont();

		if (parent != NULL &&
				LocalScreenManager::getInstance()->hasWindow(
								myScreen, parent->getId())) {

			if (parent->getContent() == sur) {
				((SDLWindow*)parent)->setRenderedSurface(NULL);
			}
		}

		if (sur != NULL) {
			SDLDeviceScreen::createReleaseContainer(sur, NULL, NULL);
		}

		sur = NULL;

		Thread::mutexUnlock(&sMutex);
		Thread::mutexDestroy(&sMutex);

		releasePendingSurface();
		Thread::mutexUnlock(&pMutex);
		Thread::mutexDestroy(&pMutex);

		releaseDrawData();
		Thread::mutexLock(&ddMutex);
		Thread::mutexUnlock(&ddMutex);
		Thread::mutexDestroy(&ddMutex);
	}

	void SDLSurface::releasePendingSurface() {
		if (pending != NULL) {
			SDLDeviceScreen::createReleaseContainer(pending, NULL, NULL);
			pending = NULL;
		}
	}

	bool SDLSurface::createPendingSurface() {
		if (pending == NULL) {
			pending = createSurface();

			if (sur != NULL && pending != NULL) {
				SDLDeviceScreen::lockSDL();
				if (SDL_UpperBlit(sur, NULL, pending, NULL) < 0) {
					clog << "SDLSurface::createPendingSurface SDL error: '";
					clog << SDL_GetError() << "'" << endl;
				}
				SDLDeviceScreen::unlockSDL();
			}
		}

		return (pending != NULL);
	}

	void SDLSurface::checkPendingSurface() {
		Thread::mutexLock(&pMutex);
		if (pending != NULL) {
			if (parent != NULL && parent->getContent() == sur) {
				((SDLWindow*)parent)->setRenderedSurface(pending);
			}

			Thread::mutexLock(&sMutex);
			SDLDeviceScreen::createReleaseContainer(sur, NULL, NULL);
			sur = pending;
			pending = NULL;
			Thread::mutexUnlock(&sMutex);
			Thread::mutexUnlock(&pMutex);
			releaseDrawData();

		} else {
			Thread::mutexUnlock(&pMutex);
		}
	}

	void SDLSurface::fill() {
		int r = 0, g = 0, b = 0, alpha = 0;

		Thread::mutexLock(&sMutex);
		if (sur != NULL) {
			if (bgColor != NULL) {
				r     = bgColor->getR();
				g     = bgColor->getG();
				b     = bgColor->getB();
				alpha = bgColor->getAlpha();
			}

			Thread::mutexLock(&pMutex);
			releasePendingSurface();

			pending = createSurface();

			if (pending != NULL && bgColor != NULL) {
				//TODO: check why we have to set BGR instead of RGB
				if (SDL_FillRect(
					pending,
					NULL,
					SDL_MapRGB(pending->format, b, g, r)) < 0) {

					clog << "SDLSurface::fill SDL error: '";
					clog << SDL_GetError() << "'" << endl;
				}
			}

			Thread::mutexUnlock(&pMutex);
		}

		releaseDrawData();
		Thread::mutexUnlock(&sMutex);
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
		if (iFont != NULL)
		{
			LocalScreenManager::getInstance()->releaseFontProvider(myScreen,
		                                                       iFont->getId());
			iFont = NULL;
		}
	}

	void SDLSurface::releaseDrawData() {
		vector<DrawData*>::iterator i;

		Thread::mutexLock(&ddMutex);
		if (!drawData.empty()) {
			i = drawData.begin();
			while (i != drawData.end()) {
				delete (*i);
				++i;
			}
			drawData.clear();
		}
		Thread::mutexUnlock(&ddMutex);
	}

	void SDLSurface::initialize(const GingaScreenID &screenId,
	                            const GingaSurfaceID &id) {
		this->myScreen      = screenId;
		this->sur           = NULL;
		this->iFont         = NULL;
		this->parent        = NULL;
		this->chromaColor   = NULL;
		this->borderColor   = NULL;
		this->bgColor       = NULL;
		this->surfaceColor  = NULL;
		this->caps          = 1;
		this->hasExtHandler = false;
		this->isDeleting    = false;
		this->pending       = NULL;
		this->myId			= id;

		this->drawData.clear();

		Thread::mutexInit(&ddMutex);
		Thread::mutexInit(&sMutex);
		Thread::mutexInit(&pMutex);
	}

	void SDLSurface::takeOwnership() {
		sur = NULL;
	}

	SDL_Surface* SDLSurface::getPendingSurface() {
		return pending;
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
		return 1;
	}

	int SDLSurface::getCaps() {
		return this->caps;
	}

	void* SDLSurface::getSurfaceContent() {
		return sur;
	}

	void SDLSurface::setSurfaceContent(void* surface) {
		Thread::mutexLock(&sMutex);
		if (sur != NULL && surface == sur) {
			Thread::mutexUnlock(&sMutex);
			return;
		}

		if (parent != NULL) {
			if (parent->getContent() == sur && sur != NULL) {
				((SDLWindow*)parent)->setRenderedSurface((SDL_Surface*)surface);
			}
		}

		if (sur != NULL) {
			SDLDeviceScreen::createReleaseContainer(sur, NULL, NULL);
		}
		this->sur = (SDL_Surface*)surface;
		Thread::mutexUnlock(&sMutex);
	}

	bool SDLSurface::setParentWindow(void* parentWindow) {
		Thread::mutexLock(&sMutex);
		if (parent != NULL) {
			parent->setChildSurface(NULL);
		}

		this->parent = (IWindow*)parentWindow;

		if (parent != NULL) {
			if (chromaColor != NULL) {
				parent->setColorKey(
						chromaColor->getR(),
						chromaColor->getG(),
						chromaColor->getB());
			}

			parent->setChildSurface(this);
		}

		Thread::mutexUnlock(&sMutex);

		return true;
	}

	void* SDLSurface::getParentWindow() {
		return this->parent;
	}

	void SDLSurface::clearContent() {
		clearSurface();
	}

	void SDLSurface::clearSurface() {
		Thread::mutexLock(&sMutex);
		if (sur == NULL) {
			releaseDrawData();
			Thread::mutexUnlock(&sMutex);

		} else {
			Thread::mutexUnlock(&sMutex);
			fill();
		}
	}

	vector<DrawData*>* SDLSurface::createDrawDataList() {
		vector<DrawData*>* cloneDD = NULL;

		Thread::mutexLock(&ddMutex);
		if (!drawData.empty()) {
			cloneDD = new vector<DrawData*>(drawData);
		}
		Thread::mutexUnlock(&ddMutex);

		return cloneDD;
	}

	void SDLSurface::pushDrawData(int c1, int c2, int c3, int c4, short type) {
		DrawData* dd;

		if (surfaceColor != NULL) {
			Thread::mutexLock(&ddMutex);
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
			Thread::mutexUnlock(&ddMutex);
		}
	}

	void SDLSurface::drawLine(int x1, int y1, int x2, int y2) {
		clog << "SDLSurface::drawLine '";
		clog << x1 << ", " << y1 << ", " << x2 << ", " << y2 << "'";
		clog << endl; 

		pushDrawData(x1, y1, x2, y2, SDLWindow::DDT_LINE);
	}

	void SDLSurface::drawRectangle(int x, int y, int w, int h) {
		/* clog << "SDLSurface::drawRectangle '";
		clog << x << ", " << y << ", " << w << ", " << h << "'";
		clog << endl; */

		pushDrawData(x, y, w, h, SDLWindow::DDT_RECT);
	}

	void SDLSurface::fillRectangle(int x, int y, int w, int h) {
		SDL_Rect rect;
		int r, g, b;

		/* clog << "SDLSurface::fillRectangle '";
		clog << x << ", " << y << ", " << w << ", " << h << "'";
		clog << endl; */

		assert(x >= 0);
		assert(y >= 0);
		assert(w > 0);
		assert(h > 0);

		Thread::mutexLock(&sMutex);
		initContentSurface();

		if (sur != NULL && surfaceColor != NULL) {
			rect.x = x;
			rect.y = y;
			rect.w = w;
			rect.h = h;

			r = surfaceColor->getR();
			g = surfaceColor->getG();
			b = surfaceColor->getB();

			Thread::mutexLock(&pMutex);
			if (createPendingSurface()) {
				//TODO: check why we have to set BGR instead of RGB
				if (SDL_FillRect(
						pending,
						&rect,
						SDL_MapRGB(pending->format, b, g, r)) < 0) {

					clog << "SDLSurface::fillRectangle SDL error: '";
					clog << SDL_GetError() << "'" << endl;
				}
			}
			Thread::mutexUnlock(&pMutex);
		}
		Thread::mutexUnlock(&sMutex);

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

			Thread::mutexLock(&pMutex);
			if (createPendingSurface()) {
				iFont->playOver(this->getId(), txt, x, y, 0);
			}
			Thread::mutexUnlock(&pMutex);
		}
	}

	void SDLSurface::setChromaColor(int r, int g, int b, int alpha) {
		releaseChromaColor();

		this->chromaColor = new Color(r, g, b, alpha);

		Thread::mutexLock(&sMutex);
		if (sur != NULL) {
			Thread::mutexLock(&pMutex);
			if (createPendingSurface()) {
				if (SDL_SetColorKey(
						pending,
						SDL_TRUE,
						SDL_MapRGB(pending->format, r, g, b)) < 0) {

					clog << "SDLSurface::setChromaColor SDL error: '";
					clog << SDL_GetError() << "'" << endl;
				}
			}
			Thread::mutexUnlock(&pMutex);
		}

		if (parent != NULL) {
			parent->setColorKey(r, g, b);
		}

		Thread::mutexUnlock(&sMutex);
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
		fill();
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
		checkPendingSurface();
	}

	void SDLSurface::scale(double x, double y) {
		int width=0, height=0;

		Thread::mutexLock(&sMutex);
		if (sur == NULL) {
			clog << "SDLSurface::scale Warning! ";
			clog << "Can't scale surface: ";
			clog << "internal surface is NULL" << endl;
			Thread::mutexUnlock(&sMutex);
			return;
		}

		/*Scale the matrix*/
		matrix_t matrix;
		Matrix::initTranslate(&matrix, width/2, height/2);				
		Matrix::scale(&matrix, x, y);

		Matrix::setMatrix(&matrix, this);

		Thread::mutexUnlock(&sMutex);
	}

	void SDLSurface::initContentSurface() {
		if (sur == NULL && parent != NULL) {
			sur = (SDL_Surface*)(parent->getContent());
			if (sur == NULL) {
				sur = createSurface();
				((SDLWindow*)parent)->setRenderedSurface(sur);
			}
		}
	}

	SDL_Surface* SDLSurface::createSurface() {
		SDL_Surface* sdlSurface = NULL;
		int w;
		int h;

		if (parent != NULL &&
				LocalScreenManager::getInstance()->hasWindow(
						myScreen, parent->getId())) {

			w = parent->getW();
			h = parent->getH();

		} else if (sur != NULL) {
			w = sur->w;
			h = sur->h;

		} else {
			return NULL;
		}

		sdlSurface = SDLDeviceScreen::createUnderlyingSurface(w, h);
		if (sdlSurface != NULL && bgColor == NULL && caps != 0) {
			if (SDL_SetColorKey(sdlSurface, 1, *((Uint8*)sdlSurface->pixels)) < 0) {
				clog << "SDLSurface::createSurface SDL error: '";
				clog << SDL_GetError() << "'" << endl;
			}
		}

		return sdlSurface;
	}

	void SDLSurface::blit(
			int x, int y, ISurface* src,
			int srcX, int srcY, int srcW, int srcH) {

		SDL_Rect srcRect;
		SDL_Rect* srcPtr = NULL;
		SDL_Rect dstRect;
		SDL_Surface* uSur;

		Thread::mutexLock(&sMutex);
		initContentSurface();

		if (sur != NULL) {
			Thread::mutexLock(&pMutex);

			if (src != this) {
				Thread::mutexLock(&((SDLSurface*)src)->sMutex);
				Thread::mutexLock(&((SDLSurface*)src)->pMutex);
			}

			uSur = (SDL_Surface*)(src->getSurfaceContent());

			if (uSur != NULL) {
				if (srcX >= 0) {
					srcRect.x = srcX;
					srcRect.y = srcY;
					srcRect.w = srcW;
					srcRect.h = srcH;

					srcPtr = &srcRect;
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

				if (createPendingSurface()) {
					SDLDeviceScreen::lockSDL();
					if (SDL_UpperBlit(uSur, srcPtr, pending, &dstRect) < 0) {
						clog << "SDLSurface::blit SDL error: '";
						clog << SDL_GetError() << "'" << endl;
					}
					SDLDeviceScreen::unlockSDL();
				}
			}

			Thread::mutexUnlock(&pMutex);

			if (src != this) {
				Thread::mutexUnlock(&((SDLSurface*)src)->sMutex);
				Thread::mutexUnlock(&((SDLSurface*)src)->pMutex);
			}

		} else {
			clog << "SDLSurface::blit(" << this << ") Warning! ";
			clog << "Can't blit: ";
			clog << "underlying surface is NULL. Destination ISurface ";
			clog << "address would be '" << src << "'" << endl;
		}

		Thread::mutexUnlock(&sMutex);
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

		Thread::mutexLock(&sMutex);
		if (sur != NULL) {
			rect.x = x;
			rect.y = y;
			rect.w = w;
			rect.h = h;

			Thread::mutexLock(&pMutex);
			if (createPendingSurface()) {
				SDL_SetClipRect(pending, &rect);
			}
			Thread::mutexUnlock(&pMutex);

		} else {
			clog << "SDLSurface::setClip Warning! NULL underlying surface";
			clog << endl;
		}
		Thread::mutexUnlock(&sMutex);
	}

	void SDLSurface::getSize(int* w, int* h) {
		Thread::mutexLock(&sMutex);
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
		Thread::mutexUnlock(&sMutex);
	}

	string SDLSurface::getDumpFileUri() {
		string uri;
		Thread::mutexLock(&sMutex);
		if (sur == NULL) {
			clog << "DFBSurface::getDumpFileUri Warning! ";
			clog << "Can't dump surface bitmap: ";
			clog << "internal surface is NULL" << endl;

			uri = "";

		} else {
			/*
			uri = SystemCompat::getTemporaryDir() + "dump_0000";
			remove((char*)((uri + ".ppm").c_str()));
			remove((char*)((uri + ".pgm").c_str()));
			sur->Dump(sur, SystemCompat::getTemporaryDir().c_str(), "dump");
			*/
			uri = SystemCompat::getTemporaryDir() + "dump_0000.bmp";

			remove((char*)(uri.c_str()));

			//winSur->Dump(winSur, SystemCompat::getTemporaryDir().c_str(), "dump");

			SDL_SaveBMP(sur,uri.c_str());
		}
		Thread::mutexUnlock(&sMutex);
		return uri;
	}

	void SDLSurface::setMatrix(void* matrix) {
		/*if (sur != NULL) {
			sur->SetMatrix(sur, (const int*)matrix);
		}*/
	}

	GingaSurfaceID SDLSurface::getId() const
	{
		return myId;
	}

	void SDLSurface::setId(const GingaSurfaceID &surId)
	{
		myId = surId;
	}
}
}
}
}
}
}
