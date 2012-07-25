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
	LinksPlayer::LinksPlayer(GingaScreenID screenId, string mrl) :
			Player(screenId, mrl), Thread::Thread() {

		clog << "LinksPlayer::LinksPlayer '" << mrl << "'" << endl;

		LocalScreenManager::addListenerInstance(this);

		mBrowser   = NULL;
		this->x    = 1;
		this->y    = 1;
		this->w    = 1;
		this->h    = 1;
		this->im   = NULL;
		screenName = dm->getScreenName(myScreen);

		if (screenName == "dfb") {
			this->im = dm->getInputManager(myScreen);

			setBrowserDFB(dm->getGfxRoot(myScreen));
			setDisplayMenu(0);
		}

		hasBrowser = false;
	}

	LinksPlayer::~LinksPlayer() {
		clog << "LinksPlayer::~LinksPlayer " << endl;

		LocalScreenManager::removeListenerInstance(this);

		if (im != NULL) {
			im->removeInputEventListener(this);
			im = NULL;
		}

#if HAVE_COMPSUPPORT
		if (hasBrowser) {
			clog << "LinksPlayer::~LinksPlayer hasBrowser" << endl;
			closeBrowser(mBrowser);
			mBrowser = NULL;
			clog << "LinksPlayer::~LinksPlayer browser closed" << endl;
		}
#endif
	}

	ISurface* LinksPlayer::getSurface() {
		void* s;

		clog << "LinksPlayer::getSurface '" << mrl << "'" << endl;

		if (this->surface == NULL) {
			if (hasBrowser) {
				s = browserGetSurface(mBrowser);
				if (s != NULL) {
					this->surface = dm->createSurfaceFrom(myScreen, s);
				}

			} else {
				clog << "LinksPlayer::getSurface Warning! Trying to get ";
				clog << "a surface from a deleted browser" << endl;
			}
		}

		return Player::getSurface();
	}

	void LinksPlayer::setNotifyContentUpdate(bool notify) {
		clog << "LinksPlayer::setNotifyContentUpdate '" << mrl << "'" << endl;

		if (hasBrowser) {
			if (notify) {
				setGhostBrowser(mBrowser);
			}
		}
		Player::setNotifyContentUpdate(notify);
	}

	bool LinksPlayer::setOutWindow(GingaWindowID windowId) {
		clog << "LinksPlayer::setOutWindow '" << mrl << "'" << endl;
		Player::setOutWindow(windowId);

		if (hasBrowser && outputWindow != NULL) {
			clog << "LinksPlayer::setOutWindow '" << mrl << "' call ";
			clog << "browserSetFlipWindow" << endl;
			browserSetFlipWindow(mBrowser, outputWindow->getContent());
			return true;
		}

		return false;
	}

	void LinksPlayer::setBounds(int x, int y, int w, int h) {
		clog << "LinksPlayer::setBounds '" << x << "', ";
		clog << "LinksPlayer::setBounds '" << y << "', ";
		clog << "LinksPlayer::setBounds '" << w << "', ";
		clog << "LinksPlayer::setBounds '" << h << "'.";
		clog << endl;

		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;

		if (hasBrowser) {
			clog << "LinksPlayer::setBounds '" << mrl;
			clog << "' call browserResizeCoord";
			clog << endl;
			browserResizeCoord(mBrowser, x, y, w, h);

		} else if (screenName == "dfb") {
			clog << "LinksPlayer::setBounds '" << mrl << "' call openBrowser";
			clog << endl;
			hasBrowser = true;
			mBrowser   = openBrowser(x, y, w, h);
		}
	}

	bool LinksPlayer::play() {
		clog << "LinksPlayer::play(" << mrl << ")" << endl;

		if (screenName == "dfb") {
#if !HAVE_MULTIPROCESS
			if (surface != NULL) {
				IWindow* parent = (IWindow*)(surface->getParentWindow());

				if (parent != NULL) {
					browserSetFlipWindow(mBrowser, parent->getContent());
				}
			}
#endif

			loadUrlOn(mBrowser, mrl.c_str());
			browserShow(mBrowser);

			if (notifyContentUpdate) {
				Thread::startThread();
			}
			//SystemCompat::uSleep(3000000);
		}

		return Player::play();
	}

	void LinksPlayer::stop() {
		clog << "LinksPlayer::stop '" << mrl << "'" << endl;
		if (hasBrowser) {
#if HAVE_COMPSUPPORT
			closeBrowser(mBrowser);
			mBrowser = NULL;
#else
			browserHide(mBrowser);
#endif
			hasBrowser = false;
		}
		Player::stop();
	}

	void LinksPlayer::setPropertyValue(string name, string value) {
		clog << "LinksPlayer::setProperty '" << name << "' value '";
		clog << value << "'" << endl;

		if (screenName == "dfb") {
			//TODO: set scrollbar, support...
			if (name == "transparency") {
				double val;

				val = stof(value);
				if (val >= 0.0 && val <= 1.0) {
					val = 1 - val;
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
		}

		Player::setPropertyValue(name, value);
	}

	bool LinksPlayer::setKeyHandler(bool isHandler) {
		clog << "LinksPlayer::setKeyHandler '" << mrl << "': isHandler = '";
		clog << isHandler << "', notifyContentUpdate = '";
		clog << notifyContentUpdate << "'" << endl;

		if (screenName == "dfb") {
			if (isHandler && notifyContentUpdate) {
				im->addInputEventListener(this, NULL);

			} else {
				im->removeInputEventListener(this);
			}

			browserSetFocusHandler((int)isHandler, mBrowser);
		}

		return isHandler;
	}

	bool LinksPlayer::userEventReceived(IInputEvent* userEvent) {
		/*clog << "LinksPlayer::userEventReceived(" << mrl << "'): ";
		if (userEvent != NULL) {
			clog << userEvent->getKeyCode() << "'";
		}
		clog << endl;*/

		if (screenName == "dfb") {
			browserReceiveEvent(mBrowser, (void*)(userEvent->getContent()));
		}

		return true;
	}

	void LinksPlayer::run() {
		clog << "LinksPlayer::run '" << mrl << "'" << endl;
		/*IWindow* parent;

		SystemCompat::uSleep(1000000);
		if (surface != NULL) {
			parent = (IWindow*)(surface->getParent());
			if (parent != NULL) {
				parent->renderFrom(surface);
				browserSetFlipWindow(mBrowser, parent->getContent());
			}
		}*/

		if (notifyContentUpdate) {
			while (status == PLAY || status == PAUSE) {
				notifyPlayerListeners(
						PL_NOTIFY_UPDATECONTENT, "", TYPE_PASSIVEDEVICE, "");

				this->mSleep(65);
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
		createLinksPlayer(
				GingaScreenID screenId, const char* mrl, bool hasVisual) {

	return new ::br::pucrio::telemidia::ginga::core::player::LinksPlayer(
			screenId, (string)mrl);
}

extern "C" void destroyLinksPlayer(
		::br::pucrio::telemidia::ginga::core::player::IPlayer* p) {

	delete p;
}
