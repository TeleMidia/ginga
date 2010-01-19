/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware
Ginga (Ginga-NCL).

Direitos Autorais Reservados (c) 1989-2007 PUC-Rio/Laboratorio TeleMidia

Este programa eh software livre; voce pode redistribui-lo e/ou modificah-lo sob
os termos da Licen�a Publica Geral GNU versao 2 conforme publicada pela Free
Software Foundation.

Este programa eh distribu�do na expectativa de que seja util, porem, SEM
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

#include "../include/Graphics.h"
#include "../include/Contents.h"
#include "../include/Inputs.h"
#include "../include/IOHandler.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace io {
	bool IOHandler::busy = false;
	pthread_cond_t* IOHandler::busyCond = new pthread_cond_t;
	pthread_mutex_t* IOHandler::busyCondLocker = new pthread_mutex_t;

	//Warning! Just for PresentationManager
	void IOHandler::clearWidgetsPools() {
		check("clearWidgetsPools");
		Inputs::release();
		Graphics::releasePoolObjects();
		busy = false;
	}

	void IOHandler::initializeGraphics() {
		if (!Graphics::isInitialized()) {
			pthread_cond_init(busyCond, NULL);
			busy = true;
			Graphics::initialize();
			busy = false;
		}
	}

	void IOHandler::releaseGraphics() {
		check("releaseGraphics");
		Graphics::release();
		busy = false;
	}

	void IOHandler::check(string source) {
		while (busy) {
			//cout << "IOHandler is busy for '" << source << "'" << endl;
			pthread_mutex_lock(busyCondLocker);
			pthread_cond_wait(busyCond, busyCondLocker);
			pthread_mutex_unlock(busyCondLocker);
		}

		if (!Graphics::isInitialized()) {
			Graphics::initialize();
		}

		busy = true;
	}

	void IOHandler::setLayerColorKey(int r, int g, int b) {
		check("setLayerColorKey");
		Graphics::setLayerColorKey(r, g, b);
		busy = false;
		pthread_cond_signal(busyCond);
	}

	void* IOHandler::createWindow(void* desc) {
		void* win;

		if (desc == NULL) {
			cout << "IOHandler::createWindow Warning! NULL desc received";
			cout << endl;
			return NULL;
		}

		check("createWindow");
		win = Graphics::createWindow(desc);
		busy = false;
		pthread_cond_signal(busyCond);
		return win;
	}

	void IOHandler::releaseWindow(Window* win) {
		if (win == NULL) {
			cout << "IOHandler::releaseWindow Warning! NULL win received";
			cout << endl;
			return;
		}

		Graphics::releaseWindow(win->getContent());
	}

	void* IOHandler::createSurface(void* dsc) {
		void* surface;

		if (dsc == NULL) {
			cout << "IOHandler::createSurface Warning! NULL desc received";
			cout << endl;
			return NULL;
		}

		check("createSurface");
		surface = Graphics::createSurface(dsc);
		busy = false;
		pthread_cond_signal(busyCond);
		return surface;
	}

	void IOHandler::releaseSurface(Surface* sur) {
		if (sur == NULL) {
			cout << "IOHandler::releaseSurface Warning! NULL sur received";
			cout << endl;
			return;
		}

		Graphics::releaseSurface(sur->getContent());
	}

	void* IOHandler::createInputEventBuffer() {
		void* ieb;
		check("createInputEventBuffer");
		ieb = Inputs::createInputEventBuffer();
		busy = false;
		pthread_cond_signal(busyCond);
		return ieb;
	}

	void IOHandler::releaseInputBuffer(InputEventBuffer* buffer) {
		if (buffer == NULL) {
			cout << "IOHandler::releaseInputBuffer Warning! NULL buf received";
			cout << endl;
			return;
		}

		Inputs::releaseInputBuffer(buffer->getContent());
	}

	void* IOHandler::createVideoProvider(char* mrl) {
		void* vp;

		check("createVideoProvider");
		vp = Contents::createVideoProvider(mrl);
		busy = false;
		pthread_cond_signal(busyCond);
		return vp;
	}

	void IOHandler::releaseVideoProvider(VideoProvider* provider) {
		if (provider == NULL) {
			cout << "IOHandler::releaseVideoProv Warning! NULL prov received";
			cout << endl;
			return;
		}

		Contents::releaseVideoProvider(provider->getContent());
	}

	void* IOHandler::createImageProvider(char* mrl) {
		void* ip;

		check("createImageProvider");
		ip = Contents::createImageProvider(mrl);
		busy = false;
		pthread_cond_signal(busyCond);
		return ip;
	}

	void IOHandler::releaseImageProvider(ImageProvider* provider) {
		if (provider == NULL) {
			cout << "IOHandler::releaseImageProv Warning! NULL prov received";
			cout << endl;
			return;
		}

		Contents::releaseImageProvider(provider->getContent());
	}

	void* IOHandler::createFont(char* fontUri, int heightInPixel) {
		void* f;

		if (!fileExists((string)fontUri)) {
			cout << "IOHandler::createFont Warning! File not found '";
			cout << fontUri << endl;
			return NULL;
		}

		check("createFont");
		f = Contents::createFont(fontUri, heightInPixel);
		busy = false;
		pthread_cond_signal(busyCond);
		return f;
	}

	void IOHandler::releaseFont(FontProvider* font) {
		if (font == NULL) {
			cout << "IOHandler::releaseImageProv Warning! NULL font received";
			cout << endl;
			return;
		}

		Contents::releaseFont(font->getContent());
	}

	void* IOHandler::getGraphicsRoot() {
		return Graphics::getRoot();
	}
}
}
}
}
}
}
