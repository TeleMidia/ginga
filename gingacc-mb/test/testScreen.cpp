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
}

#include <iostream>
using namespace std;

void testScreen(ILocalScreenManager* dm, GingaScreenID screen) {
	IImageProvider* img;
	IFontProvider* ttf;
	IContinuousMediaProvider* vid;
	IWindow* win1;
	IWindow* win2;
	IWindow* win3;
	ISurface* s;
	ISurface* vidSur;
	ISurface* ttfSur;
	int x1, y1, w1, h1;
	int x2, y2, w2, h2;
	int x3, y3, w3, h3;

	x1 = 25;
	y1 = 25;
	w1 = 100;
	h1 = 100;

	x2 = 100;
	y2 = 100;
	w2 = 100;
	h2 = 100;

	x3 = 200;
	y3 = 25;
	w3 = 100;
	h3 = 100;

	/* IMAGE PROVIDER */
	win1 = dm->createWindow(screen, x1, y1, w1, h1);
	if (win1 == NULL) {
		cout << "gingacc-mb test can't create window. exiting program...";
		cout << endl;
		exit(1);
	}
	win1->draw();

	img = dm->createImageProvider(screen, "1.png");

	s = dm->createSurface(screen);
	s->setParent(win1);
	img->playOver(s);
	win1->show();

	/* FONT PROVIDER */
	win3 = dm->createWindow(screen, x3, y3, w3, h3);
	if (win3 == NULL) {
		cout << "gingacc-mb test can't create window. exiting program...";
		cout << endl;
		exit(1);
	}
	win3->draw();

	ttf = dm->createFontProvider(screen, "decker.ttf", 14);

	ttfSur = dm->createSurface(screen);
	ttfSur->setParent(win3);
	ttfSur->setColor(0xFF, 0xFF, 0xFF, 0xFF);

	win3->show();

	ttf->playOver(ttfSur, "Testing font provider!", 10, 100);

	/* VIDEO PROVIDER */
	win2 = dm->createWindow(screen, x2, y2, w2, h2);
	if (win2 == NULL) {
		cout << "gingacc-mb test can't create window. exiting program...";
		cout << endl;
		exit(1);
	}
	win2->draw();

	vidSur = dm->createSurface(screen);
	vid    = dm->createContinuousMediaProvider(
			screen, "corrego1.mp4", true, false);

	vidSur->setParent(win2);
	win2->show();

	cout << "gingacc-mb test video for screen '" << screen << "' has '";
	cout << vid->getTotalMediaTime() << "' as its total media time." << endl;

	vid->playOver(vidSur, true, NULL);

	/*
	 * One last test.
	 * Lets do this in a more ambitious way, deleting some stuffs right away.
	 */
	ISurface* iSurLT       = NULL;
	IWindow* iWinLT        = NULL;
	IImageProvider* iImgLT = NULL;

	iImgLT = dm->createImageProvider(screen, "1.png");
	iWinLT = dm->createWindow(screen, x2 + x3, y2 + y3, w3, h3);

	iWinLT->draw();
	iWinLT->show();

	iSurLT = dm->createSurfaceFrom(screen, NULL);
	iImgLT->playOver(iSurLT);
	iWinLT->renderFrom(iSurLT);

	delete iSurLT;
	iSurLT = NULL;
}

int main(int argc, char** argv) {
	GingaScreenID screen1, screen2;
	int fakeArgc = 5;
	char* dfbArgv[5];
	char* sdlArgv[5];
	ILocalScreenManager* dm;

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

	if (argc == 1) {
		dfbArgv[0] = (char*)"testScreen";
		dfbArgv[1] = (char*)"--system";
		dfbArgv[2] = (char*)"dfb";
		dfbArgv[3] = (char*)"--mode";
		dfbArgv[4] = (char*)"400x300";

		screen1 = dm->createScreen(fakeArgc, dfbArgv);

		sdlArgv[0] = (char*)"testScreen";
		sdlArgv[1] = (char*)"--system";
		sdlArgv[2] = (char*)"sdl";
		sdlArgv[3] = (char*)"--mode";
		sdlArgv[4] = (char*)"400x300";
		screen2 = dm->createScreen(fakeArgc, sdlArgv);

		testScreen(dm, screen1);
		testScreen(dm, screen2);

	} else {
		screen1 = dm->createScreen(argc, argv);
		cout << "gingacc-mb test has created screen '" << screen1;
		cout << "'. calling providers";
		cout << endl;
		testScreen(dm, screen1);
	}

	cout << "gingacc-mb test has shown providers. ";
	cout << "press enter to automatic release";
	cout << endl;
	getchar();

	if (argc == 1) {
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
