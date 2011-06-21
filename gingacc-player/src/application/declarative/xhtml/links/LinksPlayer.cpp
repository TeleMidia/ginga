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

#include "player/LinksPlayer.h"

#include "player/PlayersComponentSupport.h"

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
		cout << "LinksPlayer::LinksPlayer '" << mrl << "'" << endl;
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

	ISurface* LinksPlayer::getSurface() {
		void* s;

		cout << "LinksPlayer::getSurface '" << mrl << "'" << endl;

		if (this->surface == NULL) {
			if (hasBrowser) {
				s = browserGetSurface(mBrowser);
				if (s != NULL) {
#if HAVE_COMPSUPPORT
					this->surface = ((SurfaceCreator*)(cm->getObject(
							"Surface")))(s, 0, 0);
#else

					this->surface = new DFBSurface(s);
#endif
				}

			} else {
				cout << "LinksPlayer::getSurface Warning! Trying to get ";
				cout << "a surface from a deleted browser" << endl;
			}
		}

		return Player::getSurface();
	}

	void LinksPlayer::setNotifyContentUpdate(bool notify) {
		cout << "LinksPlayer::setNotifyContentUpdate '" << mrl << "'" << endl;
		if (notify) {
			setGhostBrowser(mBrowser);
		}
		Player::setNotifyContentUpdate(notify);
	}

	bool LinksPlayer::setOutWindow(int windowId) {
		cout << "LinksPlayer::setOutWindow '" << mrl << "'" << endl;
		Player::setOutWindow(windowId);

		if (hasBrowser && outputWindow != NULL) {
			cout << "LinksPlayer::setOutWindow '" << mrl << "' call ";
			cout << "browserSetFlipWindow" << endl;
			browserSetFlipWindow(mBrowser, outputWindow->getContent());
			return true;
		}

		return false;
	}

	void LinksPlayer::setBounds(int x, int y, int w, int h) {
		cout << "LinksPlayer::setBounds '" << x << "', ";
		cout << "LinksPlayer::setBounds '" << y << "', ";
		cout << "LinksPlayer::setBounds '" << w << "', ";
		cout << "LinksPlayer::setBounds '" << h << "'.";
		cout << endl;

		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;

		if (hasBrowser) {
			cout << "LinksPlayer::setBounds '" << mrl;
			cout << "' call browserResizeCoord";
			cout << endl;
			browserResizeCoord(mBrowser, x, y, w, h);

		} else {
			cout << "LinksPlayer::setBounds '" << mrl << "' call openBrowser";
			cout << endl;
			hasBrowser = true;
			mBrowser   = openBrowser(x, y, w, h);
		}
	}

	void LinksPlayer::play() {
		cout << "LinksPlayer::play '" << mrl << "'" << endl;
#if !HAVE_MULTIPROCESS
		if (surface != NULL) {
			IWindow* parent = (IWindow*)(surface->getParent());
			if (parent != NULL) {
				browserSetFlipWindow(mBrowser, parent->getContent());
			}
		}
#endif

		loadUrlOn(mBrowser, mrl.c_str());
		browserShow(mBrowser);

		if (notifyContentUpdate) {
			Thread::start();
		}
		//::usleep(3000000);

		Player::play();
	}

	void LinksPlayer::stop() {
		cout << "LinksPlayer::stop '" << mrl << "'" << endl;
		if (hasBrowser) {
			closeBrowser(mBrowser);
			mBrowser = NULL;
			hasBrowser = false;
		}
		Player::stop();
	}

	void LinksPlayer::setPropertyValue(string name, string value) {
		cout << "LinksPlayer::setProperty '" << name << "' value '";
		cout << value << "'" << endl;

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

			if (value.find("%") != std::string::npos) {
				return;
			}

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

			setBounds(x, y, w, h);
			return;
		}

		Player::setPropertyValue(name, value);
	}

	bool LinksPlayer::setKeyHandler(bool isHandler) {
		cout << "LinksPlayer::setKeyHandler '" << mrl << "'" << endl;
		if (isHandler && notifyContentUpdate) {
			im->addInputEventListener(this, NULL);

		} else {
			im->removeInputEventListener(this);
		}

		browserSetFocusHandler((int)isHandler, mBrowser);
		return isHandler;
	}

	bool LinksPlayer::userEventReceived(IInputEvent* userEvent) {
		cout << "LinksPlayer::userEventReceived '" << mrl << "'" << endl;
		browserReceiveEvent(mBrowser, (void*)(userEvent->getContent()));
		return true;
	}

	void LinksPlayer::run() {
		cout << "LinksPlayer::run '" << mrl << "'" << endl;
		/*IWindow* parent;

		::usleep(1000000);
		if (surface != NULL) {
			parent = (IWindow*)(surface->getParent());
			if (parent != NULL) {
				parent->renderFrom(surface);
				browserSetFlipWindow(mBrowser, parent->getContent());
			}
		}*/

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
