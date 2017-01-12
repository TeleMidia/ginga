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

#include "config.h"

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "system/compat/SystemCompat.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;

#include "mb/ILocalScreenManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::mb;

#include "player/IPlayer.h"
using namespace ::br::pucrio::telemidia::ginga::core::player;

#include "player/PlayersComponentSupport.h"

extern "C" {
#include <stdio.h>
}

bool debugging = false;

string updateFileUri(string file) {
	if (!SystemCompat::isAbsolutePath(file)) {
		if (debugging) {
			return SystemCompat::getUserCurrentPath() + "gingacc-player/test/" + file;

		} else {
			return SystemCompat::getUserCurrentPath() + file;
		}
	}

	return file;
}

IPlayer* aud1;
IPlayer* aud2;
IPlayer* aud3;
IPlayer* aud4;

void testPlayer(IScreenManager* dm, GingaScreenID screen) {
	IWindow* w;
	IWindow* ww;
	IWindow* www;
	IWindow* wwww;

	ISurface* s;

	w    = dm->createWindow(screen, 10, 10, 100, 100, 4.0);
	ww   = dm->createWindow(screen, 90, 90, 150, 150, 4.0);
	www  = dm->createWindow(screen, 0, 0, 400, 300, 4.0);
	wwww = dm->createWindow(screen, 0, 0, 10, 10, 4.0);

	w->setCaps(w->getCap("ALPHACHANNEL"));
	w->draw();
	w->show();

	ww->setCaps(w->getCap("ALPHACHANNEL"));
	ww->draw();
	ww->show();

	www->setCaps(w->getCap("ALPHACHANNEL"));
	www->draw();
	www->show();

	wwww->setCaps(w->getCap("ALPHACHANNEL"));
	wwww->draw();
	wwww->show();

	aud1->setOutWindow(w->getId());
	s = aud1->getSurface();
	if (s != NULL && s->setParentWindow((void*)w)) {
		w->renderFrom(s);
	}

	aud2->setOutWindow(ww->getId());
	s = aud2->getSurface();
	if (s != NULL && s->setParentWindow((void*)ww)) {
		ww->renderFrom(s);
	}

	aud3->setOutWindow(www->getId());
	s = aud3->getSurface();
	if (s != NULL && s->setParentWindow((void*)www)) {
		www->renderFrom(s);
	}

	aud1->play();
	cout << "gingacc-player test is playing audio1. press enter to ";
	cout << "play audio 2 simultaneously" << endl;
	getchar();
	aud2->play();
	cout << "gingacc-player test is playing audio1 and audio2. press enter to ";
	cout << "play audio 3 simultaneously" << endl;
	getchar();
	aud3->play();
	cout << "gingacc-player test is playing audio1 and audio2. press enter to ";
	cout << "play audio 4 simultaneously" << endl;
	getchar();
	aud4->play();
}

int main(int argc, char** argv, char** envp) {
	GingaScreenID screen1;
	IScreenManager* dm;

	SystemCompat::setLogTo(SystemCompat::LOG_NULL);


	cout << "gingacc-player test works only with enabled component support.";
	cout << endl;
	exit(0);

	int i;
	bool testAllScreens = false;
	bool printScreen    = false;

	for (i = 1; i < argc; i++) {
		if ((strcmp(argv[i], "--all") == 0)) {
			testAllScreens = true;

		} else if ((strcmp(argv[i], "--printscreen") == 0)) {
			printScreen    = true;

		} else if ((strcmp(argv[i], "--enable-log") == 0) && ((i + 1) < argc)) {
			if (strcmp(argv[i + 1], "stdout") == 0) {
				SystemCompat::setLogTo(SystemCompat::LOG_STDO);

			} else if (strcmp(argv[i + 1], "file") == 0) {
				SystemCompat::setLogTo(SystemCompat::LOG_FILE);
			}

		} else if ((strcmp(argv[i], "--debug") == 0)) {
			debugging = true;
		}
	}

	cout << "gingacc-player testAudio" << endl;
	screen1 = dm->createScreen(argc, argv);
	testPlayer(dm, screen1);

	if (printScreen) {
		dm->blitScreen(screen1, "/root/printscreen1.bmp");
	}

	i = 0;
	while (i < 5) {
		if (aud1 != NULL) {
			cout << "Audio 1 media time: '" << aud1->getMediaTime() << "'";
			cout << endl;
		}

		if (aud2 != NULL) {
			cout << "Audio 2 media time: '" << aud2->getMediaTime() << "'";
			cout << endl;
		}

		if (aud3 != NULL) {
			cout << "Audio 3 media time: '" << aud3->getMediaTime() << "'";
			cout << endl;
		}

		if (aud4 != NULL) {
			cout << "Audio 4 media time: '" << aud4->getMediaTime() << "'";
			cout << endl;
		}

		SystemCompat::uSleep(1000000);
		++i;
	}

	cout << "I'm tired showing media time! Press enter to automatic release";
	cout << endl;
	getchar();

	dm->clearWidgetPools(screen1);

	//TODO: tests
	cout << "Player test done. press enter to continue" << endl;
	getchar();

	return 0;
}
