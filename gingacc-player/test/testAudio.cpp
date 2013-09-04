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

IPlayer* aud1;
IPlayer* aud2;
IPlayer* aud3;
IPlayer* aud4;

void testPlayer(ILocalScreenManager* dm, GingaScreenID screen) {
	IWindow* w;
	IWindow* ww;
	IWindow* www;
	IWindow* wwww;

	ISurface* s;

	w    = dm->createWindow(screen, 10, 10, 100, 100, 4.0);
	ww   = dm->createWindow(screen, 90, 90, 150, 150, 4.0);
	www  = dm->createWindow(screen, 0, 0, 400, 300, 4.0);
	wwww = dm->createWindow(screen, 0, 0, 10, 10, 4.0);

	w->setCaps(w->getCap("ALPHACHANNEL"));
	w->draw();
	w->show();

	ww->setCaps(w->getCap("ALPHACHANNEL"));
	ww->draw();
	ww->show();

	www->setCaps(w->getCap("ALPHACHANNEL"));
	www->draw();
	www->show();

	wwww->setCaps(w->getCap("ALPHACHANNEL"));
	wwww->draw();
	wwww->show();

#if HAVE_COMPSUPPORT
	aud1 = ((PlayerCreator*)(cm->getObject("AVPlayer")))(
			screen, "rock.mp3", false);

	aud2 = ((PlayerCreator*)(cm->getObject("AVPlayer")))(
			screen, "techno.mp3", false);

	aud3 = ((PlayerCreator*)(cm->getObject("AVPlayer")))(
			screen, "cartoon.mp3", false);

	aud4 = ((PlayerCreator*)(cm->getObject("AVPlayer")))(
			screen, "choro.mp3", false);
#endif

	aud1->setOutWindow(w->getId());
	s = aud1->getSurface();
	if (s != NULL && s->setParentWindow((void*)w)) {
		w->renderFrom(s);
	}

	aud2->setOutWindow(ww->getId());
	s = aud2->getSurface();
	if (s != NULL && s->setParentWindow((void*)ww)) {
		ww->renderFrom(s);
	}

	aud3->setOutWindow(www->getId());
	s = aud3->getSurface();
	if (s != NULL && s->setParentWindow((void*)www)) {
		www->renderFrom(s);
	}

	aud1->play();
	cout << "gingacc-player test is playing audio1. press enter to ";
	cout << "play audio 2 simultaneously" << endl;
	getchar();
	aud2->play();
	cout << "gingacc-player test is playing audio1 and audio2. press enter to ";
	cout << "play audio 3 simultaneously" << endl;
	getchar();
	aud3->play();
	cout << "gingacc-player test is playing audio1 and audio2. press enter to ";
	cout << "play audio 4 simultaneously" << endl;
	getchar();
	aud4->play();
}

int main(int argc, char** argv, char** envp) {
	GingaScreenID screen1;
	ILocalScreenManager* dm;

	SystemCompat::setLogTo(SystemCompat::LOG_NULL);


#if HAVE_COMPSUPPORT
	dm = ((LocalScreenManagerCreator*)(cm->getObject("LocalScreenManager")))();

#else
	cout << "gingacc-player test works only with enabled component support.";
	cout << endl;
	exit(0);
#endif

	int i;
	bool testAllScreens = false;
	bool printScreen    = false;

	for (i = 1; i < argc; i++) {
		if ((strcmp(argv[i], "--all") == 0)) {
			testAllScreens = true;

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

	cout << "gingacc-player testAudio" << endl;
	screen1 = dm->createScreen(argc, argv);
	testPlayer(dm, screen1);

	if (printScreen) {
		dm->blitScreen(screen1, "/root/printscreen1.bmp");
	}

	i = 0;
	while (i < 5) {
		if (aud1 != NULL) {
			cout << "Audio 1 media time: '" << aud1->getMediaTime() << "'";
			cout << endl;
		}

		if (aud2 != NULL) {
			cout << "Audio 2 media time: '" << aud2->getMediaTime() << "'";
			cout << endl;
		}

		if (aud3 != NULL) {
			cout << "Audio 3 media time: '" << aud3->getMediaTime() << "'";
			cout << endl;
		}

		if (aud4 != NULL) {
			cout << "Audio 4 media time: '" << aud4->getMediaTime() << "'";
			cout << endl;
		}

		SystemCompat::uSleep(1000000);
		++i;
	}

	cout << "I'm tired showing media time! Press enter to automatic release";
	cout << endl;
	getchar();

	dm->clearWidgetPools(screen1);

	//TODO: tests
	cout << "Player test done. press enter to continue" << endl;
	getchar();

	return 0;
}
