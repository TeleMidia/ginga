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

#include "system/compat/SystemCompat.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;

#include "ILocalScreenManager.h"
#include "IInputManager.h"
#include "interface/CodeMap.h"
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

int main(int argc, char** argv) {
	GingaScreenID screen1, screen2;
	IInputManager* im1;
	IInputManager* im2;
	IScreenManager* dm;
	TestInputEventListener* l1;
	TestInputEventListener* l2;

	int fakeArgc = 5;
	char* dfbArgv[5];
	char* sdlArgv[5];

	SystemCompat::setLogTo(SystemCompat::LOG_STDO);

	cout << "gingacc-mb works only with enabled component support" << endl;
	exit(0);

	int i;
	bool testAllScreens = false;
	for (i = 1; i < argc; i++) {
		if ((strcmp(argv[i], "--all") == 0)) {
			testAllScreens = true;
		}
	}

	if (testAllScreens) {
		dfbArgv[0] = (char*)"testInput";
		dfbArgv[1] = (char*)"--vsystem";
		dfbArgv[2] = (char*)"dfb";
		dfbArgv[3] = (char*)"--vmode";
		dfbArgv[4] = (char*)"400x300";

		screen1 = dm->createScreen(fakeArgc, dfbArgv);
		im1     = dm->getInputManager(screen1);
		l1      = new TestInputEventListener(screen1);

		im1->addInputEventListener(l1, NULL);

		sdlArgv[0] = (char*)"testInput";
		sdlArgv[1] = (char*)"--vsystem";
		sdlArgv[2] = (char*)"sdl";
		sdlArgv[3] = (char*)"--vmode";
		sdlArgv[4] = (char*)"400x300";

		screen2 = dm->createScreen(fakeArgc, sdlArgv);
		im2     = dm->getInputManager(screen2);
		l2      = new TestInputEventListener(screen2);

		im2->addInputEventListener(l2, NULL);

	} else {
		screen1 = dm->createScreen(argc, argv);
		im1     = dm->getInputManager(screen1);
		l1      = new TestInputEventListener(screen1);

		im1->addInputEventListener(l1, NULL);
	}

	cout << "gingacc-mb testing input events. ";
	cout << "press enter to automatic release";
	cout << endl;
	getchar();

	im1->release();
	delete l1;

	dm->clearWidgetPools(screen1);
	dm->releaseScreen(screen1);
	dm->releaseMB(screen1);

	if (testAllScreens) {
		im2->release();
		delete l2;

		dm->clearWidgetPools(screen2);
		dm->releaseScreen(screen2);
		dm->releaseMB(screen2);
	}

	delete dm;
	cout << "gingacc-mb test all done. ";
	cout << "press enter to exit";
	cout << endl;

	getchar();
	return 0;
}
