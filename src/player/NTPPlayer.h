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

#ifndef NTPPLAYER_H_
#define NTPPLAYER_H_

#include "player/Player.h"

#include "player/UDPClient.h"
#include "player/NTPDatagram.h"

#include <iostream>
#include <sys/timeb.h>

using namespace std;

#define TRY_NUM 4
#define TIMEOUT 2500 //milliseconds
#define NTP_PORT_NUM 123
#define NTP_CLIENT 3

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace player {
	class NTPPlayer : public Player {
		private:
			string host;
			int port;
			timeb utcTime;
			timeb currentTime;
			double diff;
			char text[20];

			void diffNtpLocal();

		public:
			NTPPlayer(GingaScreenID screenId, string mrl);
			~NTPPlayer();

			bool updateTime();
			time_t getTime(unsigned short* milliseconds = NULL);
			string getTimeString();
			double elapsedTime(string& oldNclTime);

			static double diffTime(timeb a, timeb b);
			static timeb* tm2timeb(struct tm* time, unsigned short milliseconds);
			static unsigned short ntpFrac2ms(alt_u32 value);
			static short localTimezone();
			static void addTime(timeb* a, double seconds);
	};
}
}
}
}
}
}

#endif /* NTPPLAYER_H_ */
