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

#ifndef IINPUTMANAGER_H_
#define IINPUTMANAGER_H_

#include "interface/IEventBuffer.h"
#include "interface/ICmdEventListener.h"
#include "interface/IInputEventListener.h"
#include "interface/IMotionEventListener.h"

#include <set>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
  class IInputManager {
	public:
		virtual ~IInputManager(){};

		virtual void release()=0;

		virtual void addMotionEventListener(IMotionEventListener* listener)=0;
		virtual void removeMotionEventListener(
				IMotionEventListener* listener)=0;

		virtual void addInputEventListener(
				IInputEventListener* listener, set<int>* events)=0;

		virtual void removeInputEventListener(IInputEventListener* listener)=0;

		virtual void addApplicationInputEventListener(
				IInputEventListener* listener)=0;

		virtual void removeApplicationInputEventListener(
				IInputEventListener* listener)=0;

		virtual void setCommandEventListener(ICmdEventListener* listener)=0;

		virtual void postInputEvent(IInputEvent* event)=0;
		virtual void postInputEvent(int keyCode)=0;
		virtual void postCommand(string cmd, string args)=0;

		virtual void setAxisValues(int x, int y, int z)=0;
		virtual void setAxisBoundaries(int x, int y, int z)=0;
		virtual int getCurrentXAxisValue()=0;
		virtual int getCurrentYAxisValue()=0;

		virtual IEventBuffer* getEventBuffer()=0;
  };
}
}
}
}
}
}

#endif /*IINPUTMANAGER_H_*/
