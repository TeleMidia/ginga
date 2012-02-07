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

	SDLInputEvent::SDLInputEvent(void* event) {
		this->event = (SDL_Event*)event;

		x = 0;
		y = 0;
	}

	SDLInputEvent::SDLInputEvent(const int keyCode) {
		event = new SDL_Event;

		event->type       = SDL_USEREVENT;
		event->user.code  = keyCode;
		event->user.data1 = (void*)(ET_INPUTEVENT.c_str());
		event->user.data2 = NULL;

		x = 0;
		y = 0;
	}

	SDLInputEvent::SDLInputEvent(int type, void* data) {
		event = new SDL_Event;

		event->type       = SDL_USEREVENT;
		event->user.code  = type;
		event->user.data1 = (void*)(ET_USEREVENT.c_str());
		event->user.data2 = data;

		x = 0;
		y = 0;
	}

	SDLInputEvent::~SDLInputEvent() {
		if (event != NULL) {
			delete event;
			event = NULL;
		}
	}

	void SDLInputEvent::clearContent() {
		this->event = NULL;
	}

	void* SDLInputEvent::getContent() {
		return event;
	}

	void SDLInputEvent::setKeyCode(GingaScreenID screenId, const int keyCode) {
		int sdlCode;

		if (event != NULL) {
			sdlCode = LocalScreenManager::getInstance()->fromGingaToMB(
					screenId, keyCode);

			if (event->type == SDL_USEREVENT) {
				event->user.code = sdlCode;

			} else if (event->type == SDL_KEYDOWN || event->type == SDL_KEYUP) {
				event->key.keysym.sym = sdlCode;
			}
		}
	}

	const int SDLInputEvent::getKeyCode(GingaScreenID screenId) {
		int result = CodeMap::KEY_NULL;

		if (event != NULL) {
			if (event->type == SDL_FINGERUP ||
					event->type == SDL_MOUSEBUTTONUP ||
					event->type == SDL_TOUCHBUTTONUP) {

				return CodeMap::KEY_TAP;
			}

			if (event->type == SDL_USEREVENT) {
				result = event->user.code;

			} else if (event->type == SDL_KEYDOWN || event->type == SDL_KEYUP) {

				result = event->key.keysym.sym;

			} else {
				return result;
			}

			result = LocalScreenManager::getInstance()->fromMBToGinga(
					screenId, result);

			//Mapping between keyboard and remote control
			if (result == CodeMap::KEY_F1) {
				result = CodeMap::KEY_RED;

			} else if (result == CodeMap::KEY_F2) {
				result = CodeMap::KEY_GREEN;

			} else if (result == CodeMap::KEY_F3) {
				result = CodeMap::KEY_YELLOW;
		
			} else if (result == CodeMap::KEY_F4) {
				result = CodeMap::KEY_BLUE;

			} else if (result == CodeMap::KEY_F5) {
				result = CodeMap::KEY_MENU;

			} else if (result == CodeMap::KEY_F6) {
				result = CodeMap::KEY_INFO;

			} else if (result == CodeMap::KEY_F7) {
				result = CodeMap::KEY_EPG;

			} else if (result == CodeMap::KEY_PLUS_SIGN) {
				result = CodeMap::KEY_VOLUME_UP;

			} else if (result == CodeMap::KEY_MINUS_SIGN) {
				result = CodeMap::KEY_VOLUME_DOWN;

			} else if (result == CodeMap::KEY_PAGE_UP) {
				result = CodeMap::KEY_CHANNEL_UP;
	
			} else if (result == CodeMap::KEY_PAGE_DOWN) {
				result = CodeMap::KEY_CHANNEL_DOWN;

			} else if (result == CodeMap::KEY_BACKSPACE) {
				result = CodeMap::KEY_BACK;

			} else if (result == CodeMap::KEY_ESCAPE) {
				result = CodeMap::KEY_EXIT;
			}
		}

		return result;
	}

	void* SDLInputEvent::getApplicationData() {
		if (isApplicationType()) {
			return event->user.data2;
		}

		return NULL;
	}

	unsigned int SDLInputEvent::getType() {
		unsigned int result = CodeMap::KEY_NULL;

		if (event != NULL) {
			if (event->type == SDL_USEREVENT) {
				result = event->user.code;

			} else {
				result = event->type;
			}
		}

		return result;
	}

	bool SDLInputEvent::isButtonPressType() {
		if (event != NULL) {
			if (event->type == SDL_FINGERUP ||
					event->type == SDL_MOUSEBUTTONUP ||
					event->type == SDL_TOUCHBUTTONUP ||
					event->type == SDL_FINGERDOWN ||
					event->type == SDL_MOUSEBUTTONDOWN ||
					event->type == SDL_TOUCHBUTTONDOWN) {

				return true;
			}
		}
		return false;
	}

	bool SDLInputEvent::isMotionType() {
		if (event != NULL) {
			if (event->type == SDL_MOUSEMOTION ||
					event->type == SDL_FINGERMOTION) {

				return true;
			}
		}
		return false;
	}

	bool SDLInputEvent::isPressedType() {
		if (event != NULL) {
			if (event->type == SDL_KEYDOWN) {
				return true;
			}
		}
		return false;
	}

	bool SDLInputEvent::isKeyType() {
		if (event != NULL) {
			if (event->type == SDL_KEYUP || event->type == SDL_KEYDOWN) {
				return true;
			}
		}
		return false;
	}

	bool SDLInputEvent::isApplicationType() {
		if (event != NULL) {
			if (event->type == SDL_USEREVENT &&
					event->user.data1 != NULL &&
					event->user.data2 != NULL) {

				if (strcmp(
						(char*)(event->user.data1),
						ET_USEREVENT.c_str()) == 0) {

					return true;
				}
			}
		}
		return false;
	}

	void SDLInputEvent::setAxisValue(int x, int y, int z) {
		this->x = x;
		this->y = y;
	}

	void SDLInputEvent::getAxisValue(int* x, int* y, int* z) {
		SDL_MouseMotionEvent* mouseEvent;
		SDL_TouchFingerEvent* fingerEvent;

		*x = 0;
		*y = 0;
		*z = 0;

		if (event != NULL) {
			if (event->type == SDL_MOUSEMOTION ||
					event->type == SDL_MOUSEBUTTONUP ||
					event->type == SDL_MOUSEBUTTONDOWN) {

				mouseEvent = (SDL_MouseMotionEvent*)event;

				*x = mouseEvent->x;
				*y = mouseEvent->y;
				*z = 0;

			} else if (event->type == SDL_FINGERMOTION ||
					event->type == SDL_FINGERUP ||
					event->type == SDL_TOUCHBUTTONUP ||
					event->type == SDL_FINGERDOWN ||
					event->type == SDL_TOUCHBUTTONDOWN) {

				fingerEvent = (SDL_TouchFingerEvent*)event;

				*x = fingerEvent->x;
				*y = fingerEvent->y;
				*z = 0;
			}
		}
	}
}
}
}
}
}
}
