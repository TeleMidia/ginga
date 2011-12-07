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

#include "../../../../../config.h"

#include "player/AwesomiumHandler.h"
#include "player/PlayersComponentSupport.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace player {
	IInputManager* AwesomiumHandler::im = NULL;

	WebView* AwesomiumHandler::webView = NULL;
	WebCore* AwesomiumHandler::webCore = NULL;

	AwesomiumHandler::AwesomiumHandler() {
#if HAVE_COMPSUPPORT
		dm = ((LocalDeviceManagerCreator*)(
				cm->getObject("LocalDeviceManager")))();

		if (im == NULL) {
			im = ((InputManagerCreator*)(cm->getObject("InputManager")))();
		}

		surface = ((SurfaceCreator*)(cm->getObject("Surface")))(NULL, 0, 0);
#else
		dm = LocalDeviceManager::getInstance();
		im = InputManager::getInstance();
		surface = new DFBSurface(NULL);
#endif

		w = 0;
		h = 0;

		hasFocus = false;

		if (webCore == NULL) {
			webCore = new Awesomium::WebCore();
		}
	}

	AwesomiumHandler::~AwesomiumHandler() {
		clog << "AwesomiumHandler::~AwesomiumHandler " << endl;

		im->removeInputEventListener(this);
		//Attention: Surface is deleted by Player
	}

	void AwesomiumHandler::getSize(int* w, int* h) {
		*w = this->w;
		*h = this->h;
	}

	void AwesomiumHandler::setSize(int w, int h) {
		this->w = w;
		this->h = h;
	}

	void AwesomiumHandler::loadUrl(string url) {
		mURL = url;

		if (webView == NULL) {
			webView = webCore->createWebView(w, h);
		}

		if (webView != NULL) {
			webView->loadURL(mURL);
		}
	}

	string AwesomiumHandler::getUrl() {
		return (mURL);
	}

	ISurface* AwesomiumHandler::getSurface() {
		return (surface);
	}

	void AwesomiumHandler::setFocus(bool focus) {
		if (focus && !hasFocus) {
			hasFocus = true;
			im->addInputEventListener(this);

		} else if (!focus && hasFocus) {
			hasFocus = false;
			im->removeInputEventListener(this);
		}
	}

	bool AwesomiumHandler::userEventReceived(IInputEvent* userEvent) {
		cout << "AwesomiumHandler::userEventReceived " << endl;

		return (true);
	}

	void AwesomiumHandler::refresh() {

	}
}
}
}
}
}
}
