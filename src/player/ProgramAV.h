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

#ifndef PROGRAMAV_H_
#define PROGRAMAV_H_

#include "Player.h"
#include "IProgramAV.h"

#include <iostream>
#include <map>
using namespace std;

GINGA_PLAYER_BEGIN

	class ProgramAV : public Player {
		private:
			static ProgramAV* _instance;
			map<int, IPlayer*> players;
			map<int, string> playerBounds;
			map<string, int> namePids;
			IPlayer* currentPlayer;
			int currentPid;
			string fullScreenBounds;

			ProgramAV(GingaScreenID screenId);
			virtual ~ProgramAV();

		public:
			static ProgramAV* getInstance(GingaScreenID screenId);
			void release();
			int64_t getVPts();
			GingaSurfaceID getSurface();

		private:
			void addPidName(string name, int pid);
			int getPidByName(string name);
			void forcePids(string pValue);
			void setAVPid(string name, int aPid, int vPid);
			string getNameFromMrl(string mrl);
			void showPlayer(string mrl);
			void hidePlayer(string mrl);
			void createPlayer(string mrl);
			void setPlayer(int pid, IPlayer*);
			IPlayer* getPlayer(string mrl);
			IPlayer* getPlayer(int pid);

			string getPropertyValue(string pName);
			void setPropertyValue(string pName, string pValue);
	};

GINGA_PLAYER_END
#endif /*PROGRAMAV_H_*/
