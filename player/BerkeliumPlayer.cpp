/* Copyright (C) 1989-2017 PUC-Rio/Laboratorio TeleMidia

This file is part of Ginga (Ginga-NCL).

Ginga is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

Ginga is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
License for more details.

You should have received a copy of the GNU General Public License
along with Ginga.  If not, see <http://www.gnu.org/licenses/>.  */

#include "player/BerkeliumPlayer.h"

#include "player/PlayersComponentSupport.h"

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace player {
	BBrowserFactory BerkeliumPlayer::berkeliumFactory;
	bool BerkeliumPlayer::mainLoopDone = false;

	BBrowserFactory::BBrowserFactory() {
		Thread::mutexInit(&smutex, NULL);
	}

	BBrowserFactory::~BBrowserFactory() {
		Thread::mutexDestroy(&smutex);

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
		updateMap();

		hasIt = !bMap.empty();

		unlockSet();

		return hasIt;
	}

	bool BBrowserFactory::hasRunningBrowser() {
		bool bRunning = false;

		map<BerkeliumHandler*, short>::iterator i;

		lockSet();
		i = bMap.begin();
		while (i != bMap.end()) {
			if (i->second != BPT_RELEASE) {
				bRunning = true;
				break;
			}
			++i;
		}

		unlockSet();

		return bRunning;
	}

	void BBrowserFactory::createBrowser(BerkeliumHandler* bInfo) {
		lockSet();
		bMap[bInfo] = BPT_INIT;
		unlockSet();
	}

	void BBrowserFactory::stopBrowser(BerkeliumHandler* bInfo) {
		lockSet();
		bMap[bInfo] = BPT_STOP;
		unlockSet();
	}

	void BBrowserFactory::destroyBrowser(BerkeliumHandler* bInfo) {
		lockSet();
		bMap[bInfo] = BPT_RELEASE;
		unlockSet();
	}

	void BBrowserFactory::initBrowser(BerkeliumHandler* bInfo) {
		Context* context;
		string mrl;
		int w, h;

		context = Context::create();
		std::auto_ptr<Window> bwin(Window::create(context));

		bInfo->setContext(context);
		bInfo->getSize(&w, &h);

		bwin->resize(w, h);
		bwin->setDelegate(bInfo);

		mrl = bInfo->getUrl();

		if (mrl.find("file://") == std::string::npos &&
				fileExists(mrl)) {

			mrl = "file://" + mrl;
		}

		bwin->navigateTo(URLString::point_to(mrl));

		wstring str_css(L"::-webkit-scrollbar { display: none; }");
		wstring str_js(L"document.body.style.overflow='hidden'");

		bwin->insertCSS(
				WideString::point_to(str_css.c_str()),
				WideString::empty());

		bwin->executeJavascript(
				WideString::point_to(str_js.c_str()));

		bInfo->setWindow(bwin);
	}

	void BBrowserFactory::updateMap() {
		BerkeliumHandler* bInfo;
		map<BerkeliumHandler*, short>::iterator i, j;

		i = bMap.begin();
		while (i != bMap.end()) {
			bInfo = i->first;

			switch (i->second) {
				case BPT_INIT:
					initBrowser(bInfo);
					bMap[bInfo] = BPT_UPDATE;
					break;

				case BPT_UPDATE:
					bInfo->updateEvents();
					break;

				case BPT_STOP:
					bInfo->stop();
					bMap[bInfo] = BPT_NONE;
					break;

				case BPT_RELEASE:
					delete bInfo;
					bMap.erase(i);
					i = bMap.begin();
					continue;

				default:
					break;
			}

			++i;
		}
	}

	void BBrowserFactory::lockSet() {
		Thread::mutexLock(&smutex);
	}

	void BBrowserFactory::unlockSet() {
		Thread::mutexUnlock(&smutex);
	}

	BerkeliumPlayer::BerkeliumPlayer(
			GingaScreenID myScreen, string mrl) : Player(myScreen, mrl) {
		clog << "BerkeliumPlayer::BerkeliumPlayer '" << mrl << "'" << endl;

		pthread_t tId;
		pthread_attr_t tattr;

		bInfo = NULL;

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
		clog << "BerkeliumPlayer::~BerkeliumPlayer " << endl;
		berkeliumFactory.destroyBrowser(bInfo);

		if (!berkeliumFactory.hasRunningBrowser()) {
			while (!mainLoopDone) {
				SystemCompat::uSleep(30000);
			}
		}

		clog << "BerkeliumPlayer::~BerkeliumPlayer all done" << endl;
	}

	GingaSurfaceID BerkeliumPlayer::getSurface() {
		clog << "BerkeliumPlayer::getSurface '" << mrl << "'" << endl;

		if (bInfo != NULL) {
			surface = bInfo->getSurface();
		}

		return Player::getSurface();
	}

	void BerkeliumPlayer::setNotifyContentUpdate(bool notify) {
		clog << "BerkeliumPlayer::setNotifyContentUpdate '" << mrl << "'" << endl;
		/*if (notify) {
			setGhostBrowser(mBrowser);
		}*/
		Player::setNotifyContentUpdate(notify);
	}

	bool BerkeliumPlayer::setOutWindow(GingaWindowID windowId) {
		clog << "BerkeliumPlayer::setOutWindow '" << mrl << "'" << endl;
		Player::setOutWindow(windowId);
/*
		if (hasBrowser && outputWindow != NULL) {
			clog << "BerkeliumPlayer::setOutWindow '" << mrl << "' call ";
			clog << "browserSetFlipWindow" << endl;
			browserSetFlipWindow(mBrowser, outputWindow->getContent());
			return true;
		}
*/
		return false;
	}

	void BerkeliumPlayer::setBounds(int x, int y, int w, int h) {
		clog << "BerkeliumPlayer::setBounds x = '" << x << "', y = ";
		clog << y << "', w = '" << w << "', h = '" << h << "'.";
		clog << endl;

		if (bInfo == NULL) {
			bInfo = new BerkeliumHandler(myScreen, x, y, w, h);
			bInfo->setUrl(mrl);

		} else {
			bInfo->setBounds(x, y, w, h);
		}
	}

	bool BerkeliumPlayer::play() {
		GingaWindowID iWin;

		berkeliumFactory.createBrowser(bInfo);
		iWin = dm->getSurfaceParentWindow(surface);
		if (iWin != 0) {
			dm->renderWindowFrom(myScreen, iWin, surface);
		}
		return Player::play();
	}

	void BerkeliumPlayer::stop() {
		clog << "BerkeliumPlayer::stop '" << mrl << "'" << endl;
		berkeliumFactory.stopBrowser(bInfo);
		Player::stop();
	}

	void BerkeliumPlayer::setPropertyValue(string name, string value) {
		int x, y, w, h;
		vector<string>* params;

		clog << "BerkeliumPlayer::setProperty '" << name << "' value '";
		clog << value << "'" << endl;

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

			x = (int)util::stof((*params)[0]);
			y = (int)util::stof((*params)[1]);
			w = (int)util::stof((*params)[2]);
			h = (int)util::stof((*params)[3]);

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

			w = (int)util::stof((*params)[2]);
			h = (int)util::stof((*params)[3]);

			delete params;

			setBounds(0, 0, w, h);
			return;
		}

		Player::setPropertyValue(name, value);
	}

	bool BerkeliumPlayer::setKeyHandler(bool isHandler) {
		clog << "BerkeliumPlayer::setKeyHandler '" << mrl << "'" << endl;
		if (bInfo != NULL) {
			bInfo->setKeyHandler(isHandler);
		}

		//browserSetFocusHandler((int)isHandler, mBrowser);
		return isHandler;
	}

	void* BerkeliumPlayer::mainLoop(void* ptr) {
		mainLoopDone = false;

		if (!Berkelium::init(Berkelium::FileString::empty())) {
			clog << "BerkeliumPlayer::mainLoop ";
			clog << "Failed to initialize berkelium!" << endl;

			mainLoopDone = true;
			return NULL;
		}

		clog << "BerkeliumPlayer::mainLoop" << endl;

		bool firstTime = true;

		while (berkeliumFactory.isRunning() || firstTime) {
			if (firstTime) {
				firstTime = false;
				int waitFactory = 0;
				while (waitFactory < 1000000) {
					Berkelium::update();
					SystemCompat::uSleep(30000);
					waitFactory = waitFactory + 30000;
				}
			}

			if (berkeliumFactory.hasBrowser()) {
				Berkelium::update();

				/*if (notifyContentUpdate) {
					if (status == PLAY || status == PAUSE) {
						notifyPlayerListeners(
								PL_NOTIFY_UPDATECONTENT, "",
								TYPE_PASSIVEDEVICE, "");
					}
				}*/

				SystemCompat::uSleep(30000);

			} else {
				clog << "BerkeliumPlayer::mainLoop stopping factory!" << endl;
				berkeliumFactory.stop();
			}
	    }

		clog << "BerkeliumPlayer::mainLoop exit from factory!" << endl;
		Berkelium::update();

		clog << "BerkeliumPlayer::mainLoop stopping berkelium!" << endl;
		Berkelium::stopRunning();

		clog << "BerkeliumPlayer::mainLoop destroying berkelium!" << endl;
		Berkelium::destroy();

		clog << "BerkeliumPlayer::mainLoop all done!" << endl;

		mainLoopDone = true;
		return NULL;
	}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::player::IPlayer*
		createBerkeliumPlayer(
				GingaScreenID screenId, const char* mrl) {

	return new ::br::pucrio::telemidia::ginga::core::player::BerkeliumPlayer(
			screenId, (string)mrl);
}

extern "C" void destroyBerkeliumPlayer(
		::br::pucrio::telemidia::ginga::core::player::IPlayer* p) {

	delete p;
}
