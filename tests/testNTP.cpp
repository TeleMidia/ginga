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

#include "player/NTPPlayer.h"

#include <iostream>
using namespace std;

#include "mb/LocalScreenManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::mb;

int main(int argc, char *argv[]) {
	NTPPlayer* ntpPlayer;

	GingaScreenID screen;
	IScreenManager* dm;

	SystemCompat::setLogTo(SystemCompat::LOG_NULL);

	dm = ScreenManagerFactory::getInstance();

	screen = dm->createScreen(argc, argv);

	ntpPlayer = new NTPPlayer(screen, "200.160.7.186");
	if (ntpPlayer->updateTime()) {
		string nclTime = "2014:08:22:21:30:00.000";
		cout << ntpPlayer->getTimeString() << endl;
		cout << ntpPlayer->elapsedTime(nclTime) << endl;
		cout << "Process done." << endl;

	} else {
		cout << "An error occurred during the process!" << endl;
	}

	delete ntpPlayer;

	cout << "gingacc-player test has shown ImagePlayers. ";
	cout << "press enter to automatic release";
	cout << endl;
	getchar();

	dm->clearWidgetPools(screen);
	dm->releaseScreen(screen);
	dm->releaseMB(screen);

	delete dm;

	cout << "Player test done. press enter to continue" << endl;
	getchar();

	return 0;
}
