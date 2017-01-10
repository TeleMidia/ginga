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

#include "util/Color.h"
#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "LocalScreenManager.h"
#include "IContinuousMediaProvider.h"
using namespace ::br::pucrio::telemidia::ginga::core::mb;

#include "CodeMap.h"
#include "IDeviceScreen.h"

extern "C" {
#include "string.h"
#include "pthread.h"
}

#include <iostream>
using namespace std;

IContinuousMediaProvider* vid1 = NULL;
IContinuousMediaProvider* vid2 = NULL;
IContinuousMediaProvider* vid3 = NULL;
IContinuousMediaProvider* vid4 = NULL;
IContinuousMediaProvider* vid5 = NULL;

bool singleVideo = false;

void testVideo(
		IScreenManager* dm,
		GingaScreenID screen,
		set<IWindow*>* windows) {

	IWindow* win1;
	IWindow* win2;
	IWindow* win3;
	IWindow* win4;
	IWindow* win5;

	ISurface* s1;
	ISurface* s2;
	ISurface* s3;
	ISurface* s4;
	ISurface* s5;

	int x1, y1, w1, h1, z1;
	int x2, y2, w2, h2, z2;
	int x3, y3, w3, h3, z3;
	int x4, y4, w4, h4, z4;
	int x5, y5, w5, h5, z5;

	bool notFalse = true;

	x1 = 25;
	y1 = 25;
	w1 = 100;
	h1 = 100;
	z1 = 1;

	x2 = 100;
	y2 = 100;
	w2 = 100;
	h2 = 100;
	z2 = 2;

	x3 = 180;
	y3 = 70;
	w3 = 100;
	h3 = 100;
	z3 = 3;

	x4 = 260;
	y4 = 25;
	w4 = 100;
	h4 = 100;
	z4 = 4;

	x5 = 35;
	y5 = 90;
	w5 = 100;
	h5 = 100;
	z5 = 5;

	/* VIDEO PROVIDER 1 */
	win1 = dm->createWindow(screen, x1, y1, w1, h1, z1);
	if (win1 == NULL) {
		cout << "gingacc-mb test can't create window. exiting program...";
		cout << endl;
		exit(1);
	}
	win1->draw();

	s1   = dm->createSurface(screen);
	vid1 = dm->createContinuousMediaProvider(
			screen,
			"/root/ncl/rio-itu/mainVideo/rio_1280x720.mp4",
			&notFalse,
			false);

	s1->setParentWindow(win1);
	win1->show();
	win1->raiseToTop();

	cout << "gingacc-mb test video 1 for screen '" << screen << "' has '";
	cout << vid1->getTotalMediaTime() << "' as its total media time." << endl;

	vid1->setSoundLevel(0.1);
	vid1->playOver(s1, true, NULL);

	if (singleVideo) {
		windows->insert(win1);
		return;
	}

	/* VIDEO PROVIDER 2 */
	win2 = dm->createWindow(screen, x2, y2, w2, h2, z2);
	if (win2 == NULL) {
		cout << "gingacc-mb test can't create window. exiting program...";
		cout << endl;
		exit(1);
	}
	win2->draw();

	s2   = dm->createSurface(screen);
	vid2 = dm->createContinuousMediaProvider(
			screen,
			"/root/ncl/VivaMaisPratos/video/vivamais-ws.mp4",
			&notFalse,
			false);

	vid2->setSoundLevel(1.0);
	s2->setParentWindow(win2);
	win2->show();
	win2->raiseToTop();

	cout << "gingacc-mb test video 2 for screen '" << screen << "' has '";
	cout << vid2->getTotalMediaTime() << "' as its total media time." << endl;

	vid2->playOver(s2, true, NULL);


	/* VIDEO PROVIDER 3 */
	win3 = dm->createWindow(screen, x3, y3, w3, h3, z3);
	if (win3 == NULL) {
		cout << "gingacc-mb test can't create window. exiting program...";
		cout << endl;
		exit(1);
	}
	win3->draw();

	s3   = dm->createSurface(screen);
	vid3 = dm->createContinuousMediaProvider(
			screen,
			"/root/workspaces/NCL/devel_tests/media/vid/matrix.mpg",
			&notFalse,
			false);

	s3->setParentWindow(win3);
	win3->show();
	win3->raiseToTop();

	cout << "gingacc-mb test video 3 for screen '" << screen << "' has '";
	cout << vid3->getTotalMediaTime() << "' as its total media time." << endl;

	vid3->setSoundLevel(1.0);
	vid3->playOver(s3, true, NULL);


	/* VIDEO PROVIDER 4 */
	win4 = dm->createWindow(screen, x4, y4, w4, h4, z4);
	if (win4 == NULL) {
		cout << "gingacc-mb test can't create window. exiting program...";
		cout << endl;
		exit(1);
	}
	win4->draw();

	s4   = dm->createSurface(screen);
	vid4 = dm->createContinuousMediaProvider(
			screen,
			"/root/workspaces/NCL/devel_tests/media/vid/pilha.mpg",
			&notFalse,
			false);

	s4->setParentWindow(win4);
	win4->show();
	win4->raiseToTop();

	cout << "gingacc-mb test video 4 for screen '" << screen << "' has '";
	cout << vid4->getTotalMediaTime() << "' as its total media time." << endl;

	vid4->setSoundLevel(1.0);
	vid4->playOver(s4, true, NULL);


	/* VIDEO PROVIDER 5 */
	win5 = dm->createWindow(screen, x5, y5, w5, h5, z5);
	if (win5 == NULL) {
		cout << "gingacc-mb test can't create window. exiting program...";
		cout << endl;
		exit(1);
	}
	win5->draw();

	s5   = dm->createSurface(screen);
	vid5 = dm->createContinuousMediaProvider(
			screen,
			"corrego1.mp4",
			&notFalse,
			false);

	s5->setParentWindow(win5);
	win5->show();

	cout << "gingacc-mb test video 5 for screen '" << screen << "' has '";
	cout << vid5->getTotalMediaTime() << "' as its total media time." << endl;

	vid5->setSoundLevel(1.0);
	vid5->playOver(s5, true, NULL);


	/* Inserting all windows in windows set */
	windows->insert(win1);
	windows->insert(win2);
	windows->insert(win3);
	windows->insert(win4);
	windows->insert(win5);
}

