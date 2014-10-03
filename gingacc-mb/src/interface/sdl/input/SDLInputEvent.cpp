/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware
Ginga (Ginga-NCL).

Direitos Autorais Reservados (c) 1989-2007 PUC-Rio/Laboratorio TeleMidia

Este programa eh software livre; voce pode redistribui-lo e/ou modificah-lo sob
os termos da Licenca Publica Geral GNU versao 2 conforme publicada pela Free
Software Foundation.

Este programa eh distribuido na expectativa de que seja util, porem, SEM
NENHUMA GARANTIA; nem mesmo a garantia implicita de COMERCIABILIDADE OU
ADEQUACAO A UMA FINALIDADE ESPECIFICA. Consulte a Licenca Publica Geral do
GNU versao 2 para mais detalhes.

Voce deve ter recebido uma copia da Licenca Publica Geral do GNU versao 2 junto
com este programa; se nao, escreva para a Free Software Foundation, Inc., no
endereco 59 Temple Street, Suite 330, Boston, MA 02111-1307 USA.

Para maiores informacoes:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
******************************************************************************
This file is part of the declarative environment of middleware Ginga (Ginga-NCL)

Copyright: 1989-2007 PUC-RIO/LABORATORIO TELEMIDIA, All Rights Reserved.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License version 2 as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License version 2 for more
details.

You should have received a copy of the GNU General Public License version 2
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

For further information contact:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
*******************************************************************************/

