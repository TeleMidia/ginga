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

#ifndef STREAMEVENT_H_
#define STREAMEVENT_H_

#include <string>
#include <iostream>
using namespace std;

#include <stdint.h>
#include <string.h>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace dataprocessing {
	class StreamEvent {
		private:
			unsigned int descriptorTag;
			unsigned int descriptorLength;
			unsigned int eventId;
			uint64_t timeReference;
			void* privateData;

			string eventName;

			//7 header bytes + 255 data field bytes
			char data[262];

		public:
			StreamEvent(void* descriptorData, unsigned int descriptorSize);
			virtual ~StreamEvent();

			unsigned int getDescriptorTag();
			unsigned int getDescriptorLength();
			unsigned int getId();
			long double getTimeReference();
			char* getData();
			void* getEventData();

			void setEventName(string eventName);
			string getEventName();

			void print();
	};
}
}
}
}
}
}

#endif /*STREAMEVENT_H_*/
