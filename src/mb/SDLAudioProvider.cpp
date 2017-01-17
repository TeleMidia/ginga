/* Copyright (C) 2006-2017 PUC-Rio/Laboratorio TeleMidia

This file is part of Ginga (Ginga-NCL).

Ginga is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

Ginga is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
License for more details.

You should have received a copy of the GNU General Public License
along with Ginga.  If not, see <http://www.gnu.org/licenses/>.  */

#include "config.h"
#include "LocalScreenManager.h"
#include "SDLAudioProvider.h"
#include "SDLDeviceScreen.h"
#include "SDLSurface.h"

extern "C" {
#include <string.h>
}

GINGA_MB_BEGIN

	SDLAudioProvider::SDLAudioProvider(
			GingaScreenID screenId, const char* mrl){

		type = AudioProvider;

		myScreen   = screenId;
		this->mrl  = "";
		decoder    = new SDL2ffmpeg(mrl);

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

	void SDLAudioProvider::playOver(GingaSurfaceID surface) {

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

	void SDLAudioProvider::resume(GingaSurfaceID surface) {
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

	float SDLAudioProvider::getSoundLevel() {
		float soundLevel = 0.0;

		if (decoder != NULL) {
			soundLevel = decoder->getSoundLevel();
		}

		return soundLevel;
	}

	bool SDLAudioProvider::releaseAll() {
		return false;
	}

	void SDLAudioProvider::getOriginalResolution(int* width, int* height) {

	}

	void SDLAudioProvider::refreshDR(void* data) {
		double rt;

		if (decoder != NULL) {
			SDL2ffmpeg::video_refresh(decoder, &rt);
		}
	}

GINGA_MB_END
