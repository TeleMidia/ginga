/* Copyright (C) 1989-2017 PUC-Rio/Laboratorio TeleMidia

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

#include "UnicastProvider.h"
#include "ITuner.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tuning {
	UnicastProvider::UnicastProvider(string sockAdd, int port) {
		clog << "UDP UnicastProvider address '" << sockAdd << ":";
		clog << port << "'" << endl;

		this->addr         = sockAdd;
		this->portNumber   = port;
		this->capabilities = DPC_CAN_FETCHDATA | DPC_CAN_CTLSTREAM;
	}

	UnicastProvider::~UnicastProvider() {
		if (udpSocket) delete udpSocket;
	}

	int UnicastProvider::callServer() {
		try {
			udpSocket = new UDPSocket(addr, (unsigned short) portNumber);

			return 1;
		} catch (...) {
			udpSocket = NULL;
			return 0;
		}
	}

	char* UnicastProvider::receiveData(int* len) {
		char* buff = new char[BUFFSIZE];
		*len = udpSocket->recvFrom(
				buff, BUFFSIZE, addr, (unsigned short&) portNumber);

		return buff;
	}
}
}
}
}
}
}
