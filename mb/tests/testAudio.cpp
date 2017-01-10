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

#include "system/compat/SystemCompat.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;

#include "config.h"

#include "LocalScreenManager.h"
#include "IContinuousMediaProvider.h"
using namespace ::br::pucrio::telemidia::ginga::core::mb;

#include "CodeMap.h"
#include "IDeviceScreen.h"

extern "C" {
#include "string.h"
}

#include <iostream>
using namespace std;

bool debugging = false;
bool enableChoro = false;

string updateFileUri(string file) {
	if (!SystemCompat::isAbsolutePath(file)) {
		if (debugging) {
			return SystemCompat::getUserCurrentPath() + "gingacc-mb/test/" + file;

		} else {
			return SystemCompat::getUserCurrentPath() + file;
		}
	}

	return file;
}

void testScreen(IScreenManager* dm, GingaScreenID screen) {
	IContinuousMediaProvider* aud1;
	IContinuousMediaProvider* aud2;
	IContinuousMediaProvider* aud3;
	
	string m1, m2, m3;
	bool notTrue = false;

	/* AUDIO PROVIDER */
	aud1 = dm->createContinuousMediaProvider(
			screen,
			"/root/workspaces/NCL/Garrincha/media/animGar.mp4",
			&notTrue,
			false);

	aud1->playOver(NULL, true, NULL);

	m1 = updateFileUri("techno.mp3");
	aud2 = dm->createContinuousMediaProvider(screen, m1.c_str(), &notTrue, false);
	aud2->playOver(NULL, true, NULL);

	if (enableChoro) {
		m2 = updateFileUri("choro.mp3");
		aud3 = dm->createContinuousMediaProvider(screen, m2.c_str(), &notTrue, false);
		aud3->playOver(NULL, true, NULL);
	}

	m3 = updateFileUri("rock.mp3");
	aud3 = dm->createContinuousMediaProvider(screen, m3.c_str(), &notTrue, false);
	aud3->playOver(NULL, true, NULL);
}

int main(int argc, char** argv) {
	GingaScreenID screen1;
	IScreenManager* dm;
	int i;

	SystemCompat::setLogTo(SystemCompat::LOG_STDO);
	initTimeStamp();

	cout << "gingacc-mb test works only with enabled component support";
	cout << endl;
	exit(0);

	for (i = 1; i < argc; i++) {
		if ((strcmp(argv[i], "--enable-log") == 0) && ((i + 1) < argc)) {
			if (strcmp(argv[i + 1], "stdout") == 0) {
				SystemCompat::setLogTo(SystemCompat::LOG_STDO);

			} else if (strcmp(argv[i + 1], "file") == 0) {
				SystemCompat::setLogTo(SystemCompat::LOG_FILE);
			}

		} else if ((strcmp(argv[i], "--debug") == 0)) {
			debugging = true;

		} else if ((strcmp(argv[i], "--enable-choro") == 0)) {
			enableChoro = true;
		}
	}

	cout << "gingacc-mb test has created the screen manager. ";
	cout << endl;

	screen1 = dm->createScreen(argc, argv);
	cout << "gingacc-mb test has created screen '" << screen1;
	cout << "'. calling providers";
	cout << endl;
	testScreen(dm, screen1);

	cout << "gingacc-mb test has shown providers. ";
	cout << "press enter to automatic release";
	cout << endl;
	getchar();

	dm->clearWidgetPools(screen1);
	dm->releaseScreen(screen1);
	dm->releaseMB(screen1);

	delete dm;
	cout << "gingacc-mb test all done. ";
	cout << "press enter to exit";
	cout << endl;

	//TODO: tests

	getchar();
	return 0;
}
