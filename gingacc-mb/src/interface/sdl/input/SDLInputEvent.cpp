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
	SDLInputEvent::SDLInputEvent(void* event) {
		x = 0;
		y = 0;
	}

	SDLInputEvent::SDLInputEvent(const int keyCode) {
		x = 0;
		y = 0;
	}

	SDLInputEvent::SDLInputEvent(int clazz, int type, void* data) {
		initialize(clazz, type, data);
	}

	SDLInputEvent::SDLInputEvent(int type, void* data) {
		//initialize(DFEC_USER, type, data);
	}

	SDLInputEvent::~SDLInputEvent() {

	}

	void SDLInputEvent::initialize(int clazz, int type, void* data) {
		/*if (clazz == DFEC_USER) {
			event = (SDLEvent*)(new SDLUserEvent);
			event->clazz = (SDLEventClass)clazz;
			((SDLUserEvent*)event)->type = type;
			((SDLUserEvent*)event)->data = data;

		} else if (clazz == DFEC_INPUT) {
			event = (SDLEvent*)(new SDLInputEvent);
			event->clazz = (SDLEventClass)clazz;
			((SDLInputEvent*)event)->type = (SDLInputEventType)type;
		}*/

		x = 0;
		y = 0;
	}

	void SDLInputEvent::clearContent() {

	}

	void* SDLInputEvent::getContent() {
		return NULL;
	}

	void SDLInputEvent::setKeyCode(GingaScreenID screenId, const int keyCode) {

	}

	const int SDLInputEvent::getKeyCode(GingaScreenID screenId) {
		int result = CodeMap::KEY_NULL;

		//if (event != NULL && event->clazz == DFEC_INPUT) {

			//int result = ((SDLInputEvent*)event)->key_symbol;
			
			result = LocalScreenManager::getInstance()->fromMBToGinga(
					screenId, result);

			//Maping between keyboard and remote control
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

			//return result;

		//}
		//return CodeMap::KEY_NULL;
		return result;
	}

	void SDLInputEvent::setType(unsigned int type) {
		/*if (event != NULL) {
			if (event->clazz == DFEC_INPUT) {
				((SDLInputEvent*)event)->type = (SDLInputEventType)type;

			} else if (event->clazz == DFEC_USER) {
				((SDLUserEvent*)event)->type = (SDLInputEventType)type;
			}
		}*/
	}

	void* SDLInputEvent::getData() {
		return NULL;
	}

	unsigned int SDLInputEvent::getType() {
		/*if (event != NULL) {
			if (event->clazz == DFEC_INPUT) {
				return ((SDLInputEvent*)event)->type;

			} else if (event->clazz == DFEC_USER) {
				return ((SDLUserEvent*)event)->type;
			}
		}*/
		return CodeMap::KEY_NULL;
	}

	bool SDLInputEvent::isButtonPressType() {
		/*if (event != NULL && event->clazz == DFEC_INPUT) {
			return (((SDLInputEvent*)event)->type == DIET_BUTTONPRESS);
		}*/
		return false;
	}

	bool SDLInputEvent::isMotionType() {
		/*if (event != NULL && event->clazz == DFEC_INPUT) {
			return (((SDLInputEvent*)event)->type == DIET_AXISMOTION);
		}*/
		return false;
	}

	bool SDLInputEvent::isPressedType() {
		/*if (event != NULL && event->clazz == DFEC_INPUT) {
			return (((SDLInputEvent*)event)->type == DIET_KEYPRESS);
		}*/
		return false;
	}

	bool SDLInputEvent::isKeyType() {
		/*if (event != NULL && event->clazz == DFEC_INPUT) {
			return (((SDLInputEvent*)event)->type == DIET_KEYPRESS ||
					((SDLInputEvent*)event)->type == DIET_KEYRELEASE);
		}*/
		return false;
	}

	bool SDLInputEvent::isUserClass() {
		/*if (event != NULL) {
			return (event->clazz == DFEC_USER);
		}*/
		return false;
	}

	void SDLInputEvent::setAxisValue(int x, int y, int z) {
		this->x = x;
		this->y = y;
	}

	void SDLInputEvent::getAxisValue(int* x, int* y, int* z) {
		/*if (event != NULL && ((SDLInputEvent*)event)->type == DIET_AXISMOTION) {
			if (((SDLInputEvent*)event)->flags & DIEF_AXISABS) {
				switch (((SDLInputEvent*)event)->axis) {
					case DIAI_X:
						*x = ((SDLInputEvent*)event)->axisabs;
						break;

					case DIAI_Y:
						*y = ((SDLInputEvent*)event)->axisabs;
						break;

					case DIAI_Z:
						if (z != NULL) {
							*z = ((SDLInputEvent*)event)->axisabs;
						}
						break;

					default:
						break;
				}

			} else if (((SDLInputEvent*)event)->flags & DIEF_AXISREL) {
				switch (((SDLInputEvent*)event)->axis) {
					case DIAI_X:
						*x += ((SDLInputEvent*)event)->axisrel;
						if (*x < 0) {
							*x = 0;
						}
						break;

					case DIAI_Y:
						*y += ((SDLInputEvent*)event)->axisrel;
						if (*y < 0) {
							*y = 0;
						}
						break;

					case DIAI_Z:
						if (z != NULL) {
							*z += ((SDLInputEvent*)event)->axisrel;
							if (*z < 0) {
								*z = 0;
							}
						}
						break;

					default:
						break;
				}
			}

		} else if (isButtonPressType()) {
			*x = this->x;
			*y = this->y;
		}*/
	}
}
}
}
}
}
}
