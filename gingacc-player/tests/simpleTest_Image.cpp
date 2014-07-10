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

#include "mb/LocalScreenManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::mb;

#include "player/ImagePlayer.h"
using namespace ::br::pucrio::telemidia::ginga::core::player;

#include "player/PlayersComponentSupport.h"

extern "C" {
#include <stdio.h>
}

void testPlayer(ILocalScreenManager* dm, GingaScreenID screen) {
	IWindow* w;
	bool notFalse = true;
	ISurface* s;
	IPlayer* img1;

	w = dm->createWindow(screen, 10, 10, 100, 100, 3.3);

	w->setCaps(w->getCap("ALPHACHANNEL"));
	w->draw();
	w->show();
	w->raiseToTop();

	img1 = new ImagePlayer(screen, "C:/Temp/NCLs/internas/monos/i1.png");

	img1->setOutWindow(w->getId());
	s = img1->getSurface();
	if (s != NULL && s->setParentWindow((void*)w)) {
		w->renderFrom(s);
	}
}

bool running = false;

int main(int argc, char** argv, char** envp) {
	GingaScreenID screen1;
	int fakeArgc = 5;
	char* dfbArgv[5];
	char* sdlArgv[5];
	ILocalScreenManager* dm;

	SystemCompat::setLogTo(SystemCompat::LOG_NULL);

	dm = LocalScreenManager::getInstance();

	sdlArgv[0] = (char*)"testScreen";
	sdlArgv[1] = (char*)"--vsystem";
	sdlArgv[2] = (char*)"sdl";
	sdlArgv[3] = (char*)"--vmode";
	sdlArgv[4] = (char*)"400x300";
	screen1 = dm->createScreen(fakeArgc, sdlArgv);

	testPlayer(dm, screen1);

	cout << "gingacc-player test has shown ImagePlayers. ";
	cout << "press enter to automatic release";
	cout << endl;
	getchar();

	dm->clearWidgetPools(screen1);
	dm->releaseScreen(screen1);
	dm->releaseMB(screen1);

	delete dm;

	cout << "Player test done. press enter to continue" << endl;
	getchar();

	return 0;
}