#include "mb/interface/sdl/input/SDLInputEvent.h"
#include "mb/interface/CodeMap.h"
#include "mb/LocalScreenManager.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {

	const string SDLInputEvent::ET_WAKEUP     = "GINGA_WAKEUP";
	const string SDLInputEvent::ET_INPUTEVENT = "GINGA_INPUTEVENT";
	const string SDLInputEvent::ET_USEREVENT  = "GINGA_USEREVENT";

	SDLInputEvent::SDLInputEvent(SDL_Event event) {
		this->event = event;

		x       = 0;
		y       = 0;
		capsOn  = false;
		shiftOn = false;
	}

	SDLInputEvent::SDLInputEvent(const int keyCode) {
		event.type           = SDL_KEYDOWN;
		event.key.type       = SDL_KEYDOWN;
		event.key.state      = SDL_PRESSED;
		event.key.repeat     = 0;
		event.key.keysym.sym = keyCode;

		x = 0;
		y = 0;
	}

	SDLInputEvent::SDLInputEvent(int type, void* data) {
		event.type       = SDL_USEREVENT;
		event.user.code  = type;
		event.user.data1 = (void*)(ET_USEREVENT.c_str());
		event.user.data2 = data;

		x = 0;
		y = 0;
	}

	SDLInputEvent::~SDLInputEvent() {

	}

	void SDLInputEvent::setModifiers(bool capsOn, bool shiftOn) {
		this->capsOn  = capsOn;
		this->shiftOn = shiftOn;
	}

	void SDLInputEvent::clearContent() {

	}

	void* SDLInputEvent::getContent() {
		return &event;
	}

	void SDLInputEvent::setKeyCode(GingaScreenID screenId, const int keyCode) {
		int sdlCode;

		sdlCode = ScreenManagerFactory::getInstance()->fromGingaToMB(
				screenId, keyCode);

		if (event.type == SDL_USEREVENT) {
			event.user.code = sdlCode;

		} else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
			event.key.keysym.sym = sdlCode;
		}
	}

	const int SDLInputEvent::getKeyCode(GingaScreenID screenId) {
		int gingaValue;
		int sdlValue;

		if (event.type == SDL_FINGERUP ||
				event.type == SDL_MOUSEBUTTONUP) {

			return CodeMap::KEY_TAP;
		}

		if (event.type == SDL_USEREVENT) {
			clog << "SDLInputEvent::getKeyCode user event" << endl;
			sdlValue = event.user.code;

		} else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
			sdlValue = event.key.keysym.sym;
			clog << "SDLInputEvent::getKeyCode key pressed event sdlValue = '";
			clog << sdlValue << "'" << endl;

		} else {
			clog << "SDLInputEvent::getKeyCode unknown event type.";
			clog << "Returning KEY_NULL" << endl;
			return CodeMap::KEY_NULL;;
		}

		gingaValue = ScreenManagerFactory::getInstance()->fromMBToGinga(
				screenId, sdlValue);

		if (gingaValue >= CodeMap::KEY_SMALL_A &&
				gingaValue <= CodeMap::KEY_SMALL_Z &&
				((capsOn && !shiftOn) || (!capsOn && shiftOn))) {

			gingaValue = ScreenManagerFactory::getInstance()->fromMBToGinga(
					screenId, sdlValue + 5000);
		}

		//Mapping between keyboard and remote control
		if (gingaValue == CodeMap::KEY_F1) {
			gingaValue = CodeMap::KEY_RED;

		} else if (gingaValue == CodeMap::KEY_F2) {
			gingaValue = CodeMap::KEY_GREEN;

		} else if (gingaValue == CodeMap::KEY_F3) {
			gingaValue = CodeMap::KEY_YELLOW;

		} else if (gingaValue == CodeMap::KEY_F4) {
			gingaValue = CodeMap::KEY_BLUE;

		} else if (gingaValue == CodeMap::KEY_F5) {
			gingaValue = CodeMap::KEY_MENU;

		} else if (gingaValue == CodeMap::KEY_F6) {
			gingaValue = CodeMap::KEY_INFO;

		} else if (gingaValue == CodeMap::KEY_F7) {
			gingaValue = CodeMap::KEY_EPG;

		} else if (gingaValue == CodeMap::KEY_PLUS_SIGN) {
			gingaValue = CodeMap::KEY_VOLUME_UP;

		} else if (gingaValue == CodeMap::KEY_MINUS_SIGN) {
			gingaValue = CodeMap::KEY_VOLUME_DOWN;

		} else if (gingaValue == CodeMap::KEY_PAGE_UP) {
			gingaValue = CodeMap::KEY_CHANNEL_UP;

		} else if (gingaValue == CodeMap::KEY_PAGE_DOWN) {
			gingaValue = CodeMap::KEY_CHANNEL_DOWN;

		} else if (gingaValue == CodeMap::KEY_BACKSPACE) {
			gingaValue = CodeMap::KEY_BACK;

		} else if (gingaValue == CodeMap::KEY_ESCAPE) {
			gingaValue = CodeMap::KEY_EXIT;
		}

		return gingaValue;
	}

	void* SDLInputEvent::getApplicationData() {
		if (isApplicationType()) {
			return event.user.data2;
		}

		return NULL;
	}

	unsigned int SDLInputEvent::getType() {
		unsigned int result = CodeMap::KEY_NULL;

		if (event.type == SDL_USEREVENT) {
			result = event.user.code;

		} else {
			result = event.type;
		}

		return result;
	}

	bool SDLInputEvent::isButtonPressType() {
		if (event.type == SDL_FINGERUP ||
				event.type == SDL_MOUSEBUTTONUP ||
				event.type == SDL_FINGERDOWN ||
				event.type == SDL_MOUSEBUTTONDOWN) {

			return true;
		}

		return false;
	}

	bool SDLInputEvent::isMotionType() {
		if (event.type == SDL_MOUSEMOTION ||
				event.type == SDL_FINGERMOTION) {

			return true;
		}
		return false;
	}

	bool SDLInputEvent::isPressedType() {
		if (event.type == SDL_KEYDOWN) {
			return true;
		}
		return false;
	}

	bool SDLInputEvent::isKeyType() {
		if (event.type == SDL_KEYUP || event.type == SDL_KEYDOWN) {
			return true;
		}
		return false;
	}

	bool SDLInputEvent::isApplicationType() {
		if (event.type == SDL_USEREVENT &&
				event.user.data1 != NULL &&
				event.user.data2 != NULL) {

			if (strcmp(
					(char*)(event.user.data1),
					ET_USEREVENT.c_str()) == 0) {

				return true;
			}
		}

		return false;
	}

	void SDLInputEvent::setAxisValue(int x, int y, int z) {
		this->x = x;
		this->y = y;
	}

	void SDLInputEvent::getAxisValue(int* x, int* y, int* z) {
		*x = 0;
		*y = 0;

		switch (event.type) {
			case SDL_MOUSEMOTION:
				*x = event.motion.x;
				*y = event.motion.y;
				break;

			case SDL_MOUSEBUTTONUP:
			case SDL_MOUSEBUTTONDOWN:
				*x = event.button.x;
				*y = event.button.y;
				break;

			case SDL_FINGERUP:
			case SDL_FINGERDOWN:
			case SDL_FINGERMOTION:
				*x = event.tfinger.x;
				*y = event.tfinger.y;
				break;
		}
	}
}
}
}
}
}
}
