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

	pthread_mutex_t SDLAudioProvider::iMutex;
	set<SDLAudioProvider*> SDLAudioProvider::aps;
	bool SDLAudioProvider::init   = false;

	SDLAudioProvider::SDLAudioProvider(
			GingaScreenID screenId, const char* mrl) {

		myScreen   = screenId;
		state      = ST_STOPPED;
		sync       = 0;
		this->mrl  = "";
		decoder    = new SDL2ffmpeg(mrl);
		mutex      = SDL_CreateMutex();
		soundLevel = SDL_MIX_MAXVOLUME * 0.5;

		this->mrl.assign(mrl);

		if (!init) {
			init = true;
			pthread_mutex_init(&iMutex, NULL);
			aps.clear();
		}

		if (decoder != NULL) {
			decoder->selectAudioStream(0);

			desired = decoder->getAudioSpec(
					512, SDLAudioProvider::audioCallback);

		} else {
			cout << "SDLAudioProvider::SDLAudioProvider Warning! Invalid ";
			cout << "file '" << mrl << "'" << endl;
		}
	}

	SDLAudioProvider::~SDLAudioProvider() {
		int i;
		set<SDLAudioProvider*>::iterator j;

		state = ST_STOPPED;

		pthread_mutex_lock(&iMutex);
		j = aps.find(this);
		if (j != aps.end()) {
			aps.erase(j);
			if (aps.empty()) {
				SDL_PauseAudio(1);
				init = false;
			}
		}
		pthread_mutex_unlock(&iMutex);

		if (!init) {
			pthread_mutex_destroy(&iMutex);
		}

		SDL_LockMutex(mutex);
	    if (decoder->isAudioValid()) {
	        for (i = 0; i < BUF_SIZE; i++) {
	        	decoder->releaseAudioFrame(audioFrame[i]);
	        }
	    }

		if (decoder != NULL) {
			delete decoder;
			decoder = NULL;
		}

		SDL_UnlockMutex(mutex);

		SDL_DestroyMutex(mutex);
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
		if (decoder->isAudioValid()) {
			return decoder->getDuration() / 1000;
		}

		return 0;
	}

	double SDLAudioProvider::getMediaTime() {
		if (decoder->isAudioValid()) {
			return (double)decoder->getPosition() / 1000;
		}

		return 0;
	}

	void SDLAudioProvider::setMediaTime(double pos) {
		if (decoder->isAudioValid()) {
			decoder->seek((uint64_t)pos);
		}
	}

	bool SDLAudioProvider::prepare(ISurface* surface) {
		int i;
		int frameSize;

	    if (decoder->isAudioValid()) {
	    	memset(&obtained, 0, sizeof(obtained));

	    	clog << "SDLAudioProvider::prepare desired channels = '";
	    	clog << (short)desired.channels << "', desired samples = '";
	    	clog << desired.samples << "', format = '";
	    	clog << desired.format << "', size = '";
	    	clog << desired.size << "', and freq = '";
	    	clog << desired.freq << "'";
	    	clog << endl;

	    	SDL_OpenAudio(&desired, &obtained);

	    	if (obtained.channels == 0) {
	    		memcpy(&obtained, &desired, sizeof(desired));
	    	}

	    	frameSize = obtained.channels * obtained.samples * 2;

	    	clog << "SDLAudioProvider::prepare '" << mrl;
	    	clog << "' obtained channels = '";
	    	clog << (short)obtained.channels << "', samples = '";
	    	clog << obtained.samples << "', format = '";
	    	clog << obtained.format << "', size = '";
	    	clog << obtained.size << "', and freq = '";
	    	clog << obtained.freq << "'";
	    	clog << endl;

	        for (i = 0; i < BUF_SIZE; i++) {
	            audioFrame[i] = decoder->createAudioFrame(frameSize);
	            if (!audioFrame[i]) {
	            	clog << "SDLAudioProvider::prepare Can't prepare!";
	            	clog << " Audio Frame was not created with frame size '";
	            	clog << frameSize << "'" << endl;
	                return false;
	            }

	            decoder->getAudioFrame(audioFrame[i]);
	        }

			pthread_mutex_lock(&iMutex);
			aps.insert(this);
			pthread_mutex_unlock(&iMutex);

	        return true;

	    } else {
        	clog << "SDLAudioProvider::prepare Can't play audio: ";
            clog << "there is no audio stream considering MRL '";
            clog << mrl << "'";
            clog << endl;
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
		if (level >= 1.0) {
			soundLevel = SDL_MIX_MAXVOLUME;

		} else {
			soundLevel = level * SDL_MIX_MAXVOLUME;
		}
	}

	bool SDLAudioProvider::releaseAll() {
		return false;
	}

	void SDLAudioProvider::getOriginalResolution(int* width, int* height) {

	}

	void SDLAudioProvider::clamp(short* buf, int len) {
		int i;
		long value;

		for (i = 0; i < len; i++) {
			value = (long)buf[i];
			if (value > 0x7fff) value = 0x7fff;
			else if (value < -0x7fff) value = -0x7fff;
			buf[i] = (short)value;
		}
	}

	void SDLAudioProvider::audioCallback(void* data, Uint8* stream, int len) {
		set<SDLAudioProvider*>::iterator i;
		SDLAudioProvider* ap;
		AudioFrame* auxFrame;
		int j, k, ret;

		SDL_AudioCVT acvt;
		SDL_AudioSpec destSpec;
		uint32_t capacity = 0;
		bool cvt = false;

		int16_t* dest;
		int16_t* src;

		pthread_mutex_lock(&iMutex);
		if (aps.empty()) {
			pthread_mutex_unlock(&iMutex);
			return;
		}

		i = aps.begin();
		while (i != aps.end()) {
			ap = *i;
			SDL_LockMutex(ap->mutex);

			if (ap->state == ST_PLAYING) {
				if (capacity == 0) {
					capacity = ap->audioFrame[0]->capacity;

				} else if (capacity < ap->audioFrame[0]->capacity) {
					cvt = true;
					capacity = ap->audioFrame[0]->capacity;
					memcpy(&destSpec, &ap->obtained, sizeof(ap->obtained));

				} else if (capacity > ap->audioFrame[0]->capacity) {
					cvt = true;
				}
			}

			SDL_UnlockMutex(ap->mutex);
			++i;
		}

		/*if (cvt) {
			SDL_OpenAudio(&destSpec, NULL);

        	cout << "SDLAudioProvider::audioCallback open audio with ";
			cout << "format '" << destSpec.format;
			cout << "' channels '";
			cout << (short)destSpec.channels;
			cout << "' freq '" << destSpec.freq;
            cout << "'" << endl;
		}*/

		memset(stream, 0, len);

		i = aps.begin();
		while (i != aps.end()) {
			ret = 1;
			ap  = *i;

			SDL_LockMutex(ap->mutex);

			if (ap->state == ST_PLAYING &&
					ap->audioFrame[0]->size == ap->audioFrame[0]->capacity) {

				if (cvt) {

					/*
					 * FIXME: mix works only when all sources have the
					 *        same number of channels.
					 */
					if (capacity != ap->audioFrame[0]->capacity) {
						ret = SDL_BuildAudioCVT(
								&acvt,
								ap->obtained.format,
								ap->obtained.channels,
								ap->obtained.freq,
								destSpec.format,
								destSpec.channels,
								destSpec.freq);

						if (ret != -1) {
							acvt.len = ap->audioFrame[0]->size;
							acvt.buf = (Uint8*)malloc(acvt.len * acvt.len_mult);

							if (acvt.buf != NULL) {
								memset(acvt.buf, 0, acvt.len * acvt.len_mult);

								memcpy(
										acvt.buf,
										ap->audioFrame[0]->buffer,
										ap->audioFrame[0]->size);

								SDL_ConvertAudio(&acvt);

								/*cout << endl;
								cout << "Converting(stream len = '" << len;
								cout << "' and Audio instance = '";
								cout << ap << "')" << endl;
								cout << "FROM: ";
								cout << "format '" << ap->obtained.format;
								cout << "' channels '";
								cout << (short)ap->obtained.channels;
								cout << "' freq '" << ap->obtained.freq;
								cout << "' size '" << ap->audioFrame[0]->size;
								cout << "'";
								cout << "' and capacity '";
								cout << ap->audioFrame[0]->capacity;
								cout << "'";
								cout << endl;
								cout << "TO: ";
								cout << "format '" << destSpec.format << "' ";
								cout << "channels '";
								cout << (short)destSpec.channels;
								cout << "' freq '" << destSpec.freq << "' ";
								cout << "size '";
								cout << acvt.len_cvt;
								cout << "' and capacity '";
								cout << capacity;
								cout << "'";
								cout << endl;*/

								clamp((short*)acvt.buf, acvt.len_cvt);

								SDL_MixAudioFormat(
										stream,
										acvt.buf,
										acvt.dst_format,
										acvt.len_cvt,
										ap->soundLevel);

								free(acvt.buf);
								acvt.buf = NULL;

								ret = 0;

							} else {
								ret = -2;
							}
						}
					}
				}

				if (ret == 1) {
					SDL_MixAudioFormat(
							stream,
							ap->audioFrame[0]->buffer,
							ap->obtained.format,
							ap->audioFrame[0]->size,
							ap->soundLevel);
				}

				ap->sync = ap->audioFrame[0]->pts;

				ap->audioFrame[0]->size = 0;
				auxFrame = ap->audioFrame[0];
				for (j = 1; j < BUF_SIZE; j++) {
					ap->audioFrame[j - 1] = ap->audioFrame[j];
				}

				ap->audioFrame[BUF_SIZE - 1] = auxFrame;

			} else if (ap->state == ST_PLAYING) {
				ret = 0; //not the right time to decode
			}

			if (ret < 0) {
				clog << "SDLAudioProvider::audioCallback: can't present ";
				clog << "audio sample! Exception = '" << ret << "'";
				clog << endl;
			}

			SDL_UnlockMutex(ap->mutex);

			++i;
		}

		pthread_mutex_unlock(&iMutex);
	}

	uint64_t SDLAudioProvider::getSync() {
		if (decoder != NULL) {
			if (decoder->isAudioValid()) {
				return sync;
			}

			if (decoder->isVideoValid()) {
				return (SDL_GetTicks() % decoder->getDuration());
			}
		}
		return 0;
	}

	void SDLAudioProvider::refreshDR() {
		int i;

		if (state == ST_PLAYING && decoder->isAudioValid()) {
			SDL_LockMutex(mutex);

			for (i = 0; i < BUF_SIZE; i++) {
				if (!audioFrame[i]->size) {
					decoder->getAudioFrame(audioFrame[i]);
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
