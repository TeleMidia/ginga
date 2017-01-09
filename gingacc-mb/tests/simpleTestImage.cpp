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

#include "mb/interface/IDeviceScreen.h"

extern "C" {
#include "string.h"
#include "pthread.h"
}

#include <string>
#include <iostream>
using namespace std;

/***********************************************************
 * Testing image provider.                                 *
 * You can set an image input using --src /x/y.png         *
 * In order to set the screen size, enter the following as *
 * parameter: ./simpleTestImage --vmode 800x600 --src ...  *
 * To exit, read instructions in your terminal             *
 ***********************************************************/

int main(int argc, char** argv) {
	GingaScreenID screen;
	IScreenManager* dm;
	string imageUri = "";

	//SETTING LOG INFO TO A FILE
	SystemCompat::setLogTo(SystemCompat::LOG_FILE);

/* GETTING DEVICE SCREEN */
	dm = ScreenManagerFactory::getInstance();

	cout << "gingacc-mb test has created the screen manager. ";
	cout << endl;

	/* PROCESSING MAIN PARAMETERS*/
	for (int i = 1; i < argc; i++) {
		if ((strcmp(argv[i], "--enable-log") == 0) && ((i + 1) < argc)) {
			if (strcmp(argv[i + 1], "stdout") == 0) {
				SystemCompat::setLogTo(SystemCompat::LOG_STDO);

			} else if (strcmp(argv[i + 1], "file") == 0) {
				SystemCompat::setLogTo(SystemCompat::LOG_FILE);
			}

		} else if ((strcmp(argv[i], "--src") == 0) && ((i + 1) < argc)) {
			imageUri.assign(argv[i + 1]);
		}
	}

	if (imageUri == "") {
		cout << "Please specify the image URI with --src parameter" << endl;
		exit(0);
	}

	/* CREATING MAIN SCREEN ACCORDING TO MAIN PARAMETERS */
	screen = dm->createScreen(argc, argv);
	cout << "gingacc-mb test has created screen '" << screen;
	cout << "'. creating image provider for '" << imageUri << "'";
	cout << endl;

	/***** IMAGE *****/
	GingaWindowID win;
	GingaSurfaceID s;
	GingaProviderID img = NULL;
	int x, y, w, h, z;
	bool notFalse = true;

	x = 25;
	y = 25;
	w = 100;
	h = 100;
	z = 1;

	win = dm->createWindow(screen, x, y, w, h, z);
	if (win == 0) {
		cout << "gingacc-mb test can't create window. exiting program...";
		cout << endl;
		exit(1);
	}
	dm->drawWindow(screen, win);

	s   = dm->createSurface(screen);
	img = dm->createImageProvider(screen, imageUri.c_str());

	dm->setSurfaceParentWindow(screen, s, win);
	dm->setWindowBgColor(screen, win, 255, 0, 0, 0);
	dm->showWindow(screen, win);
	dm->raiseWindowToTop(screen, win);

	cout << "gingacc-mb test image for screen '" << screen << "' has '";
	cout << dm->getProviderMediaTime(img) << "' as its total media time." << endl;

	dm->setProviderSoundLevel(img, 0.1);
	dm->playProviderOver(img, s);
	dm->validateWindow(screen, win);

	cout << "gingacc-mb image is playing. if you see a red square, we have ";
	cout << "a problem with image rendering.";
	cout << "press enter to stop and release";
	cout << endl;
	getchar();

	dm->releaseImageProvider(screen, img);
	dm->clearWidgetPools(screen);
	dm->releaseScreen(screen);
	dm->releaseMB(screen);

	delete dm;
	cout << "gingacc-mb test all done. ";
	cout << "press enter to exit";
	cout << endl;

	//TODO: more tests

	getchar();
	return 0;
}
