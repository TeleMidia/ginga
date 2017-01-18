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

#ifndef AVPLAYER_H_
#define AVPLAYER_H_

#include "system/SystemCompat.h"
using namespace ::ginga::system;

#include "system/Thread.h"
using namespace ::ginga::system;

#ifdef IPROVIDERLISTENER
#undef IPROVIDERLISTENER
#endif

#include "mb/IContinuousMediaProvider.h"
#include "mb/SDLWindow.h"
#include "mb/IProviderListener.h"
using namespace ::ginga::mb;

#include "Player.h"

GINGA_PLAYER_BEGIN

	class AVPlayer :
			public Thread,
			public Player,
			public IProviderListener {

		private:
			pthread_mutex_t pMutex; //provider mutex
			pthread_mutex_t tMutex; //AV player thread mutex

		protected:
			bool running;
			bool buffered;
			float soundLevel;

		private:
			string pSym;
			GingaProviderID provider;
			bool hasResized;
			bool mainAV;
			bool isRemote;
			bool hasData;
			GingaWindowID win;

		public:
			AVPlayer(GingaScreenID screenId, string mrl);
			virtual ~AVPlayer();
			GingaSurfaceID getSurface();

		private:
			void createProvider();
			static GingaProviderID createProviderT(void* ptr);

		public:
			void finished();
			double getEndTime();

		private:
			void solveRemoteMrl();

		public:
			static void initializeAudio(int numArgs, char* args[]);
			static void releaseAudio();

		private:
			void setSoundLevel(float level);
			GingaSurfaceID createFrame();

		public:
			void getOriginalResolution(int* width, int* height);
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
					double begin=-1, double end=-1, double outTransDur=-1);

			bool play();
			void pause();
			void stop();
			void resume();

			virtual string getPropertyValue(string name);
			virtual void setPropertyValue(string name, string value);

			void addListener(IPlayerListener* listener);
			void release();
			void setMrl(const char* mrl);
			string getMrl();
			bool isPlaying();
			bool isRunning();

			void setAVPid(int aPid, int vPid);

			bool setOutWindow(GingaWindowID windowId);

		private:
			bool checkVideoResizeEvent();
			void run();
	};

GINGA_PLAYER_END

#endif /*AVPLAYER_H_*/
