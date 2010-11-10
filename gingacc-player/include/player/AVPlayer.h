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

#ifndef AVPLAYER_H_
#define AVPLAYER_H_

#ifndef _WIN32
#ifdef __cplusplus
extern "C" {
#endif
	#include <pthread.h>
	#include <string.h>
	#include <stdio.h>
	#include <unistd.h>
#ifdef __cplusplus
}
#endif

#endif

#ifdef STx7100
#ifdef __cplusplus
extern "C" {
#endif
	#include "audio_hal.h"
	#include "video_hal.h"
#ifdef __cplusplus
}
#endif
#endif

#ifdef GEODE
#include "GeodeVideo4Linux.h"
#endif

#include "system/thread/Thread.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::thread;

#ifdef IPROVIDERLISTENER
#undef IPROVIDERLISTENER
#endif
#include "system/io/interface/content/IProviderListener.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::io;

#include "system/io/interface/content/IContinuousMediaProvider.h"
#include "system/io/interface/output/IWindow.h"
using namespace ::br::pucrio::telemidia::ginga::core::system;

#include "Player.h"
#include "PlayersComponentSupport.h"

#if HAVE_CCRTPIC
#include "RTPListener.h"
#endif

#include <vector>
#include <iostream>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace player {
	class AVPlayer :
			public Thread,
			public Player,
			public IProviderListener {

		private:
			pthread_mutex_t pMutex;
#if HAVE_CCRTPIC
			RTPListener* icListener;
#endif

		protected:
			bool running;
			bool buffered;
			bool hasVisual;
			float soundLevel;

		private:
			IContinuousMediaProvider* provider;
			bool hasResized;
			bool mainAV;
			bool hasData;
			io::IWindow* win;

		public:
			AVPlayer(string mrl, bool hasVisual);

#ifdef STx7100
		private:
			video_t *video;
			audio_t *audio;

		public:
			AVPlayer(video_t *video);
			AVPlayer(audio_t *audio);
#endif

			virtual ~AVPlayer();

			ISurface* getSurface();

		private:
			static void* createProvider(void* ptr);

		public:
			void finished();
			double getEndTime();

		private:
			void solveRemoteMrl();

		public:
			static void initializeAudio(int numArgs, char* args[]);
			static void releaseAudio();
			bool getHasVisual();

		private:
			void setSoundLevel(float level);
			ISurface* createFrame();

		public:
			void getOriginalResolution(int* height, int* width);
			double getTotalMediaTime();
			int64_t getVPts();
			void timeShift(string direction);

		private:
			double getCurrentMediaTime();

		public:
			double getMediaTime();
			void setMediaTime(double pos);
			void setStopTime(double pos);
			double getStopTime();
			void setScope(
					string scope,
					short type=TYPE_PRESENTATION,
					double begin=-1, double end=-1);

			void play();
			void pause();
			void stop();
			void resume();

			virtual void setPropertyValue(string name, string value);

			void addListener(IPlayerListener* listener);
			void release();
			void setMrl(const char* mrl);
			string getMrl();
			bool isPlaying();
			bool isRunning();

			void setAVPid(int aPid, int vPid);

			//geode:
			bool setOutWindow(int windowId);
			void setAlphaBlend(int x, int y, int w, int h);
			void checkResize();

		private:
			bool checkVideoResizeEvent();
			void run();
	};
}
}
}
}
}
}

#endif /*AVPLAYER_H_*/
