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
#ifndef SRTPLAYER_H_
#define SRTPLAYER_H_

#include "system/Thread.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::thread;

#include "TextPlayer.h"

GINGA_PLAYER_BEGIN

	class SrtPlayer : public TextPlayer, public Thread {
		protected:
			map<float, float>* textEvents;
			vector<string>* textLines;
			IPlayer* player; //time reference player
			bool isPlaying;
			bool controlParentVisibility;
			bool running;

		public:
			SrtPlayer(GingaScreenID screenId, string mrl);
			virtual ~SrtPlayer();
			void loadFile(string url);

		protected:
			void loadSrt();
			float strTimeToFloat(string time);
			void printSrt();

		public:
			void setParentVisibilityControl(bool control);
			void setReferenceTimePlayer(IPlayer* player);
			double getMediaTotalTime();
			double getMediaTime();

			bool play();
			void stop();
			void pause();
			void resume();

			virtual void setPropertyValue(string name, string value);

			void busy();
			bool isPlayingSrt();

		protected:
			void run();
	};

GINGA_PLAYER_END
#endif /*SRTPLAYER_H_*/
