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

void testAwesomium(
		ILocalScreenManager* dm,
		GingaScreenID screen,
		set<IWindow*>* windows,
		bool testStop) {

	IWindow* w;
	IWindow* ww;

	ISurface* s;

	IPlayer* awe1;
	IPlayer* awe2;

	string mrl = "www.telemidia.puc-rio.br";

	if (testStop) {
		mrl = "www.cwi.nl";
	}

	w   = dm->createWindow(screen,  10,  10, 490, 580, 3.2);
	ww  = dm->createWindow(screen,  20, 210, 470, 280, 3.1);

	w->setCaps(w->getCap("ALPHACHANNEL"));
	w->draw();
	w->show();

	ww->setCaps(w->getCap("ALPHACHANNEL"));
	ww->draw();
	ww->show();

	ww->raiseToTop();
	w->raiseToTop();

	windows->insert(w);
	windows->insert(ww);

#if HAVE_COMPSUPPORT
	void* sym;

	sym  = cm->getObject("AwesomiumPlayer");
	if (sym == NULL) {
		cout << "testAwesomium Error! Can't load AwesomiumPlayer.";
		if (cm->isAvailable("AwesomiumPlayer")) {
			cout << " (availability was checked)";
		}
		cout << endl;
		return;
	}

	awe1 = ((PlayerCreator*)(sym))(screen, mrl.c_str(), true);

	awe2 = ((PlayerCreator*)(cm->getObject("AwesomiumPlayer")))(
			screen, "www.telemidia.puc-rio.br", true);
#endif

	awe1->setOutWindow(w->getId());
	s = awe1->getSurface();
	if (s != NULL) {
		s->setParentWindow((void*)w);
		w->renderFrom(s);
		awe1->play();
		awe1->setKeyHandler(true); //necessary if we want to test input events

		if (testStop) {
			SystemCompat::uSleep(2000000);
			awe1->stop();
			delete awe1;
			w->hide();

			delete w;

			ww->hide();
			delete ww;
		}

		cout << "testAwesomium has stopped Awesomium Player" << endl;
	}

	/*awe2->setOutWindow(ww->getId());
	s = awe2->getSurface();
	if (s != NULL) {
		s->setParent((void*)ww);
		ww->renderFrom(s);
		awe2->play();
	}*/
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

int main(int argc, char** argv, char** envp) {
	GingaScreenID screen1, screen2;
	int fakeArgc = 5;
	char* dfbArgv[5];
	char* sdlArgv[5];
	ILocalScreenManager* dm;

	setLogToNullDev();

#if HAVE_COMPSUPPORT
	dm = ((LocalScreenManagerCreator*)(cm->getObject("LocalScreenManager")))();

#else
	cout << "gingacc-player test works only with enabled component support.";
	cout << endl;
	exit(0);
#endif

	int i;
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
		dfbArgv[4] = (char*)"400x600";

		screen1 = dm->createScreen(fakeArgc, dfbArgv);

		sdlArgv[0] = (char*)"testScreen";
		sdlArgv[1] = (char*)"--vsystem";
		sdlArgv[2] = (char*)"sdl";
		sdlArgv[3] = (char*)"--vmode";
		sdlArgv[4] = (char*)"400x600";
		screen2 = dm->createScreen(fakeArgc, sdlArgv);

		testAwesomium(dm, screen1, &windows, false);
		testAwesomium(dm, screen2, &windows, false);

	} else {
		screen1 = dm->createScreen(argc, argv);
		testAwesomium(dm, screen1, &windows, true);
		testAwesomium(dm, screen1, &windows, false);
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

	cout << "gingacc-player test has shown AwesomiumPlayers. ";
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
