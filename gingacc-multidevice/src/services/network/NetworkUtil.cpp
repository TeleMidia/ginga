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

#include "multidevice/services/network/NetworkUtil.h"
#include "multidevice/services/IDeviceDomain.h"

#include <map>
using namespace std;

map<int, char> _rcf; //received control frames;
map<int, char> _rdf; //received data frames;

char _scf = -1; //sending control frame;
char _sdf = -1; //sending data frame;

/*static char sendingControlFrameId   = -1;
static char receivedControlFrameId = -2;

static char sendingDataFrameId      = -1;
static char receivedDataFrameId    = -2;*/

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace multidevice {
	bool isValidRecvFrame(int recvFrom, char* frame) {
		map<int, char>::iterator i;
		int frameType;
		char frameId;

		frameId   = getFrameId(frame);
		frameType = getFrameType(frame);

		if (isControlFrame(frameType)) {
			i = _rcf.find(recvFrom);
			if (i != _rcf.end()) {
				if (frameId == i->second) {
					return false;
				}
			}

			_rcf[recvFrom] = frameId;
			/*if ((frameId == 0 && receivedControlFrameId > 110) ||
					(frameId > receivedControlFrameId) ||
					(receivedControlFrameId == 120)) {

			receivedControlFrameId = frameId;*/
			return true;
			//}

		} else {
			i = _rdf.find(recvFrom);
			if (i != _rdf.end()) {
				if (frameId == i->second) {
					return false;
				}
			}

			_rdf[recvFrom] = frameId;
			/*if ((frameId == 0 && receivedDataFrameId > 110) ||
					(frameId > receivedDataFrameId) ||
					(receivedDataFrameId == 120)) {

			receivedDataFrameId = frameId;*/
			return true;
			//}
		}

		return false;
	}

	bool isControlFrame(int frameType) {
		if (frameType < IDeviceDomain::FT_MEDIACONTENT) {
			return true;
		}

		return false;
	}

	char getFrameId(char* frame) {
		return frame[0];
	}

	int getFrameType(char* frame) {
		return (((unsigned char)frame[6]) & 0xFF);
	}

	unsigned int getUIntFromStream(const char* stream) {
		return ((((unsigned char)stream[0]) & 0xFF) |
				((((unsigned char)stream[1]) << 8) & 0xFF00) |
				((((unsigned char)stream[2]) << 16) & 0xFF0000) |
				((((unsigned char)stream[3]) << 24) & 0xFF000000));
	}

	char* getStreamFromUInt(unsigned int uint) {
		char* stream;

		stream = new char[4];
		stream[0] = uint & 0xFF;
		stream[1] = (uint & 0xFF00) >> 8;
		stream[2] = (uint & 0xFF0000) >> 16;
		stream[3] = (uint & 0xFF000000) >> 24;

		return stream;
	}

	string getStrIP(unsigned int someIp) {
		return itos(someIp & 0xFF) + "." +
				itos((someIp & 0xFF00) >> 8) + "." +
				itos((someIp & 0xFF0000) >> 16) + "." +
				itos((someIp & 0xFF000000) >> 24);
	}

	char* mountFrame(
			int sourceIp,
			int destDevClass,
			int mountFrameType,
			unsigned int payloadSize) {

		char* fourBytesStream;
		char* frame = NULL;

		try {
			frame = new char[HEADER_SIZE + payloadSize];

		} catch (bad_alloc &e) {
			cout << "BaseDeviceDomain::mountFrame Warning! ";
			cout << "can't alloc '" << HEADER_SIZE + payloadSize << "'";
			cout << " bytes" << endl;

			return NULL;
		}

		if (isControlFrame(mountFrameType)) {
			if (_scf >= 120) {
				_scf = -1;
			}

			_scf++;
			frame[0] = _scf;

		} else {
			if (_sdf >= 120) {
				_sdf = -1;
			}

			_sdf++;
			frame[0] = _sdf;
		}

		fourBytesStream = getStreamFromUInt(sourceIp);
		memcpy(frame + 1, fourBytesStream, 4);
		delete[] fourBytesStream;

		frame[5] = destDevClass;
		frame[6] = mountFrameType;

		fourBytesStream = getStreamFromUInt(payloadSize);
		memcpy(frame + 7, fourBytesStream, 4);
		delete[] fourBytesStream;

		return frame;
	}
}
}
}
}
}
}
