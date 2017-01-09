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

#include "player/ShowButton.h"

#include "player/PlayersComponentSupport.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace player {
	static IScreenManager* dm = ScreenManagerFactory::getInstance();
	ShowButton::ShowButton(GingaScreenID screenId) : Thread() {
		myScreen       = screenId;
		status         = NONE;
		previousStatus = NONE;
		win            = 0;
		running        = false;
	}

	ShowButton::~ShowButton() {
		isDeleting = true;

		while (running) {
			SystemCompat::uSleep(50000);
		}
	}

	void ShowButton::initializeWindow() {
		int x = 0, y, w, h;

		if (dm != NULL) {
			x = (int)(dm->getDeviceWidth(myScreen) - 70);
		}

		y = 10;
		w = 60;
		h = 60;

		if (dm != NULL) {
			win = dm->createWindow(myScreen, x, y, w, h, 4.0);
		}
		int caps = dm->getWindowCap (myScreen, win, "ALPHACHANNEL");
		dm->setWindowCaps (myScreen, win, caps);
		dm->drawWindow (myScreen, win);
	}

	void ShowButton::stop() {
		//lock();
		previousStatus = status;
		status = STOP;
		//Thread::start();
		//unlock();
		run();
	}

	void ShowButton::pause() {
		//lock();
		previousStatus = status;
		if (status != PAUSE) {
			status = PAUSE;

		} else {
			status = PLAY;
		}

		run();
		//Thread::start();
		//unlock();
	}

	void ShowButton::resume() {
		pause();
	}

	void ShowButton::release() {
		lock();
		if (win != 0) {
			dm->hideWindow (myScreen, win);
			dm->deleteWindow(myScreen, win);
			win = 0;
		}
		unlock();
	}

	void ShowButton::render(string mrl) {
		GingaSurfaceID surface;

		surface = dm->createRenderedSurfaceFromImageFile(myScreen, mrl.c_str());

		lock();
		if (win == 0) {
			initializeWindow();
		}

		if ( dm->setSurfaceParentWindow(myScreen, surface, win)) {
			dm->renderWindowFrom(myScreen, win, surface);
		}
		dm->showWindow (myScreen, win);
		dm->raiseWindowToTop (myScreen, win);
		unlock();
	}

	void ShowButton::run() {
		//lock();
		running = true;

		if (!isDeleting) {
			switch (status) {
				case PAUSE:
					clog << "ShowButton::run PAUSE" << endl;
					render(SystemCompat::appendGingaFilesPrefix(
							"img/button/pauseButton.png"));

					break;

				case STOP:
					clog << "ShowButton::run STOP" << endl;
					if (previousStatus == PAUSE) {
						release();
					}

					render(SystemCompat::appendGingaFilesPrefix(
							"img/button/stopButton.png"));

					SystemCompat::uSleep(1000000);
					release();
					break;

				case PLAY:
					clog << "ShowButton::run PLAY" << endl;
					if (previousStatus == PAUSE) {
						release();
					}

					render(SystemCompat::appendGingaFilesPrefix(
							"img/button/playButton.png"));

					SystemCompat::uSleep(1000000);
					release();
					break;

				default:
					clog << "ShowButton::run DEFAULT" << endl;
					break;
			}
		}

		running = false;
		//unlock();
	}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::player::IShowButton*
		createShowButton(GingaScreenID screenId) {

	br::pucrio::telemidia::ginga::core::player::ShowButton* sb;
	sb = new br::pucrio::telemidia::ginga::core::player::ShowButton(screenId);

	return sb;
}

extern "C" void destroyShowButton(
		::br::pucrio::telemidia::ginga::core::player::IShowButton* sb) {

	delete sb;
}
