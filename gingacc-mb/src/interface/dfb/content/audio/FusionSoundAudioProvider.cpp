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
#include "mb/interface/dfb/content/audio/FusionSoundAudioProvider.h"

#if DFBTM_PATCH
	static int fspRefs = 0;
#endif

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {

	IFusionSound* FusionSoundAudioProvider::_fsSound = NULL;
	bool FusionSoundAudioProvider::_fsInitialized    = false;

	FusionSoundAudioProvider::FusionSoundAudioProvider(
			GingaScreenID screenId, const char* mrl) {

		FSStreamDescription s_desc;

#if DFBTM_PATCH
		fspRefs++;
#endif

		myScreen = screenId;

		if (_fsSound == NULL) {
			initialize();
		}

		Thread::mutexInit(&decMutex);

		_fsSound->CreateMusicProvider(_fsSound, mrl, &(decoder));
		decoder->GetStreamDescription(decoder, &s_desc);

		s_desc.flags         = (FSStreamDescriptionFlags)(
				s_desc.flags | FSSDF_SAMPLEFORMAT | FSSDF_CHANNELS);

		s_desc.sampleformat  = FSSF_S16;
		s_desc.channels      = 2;

		_fsSound->CreateStream(_fsSound, &s_desc, &stream);
		stream->GetPlayback(stream, &playback);
	}

	FusionSoundAudioProvider::~FusionSoundAudioProvider() {
		Thread::mutexLock(&decMutex);
		if (decoder != NULL) {
			decoder->Stop(decoder);
			decoder->Release(decoder);
			decoder = NULL;
		}

		if (playback != NULL) {
			playback->Stop(playback);
			playback->Release(playback);
			playback = NULL;
		}

		if (stream != NULL) {
			stream->Flush(stream);
			stream->Release(stream);
			stream = NULL;
		}

		/*if (sound != NULL) {
			sound->Release(sound);
			sound = NULL;
		}*/

		Thread::mutexUnlock(&decMutex);
		pthread_mutex_destroy(&decMutex);

#if DFBTM_PATCH
		fspRefs--;

		if (fspRefs == 0) {
			DirectReleaseInterface("IFusionSoundMusicProvider");
		}
#endif //DFBTM_PATCH
	}

	void FusionSoundAudioProvider::initialize(int numArgs, char* args[]) {
		if (_fsSound == NULL) {
			if (!_fsInitialized) {
				_fsInitialized = true;
				//pool = new set<IFusionSoundStream*>;
				FusionSoundInit(&numArgs, &args);
			}
			FusionSoundCreate(&_fsSound);
		}
	}

	void FusionSoundAudioProvider::setLoadSymbol(string symbol) {
		this->symbol = symbol;
	}

	string FusionSoundAudioProvider::getLoadSymbol() {
		return this->symbol;
	}

	void* FusionSoundAudioProvider::getProviderContent() {
		return decoder;
	}

	double FusionSoundAudioProvider::getTotalMediaTime() {
		double totalTime;

		Thread::mutexLock(&decMutex);
		if (decoder == NULL) {
			Thread::mutexUnlock(&decMutex);
			return 0;
		}

		decoder->GetLength(decoder, &totalTime);
		Thread::mutexUnlock(&decMutex);
		return totalTime;
	}

	double FusionSoundAudioProvider::getMediaTime() {
		double currentTime;

		Thread::mutexLock(&decMutex);
		if (decoder == NULL) {
			Thread::mutexUnlock(&decMutex);
			return -1;
		}

		decoder->GetPos(decoder, &currentTime);
		Thread::mutexUnlock(&decMutex);
		return currentTime;
	}

	void FusionSoundAudioProvider::setMediaTime(double pos) {
		Thread::mutexLock(&decMutex);
		if (decoder == NULL) {
			Thread::mutexUnlock(&decMutex);
			return;
		}

		decoder->SeekTo(decoder, pos);
		Thread::mutexUnlock(&decMutex);
	}

	void FusionSoundAudioProvider::playOver(
			ISurface* surface, bool hasVisual, IProviderListener* listener) {

		Thread::mutexLock(&decMutex);
		if (decoder == NULL) {
			clog << "FusionSoundAudioProvider::playOver decoder = NULL" << endl;
			Thread::mutexUnlock(&decMutex);
			return;
		}

		decoder->PlayToStream(decoder, stream);
		Thread::mutexUnlock(&decMutex);
	}

	void FusionSoundAudioProvider::pause() {
		stop();
	}

	void FusionSoundAudioProvider::resume(ISurface* surface, bool hasVisual) {
		Thread::mutexLock(&decMutex);
		if (decoder == NULL) {
			Thread::mutexUnlock(&decMutex);
			return;
		}

		decoder->PlayToStream(decoder, stream);
		Thread::mutexUnlock(&decMutex);
	}

	void FusionSoundAudioProvider::stop() {
		Thread::mutexLock(&decMutex);
		if (decoder == NULL) {
			Thread::mutexUnlock(&decMutex);
			return;
		}

		clog << "FusionSoundAudioProvider::stop(" << this << ")" << endl;
		decoder->Stop(decoder);
		Thread::mutexUnlock(&decMutex);
	}

	void FusionSoundAudioProvider::setSoundLevel(float level) {
		Thread::mutexLock(&decMutex);
		if (playback == NULL) {
			Thread::mutexUnlock(&decMutex);
			return;
		}

		playback->SetVolume(playback, level);
		Thread::mutexUnlock(&decMutex);
	}

	void FusionSoundAudioProvider::getOriginalResolution(
			int* height, int* width) {

		*height = 0;
		*width = 0;
	}

	bool FusionSoundAudioProvider::releaseAll() {
		//TODO: release all structures

		clog << "FusionSoundAudioProvider::releaseAll" << endl;
		/*if (sound != NULL) {
			sound->Release(sound);
			sound = NULL;
		}*/
		return false;
	}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::mb::IContinuousMediaProvider*
	createFSAudioProvider(GingaScreenID screenId, const char* mrl) {

	return (new ::br::pucrio::telemidia::ginga::core::mb::
			FusionSoundAudioProvider(screenId, mrl));
}

extern "C" void destroyFSAudioProvider(
		::br::pucrio::telemidia::ginga::core::mb::
		IContinuousMediaProvider* cmp) {

	delete cmp;
}
