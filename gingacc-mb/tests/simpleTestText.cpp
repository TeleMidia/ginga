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

/****************************************************************
 * Testing text provider.                                       *
 * You must set a font using --src /x/y.ttf (use absolute path) *
 * You can set the changing the value of strText var            *
 * In order to set the screen size, enter the following as      *
 * parameter: ./simpleTestText --vmode 800x600 --src ...        *
 * To exit, read instructions in your terminal                  *
 ****************************************************************/

int main(int argc, char** argv) {
	GingaScreenID screen;
	IScreenManager* dm;
	string fontUri = "";

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
			fontUri.assign(argv[i + 1]);
		}
	}

	/* CREATING MAIN SCREEN ACCORDING TO MAIN PARAMETERS */
	screen = dm->createScreen(argc, argv);
	cout << "gingacc-mb test has created screen '" << screen;
	cout << "'. creating text provider using '" << fontUri << "'";
	cout << endl;

	/***** TEXT *****/
	GingaWindowID win;
	GingaSurfaceID s;
	GingaProviderID font = NULL;
	int x, y, w, h, z;
	bool notFalse = true;
	string strText;

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
	font = dm->createFontProvider(screen, fontUri.c_str(), 12);

	dm->setSurfaceParentWindow(screen, s, win);
	dm->setWindowBgColor(screen, win, 255, 0, 0, 0);
	dm->showWindow(screen, win);
	dm->raiseWindowToTop(screen, win);

	cout << "gingacc-mb test text for screen '" << screen << "' has '";
	cout << dm->getProviderMediaTime(font) << "' as its total media time." << endl;

	//drawing BLACK
	strText = "BLACK";
	dm->setSurfaceColor(s, 0, 0, 0, 0);
	dm->playProviderOver(font, s, strText.c_str(), 20, 20, 0);

	//drawing WHITE
	strText = "WHITE";
	dm->setSurfaceColor(s, 255, 255, 255, 0);
	dm->playProviderOver(font, s, strText.c_str(), 20, 50, 0);

	dm->validateWindow(screen, win);

	cout << "gingacc-mb text is playing. if you see only a red square, we ";
	cout << "have a problem with text rendering.";
	cout << "press enter to stop and release";
	cout << endl;
	getchar();

	dm->releaseFontProvider(screen, font);
	font = NULL;

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
