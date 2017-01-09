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

#ifndef IINPUTEVENT_H_
#define IINPUTEVENT_H_

#include "mb/IMBDefs.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	class IInputEvent {
		public:
			virtual ~IInputEvent(){};
			virtual void clearContent()=0;
			virtual void setKeyCode(GingaScreenID scrId, const int keyCode)=0;
			virtual const int getKeyCode(GingaScreenID screenId)=0;

			virtual unsigned int getType()=0;
			virtual void* getApplicationData()=0;
			virtual bool isButtonPressType()=0;
			virtual bool isMotionType()=0;
			virtual bool isPressedType()=0;
			virtual bool isKeyType()=0;
			virtual bool isApplicationType()=0;
			virtual void setAxisValue(int x, int y, int z)=0;
			virtual void getAxisValue(int* x, int* y, int* z)=0;
			virtual void* getContent()=0;
	};
}
}
}
}
}
}

#endif /*IINPUTEVENT_H_*/
