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

#include "mb/ILocalScreenManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::mb;

#include "gingalssm/IPresentationEngineManager.h"
using namespace ::br::pucrio::telemidia::ginga::lssm;

#include "system/compat/SystemCompat.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;

#if HAVE_COMPONENTS
#include "cm/IComponentManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::cm;
#endif

#if HAVE_COMPONENTS
	IComponentManager* cm = IComponentManager::getCMInstance();
#endif

extern "C" {
#include <pthread.h>
}

typedef struct {
	 GingaScreenID screen;
	 bool enableGfx;
	 string nclFile;
} ThreadContainer;

void* testLSSM(void* ptr) {
	ThreadContainer* tc;

	IPresentationEngineManager* pem;
	int devClass   = 0;
	int xOffset    = 0;
	int yOffset    = 0;
	int width      = 400;
	int height     = 300;

	tc = (ThreadContainer*)ptr;

#if HAVE_COMPONENTS
	pem = ((PEMCreator*)(cm->getObject("PresentationEngineManager")))(
			devClass,
			xOffset,
			yOffset,
			width,
			height,
			tc->enableGfx,
            false,
			tc->screen);
#endif

	pem->openNclFile(tc->nclFile);
	pem->startPresentation(tc->nclFile, "");

	pem->waitUnlockCondition();
	delete pem;

	return NULL;
}

int main(int argc, char** argv, char** envp) {
	ThreadContainer tc1, tc2;
	GingaScreenID screen1, screen2;
	int fakeArgc = 5;
	char* dfbArgv[5];
	char* sdlArgv[5];
	IScreenManager* dm;

	int i;
	bool testAllScreens = false;
	bool printScreen    = false;

	string nclFile      = "";
	bool enableGfx      = true;
	bool disableUC      = false;

	SystemCompat::setLogTo(SystemCompat::LOG_NULL);

	for (i = 1; i < argc; i++) {
		if ((strcmp(argv[i], "--ncl") == 0) && ((i + 1) < argc)) {
			nclFile.assign(argv[i+1], strlen(argv[i+1]));

		} else if (strcmp(argv[i], "--disable-unload") == 0) {
			disableUC      = true;

		} else if ((strcmp(argv[i], "--all") == 0)) {
			testAllScreens = true;

		} else if ((strcmp(argv[i], "--printscreen") == 0)) {
			printScreen    = true;
		}
	}

	if (nclFile == "") {
		nclFile = "/root/ncl/VivaMaisPratos/main.ncl";
	}

#if HAVE_COMPONENTS
	cm->setUnloadComponents(!disableUC);

	dm = ((LocalScreenManagerCreator*)(cm->getObject("LocalScreenManager")))();
#else
	cout << "ginga-lssm test works only when component manager support is ";
	cout << "enabled" << endl;
	exit(0);
#endif

	if (testAllScreens) {
		pthread_t tid1, tid2;

		dfbArgv[0]    = (char*)"testScreen";
		dfbArgv[1]    = (char*)"--vsystem";
		dfbArgv[2]    = (char*)"dfb";
		dfbArgv[3]    = (char*)"--vmode";
		dfbArgv[4]    = (char*)"400x300";

		screen1       = dm->createScreen(fakeArgc, dfbArgv);

		tc1.screen    = screen1;
		tc1.enableGfx = enableGfx;
		tc1.nclFile   = nclFile;

		pthread_create(&tid1, 0, testLSSM, &tc1);

		sdlArgv[0]    = (char*)"testScreen";
		sdlArgv[1]    = (char*)"--vsystem";
		sdlArgv[2]    = (char*)"sdl";
		sdlArgv[3]    = (char*)"--vmode";
		sdlArgv[4]    = (char*)"400x300";

		screen2       = dm->createScreen(fakeArgc, sdlArgv);

		tc2.screen    = screen2;
		tc2.enableGfx = enableGfx;
		tc2.nclFile   = nclFile;

		pthread_create(&tid2, 0, testLSSM, &tc2);

	} else {
		screen1       = dm->createScreen(argc, argv);
		tc1.screen    = screen1;
		tc1.enableGfx = enableGfx;
		tc1.nclFile   = nclFile;

		testLSSM(&tc1);
	}

	cout << "gingalssm-cpp test is showing NCL presentation";
	if (testAllScreens) {
		cout << "s";
	}
	cout << endl;
	cout << "press enter to exit" << endl;
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

#if HAVE_COMPONENTS
	delete cm;
#endif

	cout << "ginga-lssm test all done!" << endl;
	cout << "Please, press enter to exit!" << endl;
	getchar();

	return 0;
}
