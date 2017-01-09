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

#ifndef SDLINPUTEVENT_H_
#define SDLINPUTEVENT_H_

#include "SDL.h"

#include "mb/interface/IInputEvent.h"
#include "mb/interface/CodeMap.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	class SDLInputEvent : public IInputEvent {
		public:
			static const string ET_WAKEUP;
			static const string ET_INPUTEVENT;
			static const string ET_USEREVENT;

		private:
			SDL_Event event;
			int x;
			int y;

			bool capsOn;
			bool shiftOn;

		public:
			SDLInputEvent(SDL_Event event);
			SDLInputEvent(const int keyCode);
			SDLInputEvent(int type, void* data);

			virtual ~SDLInputEvent();

			void setModifiers(bool capsOn, bool shiftOn);

			void clearContent();
			void setKeyCode(GingaScreenID screenId, const int keyCode);
			const int getKeyCode(GingaScreenID screenId);

			unsigned int getType();
			void* getApplicationData();

			bool isButtonPressType();
			bool isMotionType();
			bool isPressedType();
			bool isKeyType();
			bool isApplicationType();

			void setAxisValue(int x, int y, int z);
			void getAxisValue(int* x, int* y, int* z);
			void* getContent();
	};
}
}
}
}
}
}

#endif /*SDLINPUTEVENT_H_*/
