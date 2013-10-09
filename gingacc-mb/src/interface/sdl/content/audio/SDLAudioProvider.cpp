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
#include "mb/interface/sdl/content/audio/SDLAudioProvider.h"
#include "mb/interface/sdl/SDLDeviceScreen.h"
#include "mb/interface/sdl/output/SDLSurface.h"

extern "C" {
#include <string.h>
}

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	SDLAudioProvider::SDLAudioProvider(
			GingaScreenID screenId, const char* mrl) {

		myScreen   = screenId;
		this->mrl  = "";
		decoder    = new SDL2ffmpeg(this, mrl);

		this->mrl.assign(mrl);
	}

	SDLAudioProvider::~SDLAudioProvider() {
		if (decoder != NULL) {
			decoder->stop();

			delete decoder;
			decoder = NULL;
		}
	}

	void SDLAudioProvider::setLoadSymbol(string symbol) {
		this->symbol = symbol;
	}

	string SDLAudioProvider::getLoadSymbol() {
		return this->symbol;
	}

	void* SDLAudioProvider::getProviderContent() {
		return NULL;
	}

	double SDLAudioProvider::getTotalMediaTime() {
		if (decoder != NULL) {
			return decoder->getDuration() / 1000;
		}

		return 0;
	}

	int64_t SDLAudioProvider::getVPts() {
		int64_t vpts = 0;

		if (decoder != NULL) {
			vpts = (uint64_t)(decoder->getPosition() * 90000);
		}

		return vpts;
	}

	double SDLAudioProvider::getMediaTime() {
		if (decoder != NULL) {
			return (double)decoder->getPosition();
		}

		return 0;
	}

	void SDLAudioProvider::setMediaTime(double pos) {
		if (decoder != NULL) {
			decoder->seek(((int64_t)pos) * 1000000);
		}
	}

	void SDLAudioProvider::playOver(
			ISurface* surface, bool hasVisual, IProviderListener* listener) {

		clog << "SDLAudioProvider::playOver" << endl;
		SDLDeviceScreen::addCMPToRendererList(this);
		if (decoder != NULL) {
			decoder->play();
		}
	}

	void SDLAudioProvider::pause() {
		if (decoder != NULL) {
			decoder->pause();
		}
	}

	void SDLAudioProvider::resume(ISurface* surface, bool hasVisual) {
		if (decoder != NULL) {
			decoder->resume();
		}
	}

	void SDLAudioProvider::stop() {
		SDLDeviceScreen::removeCMPToRendererList(this);
		if (decoder != NULL) {
			decoder->stop();
		}
	}

	void SDLAudioProvider::setSoundLevel(float level) {
		if (decoder != NULL) {
			decoder->setSoundLevel(level);
		}
	}

	bool SDLAudioProvider::releaseAll() {
		return false;
	}

	void SDLAudioProvider::getOriginalResolution(int* width, int* height) {

	}

	void SDLAudioProvider::refreshDR(void* data) {
/*		SDL_Event* event = (SDL_Event*)data;

		switch (event->type) {
			case FF_REFRESH_EVENT:
				/*cout << "SDLAudioProvider::refreshDR call video refresh";
				cout << endl;
				SDL2ffmpeg::video_refresh(decoder);
				break;

	        case FF_ALLOC_EVENT:
				/*cout << "SDLAudioProvider::refreshDR call alloc_picture";
				cout << endl;
	        	decoder->alloc_picture();
	            break;
		}*/

		SDL2ffmpeg::video_refresh(decoder);
	}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::mb::
		IContinuousMediaProvider* createSDLAudioProvider(
				GingaScreenID screenId, const char* mrl) {

	return (new ::br::pucrio::telemidia::ginga::core::mb::
			SDLAudioProvider(screenId, mrl));
}

extern "C" void destroySDLAudioProvider(
		::br::pucrio::telemidia::ginga::core::mb::
		IContinuousMediaProvider* cmp) {

	delete cmp;
}
