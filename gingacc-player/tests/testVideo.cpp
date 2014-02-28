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
#include "player/IPlayerListener.h"
using namespace ::br::pucrio::telemidia::ginga::core::player;

#include "player/PlayersComponentSupport.h"

#include <string>
using namespace std;

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

IPlayer* vid1 = NULL;
IPlayer* vid2 = NULL;
IPlayer* vid3 = NULL;
IPlayer* vid4 = NULL;

class TestPlayerListener : public IPlayerListener {
	private:
		string id;

	public:
		TestPlayerListener(string id) {
			this->id = id;
		};

		~TestPlayerListener() {

		};

		void updateStatus(
				short code, string parameter, short type, string value) {

			cout << "PlayerListener '" << id << "'";
			cout << " received the following notification: ";

			switch (code) {
				case IPlayer::PL_NOTIFY_STOP:
					cout << "video stop";
					break;

				default:
					cout << "code '" << code << "'";
					break;
			}

			cout << endl;
		}
};

void testPlayer(
		ILocalScreenManager* dm, GingaScreenID screen, set<IWindow*>* windows) {

	IWindow* w1;
	IWindow* w2;
	IWindow* w3;
	IWindow* w4;

	string m1, m2, m3, m4;

	ISurface* s;
	bool notFalse=true;

	IPlayerListener* l1 = new TestPlayerListener("L1");
	IPlayerListener* l2 = new TestPlayerListener("L2");
	IPlayerListener* l3 = new TestPlayerListener("L3");
	IPlayerListener* l4 = new TestPlayerListener("L4");

	w1 = dm->createWindow(screen, 10, 10, 100, 100, 3.3);
	w2 = dm->createWindow(screen, 90, 90, 150, 150, 3.2);
	w3 = dm->createWindow(screen, 0, 0, 200, 200, 3.1);
	w4 = dm->createWindow(screen, 0, 0, 400, 300, 3.0);

	w1->setCaps(w1->getCap("ALPHACHANNEL"));
	w1->draw();
	w1->show();

	w2->setCaps(w2->getCap("ALPHACHANNEL"));
	w2->draw();
	w2->show();

	w3->setCaps(w3->getCap("ALPHACHANNEL"));
	w3->draw();
	w3->show();

	w4->setCaps(w4->getCap("ALPHACHANNEL"));
	w4->draw();
	w4->show();

	w3->raiseToTop();
	w2->raiseToTop();
	w1->raiseToTop();

	windows->insert(w1);
	windows->insert(w2);
	windows->insert(w3);
	windows->insert(w4);

#if HAVE_COMPONENTS
	m1 = updateFileUri("corrego1.mp4");
	vid1 = ((PlayerCreator*)(cm->getObject("AVPlayer")))(
			screen, m1.c_str(), &notFalse);

	m2 = updateFileUri("corrego1.mp4");
	vid2 = ((PlayerCreator*)(cm->getObject("AVPlayer")))(
			screen, m2.c_str(), &notFalse);

	m3 = updateFileUri("corrego1.mp4");
	vid3 = ((PlayerCreator*)(cm->getObject("AVPlayer")))(
			screen, m3.c_str(), &notFalse);

	vid4 = ((PlayerCreator*)(cm->getObject("AVPlayer")))(
			screen, "/root/workspaces/NCL/Garrincha/media/animGar.mp4", &notFalse);
#endif

	vid1->addListener(l1);
	vid1->setOutWindow(w1->getId());
	s = vid1->getSurface();
	if (s != NULL) {
		s->setParentWindow((void*)w1);
		vid1->play();
	}

	vid2->addListener(l2);
	vid2->setOutWindow(w2->getId());
	s = vid2->getSurface();
	if (s != NULL) {
		s->setParentWindow((void*)w2);
		vid2->play();
	}

	vid3->addListener(l3);
	vid3->setOutWindow(w3->getId());
	s = vid3->getSurface();
	if (s != NULL) {
		s->setParentWindow((void*)w3);
		vid3->play();
	}

	vid4->addListener(l4);
	vid4->setOutWindow(w4->getId());
	s = vid4->getSurface();
	if (s != NULL) {
		s->setParentWindow((void*)w4);
		vid4->play();
	}
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

	SystemCompat::setLogTo(SystemCompat::LOG_NULL);

#if HAVE_COMPONENTS
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

	if (printScreen) {
		dm->blitScreen(screen1, "/root/printscreen1.bmp");

		if (testAllScreens) {
			dm->blitScreen(screen2, "/root/printscreen2.bmp");
		}
	}

	i = 0;
	while (i < 10) {
		if (vid1 != NULL) {
			cout << "Video 1 media time: '" << vid1->getMediaTime() << "'";
			cout << endl;
		}

		if (vid2 != NULL) {
			cout << "Video 2 media time: '" << vid2->getMediaTime() << "'";
			cout << endl;
		}

		if (vid3 != NULL) {
			cout << "Video 3 media time: '" << vid3->getMediaTime() << "'";
			cout << endl;
		}

		if (vid4 != NULL) {
			cout << "Video 4 media time: '" << vid4->getMediaTime() << "'";
			cout << endl;
		}

		SystemCompat::uSleep(1000000);
		++i;
	}

	cout << "gingacc-player test has shown AVPlayers. ";
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
