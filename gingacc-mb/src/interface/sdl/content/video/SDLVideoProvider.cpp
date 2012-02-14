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
	uint64_t SDLVideoProvider::getSync() {
		/*uint64_t _sync = SDLAudioProvider::getSync();
		if (_sync != 0) {
			return _sync;
		}*/

		if (file != NULL) {
			if (SDL_ffmpegValidVideo(file)) {
				return (SDL_GetTicks() % SDL_ffmpegDuration(file));
			}
		}

		return 0;
	}

	SDLVideoProvider::SDLVideoProvider(GingaScreenID screenId, const char* mrl)
			: SDLAudioProvider(screenId, mrl) {

		SDL_Renderer* renderer;

		videoFrame = NULL;
		myScreen   = screenId;

		if (file != NULL) {
			SDL_ffmpegSelectVideoStream(file, 0);
			getOriginalResolution(&wRes, &hRes);

			videoFrame = SDL_ffmpegCreateVideoFrame();
		    videoFrame->tempw = wRes;
		    videoFrame->temph = hRes;

			renderer = (SDL_Renderer*)(LocalScreenManager::getInstance()->
					getGfxRoot(myScreen));

			videoFrame->texture = SDL_CreateTexture(
					renderer,
					SDL_PIXELFORMAT_RGB24,
					SDL_TEXTUREACCESS_STREAMING,
					wRes,
					hRes);
		}
	}

	SDLVideoProvider::~SDLVideoProvider() {
		if (videoFrame != NULL) {
			SDL_ffmpegFreeVideoFrame(videoFrame);
			videoFrame = NULL;
		}
	}

	void SDLVideoProvider::setLoadSymbol(string symbol) {
		this->symbol = symbol;
	}

	string SDLVideoProvider::getLoadSymbol() {
		return this->symbol;
	}

	void* SDLVideoProvider::getContent() {
		return NULL;
	}

	void SDLVideoProvider::feedBuffers() {

	}

	void SDLVideoProvider::getVideoSurfaceDescription(void* dsc) {

	}

	bool SDLVideoProvider::checkVideoResizeEvent(ISurface* frame) {
		return false;
	}

	void SDLVideoProvider::getOriginalResolution(int* height, int* width) {
		if (SDL_ffmpegValidVideo(file)) {
			SDL_ffmpegGetVideoSize(file, width, height);
		}
	}

	double SDLVideoProvider::getTotalMediaTime() {
		if (SDL_ffmpegValidVideo(file)) {
			return SDL_ffmpegVideoDuration(file) / 1000;
		}
		return 0;
	}

	double SDLVideoProvider::getMediaTime() {
		if (SDL_ffmpegValidVideo(file)) {
			return SDL_ffmpegGetPosition(file) / 1000;
		}
		return 0;
	}

	void SDLVideoProvider::setMediaTime(double pos) {
		if (SDL_ffmpegValidVideo(file)) {
			SDL_ffmpegSeek(file, (uint64_t)pos);
		}
	}

	bool SDLVideoProvider::prepare(ISurface* surface) {
		IWindow* parent;
		SDLWindow* win;

		if (SDLAudioProvider::prepare(surface)) {
			if (SDL_ffmpegValidVideo(file) && videoFrame != NULL) {
				parent = (IWindow*)(surface->getParent());
				if (parent != NULL) {
					win = dynamic_cast<SDLWindow*>(parent);
					if (win != NULL) {
						win->setTexture(videoFrame->texture);
					}
				}

				cout << "SDLVideoProvider::prepare OK" << endl;
				return true;
			}
		}

		cout << "SDLVideoProvider::prepare FALSE" << endl;
		return false;
	}

	void SDLVideoProvider::playOver(
			ISurface* surface, bool hasVisual, IProviderListener* listener) {

		int i;
		SDL_TimerID rendererTimer;

		if (prepare(surface)) {
			state = ST_PLAYING;
			SDL_PauseAudio(0);
		}
	}

	void SDLVideoProvider::resume(ISurface* surface, bool hasVisual) {
		state = ST_PLAYING;
	}

	void SDLVideoProvider::pause() {
		state = ST_PAUSED;
	}

	void SDLVideoProvider::stop() {
		state = ST_STOPPED;
	}

	void SDLVideoProvider::setSoundLevel(float level) {

	}

	bool SDLVideoProvider::releaseAll() {
		//TODO: release all structures
		return false;
	}

	void SDLVideoProvider::refreshDR() {
		int i;
		int sleepTime;

		if (state == ST_PLAYING) {
			if (SDL_ffmpegValidAudio(file)) {
				SDL_LockMutex(mutex);
				for (i = 0; i < BUF_SIZE; i++) {
					if (audioFrame[i]->size == 0) {
						SDL_ffmpegGetAudioFrame(file, audioFrame[i]);
					}
				}
				SDL_UnlockMutex(mutex);
			}

			if (videoFrame != NULL) {
				if (!videoFrame->ready) {
					SDL_ffmpegGetVideoFrame(file, videoFrame);

				} else {
					if (videoFrame->pts <= getSync()) {
						videoFrame->ready = 0;

					} else {
						sleepTime = videoFrame->pts - getSync();
						::usleep(sleepTime * 1000);
					}
				}
			}
		}
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
