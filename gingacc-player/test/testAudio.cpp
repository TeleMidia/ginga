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

#include "player/IPlayer.h"
using namespace ::br::pucrio::telemidia::ginga::core::player;

#include "player/PlayersComponentSupport.h"

extern "C" {
#include <stdio.h>
}

void testPlayer(ILocalScreenManager* dm, GingaScreenID screen) {
	IWindow* w;
	IWindow* ww;
	IWindow* www;

	ISurface* s;

	IPlayer* aud1;
	IPlayer* aud2;
	IPlayer* aud3;

	w   = dm->createWindow(screen, 10, 10, 100, 100, 32766);
	ww  = dm->createWindow(screen, 90, 90, 150, 150, 32766);
	www = dm->createWindow(screen, 0, 0, 400, 300, 32766);

	w->setCaps(w->getCap("ALPHACHANNEL"));
	w->draw();
	w->show();

	ww->setCaps(w->getCap("ALPHACHANNEL"));
	ww->draw();
	ww->show();

	www->setCaps(w->getCap("ALPHACHANNEL"));
	www->draw();
	www->show();

#if HAVE_COMPSUPPORT
	aud1 = ((PlayerCreator*)(cm->getObject("AVPlayer")))(
			screen, "rock.mp3", false);

	aud2 = ((PlayerCreator*)(cm->getObject("AVPlayer")))(
			screen, "techno.mp3", false);

	aud3 = ((PlayerCreator*)(cm->getObject("AVPlayer")))(
			screen, "choro.mp3", false);
#endif

	aud1->setOutWindow(w->getId());
	s = aud1->getSurface();
	if (s != NULL && s->setParent((void*)w)) {
		w->renderFrom(s);
	}

	aud2->setOutWindow(ww->getId());
	s = aud2->getSurface();
	if (s != NULL && s->setParent((void*)ww)) {
		ww->renderFrom(s);
	}

	aud3->setOutWindow(www->getId());
	s = aud3->getSurface();
	if (s != NULL && s->setParent((void*)www)) {
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
}

int main(int argc, char** argv, char** envp) {
	GingaScreenID screen1;
	ILocalScreenManager* dm;

	setLogToNullDev();

#if HAVE_COMPSUPPORT
	dm = ((LocalScreenManagerCreator*)(cm->getObject("LocalScreenManager")))();

#else
	cout << "gingacc-player test works only with enabled component support.";
	cout << endl;
	exit(0);
#endif

	cout << "gingacc-player testAudio" << endl;
	screen1 = dm->createScreen(argc, argv);
	testPlayer(dm, screen1);
	getchar();

	dm->clearWidgetPools(screen1);

	//TODO: tests
	cout << "Player test done. press enter to continue" << endl;
	getchar();

	return 0;
}
