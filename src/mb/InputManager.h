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

#ifndef INPUTMANAGER_H_
#define INPUTMANAGER_H_

#include "system/Thread.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::thread;

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "system/SystemCompat.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;

#include "IMBDefs.h"
#include "IInputManager.h"

#include <iostream>
#include <vector>
#include <set>
#include <map>
using namespace std;

typedef struct lockedLitenerAction {
	::br::pucrio::telemidia::ginga::core::mb::IInputEventListener* l;
	bool isAdd;
	set<int>* events;
} LockedAction;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	class InputManager : public IInputManager, public Thread {
		protected:
			map<IInputEventListener*, set<int>*> eventListeners;
			vector<LockedAction*> actionsToInpListeners;
			set<IInputEventListener*> applicationListeners;
			vector<LockedAction*> actionsToAppListeners;

			set<IMotionEventListener*> motionListeners;

			ICmdEventListener* cmdListener;

			pthread_mutex_t mlMutex;

			bool running;
			bool notifying;
			bool notifyingApp;
			IEventBuffer* eventBuffer;
			double lastEventTime;
			double imperativeIntervalTime;
			double declarativeIntervalTime;
			GingaScreenID myScreen;

			int currentXAxis;
			int currentYAxis;
			int currentZAxis;
			int maxX;
			int maxY;

			pthread_mutex_t actAppMutex;
			pthread_mutex_t actInpMutex;

			pthread_mutex_t appMutex;

		public:
			InputManager(GingaScreenID screenId);
			~InputManager();

		protected:
			void initializeInputIntervalTime();

			void release();

		public:
			void addMotionEventListener(IMotionEventListener* listener);
			void removeMotionEventListener(IMotionEventListener* listener);

		protected:
			virtual void notifyMotionListeners(int x, int y, int z);

		public:
			void addInputEventListener(
					IInputEventListener* listener, set<int>* events=NULL);

			void removeInputEventListener(IInputEventListener* listener);

		public:
			void setCommandEventListener(ICmdEventListener* listener);

		protected:
			void performInputLockedActions();
			void performApplicationLockedActions();
			virtual bool dispatchEvent(IInputEvent* keyEvent);
			virtual bool dispatchApplicationEvent(IInputEvent* keyEvent);

		public:
			void addApplicationInputEventListener(IInputEventListener* listener);
			void removeApplicationInputEventListener(
					IInputEventListener* listener);

			void postInputEvent(IInputEvent* event);
			void postInputEvent(int keyCode);
			void postCommand(string cmd, string args);

			void setAxisValues(int x, int y, int z);
			void setAxisBoundaries(int x, int y, int z);
			int getCurrentXAxisValue();
			int getCurrentYAxisValue();

			IEventBuffer* getEventBuffer();

		protected:
			virtual void run();
			void handleInputEvent (
				IInputEvent *inputEvent, int& pLastCode, int& lastCode,
				double& pTimeStamp, double& timeStamp, int& mouseX, int& mouseY);
	};
}
}
}
}
}
}

#endif /*INPUTMANAGER_H_*/
