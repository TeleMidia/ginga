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

#ifndef BerkeliumPlayer_h_
#define BerkeliumPlayer_h_

#include "Player.h"
#include "BerkeliumHandler.h"



GINGA_PLAYER_BEGIN

	class BBrowserFactory {
		private:
			bool running;

			/* Browser Pending Tasks*/
			static const short BPT_INIT          = 0;
			static const short BPT_UPDATE        = 1;
			static const short BPT_STOP          = 2;
			static const short BPT_RELEASE       = 3;
			static const short BPT_NONE          = 4;

			map<BerkeliumHandler*, short> bMap;
			pthread_mutex_t smutex;

		public:
			BBrowserFactory();
			virtual ~BBrowserFactory();

			void start();
			void stop();
			bool isRunning();
			bool hasBrowser();
			bool hasRunningBrowser();
			void createBrowser(BerkeliumHandler* bInfo);
			void stopBrowser(BerkeliumHandler* bInfo);
			void destroyBrowser(BerkeliumHandler* bInfo);

		private:
			void initBrowser(BerkeliumHandler* bInfo);
			void updateMap();

			void lockSet();
			void unlockSet();
	};

	class BerkeliumPlayer : public Player {
		private:
			static BBrowserFactory berkeliumFactory;
			static bool mainLoopDone;
			BerkeliumHandler* bInfo;

		public:
			BerkeliumPlayer(GingaScreenID myScreen, string mrl);
			virtual ~BerkeliumPlayer();

			GingaSurfaceID getSurface();
			void setNotifyContentUpdate(bool notify);

			bool play();
			void stop();
			bool setOutWindow(GingaWindowID windowId);
			void setBounds(int x, int y, int w, int h);

			void setPropertyValue(string name, string value);

			bool setKeyHandler(bool isHandler);

		private:
			static void* mainLoop(void* ptr);
	};

GINGA_PLAYER_END

#endif /*BerkeliumPlayer_h_*/
