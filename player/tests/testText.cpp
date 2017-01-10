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

typedef struct {
	IScreenManager* dm;
	GingaScreenID screen;
	set<IWindow*>* windows;
} TestContainer;

bool running = false;

void preparePlayer(
		IPlayer* txt,
		string fontUri,
		string fileUri,
		string fontSize,
		string fontColor,
		string bgColor,
		string content) {

	txt->setPropertyValue("x-setFile", fileUri);
	txt->setPropertyValue("fontUri",   fontUri);
	txt->setPropertyValue("fontSize",  fontSize);
	txt->setPropertyValue("fontColor", fontColor);
	txt->setPropertyValue("bgColor",   bgColor);
	//txt->setPropertyValue("x-content", content);
}

IPlayer* txt1;
IPlayer* txt2;
IPlayer* txt3;

void* testPlayer(void* ptr) {
	IScreenManager* dm;
	GingaScreenID screen;
	set<IWindow*>* windows;

	IWindow* w;
	IWindow* ww;
	IWindow* www;

	bool notFalse = true;

	ISurface* s;

	TestContainer* tc = (TestContainer*)ptr;

	string fontUri = SystemCompat::appendGingaFilesPrefix("font/vera.ttf");

	dm      = tc->dm;
	screen  = tc->screen;
	windows = tc->windows;

	w   = dm->createWindow(screen,  10,  10, 100, 100, 3.3);
	ww  = dm->createWindow(screen,  90,  90, 150, 150, 3.2);
	www = dm->createWindow(screen, 220, 220, 400, 300, 3.1);

	w->setCaps(w->getCap("ALPHACHANNEL"));
	w->draw();
	w->show();

	ww->setCaps(w->getCap("ALPHACHANNEL"));
	ww->draw();
	ww->show();

	www->setCaps(w->getCap("ALPHACHANNEL"));
	www->draw();
	www->show();

	ww->raiseToTop();
	w->raiseToTop();

	windows->insert(w);
	windows->insert(ww);
	windows->insert(www);

	txt1->setOutWindow(w->getId());
	s = txt1->getSurface();
	if (s != NULL) {
		s->setParentWindow((void*)w);
		preparePlayer(
				txt1,
				fontUri,
				"file.txt",
				"10",
				"white",
				"black",
				"f=white bg=black");

		txt1->play();
	}

	txt2->setOutWindow(ww->getId());
	s = txt2->getSurface();
	if (s != NULL) {
		s->setParentWindow((void*)ww);
		preparePlayer(
				txt2,
				fontUri,
				"file.txt",
				"14",
				"red",
				"white",
				"f=red bg=white");

		txt2->play();
	}

	txt3->setOutWindow(www->getId());
	s = txt3->getSurface();
	if (s != NULL) {
		s->setParentWindow((void*)www);
		preparePlayer(
				txt3,
				fontUri,
				"file.txt",
				"20",
				"green",
				"black",
				"f=green bg=black");

		txt3->play();
	}

	IPlayer* aux;
	string auxFile = "";

	while (running) {
		SystemCompat::uSleep(2000000);

		aux  = txt1;
		txt1 = txt2;
		txt2 = txt3;
		txt3 = aux;

		if (auxFile == "") {
			auxFile = "file.txt";

			preparePlayer(
					txt1,
					"",
					auxFile,
					"18",
					"white",
					"black",
					"f=white bg=black");

			preparePlayer(
					txt2,
					"",
					auxFile,
					"16",
					"red",
					"white",
					"f=red bg=white");

			preparePlayer(
					txt3,
					"",
					auxFile,
					"14",
					"blue",
					"yellow",
					"f=blue bg=yellow");

		} else {
			auxFile = "";

			txt1->stop();
			txt2->stop();
			txt3->stop();

			txt1->play();
			txt2->play();
			txt3->play();
		}
	}

	return NULL;
}

