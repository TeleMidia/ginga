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

			void diffNtpLocal();

		public:
			NTPPlayer(GingaScreenID screenId, string mrl);
			~NTPPlayer();

			bool updateTime();
			time_t getTime(unsigned short* milliseconds = NULL);
			void printCurrentDateTime();

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
