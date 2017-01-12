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

#ifndef IEVENTBUFFER_H_
#define IEVENTBUFFER_H_

#include "IInputEvent.h"

#include "IMBDefs.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	class IEventBuffer {
		public:
			virtual ~IEventBuffer(){};
			virtual void wakeUp()=0;
			virtual void postInputEvent(IInputEvent* event)=0;
			virtual void waitEvent()=0;
			virtual IInputEvent* getNextEvent()=0;
			virtual void* getContent()=0;
	};
}
}
}
}
}
}

#endif /*IEVENTBUFFER_H_*/