void* blinkWindowSet(void* ptr) {
	set<IWindow*>* windows;
	set<IWindow*>::iterator i;
	bool showing = false;

	windows = (set<IWindow*>*)ptr;

	while (running) {
		i = windows->begin();
		while (i != windows->end()) {
			if (showing) {
				(*i)->hide();
			} else {
				(*i)->show();
			}
			++i;
		}
		SystemCompat::uSleep(100000);
		showing = !showing;
	}

	i = windows->begin();
	while (i != windows->end()) {
		(*i)->show();
		++i;
	}

	return NULL;
}

int main(int argc, char** argv, char** envp) {
	GingaScreenID screen1, screen2;
	int fakeArgc = 5;
	char* dfbArgv[5];
	char* sdlArgv[5];
	IScreenManager* dm;

	//SystemCompat::setLogTo(SystemCompat::LOG_NULL);

	cout << "gingacc-player test works only with enabled component support.";
	cout << endl;
	exit(0);

	running = true;

	int i;

	pthread_t ptid;
	TestContainer tc1;
	TestContainer tc2;
	bool testAllScreens = false;
	bool blinkWindows   = false;
	bool printScreen    = false;
	set<IWindow*> windows;

	for (i = 1; i < argc; i++) {
		if ((strcmp(argv[i], "--all") == 0)) {
			testAllScreens = true;

		} else if ((strcmp(argv[i], "--blink") == 0)) {
			blinkWindows   = true;

		} else if ((strcmp(argv[i], "--printscreen") == 0)) {
			printScreen    = true;
		}
	}

	if (testAllScreens) {
		dfbArgv[0] = (char*)"testScreen";
		dfbArgv[1] = (char*)"--vsystem";
		dfbArgv[2] = (char*)"dfb";
		dfbArgv[3] = (char*)"--vmode";
		dfbArgv[4] = (char*)"400x300";

		screen1 = dm->createScreen(fakeArgc, dfbArgv);

		sdlArgv[0] = (char*)"testScreen";
		sdlArgv[1] = (char*)"--vsystem";
		sdlArgv[2] = (char*)"sdl";
		sdlArgv[3] = (char*)"--vmode";
		sdlArgv[4] = (char*)"400x300";
		screen2 = dm->createScreen(fakeArgc, sdlArgv);

		tc1.dm      = dm;
		tc1.screen  = screen1;
		tc1.windows = &windows;

		tc2.dm      = dm;
		tc2.screen  = screen2;
		tc2.windows = &windows;

		pthread_create(&ptid, 0, testPlayer, &tc1);
		pthread_create(&ptid, 0, testPlayer, &tc2);

	} else {
		screen1     = dm->createScreen(argc, argv);

		tc1.dm      = dm;
		tc1.screen  = screen1;
		tc1.windows = &windows;

		testPlayer(&tc1);
	}

	if (blinkWindows) {
		pthread_t tid;

		pthread_create(&tid, 0, blinkWindowSet, &windows);

		cout << "gingacc-mb test is blinking windows. ";
		cout << "press enter to stop it";
		cout << endl;
		getchar();
		running = false;

	} else {
		getchar();
		running = false;
	}

	if (printScreen) {
		dm->blitScreen(screen1, "/root/printscreen1.bmp");

		if (testAllScreens) {
			dm->blitScreen(screen2, "/root/printscreen2.bmp");
		}
	}

	cout << "gingacc-player test has shown TextPlayers. ";
	cout << "press enter to automatic release";
	cout << endl;
	getchar();

	if (testAllScreens) {
		dm->clearWidgetPools(screen1);
		dm->releaseScreen(screen1);
		dm->releaseMB(screen1);

		dm->clearWidgetPools(screen2);
		dm->releaseScreen(screen2);
		dm->releaseMB(screen2);

	} else {
		dm->clearWidgetPools(screen1);
		dm->releaseScreen(screen1);
		dm->releaseMB(screen1);
	}

	delete dm;

	cout << "Player test done. press enter to continue" << endl;
	getchar();

	return 0;
}
