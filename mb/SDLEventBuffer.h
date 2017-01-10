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

#ifndef SDLEVENTBUFFER_H_
#define SDLEVENTBUFFER_H_

#include "IEventBuffer.h"
#include "LocalScreenManager.h"

#include "SDL.h"

#include <vector>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	class SDLEventBuffer : public IEventBuffer {
		private:
			GingaScreenID myScreen;
			pthread_mutex_t ebMutex;
			vector<SDL_Event> eventBuffer;
			bool capsOn;
			bool shiftOn;

			bool isWaiting;
			pthread_cond_t cond;
			pthread_mutex_t condMutex;

		public:
			SDLEventBuffer(GingaScreenID screen);
			virtual ~SDLEventBuffer();

			static bool checkEvent(Uint32 winId, SDL_Event event);
			void feed(SDL_Event event, bool capsOn, bool shiftOn);

			void wakeUp();
			void postInputEvent(IInputEvent* event);
			void waitEvent();
			IInputEvent* getNextEvent();
			void* getContent();

		private:
			void waitForEvent();
			bool eventArrived();
	};
}
}
}
}
}
}

#endif /*SDLINPUTEVENTBUFFER_H_*/
