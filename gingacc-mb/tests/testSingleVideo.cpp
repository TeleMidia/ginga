/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware
Ginga (Ginga-NCL).

Direitos Autorais Reservados (c) 1989-2007 PUC-Rio/Laboratorio TeleMidia

Este programa eh software livre; voce pode redistribui-lo e/ou modificah-lo sob
os termos da Licenca Publica Geral GNU versao 2 conforme publicada pela Free
Software Foundation.

Este programa eh distribuido na expectativa de que seja util, porem, SEM
NENHUMA GARANTIA; nem mesmo a garantia implicita de COMERCIABILIDADE OU
ADEQUACAO A UMA FINALIDADE ESPECIFICA. Consulte a Licenca Publica Geral do
GNU versao 2 para mais detalhes.

Voce deve ter recebido uma copia da Licenca Publica Geral do GNU versao 2 junto
com este programa; se nao, escreva para a Free Software Foundation, Inc., no
endereco 59 Temple Street, Suite 330, Boston, MA 02111-1307 USA.

Para maiores informacoes:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
******************************************************************************
This file is part of the declarative environment of middleware Ginga (Ginga-NCL)

Copyright: 1989-2007 PUC-RIO/LABORATORIO TELEMIDIA, All Rights Reserved.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License version 2 as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License version 2 for more
details.

You should have received a copy of the GNU General Public License version 2
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

For further information contact:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
*******************************************************************************/

#include "util/Color.h"
#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "config.h"

#include "mb/LocalScreenManager.h"
#include "mb/interface/IContinuousMediaProvider.h"
using namespace ::br::pucrio::telemidia::ginga::core::mb;

#include "mb/interface/CodeMap.h"
#include "mb/interface/IDeviceScreen.h"

extern "C" {
#include "string.h"
#include "pthread.h"
}

#include <string>
#include <iostream>
using namespace std;

string videoUri = "";
IContinuousMediaProvider* vid1 = NULL;

void testVideo(
		IScreenManager* dm,
		GingaScreenID screen,
		set<IWindow*>* windows) {

	IWindow* win1;

	ISurface* s1;

	int x1, y1, w1, h1, z1;

	bool notFalse = true;

	x1 = 25;
	y1 = 25;
	w1 = 100;
	h1 = 100;
	z1 = 1;

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
			videoUri.c_str(),
			&notFalse,
			false);

	s1->setParentWindow(win1);
	win1->show();
	win1->raiseToTop();

	cout << "gingacc-mb test video 1 for screen '" << screen << "' has '";
	cout << vid1->getTotalMediaTime() << "' as its total media time." << endl;

	vid1->setSoundLevel(0.1);
	vid1->playOver(s1, true, NULL);

	windows->insert(win1);
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

	SystemCompat::setLogTo(SystemCompat::LOG_FILE);

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
		if ((strcmp(argv[i], "--enable-log") == 0) && ((i + 1) < argc)) {
			if (strcmp(argv[i + 1], "stdout") == 0) {
				SystemCompat::setLogTo(SystemCompat::LOG_STDO);

			} else if (strcmp(argv[i + 1], "file") == 0) {
				SystemCompat::setLogTo(SystemCompat::LOG_FILE);
			}

		} else if ((strcmp(argv[i], "--all") == 0)) {
			testAllScreens = true;

		} else if ((strcmp(argv[i], "--blink") == 0)) {
			blinkWindows   = true;

		} else if ((strcmp(argv[i], "--printscreen") == 0)) {
			printScreen    = true;

		} else if ((strcmp(argv[i], "--src") == 0) && ((i + 1) < argc)) {
			videoUri.assign(argv[i + 1]);
		}
	}

	if (videoUri == "") {
		cout << "Please specify the video URI with --src parameter" << endl;
		exit(0);
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
		dm->blitScreen(screen1, "/tmp/printscreen1.bmp");

		if (testAllScreens) {
			dm->blitScreen(screen2, "/tmp/printscreen2.bmp");
		}
	}

	cout << "gingacc-mb test has shown providers. ";
	cout << "press enter to stop all of them";
	cout << endl;
	getchar();

	if (vid1 != NULL) {
		vid1->stop();
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

	/*if (argc == 3) {
		if (strcmp(argv[1], "xine") == 0) {
			cout << "gingacc-mb test will test xine video provider" << endl;
#if HAVE_XINEPROVIDER
			xineProvider = new XineVideoProvider(argv[2]);
			s->setParent(w);
			xineProvider->playOver(s, true);
			xineProvider->feedBuffers();
			xineProvider->checkVideoResizeEvent(s);
			xineProvider->setSoundLevel(100);
			xineProvider->playOver(s, true);
			win->show();
#endif
		} else if (strcmp(argv[1], "ffmpeg") == 0) {
			cout << "gingacc-mb test will test ffmpeg video provider" << endl;
#if HAVE_FFMPEGPROVIDER
			ffmpegProvider = new FFmpegVideoProvider(argv[2]);
			s->setParent(w);
			win->show();
			ffmpegProvider->feedBuffers();
			ffmpegProvider->checkVideoResizeEvent(s);
			ffmpegProvider->setSoundLevel(100);
			ffmpegProvider->playOver(s, true);
#endif
		}
	}*/

	//TODO: tests

	getchar();
	return 0;
}
