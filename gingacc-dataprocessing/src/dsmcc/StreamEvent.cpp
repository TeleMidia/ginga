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

#include "../../include/dsmcc/StreamEvent.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace dataprocessing {
	StreamEvent::StreamEvent(
			void* descriptorData, unsigned int descriptorSize) {

		memset(data, 0, sizeof(data));
		memcpy((void*)&(data[0]), descriptorData, descriptorSize);

		this->descriptorTag = (data[0] & 0xFF);
		this->descriptorLength = (data[1] & 0xFF);
		this->eventId = ((data[2] & 0xFF) << 8) | (data[3] & 0xFF);

		//4,5,6,7* reserved
		this->timeReference = ((data[7] & 0x01) << 32) |
				((data[8] & 0xFF) << 24) | ((data[9] & 0xFF) << 16) |
				((data[10] & 0xFF) << 8) | (data[11] & 0xFF);
	}

	StreamEvent::~StreamEvent() {

	}

	unsigned int StreamEvent::getDescriptorTag() {
		return descriptorTag;
	}

	unsigned int StreamEvent::getDescriptorLength() {
		return descriptorLength;
	}

	unsigned int StreamEvent::getId() {
		return eventId;
	}

	long double StreamEvent::getTimeReference() {
		return timeReference;
	}

	char* StreamEvent::getData() {
		return data + 2;
	}

	void* StreamEvent::getEventData() {
		return (void*)&(data[12]);
	}

	void StreamEvent::setEventName(string name) {
		this->eventName = name;
	}

	string StreamEvent::getEventName() {
		return eventName;
	}

	void StreamEvent::print() {
		cout << "descriptorTag: " << descriptorTag << endl;
		cout << "descriptorLength: " << descriptorLength << endl;
		cout << "eventId: " << eventId << endl;
		cout << "eventNPT: " << timeReference << endl;
		cout << "eventName: " << eventName << endl;
		cout << "privateData: " << (string)(char*)getEventData() << endl;
	}
}
}
}
}
}
}
