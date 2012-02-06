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
			GingaWindowID underlyingWindowID,
			GingaWindowID parentWindowID,
			GingaScreenID screenId,
			int x, int y, int width, int height) {

		initialize(
				underlyingWindowID,
				parentWindowID,
				screenId,
				x, y, width, height);

		if (underlyingWindowID != NULL) {
			draw();
		}
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

		LocalScreenManager::getInstance()->releaseWindow(myScreen, this);

		if (winSur != NULL) {
			/*winSur->Clear(winSur, 0, 0, 0, 0x00);
			winSur->Release(winSur);
			winSur = NULL;*/
		}

		if (win != NULL) {
			//SDLDeviceScreen::releaseUnderlyingWindow(win);
		}
		unlock();

		pthread_mutex_destroy(&mutex);
		pthread_mutex_destroy(&mutexC);

		clog << "SDLWindow::~SDLWindow(" << this << ") all done" << endl;
	}

	void SDLWindow::initialize(
			GingaWindowID underlyingWindowID,
			GingaWindowID parentWindowID,
			GingaScreenID screenId,
			int x, int y, int w, int h) {

		if (underlyingWindowID != NULL) {
			this->windowId    = (SDL_WindowID)(unsigned long)underlyingWindowID;

		} else {
			this->windowId    = 0;
		}

		if (parentWindowID != NULL) {
			this->parentId    = (SDL_WindowID)(unsigned long)parentWindowID;

		} else {
			this->parentId    = 0;
		}

		this->win             = NULL;
		this->winSur          = NULL;
		this->myScreen        = screenId;

		this->x               = x;
		this->y               = y;
		this->width           = w;
		this->height          = h;
		this->ghost           = false;
		this->visible         = false;
		this->r               = -1;
		this->g               = -1;
		this->b               = -1;
		this->alpha           = 0xFF;
		this->childSurfaces   = new vector<ISurface*>;
		this->releaseListener = NULL;
		this->fit             = true;
		this->stretch         = true;
//		this->caps            = DWCAPS_NODECORATION;
		transparencyValue     = 0x00;

		pthread_mutex_init(&mutex, NULL);
		pthread_mutex_init(&mutexC, NULL);
	}

	GingaScreenID SDLWindow::getScreen() {
		return myScreen;
	}

	void SDLWindow::revertContent() {
		lock();
		win    = NULL;
		winSur = NULL;
		unlock();
	}

	void SDLWindow::setReleaseListener(ISurface* listener) {
		this->releaseListener = listener;
	}

	int SDLWindow::getCap(string cap) {
		/*if (cap == "ALL") {
			return DWCAPS_ALL;

		} else if (cap == "NOSTRUCTURE") {
			return DWCAPS_NODECORATION;

		} else if (cap == "ALPHACHANNEL") {
			return DWCAPS_ALPHACHANNEL;

		} else if (cap == "DOUBLEBUFFER") {
			return DWCAPS_DOUBLEBUFFER;

		} else {
			return DWCAPS_NONE;
		}*/
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
		if (win != NULL) {
			clog << "SDLWindow::draw Warning! Requesting redraw" << endl;

		} else if (windowId <= 0) {
			/*SDLWindowDescription dsc;

			dsc.flags  = (SDLWindowDescriptionFlags)(
				    DWDESC_POSX |
				    DWDESC_POSY |
				    DWDESC_WIDTH |
				    DWDESC_HEIGHT);

			dsc.posx   = x;
			dsc.posy   = y;
			dsc.width  = width;
			dsc.height = height;

			if (caps > 0) {
				dsc.flags = (SDLWindowDescriptionFlags)(
						dsc.flags | DWDESC_CAPS);

				dsc.caps  = (SDLWindowCapabilities)(caps);
			}

			win = SDLDeviceScreen::createUnderlyingWindow(&dsc);

			if (win != NULL) {
				SDLCHECK(win->SetOpacity(win, 0x00));
				SDLCHECK(win->GetSurface(win, &winSur));
				SDLCHECK(win->GetID(win, &windowId));

			} else {
				clog << "SDLWindow::draw Warning! Can't create window from ";
				clog << "screen '" << myScreen << "'" << endl;
			}
*/
		} else {
			/*win = SDLDeviceScreen::getUnderlyingWindow(
					(GingaWindowID)(unsigned long)windowId);

			if (win != NULL) {
				win->GetPosition(win, &x, &y);
				win->GetSize(win, &width, &height);
				SDLCHECK(win->GetSurface(win, &winSur));
			}*/
			return;
		}

		/*if (win != NULL && (caps & DWCAPS_ALPHACHANNEL)) {
			SDLCHECK(win->SetOptions(win, (SDLWindowOptions)DWOP_ALPHACHANNEL));
		}*/

		setBackgroundColor(r, g, b, alpha);
	}

	void SDLWindow::setBounds(int posX, int posY, int w, int h) {
		this->x      = posX;
		this->y      = posY;
		this->width  = w;
		this->height = h;

		lock();
		if (win != NULL) {
			//SDLCHECK(win->SetBounds(win, x, y, width, height));
			unprotectedValidate();
		}
		unlock();
	}

	void SDLWindow::setBackgroundColor(int r, int g, int b, int alpha) {
		//clog << this << ">> SDLWindow::setBackgroundColor" << endl;
		if (win == NULL) {
			return;
		}

		if (winSur != NULL) {
			this->r = r;
			this->g = g;
			this->b = b;
			this->alpha = alpha;
/*
			if (r < 0 || g < 0 || b < 0) {
				if (caps & DWCAPS_ALPHACHANNEL) {
					SDLCHECK(win->SetOptions(win, (SDLWindowOptions)
						    (DWOP_ALPHACHANNEL)));
				}
				SDLCHECK(winSur->SetColor(winSur, 0x00, 0x00, 0x00, 0x00));
				SDLCHECK(winSur->Clear(winSur, 0x00, 0x00, 0x00, 0x00));

			} else {
				//SDLCHECK(win->SetOptions(win, (SDLWindowOptions)(DWOP_OPAQUE_REGION)));
				SDLCHECK(winSur->Clear(winSur, r, g, b, alpha));
				SDLCHECK(winSur->SetColor(winSur, r, g, b, alpha));
				SDLCHECK(winSur->FillRectangle(winSur, 0, 0, width, height));
			}

			SDLCHECK(winSur->FillRectangle(winSur, 0, 0, width, height));
			SDLCHECK(winSur->Flip(winSur, NULL, (SDLSurfaceFlipFlags)0));*/
		}
	}

	IColor* SDLWindow::getBgColor() {
		return new Color(r, g, b, alpha);
	}

	void SDLWindow::setColorKey(int r, int g, int b) {
		//clog << this << ">> SDLWindow::setColorKey" << endl;
		//lock();
		if (win == NULL) {
			return;
		}
/*
		SDLCHECK(win->SetColorKey(win, r, g, b));
		if (caps & DWCAPS_ALPHACHANNEL) {
			SDLCHECK(win->SetOptions(win, (SDLWindowOptions)
				    (DWOP_COLORKEYING | DWOP_ALPHACHANNEL)));

		} else {
			SDLCHECK(win->SetOptions(
					win, (SDLWindowOptions)(DWOP_COLORKEYING)));
		}
*/
		//unlock();
		//setBackgroundColor(r, g, b, alpha);
	}

	void SDLWindow::moveTo(int posX, int posY) {
		this->x = posX;
		this->y = posY;

		lock();
		if (win != NULL) {
			//SDLCHECK(win->MoveTo(win, x, y));
		}
		unlock();
	}

	void SDLWindow::resize(int width, int height) {
		this->width = width;
		this->height = height;

		lock();
		if (win != NULL) {
			//SDLCHECK(win->Resize(win, width, height));
		}
		unlock();
	}

	void SDLWindow::raise() {
		lock();
		if (win != NULL) {
			//win->Raise(win);
			unprotectedValidate();
		}
		unlock();
	}

	void SDLWindow::lower() {
		lock();
		if (win != NULL) {
			//win->Lower(win);
			unprotectedValidate();
		}
		unlock();
	}

	void SDLWindow::raiseToTop() {
		lock();
		if (win != NULL) {
			//win->RaiseToTop(win);
			unprotectedValidate();
		}
		unlock();
	}

	void SDLWindow::lowerToBottom() {
		lock();
		if (win != NULL) {
			//win->LowerToBottom(win);
			unprotectedValidate();
		}
		unlock();
	}

	void SDLWindow::setCurrentTransparency(int alpha) {
		if (alpha != 255) {
			this->visible = true;

		} else {
			this->visible = false;
		}

		lock();
		transparencyValue = alpha;
		if (win != NULL) {
			if (alpha == 255 || !ghost) {
				//win->SetOpacity(win, (255 - alpha));
			}
		}
		unlock();
	}

	void SDLWindow::setOpaqueRegion(int x1, int y1, int x2, int y2) {
		lock();
		if (win != NULL) {
			//win->SetOpaqueRegion(win, x1, y1, x2, y2);
		}
		unlock();
	}

	int SDLWindow::getTransparencyValue() {
		return this->transparencyValue;
	}

	GingaWindowID SDLWindow::getId() {
		GingaWindowID myId;

		if (win == NULL) {
			myId = NULL;

		} else {
			myId = (GingaWindowID)(unsigned long)windowId;
		}

		return myId;
	}

	void SDLWindow::show() {
		this->visible = true;

		if (win != NULL) {
			if (!ghost) {
				//win->SetOpacity(win, (255 - transparencyValue));
			}
		}
	}

	void SDLWindow::hide() {
		this->visible = false;

		lock();
		if (win != NULL) {
			//win->SetOpacity(win, 0x00);
		}
		unlock();
	}

	int SDLWindow::getX() {
		return this->x;
	}

	int SDLWindow::getY() {
		return this->y;
	}

	int SDLWindow::getW() {
		return this->width;
	}

	int SDLWindow::getH() {
		return this->height;
	}

	void SDLWindow::setX(int x) {
		lock();
		if (win != NULL) {
			//SDLCHECK(win->MoveTo(win, x, y));
		}
		unlock();
	}

	void SDLWindow::setY(int y) {
		lock();
		if (win != NULL) {
			//DLCHECK(win->MoveTo(win, x, y));
		}
		unlock();
	}

	void SDLWindow::setW(int w) {
		lock();
		if (win != NULL) {
			//SDLCHECK(win->Resize(win, w, height));
		}
		unlock();
	}

	void SDLWindow::setH(int h) {
		lock();
		if (win != NULL) {
			//SDLCHECK(win->Resize(win, width, h));
		}
		unlock();
	}

	void* SDLWindow::getContent() {
		return win;
	}

	void SDLWindow::setColor(int r, int g, int b, int alpha) {
		if (win != NULL && winSur != NULL) {
			//SDLCHECK(winSur->SetColor(winSur, r, g, b, alpha));
		}
	}

	void SDLWindow::setBorder(int r, int g, int b, int alpha, int bWidth) {
		int i;

		//lock();
		if (win != NULL && winSur != NULL) {
			//SDLCHECK(winSur->SetColor(winSur, r, g, b, alpha));
			if (bWidth < 0) {
				bWidth = bWidth * -1;
			}

			for (i=0; i < bWidth; i++) {
				/*SDLCHECK(winSur->DrawRectangle(
						winSur, i, i, width - (2*i), height - (2*i)));*/
			}

			//SDLCHECK(winSur->Flip(winSur, NULL, (SDLSurfaceFlipFlags)0));
		}
		//unlock();
	}

	void SDLWindow::setBorder(IColor* color, int bWidth) {
		setBorder(
			    color->getR(),
			    color->getG(),
			    color->getB(),
			    color->getAlpha(),
			    bWidth);
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

		if (win != NULL && winSur != NULL) {
			if (winSur != NULL) {
				lockChilds();
				if (childSurfaces != NULL && !childSurfaces->empty()) {
					surface = childSurfaces->at(0);
					if (surface != NULL) {
						renderFrom(surface);
					}

				} else {
					/*SDLCHECK(winSur->Flip(
							winSur, NULL, (SDLSurfaceFlipFlags)0));*/
				}
				unlockChilds();
			}
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
		if (winSur != NULL) {
			//SDLCHECK(winSur->Clear(winSur, 0, 0, 0, 0));
		}
	}

	bool SDLWindow::isMine(ISurface* surface) {
		SDL_Surface* contentSurface;

		if (win == NULL || winSur == NULL || surface == NULL) {
			return false;
		}

		contentSurface = (SDL_Surface*)(surface->getContent());
		if (contentSurface == winSur) {
			return true;
		}
		return false;
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

		if (win != NULL && !isMine(surface)) {
			contentSurface = (SDL_Surface*)(surface->getContent());
			if (contentSurface == NULL) {
				return;
			}

			renderFrom(contentSurface);
		}
	}

	void SDLWindow::renderFrom(SDL_Surface* contentSurface) {
		int w, h;
		ISurface* sur;
		/*SDL_Surface* s2;

		SDLCHECK(contentSurface->GetSize(contentSurface, &w, &h));
		if (winSur != NULL && winSur != contentSurface) {
			SDLCHECK(winSur->Clear(winSur, 0, 0, 0, 0));
			if ((w != width || h != height) && fit) {
				if (stretch) {
					SDLCHECK(winSur->StretchBlit(
							winSur, contentSurface, NULL, NULL));

				} else {
					sur = new SDLSurface(myScreen, width, height);
					s2 = (SDL_Surface*)(sur->getContent());

					SDLCHECK(s2->StretchBlit(
						    s2,
						    contentSurface,
						    NULL,
						    NULL));

					SDLCHECK(s2->Flip(
							s2, NULL, (SDLSurfaceFlipFlags)DSFLIP_BLIT));

					SDLCHECK(winSur->Blit(winSur, s2, NULL, 0, 0));
					SDLCHECK(winSur->Flip(
							winSur,
							NULL, (SDLSurfaceFlipFlags) DSFLIP_BLIT));

					delete sur;
				}

			} else {
				SDLCHECK(winSur->Blit(winSur, contentSurface, NULL, 0, 0));
			}
			SDLCHECK(winSur->Flip(winSur, NULL, (SDLSurfaceFlipFlags)0));
		}*/
	}

	void SDLWindow::blit(IWindow* src) {
		/*SDL_Window* srcWin;
		SDL_Surface* srcSur;

		lock();
		if (src != NULL) {
			src->lock();
			srcWin = (SDL_Window*)(src->getContent());
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
			srcWin = (SDL_Window*)(src->getContent());
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
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::mb::IWindow*
		createSDLWindow(
				GingaWindowID underlyingWindowID,
				GingaWindowID parentWindowID,
				GingaScreenID screenID,
				int x, int y, int w, int h) {

	return new ::br::pucrio::telemidia::ginga::core::mb::SDLWindow(
			underlyingWindowID, parentWindowID, screenID, x, y, w, h);
}

extern "C" void destroySDLWindow(
		::br::pucrio::telemidia::ginga::core::mb::IWindow* w) {

	delete w;
}
