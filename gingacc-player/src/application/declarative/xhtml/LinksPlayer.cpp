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

#include "../../../../include/LinksPlayer.h"

#include "../../../../include/PlayersComponentSupport.h"

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace player {
	IInputManager* LinksPlayer::im = NULL;

	LinksPlayer::LinksPlayer(string mrl) : Player(mrl), Thread::Thread() {
		ILocalDeviceManager* dm = NULL;

		mBrowser = NULL;
		this->x = 1;
		this->y = 1;
		this->w = 1;
		this->h = 1;

#if HAVE_COMPSUPPORT
		dm = ((LocalDeviceManagerCreator*)(cm->getObject(
				"LocalDeviceManager")))();

		if (im == NULL) {
			im = ((InputManagerCreator*)(cm->getObject("InputManager")))();
		}
#else
		dm = LocalDeviceManager::getInstance();
		im = InputManager::getInstance();
#endif

		if (dm != NULL) {
			setBrowserDFB(dm->getGfxRoot());
			setDisplayMenu(0);
		}

		mBrowser = openBrowser(x, y, w, h);
		hasBrowser = false;
	}

	LinksPlayer::~LinksPlayer() {
		cout << "LinksPlayer::~LinksPlayer " << endl;
		im->removeInputEventListener(this);

		if (hasBrowser) {
			cout << "LinksPlayer::~LinksPlayer hasBrowser" << endl;
			closeBrowser(mBrowser);
			mBrowser = NULL;
			cout << "LinksPlayer::~LinksPlayer browser closed" << endl;
		}
	}

	void LinksPlayer::setNotifyContentUpdate(bool notify) {
		if (notify) {
			setGhostBrowser(mBrowser);
		}
		Player::setNotifyContentUpdate(notify);
	}

	void LinksPlayer::setBounds(int x, int y, int w, int h) {
		void* s;

		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;

		if (!hasBrowser) {
			//mBrowser = openBrowser(x, y, w, h);
			loadUrlOn(mBrowser, mrl.c_str());
			browserResizeCoord(mBrowser, x, y, w, h);
			if (mBrowser != NULL) {
				s = browserGetSurface(mBrowser);
				if (s != NULL) {
#if HAVE_COMPSUPPORT
					this->surface = ((SurfaceCreator*)(cm->getObject(
							"Surface")))(s, 0, 0);
#else
					this->surface = new DFBSurface(s);
#endif
				}
			}
			hasBrowser = true;
		}
	}

	void LinksPlayer::updateBounds(int x, int y, int w, int h) {
//		closeBrowser(mBrowser);
//		mBrowser = NULL;
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;

//		mBrowser = openBrowser(x, y, w, h);
		browserResizeCoord(mBrowser, x, y, w, h);
		//browserRequestFocus(mBrowser);
		/*parent = (Window*)(surface->getParent());
		if (parent != NULL) {
			cout << " UPDATE RENDER" << endl;
			parent->renderFrom(surface);
			parent->validate();

		} else {
			cout << " UPDATE PARENT IS NULL" << endl;
		}*/
	}

	void LinksPlayer::play() {
		browserShow(mBrowser);
		//browserResizeCoord(mBrowser, x, y, w, h);
		//loadUrlOn(mBrowser, mrl.c_str());
		//browserRequestFocus(mBrowser);

		Thread::start();
		//::usleep(3000000);

		Player::play();
	}

	void LinksPlayer::stop() {
		if (hasBrowser) {
			closeBrowser(mBrowser);
			mBrowser = NULL;
			hasBrowser = false;
		}
		Player::stop();
	}

	void LinksPlayer::setPropertyValue(
			string name, string value, double duration, double by) {

		//TODO: set scrollbar, support...
		if (name == "transparency") {
			double val;

			val = stof(value);
			if (val >= 0.0 && val <= 1.0) {
				browserSetAlpha((int)(val * 0xFF), mBrowser);
			}

		} else if (name == "bounds") {
			int x, y, w, h;
			vector<string>* params;

			params = split(value, ",");
			if (params->size() != 4) {
				delete params;
				return;
			}

			x = (int)stof((*params)[0]);
			y = (int)stof((*params)[1]);
			w = (int)stof((*params)[2]);
			h = (int)stof((*params)[3]);

			delete params;

			if (!hasBrowser) {
				setBounds(x, y, w, h);

			} else {
				updateBounds(x, y, w, h);
			}

			return;
		}

		Player::setPropertyValue(name, value, duration, by);
	}

	bool LinksPlayer::setKeyHandler(bool isHandler) {
		if (isHandler && notifyContentUpdate) {
			im->addInputEventListener(this, NULL);

		} else {
			im->removeInputEventListener(this);
		}

		browserSetFocusHandler((int)isHandler, mBrowser);
		return isHandler;
	}

	bool LinksPlayer::userEventReceived(IInputEvent* userEvent) {
		browserReceiveEvent(mBrowser, (void*)(userEvent->getContent()));
		return true;
	}

	void LinksPlayer::run() {
		IWindow* parent;

		::usleep(1000000);
		if (surface != NULL) {
			parent = (IWindow*)(surface->getParent());
			if (parent != NULL) {
				parent->renderFrom(surface);
				browserSetFlipWindow(mBrowser, parent->getContent());
			}
		}

		if (notifyContentUpdate) {
			while (status == PLAY || status == PAUSE) {
				notifyListeners(
						PL_NOTIFY_UPDATECONTENT, "", TYPE_PASSIVEDEVICE);

				this->usleep(65);
			}
		}
	}
}
}
}
}
}
}


extern "C" ::br::pucrio::telemidia::ginga::core::player::IPlayer*
		createLinksPlayer(const char* mrl, bool hasVisual) {

	return new ::br::pucrio::telemidia::ginga::core::player::LinksPlayer(
			(string)mrl);
}

extern "C" void destroyLinksPlayer(
		::br::pucrio::telemidia::ginga::core::player::IPlayer* p) {

	delete p;
}
