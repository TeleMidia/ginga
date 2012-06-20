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

#include "player/AnimePlayer.h"
using namespace ::br::pucrio::telemidia::ginga::core::player;

#include "player/PlayersComponentSupport.h"

extern "C" {
#include <stdio.h>
#include <pthread.h>
}

AnimePlayer* a1;
AnimePlayer* a2;
AnimePlayer* a3;

void testPlayer(
		ILocalScreenManager* dm, GingaScreenID screen, set<IWindow*>* windows) {

	IWindow* w;
	IWindow* ww;
	IWindow* www;

	ISurface* s;

	w   = dm->createWindow(screen, 10, 10, 100, 100, 4.0);
	ww  = dm->createWindow(screen, 90, 90, 150, 150, 4.0);
	www = dm->createWindow(screen, 0, 0, 400, 300, 4.0);

	w->setCaps(w->getCap("ALPHACHANNEL"));
	w->draw();
	w->show();
	w->raiseToTop();

	ww->setCaps(w->getCap("ALPHACHANNEL"));
	ww->draw();
	ww->show();
	ww->raiseToTop();

	www->setCaps(w->getCap("ALPHACHANNEL"));
	www->draw();
	www->show();
	www->raiseToTop();

	windows->insert(w);
	windows->insert(ww);
	windows->insert(www);

	vector<string>* mrls, *mrlsAux;

	mrls = new vector<string>;

	mrls->push_back(SystemCompat::appendGingaFilesPrefix("img/roller/b1.png"));
	mrls->push_back(SystemCompat::appendGingaFilesPrefix("img/roller/b2.png"));
	mrls->push_back(SystemCompat::appendGingaFilesPrefix("img/roller/b3.png"));
	mrls->push_back(SystemCompat::appendGingaFilesPrefix("img/roller/b4.png"));
	mrls->push_back(SystemCompat::appendGingaFilesPrefix("img/roller/b5.png"));
	mrls->push_back(SystemCompat::appendGingaFilesPrefix("img/roller/b6.png"));
	mrls->push_back(SystemCompat::appendGingaFilesPrefix("img/roller/b7.png"));
	mrls->push_back(SystemCompat::appendGingaFilesPrefix("img/roller/b8.png"));

	mrlsAux = new vector<string>(*mrls);
	a1 = new AnimePlayer(screen, mrlsAux);
	a1->setVoutWindow(w);

	mrlsAux = new vector<string>(*mrls);
	a2 = new AnimePlayer(screen, mrlsAux);
	a2->setVoutWindow(ww);

	mrlsAux = new vector<string>(*mrls);
	a3 = new AnimePlayer(screen, mrlsAux);
	a3->setVoutWindow(www);

	delete mrls;
	mrls = NULL;

	a1->play();
	a2->play();
	a3->play();
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
	bool blinkWindows = false;
	set<IWindow*> windows;

	for (i = 1; i < argc; i++) {
		if ((strcmp(argv[i], "--all") == 0)) {
			testAllScreens = true;

		} else if ((strcmp(argv[i], "--blink") == 0)) {
			blinkWindows   = true;
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

		testPlayer(dm, screen1, &windows);
		testPlayer(dm, screen2, &windows);

	} else {
		cout << "gingacc-playet testImage" << endl;
		screen1 = dm->createScreen(argc, argv);
		testPlayer(dm, screen1, &windows);
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

	cout << "gingacc-player test has shown AnimePlayers. ";
	cout << "press enter to stop AnimePlayers and to an automatic release";
	cout << endl;
	getchar();

	a1->stop();
	a2->stop();
	a3->stop();

	delete a1;
	delete a2;
	delete a3;

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
