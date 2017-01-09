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

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "dataprocessing/ncl/edit/EventDescriptor.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace dataprocessing {
namespace ncl {
	string EventDescriptor::getEventId(string event) {
		return event.substr(0, 2);
	}

	uint64_t EventDescriptor::getEventNPT(string event) {
		uint64_t nptRef = 0;
		char* strNpt;

		strNpt = (char*)(event.substr(5, 5).c_str());

		nptRef = (strNpt[0] & 0x01);
		nptRef = nptRef << 8;
		nptRef = nptRef | (strNpt[1] & 0xFF);
		nptRef = nptRef << 8;
		nptRef = nptRef | (strNpt[2] & 0xFF);
		nptRef = nptRef << 8;
		nptRef = nptRef | (strNpt[3] & 0xFF);
		nptRef = nptRef << 8;
		nptRef = nptRef | (strNpt[4] & 0xFF);

		return nptRef;
	}

	string EventDescriptor::getCommandTag(string event) {
		string cmdTag = "0x" + itos(event[11] & 0xFF);

		return cmdTag;
	}

	int EventDescriptor::getSequenceNumber(string event) {
		char* strSeq;

		strSeq = (char*)(event.substr(12, 1).c_str());
		return strSeq[0] & 0xFE;
	}

	bool EventDescriptor::getFinalFlag(string event) {
		char* strFF;

		strFF = (char*)(event.substr(12, 1).c_str());
		return strFF[0] & 0x01;
	}

	string EventDescriptor::getPrivateDataPayload(string event) {
		unsigned int privateDataLength;

		privateDataLength = event[10] & 0xFF;
		if (privateDataLength + 11 != event.length()) {
			clog << "EventDescriptor::getPrivateDataPayload Warning! ";
			clog << "invalid private data length(" << privateDataLength;
			clog << ") for event length(" << event.length() << ")";
			clog << endl;
		}
		return event.substr(13, privateDataLength - 3);
	}

	bool EventDescriptor::checkFCS(string event) {
		//TODO: check FCS
		return true;
	}

	string EventDescriptor::extractMarks(string eventParam) {
		string noMarks = trim(eventParam);

		if (eventParam.find("\"") != std::string::npos) {
			noMarks = eventParam.substr(
					eventParam.find_first_of("\"") + 1,
					eventParam.length() - (eventParam.find_first_of("\"") + 1));

			noMarks = noMarks.substr(0, noMarks.find_last_of("\""));
		}

		return noMarks;
	}
}
}
}
}
}
}
}
