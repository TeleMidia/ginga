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

#ifndef IFORMATTERFOCUSMANAGER_H_
#define IFORMATTERFOCUSMANAGER_H_

#include "mb/IMotionEventListener.h"
using namespace ::br::pucrio::telemidia::ginga::core::mb;

#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace focus {
	class IFormatterFocusManager : public IMotionEventListener {
		public:
			virtual ~IFormatterFocusManager(){};

			virtual void setMotionBoundaries(int x, int y, int w, int h)=0;
			virtual bool motionEventReceived(int x, int y, int z)=0;
			virtual bool isKeyHandler()=0;
			virtual bool setKeyHandler(bool isHandler)=0;

			virtual void tapObject(void* executionObject)=0;
			virtual void setKeyMaster(string mediaId)=0;
			virtual void setFocus(string focusIndex)=0;
	};
}
}
}
}
}
}

#endif /*FORMATTERFOCUSMANAGER_H_*/
