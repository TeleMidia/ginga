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

#include "dataprocessing/dsmcc/StreamEvent.h"

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
		clog << "descriptorTag: " << descriptorTag << endl;
		clog << "descriptorLength: " << descriptorLength << endl;
		clog << "eventId: " << eventId << endl;
		clog << "eventNPT: " << timeReference << endl;
		clog << "eventName: " << eventName << endl;
		clog << "privateData: " << (string)(char*)getEventData() << endl;
	}
}
}
}
}
}
}
