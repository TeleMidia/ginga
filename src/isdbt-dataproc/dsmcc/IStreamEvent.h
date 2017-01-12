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

#ifndef ISTREAMEVENT_H_
#define ISTREAMEVENT_H_

#include <string>
#include <iostream>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace dataprocessing {
	class IStreamEvent {
		public:
			virtual ~IStreamEvent(){};
			virtual unsigned int getDescriptorTag()=0;
			virtual unsigned int getDescriptorLength()=0;
			virtual unsigned int getId()=0;
			virtual long double getTimeReference()=0;
			virtual char* getData()=0;
			virtual void* getEventData()=0;
			virtual void setEventName(string eventName)=0;
			virtual string getEventName()=0;
			virtual void print()=0;
	};
}
}
}
}
}
}

#endif /*ISTREAMEVENT_H_*/
