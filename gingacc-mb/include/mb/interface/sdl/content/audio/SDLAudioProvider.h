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

#ifndef SDLAUDIOPROVIDER_H_
#define SDLAUDIOPROVIDER_H_

#include "mb/interface/IContinuousMediaProvider.h"

/* SDL_ffmpeg cplusplus compat begin */
extern "C" {
#include <pthread.h>
#include <stdint.h>
}

#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#endif //INT64_C

#ifndef UINT64_C
#define UINT64_C(c) (c ## ULL)
#endif //UINT64_C

/* SDL_ffmpeg cplusplus compat end*/

#include <cmath>

#include "SDL.h"
#include "SDL_thread.h"
#include "SDL_ffmpeg.h"

#include <set>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	class SDLAudioProvider : public IContinuousMediaProvider {
		protected:
			static const short ST_PLAYING = 0;
			static const short ST_PAUSED  = 1;
			static const short ST_STOPPED = 2;
			static const short BUF_SIZE   = 10;

			GingaScreenID myScreen;
			string symbol;
			string mrl;
			short state;
			short soundLevel;

			SDL_mutex* mutex;
			SDL_ffmpegFile* file;

			SDL_AudioCVT* acvt;
			SDL_AudioSpec desired;
			SDL_AudioSpec obtained;

			SDL_ffmpegAudioFrame* audioFrame[BUF_SIZE];
			uint64_t sync;

			static pthread_mutex_t iMutex;
			static set<SDLAudioProvider*> aps;

			static bool init;

		public:
			SDLAudioProvider(GingaScreenID screenId, const char* mrl);
			virtual ~SDLAudioProvider();

			virtual void setLoadSymbol(string symbol);
			virtual string getLoadSymbol();

			virtual bool getHasVisual(){return false;};

			virtual void setAVPid(int aPid, int vPid){};
			virtual void feedBuffers(){};
			void* getProviderContent();
			virtual void setProviderContent(void* content){};
			virtual bool checkVideoResizeEvent(ISurface* frame){return false;};
			double getTotalMediaTime();
			virtual int64_t getVPts(){return 0;};
			double getMediaTime();
			void setMediaTime(double pos);

		protected:
			virtual bool prepare(ISurface* surface);

		public:
			void playOver(
					ISurface* surface,
					bool hasVisual=true, IProviderListener* listener=NULL);

			void resume(ISurface* surface, bool hasVisual=false);
			void pause();
			void stop();
			void setSoundLevel(float level);
			bool releaseAll();
			void getOriginalResolution(int* width, int* height);

		private:
			static void clamp(short* outbuf, int len);

		protected:
			static void audioCallback(void* data, Uint8* stream, int len);
			virtual uint64_t getSync();

		public:
			virtual void refreshDR();
	};
}
}
}
}
}
}

#endif /*SDLAUDIOPROVIDER_H_*/
