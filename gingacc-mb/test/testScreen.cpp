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

#if HAVE_COMPSUPPORT
#include "cm/IComponentManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::cm;
#endif

#include "mb/LocalScreenManager.h"
#include "mb/interface/IContinuousMediaProvider.h"
#include "mb/interface/IImageProvider.h"
#include "mb/interface/IFontProvider.h"
using namespace ::br::pucrio::telemidia::ginga::core::mb;

#include "mb/interface/CodeMap.h"
#include "mb/interface/IDeviceScreen.h"

extern "C" {
#include "string.h"
#include "pthread.h"
}

#include <iostream>
using namespace std;

void testScreen(
		ILocalScreenManager* dm,
		GingaScreenID screen,
		set<IWindow*>* windows) {

	IImageProvider* img;
	IFontProvider* ttf;
	IContinuousMediaProvider* vid;
	IWindow* win1;
	IWindow* win2;
	IWindow* win2T;
	IWindow* win3;
	ISurface* s;
	ISurface* vidSur;
	ISurface* ttfSur;
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
	y4 = 45;
	w4 = 100;
	h4 = 100;
	z4 = 4;

	x5 = 340;
	y5 = 15;
	w5 = 100;
	h5 = 100;
	z5 = 5;

	/* IMAGE PROVIDER */
	win1 = dm->createWindow(screen, x1, y1, w1, h1, z1);
	if (win1 == NULL) {
		cout << "gingacc-mb test can't create window. exiting program...";
		cout << endl;
		exit(1);
	}
	win1->draw();
	win1->renderImgFile("2.png");

	win1->show();
	win1->raiseToTop();

	/* VIDEO PROVIDER */
	win2 = dm->createWindow(screen, x2, y2, w2, h2, z2);
	if (win2 == NULL) {
		cout << "gingacc-mb test can't create window. exiting program...";
		cout << endl;
		exit(1);
	}
	win2->draw();

	vidSur = dm->createSurface(screen);
	vid    = dm->createContinuousMediaProvider(
			screen, "corrego1.mp4", &notFalse, false);

	vidSur->setParentWindow(win2);
	win2->show();
	win2->raiseToTop();

	cout << "gingacc-mb test video for screen '" << screen << "' has '";
	cout << vid->getTotalMediaTime() << "' as its total media time." << endl;

	vid->playOver(vidSur, true, NULL);

	/* FONT PROVIDER */
	/* In front of video */
	win2T = dm->createWindow(screen, x2, y2, w2, h2, z2);
	if (win2T == NULL) {
		cout << "gingacc-mb test can't create window. exiting program...";
		cout << endl;
		exit(1);
	}
	win2T->draw();

	ttf = dm->createFontProvider(screen, "decker.ttf", 14);

	ttfSur = dm->createSurface(screen);
	ttfSur->setParentWindow(win2T);

	win2T->show();
	win2T->raiseToTop();

	ttfSur->setColor(0xFF, 0xFF, 0xFF, 0xFF);
	ttf->playOver(ttfSur, "Test White!", 10, 5);

	ttfSur->setColor(0xFF, 0xFF, 0x00, 0xFF);
	ttf->playOver(ttfSur, "Test Yellow!", 10, 15);

	ttfSur->setColor(0x00, 0x00, 0x00, 0xFF);
	ttf->playOver(ttfSur, "Test Black!", 10, 25);

	ttfSur->setColor(0x00, 0xFF, 0x00, 0xFF);
	ttf->playOver(ttfSur, "Test Green!", 10, 35);

	ttfSur->setColor(0x00, 0x00, 0xFF, 0xFF);
	ttf->playOver(ttfSur, "Test Blue!", 10, 45);

	ttfSur->setColor(0xFF, 0x00, 0x00, 0xFF);
	ttf->playOver(ttfSur, "Test Red!", 10, 55);

	delete ttfSur;

	/* right side of video, but this time with background color */
	win3 = dm->createWindow(screen, x3, y3, w3, h3, z3);
	if (win3 == NULL) {
		cout << "gingacc-mb test can't create window. exiting program...";
		cout << endl;
		exit(1);
	}
	win3->draw();

	ttfSur = dm->createSurface(screen);
	ttfSur->setParentWindow(win3);

	win3->show();
	win3->raiseToTop();
	win3->setBgColor(255, 255, 0, 0);

	ttfSur->setColor(0xFF, 0xFF, 0xFF, 0xFF);
	ttfSur->setSurfaceFont(ttf);
	ttfSur->drawString(-1, 10, "Test White!");

	ttfSur->setColor(0xFF, 0xFF, 0x00, 0xFF);
	ttf->playOver(ttfSur, "Test Yellow!", 10, 15);

	ttfSur->setColor(0x00, 0x00, 0x00, 0xFF);
	ttf->playOver(ttfSur, "Test Black!", 10, 25);

	ttfSur->setColor(0x00, 0xFF, 0x00, 0xFF);
	ttf->playOver(ttfSur, "Test Green!", 10, 35);

	ttfSur->setColor(0x00, 0x00, 0xFF, 0xFF);
	ttf->playOver(ttfSur, "Test Blue!", 10, 45);

	ttfSur->setColor(0xFF, 0x00, 0x00, 0xFF);
	ttf->playOver(ttfSur, "Test Red!", 10, 55);

	delete ttfSur;
	ttfSur = NULL;

	/*
	 * Two more tests: createSurfaceFrom and createRenderedSurfaceFromImageFile
	 */

	/* create surface from */
	ISurface* iSurLT       = NULL;
	IWindow* iWinLT        = NULL;
	IImageProvider* iImgLT = NULL;

	iImgLT = dm->createImageProvider(screen, "1.png");
	iWinLT = dm->createWindow(screen, x4, y4, w4, h4, z4);

	iWinLT->draw();
	iWinLT->show();
	iWinLT->raiseToTop();

	iSurLT = dm->createSurfaceFrom(screen, NULL);
	iImgLT->playOver(iSurLT);
	iWinLT->renderFrom(iSurLT);

	delete iSurLT;
	iSurLT = NULL;

	/* createRenderedSurfaceFromImageFile */
	ISurface* s2;
	IWindow* bg;

	bg = dm->createWindow(screen, x5, y5, w5, h5, z5);

	s2 = dm->createRenderedSurfaceFromImageFile(
		screen, (char*)("/root/workspaces/NCL/devel_tests/84/bg.gif"));

	bg->setCaps(bg->getCap("ALPHACHANNEL"));
	bg->draw();
	bg->show();
	bg->raiseToTop();

	bg->renderFrom(s2);

	delete s2;
	s2 = NULL;


	/* Inserting all windows in windows set */
	windows->insert(win1);
	windows->insert(win2);
	windows->insert(win3);
	windows->insert(iWinLT);
	windows->insert(bg);
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
	ILocalScreenManager* dm;

	setLogToNullDev();

#if HAVE_COMPSUPPORT
	IComponentManager* cm = IComponentManager::getCMInstance();

	dm = ((LocalScreenManagerCreator*)(cm->getObject("LocalScreenManager")))();
#else
	cout << "gingacc-mb test works only with enabled component support";
	cout << endl;
	exit(0);
#endif

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

		testScreen(dm, screen1, &windows);
		testScreen(dm, screen2, &windows);

	} else {
		screen1 = dm->createScreen(argc, argv);
		cout << "gingacc-mb test has created screen '" << screen1;
		cout << "'. calling providers";
		cout << endl;
		testScreen(dm, screen1, &windows);
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
