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
#include "mb/interface/sdl/content/video/SDLVideoProvider.h"
#include "mb/interface/sdl/SDLDeviceScreen.h"
#include "mb/interface/sdl/output/SDLSurface.h"
#include "mb/interface/sdl/output/SDLWindow.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	SDLVideoProvider::SDLVideoProvider(GingaScreenID screenId, const char* mrl)
			: SDLAudioProvider(screenId, mrl) {

		myScreen = screenId;
		win      = NULL;
		hasTex   = false;

		if (decoder != NULL) {
			getOriginalResolution(&wRes, &hRes);
		}
	}

	SDLVideoProvider::~SDLVideoProvider() {
		hasTex = false;
	}

	void SDLVideoProvider::setLoadSymbol(string symbol) {
		this->symbol = symbol;
	}

	string SDLVideoProvider::getLoadSymbol() {
		return this->symbol;
	}

	bool SDLVideoProvider::getHasVisual() {
		assert(decoder != NULL);

		return decoder->hasVideoStream();
	}

	void* SDLVideoProvider::getProviderContent() {
		assert(decoder != NULL);

		return (void*)(decoder->getTexture());
	}

	void SDLVideoProvider::setProviderContent(void* texture) {
		assert(decoder != NULL);
		assert(texture != NULL);

		decoder->setTexture((SDL_Texture*)texture);
		textureCreated();
	}

	void SDLVideoProvider::feedBuffers() {

	}

	void SDLVideoProvider::getVideoSurfaceDescription(void* dsc) {

	}

	bool SDLVideoProvider::checkVideoResizeEvent(ISurface* frame) {
		return false;
	}

	void SDLVideoProvider::getOriginalResolution(int* width, int* height) {
		assert(decoder != NULL);
		assert(width != NULL);
		assert(height != NULL);

		decoder->getOriginalResolution(width, height);
	}

	double SDLVideoProvider::getTotalMediaTime() {
		return SDLAudioProvider::getTotalMediaTime();
	}

	double SDLVideoProvider::getMediaTime() {
		return SDLAudioProvider::getMediaTime();
	}

	void SDLVideoProvider::setMediaTime(double pos) {
		SDLAudioProvider::setMediaTime(pos);
	}

	void SDLVideoProvider::playOver(
			ISurface* surface, bool hasVisual, IProviderListener* listener) {

		IWindow* parent;

		SDLDeviceScreen::addCMPToRendererList(this);
		parent = (IWindow*)(surface->getParentWindow());

		clog << "SDLVideoProvider::playOver parent(" << parent << ")" << endl;
		if (LocalScreenManager::getInstance()->hasWindow(myScreen, parent)) {
			win = (SDLWindow*)parent;

		} else {
			clog << "SDLVideoProvider::playOver parent(" << parent << ") ";
			clog << "Warning! hasWindow(parent) has returned false" << endl;
		}
	}

	void SDLVideoProvider::resume(ISurface* surface, bool hasVisual) {
		SDLAudioProvider::resume(surface, hasVisual);
	}

	void SDLVideoProvider::pause() {
		SDLAudioProvider::pause();
	}

	void SDLVideoProvider::stop() {
		SDLAudioProvider::stop();
	}

	void SDLVideoProvider::setSoundLevel(float level) {
		SDLAudioProvider::setSoundLevel(level);
	}

	bool SDLVideoProvider::releaseAll() {
		//TODO: release all structures
		return false;
	}

	void SDLVideoProvider::refreshDR(void* data) {
		SDLAudioProvider::refreshDR(data);
	}

	bool SDLVideoProvider::textureCreated() {
		assert(decoder != NULL);

		if (!hasTex) {
			while (win == NULL) {
				SystemCompat::uSleep(10000);
			}

			((SDLWindow*)win)->setTexture(decoder->getTexture());
			hasTex = true;
			decoder->play();
			return true;
		}

		return false;
	}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::mb::IContinuousMediaProvider*
		createSDLVideoProvider(GingaScreenID screenId, const char* mrl) {

	return (new ::br::pucrio::telemidia::ginga::core::mb::SDLVideoProvider(
			screenId, mrl));
}

extern "C" void destroySDLVideoProvider(
		::br::pucrio::telemidia::ginga::core::mb::
		IContinuousMediaProvider* cmp) {

	delete cmp;
}
