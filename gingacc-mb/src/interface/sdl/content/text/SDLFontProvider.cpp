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
#include "mb/interface/sdl/SDLDeviceScreen.h"
#include "mb/interface/sdl/content/text/SDLFontProvider.h"
#include "mb/interface/sdl/output/SDLWindow.h"
#include "mb/interface/sdl/output/SDLSurface.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	const short SDLFontProvider::A_LEFT          = 0;
	const short SDLFontProvider::A_CENTER        = 1;
	const short SDLFontProvider::A_RIGHT         = 2;

	const short SDLFontProvider::A_TOP           = 3;
	const short SDLFontProvider::A_TOP_CENTER    = 4;
	const short SDLFontProvider::A_TOP_LEFT      = 5;
	const short SDLFontProvider::A_TOP_RIGHT     = 6;

	const short SDLFontProvider::A_BOTTOM        = 7;
	const short SDLFontProvider::A_BOTTOM_CENTER = 8;
	const short SDLFontProvider::A_BOTTOM_LEFT   = 9;
	const short SDLFontProvider::A_BOTTOM_RIGHT  = 10;

	pthread_mutex_t SDLFontProvider::ntsMutex;
	bool SDLFontProvider::initialized = false;
	short SDLFontProvider::fontRefs   = 0;

	SDLFontProvider::SDLFontProvider(
			GingaScreenID screenId, const char* fontUri, int heightInPixel) {

		Thread::mutexInit(&pMutex);

		fontRefs++;

		this->fontInit  = false;
		this->fontUri   = "";
		this->dfltFont  = SystemCompat::appendGingaFilesPrefix("font/vera.ttf");
		this->myScreen  = screenId;
		this->height    = heightInPixel;
		this->font      = NULL;
		this->content   = NULL;
		this->plainText = "";
		this->coordX    = 0;
		this->coordY    = 0;
		this->align     = A_TOP_LEFT;
		this->fontUri.assign(fontUri);
	}

	SDLFontProvider::~SDLFontProvider() {
		Thread::mutexLock(&ntsMutex);
		Thread::mutexLock(&pMutex);

		fontRefs--;

		content   = NULL;
		plainText = "";
		fontUri   = "";
		dfltFont  = "";

		if (font != NULL) {
			TTF_CloseFont(font);
			font = NULL;
		}

		if (fontRefs == 0) {
			// FIXME: Find a better way to do this!
			// initialized = false;
			// TTF_Quit();
		}

		Thread::mutexUnlock(&pMutex);
		pthread_mutex_destroy(&pMutex);
		Thread::mutexUnlock(&ntsMutex);
	}

	bool SDLFontProvider::initializeFont() {
		if (!initialized) {
			Thread::mutexInit(&ntsMutex);
			initialized = true;
			Thread::mutexLock(&ntsMutex);
			if (TTF_Init() < 0) {
				clog << "SDLFontProvider::initializeFont Warning! ";
				clog << "Couldn't initialize TTF: " << SDL_GetError();
				clog << endl;
				Thread::mutexUnlock(&ntsMutex);
				return false;
			}
			Thread::mutexUnlock(&ntsMutex);
		}

		if (height < 1) {
			height = 12;
		}

		Thread::mutexLock(&ntsMutex);
		createFont();
		Thread::mutexUnlock(&ntsMutex);

		return true;
	}

	bool SDLFontProvider::createFont() {
		if (fileExists(fontUri)) {
			font = TTF_OpenFont(fontUri.c_str(), height);
			if (font == NULL) {
				clog << "SDLFontProvider::initializeFont Warning! ";
				clog << "Couldn't initialize font: " << fontUri;
				clog << endl;
				return false;
			}

			TTF_SetFontStyle(font, (int)TTF_STYLE_BOLD);
			TTF_SetFontOutline(font, 0);
			TTF_SetFontKerning(font, 1);
			TTF_SetFontHinting(font, (int)TTF_HINTING_NORMAL);

			return true;
		}

		return false;
	}

	void* SDLFontProvider::getFontProviderContent() {
		return (void*)font;
	}

	void SDLFontProvider::getStringExtents(const char* text, int* w, int* h) {
		if (font == NULL) {
			initializeFont();
		}

		if (font != NULL) {
			Thread::mutexLock(&ntsMutex);

			TTF_SizeText(font, text, w, h);

			Thread::mutexUnlock(&ntsMutex);

		} else {
			clog << "SDLFontProvider::getStringExtents Warning! ";
			clog << "Can't get text size: font is NULL." << endl;
		}
	}

	int SDLFontProvider::getStringWidth(const char* text, int textLength) {
		int w = -1, h = -1;
		string aux = "";

		if (textLength == 0 || textLength > strlen(text)) {
			getStringExtents(text, &w, &h);

		} else {
			aux.assign(text);
			aux = aux.substr(0, textLength);
			getStringExtents(aux.c_str(), &w, &h);
		}
		return w;
	}

	int SDLFontProvider::getHeight() {
		return height;
	}

	void SDLFontProvider::playOver(
			ISurface* surface, const char* text, int x, int y, short align) {

		Thread::mutexLock(&pMutex);

		if (font == NULL) {
			initializeFont();
		}

		plainText.assign(text, strlen(text));
		coordX      = x;
		coordY      = y;
		this->align = align;

		playOver(surface);

		Thread::mutexUnlock(&pMutex);
	}

	void SDLFontProvider::playOver(ISurface* surface) {
		SDLWindow* parent;
		IColor* fontColor = NULL;

		SDL_Color sdlColor;
		SDL_Rect rect;
		SDL_Surface* renderedSurface = NULL;
		SDL_Surface* text;

		Thread::mutexLock(&ntsMutex);
		this->content = surface;
		if (plainText == "") {
			clog << "SDLFontProvider::playOver Warning! Empty text.";
			clog << endl;
			Thread::mutexUnlock(&ntsMutex);
			return;
		}

		if (font == NULL) {
			if (!createFont()) {
				clog << "SDLFontProvider::playOver Warning! NULL font.";
				clog << endl;

				Thread::mutexUnlock(&ntsMutex);
				return;
			}
		}

		if (LocalScreenManager::getInstance()->hasSurface(myScreen, content)) {
			parent = (SDLWindow*)(content->getParentWindow());

			if (parent == NULL) {
				clog << "SDLFontProvider::playOver Warning! NULL parent.";
				clog << endl;
				Thread::mutexUnlock(&ntsMutex);
				return;
			}

			if (coordX >= parent->getW() || coordY >= parent->getH()) {
				clog << "SDLFontProvider::playOver Warning! Invalid coords.";
				clog << endl;
				Thread::mutexUnlock(&ntsMutex);
				return;
			}

			fontColor = content->getColor();

			if (fontColor != NULL) {
				sdlColor.r = fontColor->getR();
				sdlColor.g = fontColor->getG();
				sdlColor.b = fontColor->getB();

			} else {
				sdlColor.r = 0x00;
				sdlColor.g = 0x00;
				sdlColor.b = 0x00;
			}

			assert(font != NULL);
			text = TTF_RenderUTF8_Solid(
					font, plainText.c_str(), sdlColor);

			if (text == NULL) {
				clog << "SDLFontProvider::playOver Warning! Can't create ";
				clog << "underlying surface from text" << endl;
				Thread::mutexUnlock(&ntsMutex);
				return;
			}

			SDLDeviceScreen::addUnderlyingSurface(text);

			rect.x = coordX;
			rect.y = coordY;
			rect.w = text->w;
			rect.h = text->h;

			renderedSurface = ((SDLSurface*)content)->getPendingSurface();

			if (renderedSurface == NULL) {
				renderedSurface = (SDL_Surface*)(parent->getContent());
			}

			if (renderedSurface == NULL) {
				renderedSurface = SDLDeviceScreen::createUnderlyingSurface(
						parent->getW(),
						parent->getH());

				SDL_SetColorKey(
						renderedSurface,
						1,
						*((Uint8*)renderedSurface->pixels));

				content->setSurfaceContent((void*)renderedSurface);
				parent->setRenderedSurface(renderedSurface);

				clog << "SDLFontProvider::playOver parent = '" << parent;
				clog << "' bounds = '" << parent->getX() << ",";
				clog << parent->getY() << ",";
				clog << parent->getW() << ",";
				clog << parent->getH() << "' text rectangle = '";
				clog << rect.x << ", " << rect.y << ", ";
				clog << rect.w << ", " << rect.h << "'";
				clog << endl;
			}

			if (SDL_UpperBlit(text, NULL, renderedSurface, &rect) < 0) {
				clog << "SDLFontProvider::playOver Warning! Can't blit ";
				clog << "text considering rectangle = '";
				clog << rect.x << ", " << rect.y << ", ";
				clog << rect.w << ", " << rect.h << "': ";
				clog << SDL_GetError();
				clog << endl;
			}

			SDLDeviceScreen::createReleaseContainer(text, NULL, NULL);
			text = NULL;

		} else {
			clog << "SDLFontProvider::playOver Warning! Invalid Surface.";
			clog << endl;
		}

		Thread::mutexUnlock(&ntsMutex);
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
