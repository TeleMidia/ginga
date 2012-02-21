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

		myScreen = screenId;
		state    = ST_STOPPED;
		sync     = 0;
		mutex    = SDL_CreateMutex();
		file     = SDL_ffmpegOpen(mrl);

		if (file != NULL) {
			file->private_data = this;
			SDL_ffmpegSelectAudioStream(file, 0);

			specs = SDL_ffmpegGetAudioSpec(
					file,
					512,
					SDLAudioProvider::audioCallback);
		}
	}

	SDLAudioProvider::~SDLAudioProvider() {
		int i;

	    if (SDL_ffmpegValidAudio(file)) {
	        for (i = 0; i < BUF_SIZE; i++) {
	            SDL_ffmpegFreeAudioFrame(audioFrame[i]);
	        }
	    }

		if (file != NULL) {
			SDL_ffmpegFree(file);
			file = NULL;
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
		if (SDL_ffmpegValidAudio(file)) {
			return SDL_ffmpegAudioDuration(file) / 1000;
		}

		return 0;
	}

	double SDLAudioProvider::getMediaTime() {
		if (SDL_ffmpegValidAudio(file)) {
			return SDL_ffmpegGetPosition(file) / 1000;
		}

		return 0;
	}

	void SDLAudioProvider::setMediaTime(double pos) {
		if (SDL_ffmpegValidAudio(file)) {
			SDL_ffmpegSeek(file, (uint64_t)pos);
		}
	}

	bool SDLAudioProvider::prepare(ISurface* surface) {
		int i;
		int frameSize;

	    if (SDL_ffmpegValidAudio(file)) {
	    	SDL_OpenAudio(&specs, 0);

	        frameSize = specs.channels * specs.samples * 2;

	        for (i = 0; i < BUF_SIZE; i++) {
	            audioFrame[i] = SDL_ffmpegCreateAudioFrame(file, frameSize);
	            if (!audioFrame[i]) {
	                return false;
	            }

	            SDL_ffmpegGetAudioFrame(file, audioFrame[i]);
	        }

	        return true;

	    } else {
        	clog << "SDLAudioProvider::prepare Can't play since there is no ";
            clog << "selected audio stream" << endl;
	    }

	    return false;
	}

	void SDLAudioProvider::playOver(
			ISurface* surface, bool hasVisual, IProviderListener* listener) {

	    if (prepare(surface)) {
	    	state = ST_PLAYING;
	    	SDLDeviceScreen::addCMPToRendererList(this);
	    	SDL_PauseAudio(0);
	    }
	}

	void SDLAudioProvider::pause() {
		state = ST_PAUSED;
	}

	void SDLAudioProvider::resume(ISurface* surface, bool hasVisual) {
		state = ST_PLAYING;
	}

	void SDLAudioProvider::stop() {
		SDLDeviceScreen::removeCMPToRendererList(this);
		state = ST_STOPPED;
	}

	void SDLAudioProvider::setSoundLevel(float level) {

	}

	bool SDLAudioProvider::releaseAll() {
		return false;
	}

	void SDLAudioProvider::getOriginalResolution(int* width, int* height) {

	}

	void SDLAudioProvider::audioCallback(void* data, Uint8* stream, int len) {
		int i;
		SDL_ffmpegAudioFrame* auxFrame;
		SDL_ffmpegFile* _file = (SDL_ffmpegFile*)data;

		SDLAudioProvider* ap = (SDLAudioProvider*)_file->private_data;

		if (ap == NULL) {
			return;
		}

		SDL_LockMutex(ap->mutex);

	    if (ap->audioFrame[0]->size == len) {
	    	ap->sync = ap->audioFrame[0]->pts;

	        memcpy(
	        		(void*)stream,
	        		(const void*)(ap->audioFrame[0]->buffer),
	        		(size_t)(ap->audioFrame[0]->size));

	        ap->audioFrame[0]->size = 0;
	        auxFrame = ap->audioFrame[0];
	        for (i = 1; i < BUF_SIZE; i++) {
	        	ap->audioFrame[i - 1] = ap->audioFrame[i];
	        }

	        ap->audioFrame[BUF_SIZE - 1] = auxFrame;

	    } else {
	        memset((void*)stream, (int)0, (size_t)len);
	    }

	    SDL_UnlockMutex(ap->mutex);
	}

	uint64_t SDLAudioProvider::getSync() {
		if (file != NULL) {
			if (SDL_ffmpegValidAudio(file)) {
				return sync;
			}

			if (SDL_ffmpegValidVideo(file)) {
				return (SDL_GetTicks() % SDL_ffmpegDuration(file));
			}
		}
		return 0;
	}

	void SDLAudioProvider::refreshDR() {
		int i;

		if (state == ST_PLAYING && SDL_ffmpegValidAudio(file)) {
			SDL_LockMutex(mutex);

			for (i = 0; i < BUF_SIZE; i++) {
				if (!audioFrame[i]->size) {
					SDL_ffmpegGetAudioFrame(file, audioFrame[i]);
				}
			}

			SDL_UnlockMutex(mutex);
		}
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
