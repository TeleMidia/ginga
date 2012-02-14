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

#include "mb/interface/sdl/input/SDLEventBuffer.h"
#include "mb/interface/sdl/input/SDLInputEvent.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace mb {
	SDLEventBuffer::SDLEventBuffer(GingaScreenID screen) {
		pthread_mutex_init(&ebMutex, NULL);
	}

	SDLEventBuffer::~SDLEventBuffer() {
		vector<SDL_Event>::iterator i;

		pthread_mutex_lock(&ebMutex);
		eventBuffer.clear();
		pthread_mutex_unlock(&ebMutex);
		pthread_mutex_destroy(&ebMutex);
	}

	void SDLEventBuffer::wakeUp() {
		SDL_Event event;

		event.type       = SDL_USEREVENT;
		event.user.code  = SDLK_APPLICATION;
		event.user.data1 = (void*)(SDLInputEvent::ET_WAKEUP.c_str());
		event.user.data2 = NULL;

		SDL_PushEvent(&event);
	}

	void SDLEventBuffer::postInputEvent(IInputEvent* event) {
		SDL_Event* ev;

		if (event != NULL && event->getContent() != NULL) {
			ev = (SDL_Event*)(event->getContent());
			SDL_PushEvent(ev);
		}

		if (event != NULL) {
			event->clearContent();
			delete event;
		}
	}

	void SDLEventBuffer::waitEvent() {
		SDL_Event event;

		pthread_mutex_lock(&ebMutex);
		if (!eventBuffer.empty()) {
			pthread_mutex_unlock(&ebMutex);
			return;
		}
		pthread_mutex_unlock(&ebMutex);

		if (SDL_WaitEvent(&event)) {
			if (event.type == SDL_USEREVENT &&
					event.user.code == SDLK_APPLICATION &&
					event.user.data1 != NULL &&
					event.user.data2 == NULL) {

				if (strcmp(
						(char*)event.user.data1,
						SDLInputEvent::ET_WAKEUP.c_str()) == 0) {

					return;
				}
			}

			pthread_mutex_lock(&ebMutex);
			eventBuffer.push_back(event);
			pthread_mutex_unlock(&ebMutex);
		}
	}

	IInputEvent* SDLEventBuffer::getNextEvent() {
		SDL_Event sdlEvent;
		IInputEvent* gingaEvent = NULL;
		vector<SDL_Event>::iterator i;

		pthread_mutex_lock(&ebMutex);
		if (!eventBuffer.empty()) {
			i = eventBuffer.begin();
			sdlEvent = *i;
			eventBuffer.erase(i);

			gingaEvent = new SDLInputEvent(sdlEvent);
		}
		pthread_mutex_unlock(&ebMutex);

		return gingaEvent;
	}

	void* SDLEventBuffer::getContent() {
		return (void*)&eventBuffer;
	}
}
}
}
}
}
}
