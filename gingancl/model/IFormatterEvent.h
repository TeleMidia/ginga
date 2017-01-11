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

#ifndef _IFORMATTEREVENT_H_
#define _IFORMATTEREVENT_H_

#include "IEventListener.h"

#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace event {
  class IFormatterEvent {
	public:
		virtual ~IFormatterEvent(){};
		virtual bool instanceOf(string s)=0;
		virtual void setId(string id)=0;
		virtual void addEventListener(IEventListener* listener)=0;
		virtual bool containsEventListener(IEventListener* listener)=0;
		virtual bool abort()=0;
		virtual bool start()=0;
		virtual bool stop()=0;
		virtual bool pause()=0;
		virtual bool resume()=0;
		virtual void setCurrentState(short newState)=0;
		virtual short getCurrentState()=0;
		virtual void* getExecutionObject()=0;
		virtual void setExecutionObject(void* object)=0;
		virtual string getId()=0;
		virtual long getOccurrences()=0;
		virtual void removeEventListener(IEventListener* listener)=0;
  };
}
}
}
}
}
}
}

#endif //_IFORMATTEREVENT_H_
