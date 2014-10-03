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
#else
#include "mb/LocalScreenManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::mb;
#endif

#include "system/compat/SystemCompat.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;

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

/***********************************************************
 * Testing input events.                                   *
 * You can see which event was created when you hit a key  *
 * You must set your focus on the blank screen and see the *
 * output in your terminal                                 *
 * In order to set the screen size, enter the following as *
 * parameter: ./simpleTestInput --vmode 800x600            *
 * To exit, hit enter in your terminal                     *
 ***********************************************************/

int main(int argc, char** argv) {
	GingaScreenID screen;
	IInputManager* im;
	IScreenManager* dm;
	TestInputEventListener* l;
	int fakeArgc = 5;
	char* sdlArgv[5];

	SystemCompat::setLogTo(SystemCompat::LOG_FILE);

#if HAVE_COMPONENTS
	IComponentManager* cm = IComponentManager::getCMInstance();

	dm = ((LocalScreenManagerCreator*)(
			cm->getObject("LocalScreenManager")))();

#else
	dm = ScreenManagerFactory::getInstance();
#endif

	screen = dm->createScreen(argc, argv);
	im     = dm->getInputManager(screen);
	l      = new TestInputEventListener(screen);
	im->addInputEventListener(l, NULL); //NULL == I want to receive all events

	cout << "gingacc-mb testing input events. ";
	cout << "press enter to automatic release";
	cout << endl;
	getchar();

	im->release();
	delete l;

	dm->clearWidgetPools(screen);
	dm->releaseScreen(screen);
	dm->releaseMB(screen);

	delete dm;
	cout << "gingacc-mb test all done. ";
	cout << "press enter to exit";
	cout << endl;

	getchar();
	return 0;
}
