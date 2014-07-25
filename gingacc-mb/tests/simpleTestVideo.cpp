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

#if HAVE_COMPONENTS
#include "cm/IComponentManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::cm;
#endif

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

int main(int argc, char** argv) {
	GingaScreenID screen;
	ILocalScreenManager* dm;
	string videoUri = "";

	//SETTING LOG INFO TO A FILE
	SystemCompat::setLogTo(SystemCompat::LOG_FILE);

/* GETTING DEVICE SCREEN */
#if HAVE_COMPONENTS
	IComponentManager* cm = IComponentManager::getCMInstance();

	dm = ((LocalScreenManagerCreator*)(cm->getObject("LocalScreenManager")))();
#else
	dm = LocalScreenManager::getInstance();
#endif

	cout << "gingacc-mb test has created the screen manager. ";
	cout << endl;

	/* PROCESSING MAIN PARAMETERS*/
	for (int i = 1; i < argc; i++) {
		if ((strcmp(argv[i], "--enable-log") == 0) && ((i + 1) < argc)) {
			if (strcmp(argv[i + 1], "stdout") == 0) {
				SystemCompat::setLogTo(SystemCompat::LOG_STDO);

			} else if (strcmp(argv[i + 1], "file") == 0) {
				SystemCompat::setLogTo(SystemCompat::LOG_STDO);
			}

		} else if ((strcmp(argv[i], "--src") == 0) && ((i + 1) < argc)) {
			videoUri.assign(argv[i + 1]);
		}
	}

	if (videoUri == "") {
		cout << "Please specify the video URI with --src parameter" << endl;
		exit(0);
	}

	/* CREATING MAIN SCREEN ACCORDING TO MAIN PARAMETERS */
	screen = dm->createScreen(argc, argv);
	cout << "gingacc-mb test has created screen '" << screen;
	cout << "'. creating video provider";
	cout << endl;

	/***** VIDEO *****/
	IContinuousMediaProvider* vid = NULL;
	IWindow* win;
	ISurface* s;
	int x, y, w, h, z;
	bool notFalse = true;

	x = 25;
	y = 25;
	w = 100;
	h = 100;
	z = 1;

	win = dm->createWindow(screen, x, y, w, h, z);
	if (win == NULL) {
		cout << "gingacc-mb test can't create window. exiting program...";
		cout << endl;
		exit(1);
	}
	win->draw();

	s   = dm->createSurface(screen);
	vid = dm->createContinuousMediaProvider(
			screen,
			videoUri.c_str(),
			&notFalse,
			false);

	s->setParentWindow(win);
	win->setBgColor(255, 0, 0, 0);
	win->show();
	win->raiseToTop();
	win->validate();

	cout << "gingacc-mb test video for screen '" << screen << "' has '";
	cout << vid->getTotalMediaTime() << "' as its total media time." << endl;

	vid->setSoundLevel(0.1);
	vid->playOver(s, true, NULL);

	cout << "gingacc-mb video is playing. if you see a red square, we have ";
	cout << "a problem with video rendering. ";
	cout << " check if at least the audio is ok.";
	cout << "press enter to stop and release";
	cout << endl;
	getchar();

	if (vid != NULL) {
		vid->stop();
	}

	dm->clearWidgetPools(screen);
	dm->releaseScreen(screen);
	dm->releaseMB(screen);

	delete dm;
	cout << "gingacc-mb test all done. ";
	cout << "press enter to exit";
	cout << endl;

	//TODO: more tests

	getchar();
	return 0;
}
