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

#include "player/BerkeliumPlayer.h"

#include "player/PlayersComponentSupport.h"

#include <glib.h>
#include <glib-object.h>

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace player {
	BBrowserFactory BerkeliumPlayer::berkeliumFactory;

	BBrowserFactory::BBrowserFactory() {
		pthread_mutex_init(&smutex, NULL);
		pthread_mutex_init(&cmutex, NULL);
		pthread_mutex_init(&dmutex, NULL);
	}

	BBrowserFactory::~BBrowserFactory() {
		pthread_mutex_destroy(&smutex);
		pthread_mutex_destroy(&cmutex);
		pthread_mutex_destroy(&dmutex);

		stop();
	}

	void BBrowserFactory::start() {
		running = true;
	}

	void BBrowserFactory::stop() {
		running = false;
	}

	bool BBrowserFactory::isRunning() {
		return running;
	}

	bool BBrowserFactory::hasBrowser() {
		bool hasIt;

		lockSet();
		updateSets();

		hasIt = !bSet.empty();

		unlockSet();

		return hasIt;
	}

	void BBrowserFactory::createBrowser(BerkeliumHandler* bInfo) {
		Context* context;

		lockCSet();
		cBSet.insert(bInfo);
		unlockCSet();
	}

	void BBrowserFactory::destroyBrowser(BerkeliumHandler* bInfo) {
		lockDSet();
		dBSet.insert(bInfo);
		unlockDSet();
	}

	void BBrowserFactory::updateSets() {
		Context* context;
		BerkeliumHandler* bInfo;
		set<BerkeliumHandler*>::iterator i, j;
		int w, h;

		lockCSet();
		i = cBSet.begin();
		while (i != cBSet.end()) {
			bInfo = (*i);

			bSet.insert(bInfo);

			context = Context::create();
			std::auto_ptr<Window> bwin(Window::create(context));

			bInfo->setContext(context);
			bInfo->getSize(&w, &h);

			bwin->resize(w, h);
			bwin->setDelegate(bInfo);
			bwin->navigateTo(URLString::point_to(bInfo->getUrl()));

			bInfo->setWindow(bwin);

			++i;
		}
		cBSet.clear();
		unlockCSet();

		lockDSet();
		i = dBSet.begin();
		while (i != dBSet.end()) {
			bInfo = (*i);

			j = bSet.find(bInfo);
			if (j != bSet.end()) {
				bSet.erase(j);

				delete bInfo;

				if (bSet.empty()) {
					stop();
				}
			}

			++i;
		}
		dBSet.clear();
		unlockDSet();
	}

	void BBrowserFactory::lockCSet() {
		pthread_mutex_lock(&cmutex);
	}

	void BBrowserFactory::unlockCSet() {
		pthread_mutex_unlock(&cmutex);
	}

	void BBrowserFactory::lockDSet() {
		pthread_mutex_lock(&dmutex);
	}

	void BBrowserFactory::unlockDSet() {
		pthread_mutex_unlock(&dmutex);
	}

	void BBrowserFactory::lockSet() {
		pthread_mutex_lock(&smutex);
	}

	void BBrowserFactory::unlockSet() {
		pthread_mutex_unlock(&smutex);
	}

	BerkeliumPlayer::BerkeliumPlayer(string mrl) : Player(mrl) {
		cout << "BerkeliumPlayer::BerkeliumPlayer '" << mrl << "'" << endl;

		pthread_t tId;
		pthread_attr_t tattr;

		bInfo = new BerkeliumHandler();

		bInfo->setUrl(mrl);

		if (!berkeliumFactory.isRunning()) {
			berkeliumFactory.start();

			pthread_attr_init(&tattr);
			pthread_attr_setdetachstate(&tattr,PTHREAD_CREATE_DETACHED);
			pthread_attr_setscope(&tattr, PTHREAD_SCOPE_SYSTEM);

			pthread_create(&tId, &tattr, BerkeliumPlayer::mainLoop, this);
			pthread_detach(tId);
		}
	}

	BerkeliumPlayer::~BerkeliumPlayer() {
		cout << "BerkeliumPlayer::~BerkeliumPlayer " << endl;
	}

	ISurface* BerkeliumPlayer::getSurface() {
		cout << "BerkeliumPlayer::getSurface '" << mrl << "'" << endl;

		surface = bInfo->getSurface();
		return Player::getSurface();
	}

	void BerkeliumPlayer::setNotifyContentUpdate(bool notify) {
		cout << "BerkeliumPlayer::setNotifyContentUpdate '" << mrl << "'" << endl;
		/*if (notify) {
			setGhostBrowser(mBrowser);
		}*/
		Player::setNotifyContentUpdate(notify);
	}

	bool BerkeliumPlayer::setOutWindow(int windowId) {
		cout << "BerkeliumPlayer::setOutWindow '" << mrl << "'" << endl;
		Player::setOutWindow(windowId);
/*
		if (hasBrowser && outputWindow != NULL) {
			cout << "BerkeliumPlayer::setOutWindow '" << mrl << "' call ";
			cout << "browserSetFlipWindow" << endl;
			browserSetFlipWindow(mBrowser, outputWindow->getContent());
			return true;
		}
*/
		return false;
	}

	void BerkeliumPlayer::setBounds(int x, int y, int w, int h) {
		cout << "BerkeliumPlayer::setBounds x = '" << x << "', y = ";
		cout << y << "', w = '" << w << "', h = '" << h << "'.";
		cout << endl;

		bInfo->setSize(w, h);
/*
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;

		if (hasBrowser) {
			cout << "BerkeliumPlayer::setBounds '" << mrl;
			cout << "' call browserResizeCoord";
			cout << endl;
			browserResizeCoord(mBrowser, x, y, w, h);

		} else {
			cout << "BerkeliumPlayer::setBounds '" << mrl << "' call openBrowser";
			cout << endl;
			hasBrowser = true;
			mBrowser   = openBrowser(x, y, w, h);
		}*/
	}

	void BerkeliumPlayer::play() {
		berkeliumFactory.createBrowser(bInfo);
		Player::play();
	}

	void BerkeliumPlayer::stop() {
		cout << "BerkeliumPlayer::stop '" << mrl << "'" << endl;
		berkeliumFactory.destroyBrowser(bInfo);
		Player::stop();
	}

	void BerkeliumPlayer::setPropertyValue(string name, string value) {
		double val;
		int x, y, w, h;
		vector<string>* params;

		cout << "BerkeliumPlayer::setProperty '" << name << "' value '";
		cout << value << "'" << endl;

		//TODO: set transparency, scrollbar, support...
		if (name == "bounds") {
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

		} else if (name == "size") {
			if (value.find("%") != std::string::npos) {
				return;
			}

			params = split(value, ",");
			if (params->size() != 2) {
				delete params;
				return;
			}

			w = (int)stof((*params)[2]);
			h = (int)stof((*params)[3]);

			delete params;

			setBounds(0, 0, w, h);
			return;
		}

		Player::setPropertyValue(name, value);
	}

	bool BerkeliumPlayer::setKeyHandler(bool isHandler) {
		cout << "BerkeliumPlayer::setKeyHandler '" << mrl << "'" << endl;
/*		if (isHandler && notifyContentUpdate) {
			im->addInputEventListener(this, NULL);

		} else {
			im->removeInputEventListener(this);
		}
*/
		//browserSetFocusHandler((int)isHandler, mBrowser);
		return isHandler;
	}

	void* BerkeliumPlayer::mainLoop(void* ptr) {
		if (!Berkelium::init(FileString::empty())) {
			cout << "BerkeliumPlayer::mainLoop ";
			cout << "Failed to initialize berkelium!" << endl;
			return NULL;
		}

		cout << "BerkeliumPlayer::mainLoop" << endl;

		while (berkeliumFactory.isRunning()) {
			if (berkeliumFactory.hasBrowser()) {
				Berkelium::update();
			}

#ifdef _WIN32
			Sleep(10);
#else
			struct timeval tv;

			tv.tv_sec = 0;
			tv.tv_usec = 1000;
			::select(0, NULL, NULL, NULL, &tv);
#endif
	    }

/*
		if (notifyContentUpdate) {
			while (status == PLAY || status == PAUSE) {
				notifyListeners(
						PL_NOTIFY_UPDATECONTENT, "", TYPE_PASSIVEDEVICE);

				this->usleep(65);
			}
		}*/

		Berkelium::destroy();

		cout << "BerkeliumPlayer::mainLoop all done!" << endl;
		return NULL;
	}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::player::IPlayer*
		createBerkeliumPlayer(const char* mrl, bool hasVisual) {

	return new ::br::pucrio::telemidia::ginga::core::player::BerkeliumPlayer(
			(string)mrl);
}

extern "C" void destroyBerkeliumPlayer(
		::br::pucrio::telemidia::ginga::core::player::IPlayer* p) {

	delete p;
}
