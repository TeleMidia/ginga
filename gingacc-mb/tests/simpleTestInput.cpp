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

#include "util/Color.h"
#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "config.h"

#include "mb/LocalScreenManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::mb;

#include "system/compat/SystemCompat.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;

#include "mb/ILocalScreenManager.h"
#include "mb/IInputManager.h"
#include "mb/interface/CodeMap.h"
using namespace ::br::pucrio::telemidia::ginga::core::mb;

extern "C" {
#include "string.h"
}

#include <iostream>
using namespace std;

class TestInputEventListener : public IInputEventListener {
private:
	GingaScreenID myScreen;

public:
	TestInputEventListener(GingaScreenID screenId) {
		myScreen = screenId;
	}

	~TestInputEventListener() {

	}

	bool userEventReceived(IInputEvent* ev) {
		int value = ev->getKeyCode(myScreen);

		cout << "SCREEN(" << myScreen << ") input listener has RECEIVED '";
		cout << value << "' = '";
		cout << CodeMap::getInstance()->getValue(value) << "'";
		cout << endl;
		std::flush(cout);

		return true;
	}
};

/***********************************************************
 * Testing input events.                                   *
 * You can see which event was created when you hit a key  *
 * You must set your focus on the blank screen and see the *
 * output in your terminal                                 *
 * In order to set the screen size, enter the following as *
 * parameter: ./simpleTestInput --vmode 800x600            *
 * To exit, hit enter in your terminal                     *
 ***********************************************************/

int main(int argc, char** argv) {
	GingaScreenID screen;
	IInputManager* im;
	IScreenManager* dm;
	TestInputEventListener* l;
	int fakeArgc = 5;
	char* sdlArgv[5];

	SystemCompat::setLogTo(SystemCompat::LOG_FILE);

	dm = ScreenManagerFactory::getInstance();

	screen = dm->createScreen(argc, argv);
	im     = dm->getInputManager(screen);
	l      = new TestInputEventListener(screen);
	im->addInputEventListener(l, NULL); //NULL == I want to receive all events

	cout << "gingacc-mb testing input events. ";
	cout << "press enter to automatic release";
	cout << endl;
	getchar();

	im->release();
	delete l;

	dm->clearWidgetPools(screen);
	dm->releaseScreen(screen);
	dm->releaseMB(screen);

	delete dm;
	cout << "gingacc-mb test all done. ";
	cout << "press enter to exit";
	cout << endl;

	getchar();
	return 0;
}
