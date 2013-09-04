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

	IFontProvider* ttf1;
	IFontProvider* ttf2;
	IFontProvider* ttf3;

	IWindow* win1;
	IWindow* win2;
	IWindow* win3;

	ISurface* ttfSur;

	int x1, y1, w1, h1, z1;
	int x2, y2, w2, h2, z2;
	int x3, y3, w3, h3, z3;

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

	/* FONT PROVIDER */
	win1 = dm->createWindow(screen, x1, y1, w1, h1, z1);
	if (win1 == NULL) {
		cout << "gingacc-mb test can't create window. exiting program...";
		cout << endl;
		exit(1);
	}
	win1->draw();

	ttf1   = dm->createFontProvider(screen, "decker.ttf", 14);
	ttfSur = dm->createSurface(screen);
	ttfSur->setParentWindow(win1);

	win1->show();
	win1->raiseToTop();

	ttfSur->setColor(0xFF, 0xFF, 0xFF, 0xFF);
	ttfSur->setSurfaceFont(ttf1);
	ttfSur->drawString(0, 0, "Test White!");

	/* Inserting all windows in windows set */
	windows->insert(win1);
	windows->insert(win2);
	windows->insert(win3);
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

	SystemCompat::setLogTo(SystemCompat::LOG_NULL);

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

	//TODO: tests

	getchar();
	return 0;
}
