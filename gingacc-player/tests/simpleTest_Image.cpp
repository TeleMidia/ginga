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

#include "mb/LocalScreenManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::mb;

#include "player/ImagePlayer.h"
using namespace ::br::pucrio::telemidia::ginga::core::player;

#include "player/PlayersComponentSupport.h"

extern "C" {
#include <stdio.h>
}

void testPlayer(IScreenManager* dm, GingaScreenID screen) {
	IWindow* w;
	bool notFalse = true;
	ISurface* s;
	IPlayer* img1;

	w = dm->createWindow(screen, 10, 10, 100, 100, 3.3);

	w->setCaps(w->getCap("ALPHACHANNEL"));
	w->draw();
	w->show();
	w->raiseToTop();

	img1 = new ImagePlayer(screen, "C:/Temp/NCLs/internas/monos/i1.png");

	img1->setOutWindow(w->getId());
	s = img1->getSurface();
	if (s != NULL && s->setParentWindow((void*)w)) {
		w->renderFrom(s);
	}
}

bool running = false;

int main(int argc, char** argv, char** envp) {
	GingaScreenID screen1;
	int fakeArgc = 5;
	char* dfbArgv[5];
	char* sdlArgv[5];
	IScreenManager* dm;

	SystemCompat::setLogTo(SystemCompat::LOG_NULL);

	dm = ScreenManagerFactory::getInstance();

	sdlArgv[0] = (char*)"testScreen";
	sdlArgv[1] = (char*)"--vsystem";
	sdlArgv[2] = (char*)"sdl";
	sdlArgv[3] = (char*)"--vmode";
	sdlArgv[4] = (char*)"400x300";
	screen1 = dm->createScreen(fakeArgc, sdlArgv);

	testPlayer(dm, screen1);

	cout << "gingacc-player test has shown ImagePlayers. ";
	cout << "press enter to automatic release";
	cout << endl;
	getchar();

	dm->clearWidgetPools(screen1);
	dm->releaseScreen(screen1);
	dm->releaseMB(screen1);

	delete dm;

	cout << "Player test done. press enter to continue" << endl;
	getchar();

	return 0;
}