bool running = false;

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

int main(int argc, char** argv) {
	set<IWindow*> windows;
	GingaScreenID screen1, screen2;
	int fakeArgc = 5;
	char* dfbArgv[5];
	char* sdlArgv[5];
	IScreenManager* dm;

	SystemCompat::setLogTo(SystemCompat::LOG_STDO);

	cout << "gingacc-mb test works only with enabled component support";
	cout << endl;
	exit(0);

	cout << "gingacc-mb test has created the screen manager. ";
	cout << endl;

	int i;
	bool testAllScreens = false;
	bool blinkWindows   = false;
	bool printScreen    = false;
	for (i = 1; i < argc; i++) {
		if ((strcmp(argv[i], "--all") == 0)) {
			testAllScreens = true;

		} else if ((strcmp(argv[i], "--blink") == 0)) {
			blinkWindows   = true;

		} else if ((strcmp(argv[i], "--printscreen") == 0)) {
			printScreen    = true;

		} else if ((strcmp(argv[i], "--single-video") == 0)) {
			singleVideo    = true;
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

		testVideo(dm, screen1, &windows);
		testVideo(dm, screen2, &windows);

	} else {
		screen1 = dm->createScreen(argc, argv);
		cout << "gingacc-mb test has created screen '" << screen1;
		cout << "'. calling providers";
		cout << endl;
		testVideo(dm, screen1, &windows);
	}

	if (blinkWindows) {
		pthread_t tid;

		running = true;

		pthread_create(&tid, 0, blinkWindowSet, &windows);

		cout << "gingacc-mb test is blinking windows. ";
		cout << "press enter to stop it";
		cout << endl;
		getchar();
		running = false;
	}

	if (printScreen) {
		dm->blitScreen(screen1, "/root/printscreen1.bmp");

		if (testAllScreens) {
			dm->blitScreen(screen2, "/root/printscreen2.bmp");
		}
	}

	cout << "gingacc-mb test has shown providers. ";
	cout << "press enter to stop all of them";
	cout << endl;
	getchar();

	if (vid1 != NULL) {
		vid1->stop();
	}

	if (vid2 != NULL) {
		vid2->stop();
	}

	if (vid3 != NULL) {
		vid3->stop();
	}

	if (vid4 != NULL) {
		vid4->stop();
	}

	if (vid5 != NULL) {
		vid5->stop();
	}

	cout << "gingacc-mb test has shown providers. ";
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
	cout << "gingacc-mb test all done. ";
	cout << "press enter to exit";
	cout << endl;

	getchar();
	return 0;
}
