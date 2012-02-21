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

#include "mb/LocalScreenManager.h"
#include "mb/interface/sdl/output/SDLWindow.h"
#include "mb/interface/sdl/output/SDLSurface.h"
#include "mb/interface/sdl/SDLDeviceScreen.h"

extern "C" {
#include <stdlib.h>
}

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	SDLWindow::SDLWindow(
			GingaWindowID windowID,
			GingaWindowID parentWindowID,
			GingaScreenID screenId,
			int x, int y, int width, int height) {

		initialize(
				windowID,
				parentWindowID,
				screenId,
				x, y, width, height);
	}

	SDLWindow::~SDLWindow() {
		ISurface* surface;
		vector<ISurface*>::iterator i;

		lock();
		lockChilds();
		if (releaseListener != NULL) {
			releaseListener->setParent(NULL);
		}

		if (childSurfaces != NULL) {
			i = childSurfaces->begin();
			while (i != childSurfaces->end()) {
				surface = *i;
				if (surface != NULL) {
					surface->setParent(NULL);
				}
				++i;
			}
			delete childSurfaces;
			childSurfaces = NULL;
		}
		unlockChilds();

		releaseBGColor();
		releaseBorderColor();
		releaseWinColor();
		releaseColorKey();

		LocalScreenManager::getInstance()->releaseWindow(myScreen, this);

//		if (winSur != NULL) {
//			SDLDeviceScreen::createReleaseContainer(winSur, NULL, NULL);
//		}

		unlock();

		pthread_mutex_destroy(&mutex);
		pthread_mutex_destroy(&mutexC);

		this->isWaiting = false;
	    pthread_mutex_destroy(&cMutex);
	    pthread_cond_destroy(&cond);

	    pthread_mutex_destroy(&rMutex);

		clog << "SDLWindow::~SDLWindow(" << this << ") all done" << endl;
	}

	void SDLWindow::initialize(
			GingaWindowID windowID,
			GingaWindowID parentWindowID,
			GingaScreenID screenId,
			int x, int y, int w, int h) {

		this->windowId = windowID;

		if (parentWindowID != NULL) {
			this->parentId = parentWindowID;

		} else {
			this->parentId = 0;
		}

		this->texture           = NULL;
		this->winSur            = NULL;
		this->curSur            = NULL;
		this->myScreen          = screenId;

		this->rect.x            = x;
		this->rect.y            = y;
		this->rect.w            = w;
		this->rect.h            = h;
		this->ghost             = false;
		this->visible           = false;
		this->childSurfaces     = new vector<ISurface*>;
		this->releaseListener   = NULL;
		this->fit               = true;
		this->stretch           = true;
		this->caps              = 0;
		this->transparencyValue = 0x00;
		this->borderWidth       = 0;
		this->bgColor           = NULL;
		this->borderColor       = NULL;
		this->winColor          = NULL;
		this->colorKey          = NULL;

		pthread_mutex_init(&mutex, NULL);
		pthread_mutex_init(&mutexC, NULL);

		this->isWaiting = false;
	    pthread_mutex_init(&cMutex, NULL);
	    pthread_cond_init(&cond, NULL);

	    pthread_mutex_init(&rMutex, NULL);
	}

	void SDLWindow::releaseBGColor() {
		if (bgColor != NULL) {
			delete bgColor;
			bgColor = NULL;
		}
	}

	void SDLWindow::releaseBorderColor() {
		if (borderColor != NULL) {
			delete borderColor;
			borderColor = NULL;
		}
	}

	void SDLWindow::releaseWinColor() {
		if (winColor != NULL) {
			delete winColor;
			winColor = NULL;
		}
	}

	void SDLWindow::releaseColorKey() {
		if (colorKey != NULL) {
			delete colorKey;
			colorKey = NULL;
		}
	}

	GingaScreenID SDLWindow::getScreen() {
		return myScreen;
	}

	void SDLWindow::revertContent() {
		lock();
		winSur = NULL;
		unlock();
	}

	void SDLWindow::setReleaseListener(ISurface* listener) {
		this->releaseListener = listener;
	}

	int SDLWindow::getCap(string cap) {
		return 0;
	}

	void SDLWindow::setCaps(int caps) {
		this->caps = caps;
	}

	void SDLWindow::addCaps(int capability) {
		this->caps = (this->caps | capability);
	}

	int SDLWindow::getCaps() {
		return caps;
	}

	void SDLWindow::draw() {

	}

	void SDLWindow::setBounds(int posX, int posY, int w, int h) {
		this->rect.x = posX;
		this->rect.y = posY;
		this->rect.w = w;
		this->rect.h = h;
	}

	void SDLWindow::setBackgroundColor(int r, int g, int b, int alpha) {
		releaseBGColor();

		if (r < 0 || g < 0 || b < 0) {
			return;
		}

		bgColor = new Color(r, g, b, alpha);
	}

	void SDLWindow::setColor(int r, int g, int b, int alpha) {
		releaseWinColor();

		if (r < 0 || g < 0 || b < 0) {
			return;
		}

		winColor = new Color(r, g, b, alpha);
	}

	void SDLWindow::setColorKey(int r, int g, int b) {
		releaseColorKey();

		if (r < 0 || g < 0 || b < 0) {
			return;
		}

		colorKey = new Color(r, g, b);
	}

	void SDLWindow::setBorder(int r, int g, int b, int alpha, int bWidth) {
		int i;

		releaseBorderColor();

		borderWidth = bWidth;

		if (r < 0 || g < 0 || b < 0) {
			return;
		}

		borderColor = new Color(r, g, b, alpha);
	}

	void SDLWindow::setBorder(IColor* color, int bWidth) {
		setBorder(
			    color->getR(),
			    color->getG(),
			    color->getB(),
			    color->getAlpha(),
			    bWidth);
	}

	void SDLWindow::moveTo(int posX, int posY) {
		this->rect.x = posX;
		this->rect.y = posY;
	}

	void SDLWindow::resize(int width, int height) {
		this->rect.w = width;
		this->rect.h = height;
	}

	void SDLWindow::raiseToTop() {
		SDLDeviceScreen::updateWindowState(
				myScreen, this, SDLDeviceScreen::SUW_RAISETOTOP);
	}

	void SDLWindow::lowerToBottom() {
		SDLDeviceScreen::updateWindowState(
				myScreen, this, SDLDeviceScreen::SUW_LOWERTOBOTTOM);
	}

	void SDLWindow::setCurrentTransparency(int alpha) {
		if (alpha != 255) {
			this->visible = true;

		} else {
			this->visible = false;
		}

		transparencyValue = alpha;
	}

	int SDLWindow::getTransparencyValue() {
		return this->transparencyValue;
	}

	GingaWindowID SDLWindow::getId() {
		GingaWindowID myId;

		myId = windowId;
		return myId;
	}

	void SDLWindow::show() {
		this->visible = true;
		SDLDeviceScreen::updateWindowState(
				myScreen, this, SDLDeviceScreen::SUW_SHOW);
	}

	void SDLWindow::hide() {
		visible = false;
		SDLDeviceScreen::updateWindowState(
				myScreen, this, SDLDeviceScreen::SUW_HIDE);
	}

	int SDLWindow::getX() {
		return this->rect.x;
	}

	int SDLWindow::getY() {
		return this->rect.y;
	}

	int SDLWindow::getW() {
		return this->rect.w;
	}

	int SDLWindow::getH() {
		return this->rect.h;
	}

	void SDLWindow::setX(int x) {
		this->rect.x = x;
	}

	void SDLWindow::setY(int y) {
		this->rect.y = y;
	}

	void SDLWindow::setW(int w) {
		this->rect.w = w;
	}

	void SDLWindow::setH(int h) {
		this->rect.h = h;
	}

	void SDLWindow::setGhostWindow(bool ghost) {
		this->ghost = ghost;
	}

	bool SDLWindow::isVisible() {
		return this->visible;
	}

	void SDLWindow::validate() {
		lock();
		unprotectedValidate();
		unlock();
	}

	void SDLWindow::unprotectedValidate() {
		ISurface* surface;

/*		lockChilds();
		if (childSurfaces != NULL && !childSurfaces->empty()) {
			surface = childSurfaces->at(0);
			if (surface != NULL && surface->getSurfaceContent() != NULL) {
				winSur = (SDL_Surface*)(surface->getSurfaceContent());
			}

		} else {
			cout << "SDLWindow::unprotectedValidate empty child surface '";
			cout << childSurfaces << "'" << endl;
		}
		unlockChilds();
*/
		if (winSur != NULL) {
			curSur = winSur;
		}
	}

	void SDLWindow::addChildSurface(ISurface* s) {
		unsigned int i;
		ISurface* surface;

		lockChilds();
		for (i = 0; i < childSurfaces->size(); i++) {
			surface = childSurfaces->at(i);
			if (surface == s) {
				unlockChilds();
				return;
			}
		}
		childSurfaces->push_back(s);
		unlockChilds();
	}

	bool SDLWindow::removeChildSurface(ISurface* s) {
		unsigned int i;
		vector<ISurface*>::iterator j;
		ISurface* surface;

		lockChilds();
		if (releaseListener == s) {
			releaseListener = NULL;
		}

		if (childSurfaces == NULL) {
			unlockChilds();
			return false;
		}

		for (i = 0; i < childSurfaces->size(); i++) {
			surface = childSurfaces->at(i);
			if (surface == s) {
				j = childSurfaces->begin() + i;
				childSurfaces->erase(j);
				unlockChilds();
				return true;
			}
		}
		unlockChilds();
		return false;
	}

	void SDLWindow::setStretch(bool stretchTo) {
		this->stretch = stretchTo;
	}

	bool SDLWindow::getStretch() {
		return this->stretch;
	}

	void SDLWindow::setFit(bool fitTo) {
		this->fit = fitTo;
	}

	bool SDLWindow::getFit() {
		return this->fit;
	}

	void SDLWindow::clearContent() {
		winSur = NULL;
	}

	void SDLWindow::setRenderedSurface(SDL_Surface* uSur) {
		curSur = uSur;
	}

	void* SDLWindow::getContent() {
		return curSur;
	}

	void SDLWindow::setTexture(SDL_Texture* texture) {
		this->texture = texture;
	}

	SDL_Texture* SDLWindow::getTexture() {
		return texture;
	}

	bool SDLWindow::isMine(ISurface* surface) {
		SDL_Surface* contentSurface;
		bool itIs = false;

		if (surface != NULL && surface->getSurfaceContent() != NULL) {
			contentSurface = (SDL_Surface*)(surface->getSurfaceContent());
			if (contentSurface == winSur) {
				itIs = true;
			}
		}

		return itIs;
	}

	void SDLWindow::renderFrom(string serializedImageUrl) {
		/*IDirectFB* sdl;
		IDirectFBImageProvider* ip;
		SDLImageDescription imgDsc;
		SDL_Surface* destination = NULL;
		SDLSurfaceDescription surDsc;
		IColor* chromaKey = NULL;

		sdl = (IDirectFB*)(LocalScreenManager::getInstance()->getGfxRoot(
				myScreen));

		SDLCHECK(sdl->CreateImageProvider(
				sdl, serializedImageUrl.c_str(), &ip));

		if ((ip->GetImageDescription(ip, &imgDsc) == SDL_OK) &&
			 (ip->GetSurfaceDescription(ip, &surDsc) == SDL_OK)) {

			destination = SDLDeviceScreen::createUnderlyingSurface(&surDsc);

			if (imgDsc.caps & DICAPS_ALPHACHANNEL) {
				SDLCHECK(destination->SetBlittingFlags(destination,
					 (SDLSurfaceBlittingFlags)(DSBLIT_BLEND_ALPHACHANNEL)));
			}

			if (imgDsc.caps & DICAPS_COLORKEY) {
				SDLCHECK(destination->SetSrcColorKey(
						destination,
					    imgDsc.colorkey_r,
					    imgDsc.colorkey_g,
					    imgDsc.colorkey_b));

				SDLCHECK(destination->SetBlittingFlags(destination,
					    (SDLSurfaceBlittingFlags)(
					    DSBLIT_BLEND_ALPHACHANNEL |
					    DSBLIT_SRC_COLORKEY)));

			} else if (imgDsc.caps & DICAPS_NONE) {
				SDLCHECK(destination->SetBlittingFlags(destination,
					    (SDLSurfaceBlittingFlags)DSBLIT_NOFX));
			}
		}

		if (destination != NULL) {
			SDLCHECK(ip->RenderTo(ip, (SDL_Surface*)(destination), NULL));
			renderFrom(destination);
			SDLDeviceScreen::releaseUnderlyingSurface(destination);
		}*/
	}

	void SDLWindow::renderFrom(ISurface* surface) {
		SDL_Surface* contentSurface;

		pthread_mutex_lock(&rMutex);
		if (!isMine(surface)) {
			contentSurface = (SDL_Surface*)(surface->getSurfaceContent());
			if (contentSurface == NULL) {
				clog << "SDLWindow::renderFrom Warning! NULL underlying ";
				clog << "surface!" << endl;
				return;
			}

			renderFrom(contentSurface);
		}
		pthread_mutex_unlock(&rMutex);
	}

	void SDLWindow::renderFrom(SDL_Surface* contentSurface) {
		if (winSur == NULL) {
			winSur = contentSurface;
		}

		curSur = contentSurface;
		//waitRenderer();
	}

	void SDLWindow::blit(IWindow* src) {
		/*SDL_Window* srcWin;
		SDL_Surface* srcSur;

		lock();
		if (src != NULL) {
			src->lock();
			srcWin = (SDL_Window*)(src->getSurfaceContent());
			srcWin->GetSurface(srcWin, &srcSur);

			if (winSur != NULL) {
				SDLCHECK(winSur->SetBlittingFlags(
						winSur,
						(SDLSurfaceBlittingFlags)DSBLIT_BLEND_ALPHACHANNEL));

				SDLCHECK(winSur->Blit(
						winSur, srcSur, NULL, src->getX(), src->getY()));
			}
			src->unlock();
		}
		unlock();*/
	}

	void SDLWindow::stretchBlit(IWindow* src) {
		//SDLRectangle rect, *r = NULL;
		SDL_Window* srcWin;
		SDL_Surface* srcSur;

		lock();
		/*if (src != NULL) {
			src->lock();
			srcWin = (SDL_Window*)(src->getSurfaceContent());
			srcWin->GetSurface(srcWin, &srcSur);

			if (winSur != NULL) {
				SDLCHECK(winSur->SetBlittingFlags(
						winSur,
						(SDLSurfaceBlittingFlags)DSBLIT_BLEND_ALPHACHANNEL));

				SDLCHECK(winSur->StretchBlit(winSur, srcSur, NULL, NULL));
			}
			src->unlock();
		}*/
		unlock();
	}

	string SDLWindow::getDumpFileUri(int quality, int dumpW, int dumpH) {
		string uri, strCmd;

		lock();
		if (winSur == NULL) {
			uri = "";

		} else {
			uri = "/tmp/dump_0000";
			remove((char*)((uri + ".ppm").c_str()));
			remove((char*)((uri + ".jpg").c_str()));
			remove((char*)((uri + ".pgm").c_str()));
			//winSur->Dump(winSur, "/tmp", "dump");

			if (fileExists(uri + ".ppm")) {
				strCmd = ("convert -quality " + itos(quality) + " " +
						"-resize " + itos(dumpW) + "x" + itos(dumpH) + " " +
						//"-colors 32 " +
						"-depth 4 " +
						"-interlace None " +
						"-compress BZip " +
						"-colorspace RGB " +
						//"-mask " + uri + ".pgm" + " " +
						uri + ".ppm " + uri + ".jpg" + " 2> /dev/null");

				if (::system(strCmd.c_str()) < 0) {
					::usleep(10000);
					remove((char*)((uri + ".jpg").c_str()));
					if (::system(strCmd.c_str()) < 0) {
						clog << "SDLWindow::getDumpFileUri Warning!!! ";
						clog << " Can't create JPEG file" << endl;
						unlock();
						return uri + ".ppm";
					}
				}

				unlock();
				return uri + ".jpg";
			}
		}

		unlock();
		return "";
	}

	void SDLWindow::lock() {
		pthread_mutex_lock(&mutex);
	}

	void SDLWindow::unlock() {
		pthread_mutex_unlock(&mutex);
	}

	void SDLWindow::lockChilds() {
		pthread_mutex_lock(&mutexC);
	}

	void SDLWindow::unlockChilds() {
		pthread_mutex_unlock(&mutexC);
	}

	bool SDLWindow::rendered() {
		if (isWaiting) {
			pthread_cond_signal(&cond);
			return true;
		}
		return false;
	}

	void SDLWindow::waitRenderer() {
		isWaiting = true;
		pthread_mutex_lock(&cMutex);
		pthread_cond_wait(&cond, &cMutex);
		isWaiting = false;
		pthread_mutex_unlock(&cMutex);
	}
}
}
}
}
}
}
