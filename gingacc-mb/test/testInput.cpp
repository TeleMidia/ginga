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

#include "mb/ILocalScreenManager.h"
#include "mb/IInputManager.h"
#include "mb/interface/CodeMap.h"
using namespace ::br::pucrio::telemidia::ginga::core::mb;

extern "C" {
#include "string.h"
}

#include <iostream>
using namespace std;

class TestInputEventListener : public IInputEventListener {
private:
	GingaScreenID myScreen;

public:
	TestInputEventListener(GingaScreenID screenId) {
		myScreen = screenId;
	}

	~TestInputEventListener() {

	}

	bool userEventReceived(IInputEvent* ev) {
		int value = ev->getKeyCode(myScreen);

		cout << "SCREEN(" << myScreen << ") input listener has RECEIVED '";
		cout << value << "' = '";
		cout << CodeMap::getInstance()->getValue(value) << "'";
		cout << endl;
		std::flush(cout);

		return true;
	}
};

int main(int argc, char** argv) {
	GingaScreenID screen1, screen2;
	IInputManager* im1;
	IInputManager* im2;
	ILocalScreenManager* dm;
	TestInputEventListener* l1;
	TestInputEventListener* l2;

	int fakeArgc = 5;
	char* dfbArgv[5];
	char* sdlArgv[5];

	setLogToFile();

#if HAVE_COMPSUPPORT
	IComponentManager* cm = IComponentManager::getCMInstance();

	dm = ((LocalScreenManagerCreator*)(
			cm->getObject("LocalScreenManager")))();

#else
	cout << "gingacc-mb works only with enabled component support" << endl;
	exit(0);
#endif

	int i;
	bool testAllScreens = false;
	for (i = 1; i < argc; i++) {
		if ((strcmp(argv[i], "--all") == 0)) {
			testAllScreens = true;
		}
	}

	if (testAllScreens) {
		dfbArgv[0] = (char*)"testInput";
		dfbArgv[1] = (char*)"--vsystem";
		dfbArgv[2] = (char*)"dfb";
		dfbArgv[3] = (char*)"--vmode";
		dfbArgv[4] = (char*)"400x300";

		screen1 = dm->createScreen(fakeArgc, dfbArgv);
		im1     = dm->getInputManager(screen1);
		l1      = new TestInputEventListener(screen1);

		im1->addInputEventListener(l1, NULL);

		sdlArgv[0] = (char*)"testInput";
		sdlArgv[1] = (char*)"--vsystem";
		sdlArgv[2] = (char*)"sdl";
		sdlArgv[3] = (char*)"--vmode";
		sdlArgv[4] = (char*)"400x300";

		screen2 = dm->createScreen(fakeArgc, sdlArgv);
		im2     = dm->getInputManager(screen2);
		l2      = new TestInputEventListener(screen2);

		im2->addInputEventListener(l2, NULL);

	} else {
		screen1 = dm->createScreen(argc, argv);
		im1     = dm->getInputManager(screen1);
		l1      = new TestInputEventListener(screen1);

		im1->addInputEventListener(l1, NULL);
	}

	cout << "gingacc-mb testing input events. ";
	cout << "press enter to automatic release";
	cout << endl;
	getchar();

	im1->release();
	delete l1;

	dm->clearWidgetPools(screen1);
	dm->releaseScreen(screen1);
	dm->releaseMB(screen1);

	if (testAllScreens) {
		im2->release();
		delete l2;

		dm->clearWidgetPools(screen2);
		dm->releaseScreen(screen2);
		dm->releaseMB(screen2);
	}

	delete dm;
	cout << "gingacc-mb test all done. ";
	cout << "press enter to exit";
	cout << endl;

	getchar();
	return 0;
}
