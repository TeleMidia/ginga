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

#ifndef _NetworkUtil_H_
#define _NetworkUtil_H_

struct frame {
	char* data;
	int size;
	bool repeat;
};

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include <string>
#include <iostream>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace multidevice {
	static const unsigned int BROADCAST_PORT = 41000;
	static const string PASSIVE_MCAST_ADDR   = "239.10.10.3";
	static const string ACTIVE_MCAST_ADDR    = "239.10.10.4";
	static const string INTERFACE_NAME_A     = "eth0";
	static const string INTERFACE_NAME_B     = "wlan0";
	static const int PASSIVE_FPS             = 15;
	static const int MCAST_TTL               = 31;
	static const int HEADER_SIZE             = 11;
	static const int MAX_FRAME_SIZE          = 400000;
	static const int NUM_OF_COPIES           = 2;
	static const double IFS                  = 1;

	bool isValidRecvFrame(char* frame);
	bool isControlFrame(int frameType);
	char getFrameId(char* frame);
	int getFrameType(char* frame);
	unsigned int getUIntFromStream(const char* stream);
	char* getStreamFromUInt(unsigned int uint);
	string getStrIP(unsigned int someIp);
	char* mountFrame(
			int sourceIp,
			int destDevClass,
			int mountFrameType,
			unsigned int payloadSize);
}
}
}
}
}
}

#endif /*_NetworkUtil_H_*/
