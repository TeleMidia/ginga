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

#include "../config.h"

#include "../include/ImagePlayer.h"
using namespace ::br::pucrio::telemidia::ginga::core::player;

#if HAVE_MULTIPROCESS
#include "system/process/Process.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::process;

#include "../include/PlayerProcess.h"
#endif

#include "system/io/ILocalDeviceManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::system;

#include "../include/PlayersComponentSupport.h"

int main(int argc, char** argv, char** envp) {
	IWindow* w;
	IWindow* ww;
	IWindow* www;
	ISurface* s;
	ILocalDeviceManager* dm;
	IPlayer* player;
	IPlayer* img;
	IPlayer* avp;

#if HAVE_MULTIPROCESS
	Process* process;
	IPlayer* pprocessA;
	IPlayer* pprocessB;
	IPlayer* pprocessC;
#endif

	setLogToNullDev();

#if HAVE_COMPSUPPORT
	IComponentManager* cm = IComponentManager::getCMInstance();
	dm = ((LocalDeviceManagerCreator*)(cm->getObject(
			"LocalDeviceManager")))();

#else
	dm = LocalDeviceManager::getInstance();
#endif

	dm->createDevice("systemScreen(0)");
#if HAVE_COMPSUPPORT
	w = ((WindowCreator*)(cm->getObject("Window")))(-1, 10, 10, 100, 100);
	ww = ((WindowCreator*)(cm->getObject("Window")))(-1, 90, 90, 150, 150);
	www = ((WindowCreator*)(cm->getObject("Window")))(-1, 120, 120, 400, 400);
#else
	w  = new DFBWindow(10, 10, 100, 100);
	ww = new DFBWindow(90, 90, 400, 400);
#endif

	w->setCaps(w->getCap("ALPHACHANNEL"));
	w->draw();
	w->show();

	ww->setCaps(w->getCap("ALPHACHANNEL"));
	ww->draw();
	ww->show();

	www->setCaps(w->getCap("ALPHACHANNEL"));
	www->draw();
	www->show();

	if (argc > 1 && strcmp(argv[1], "img") == 0) {
		player = new Player("teste");
		img = new ImagePlayer("/root/img1.png");
		img->setOutWindow(w->getId());
		w->validate();

		cout << "Player test has shown image. press enter to continue" << endl;
		getchar();
		dm->clearWidgetPools();

	} else if (argc > 1 && strcmp(argv[1], "avpipe") == 0) {
#if HAVE_COMPSUPPORT
		avp = ((PlayerCreator*)(cm->getObject("AVPlayer")))(
				"sbtvd-ts://dvr0.ts", true);
#else
		avp = (IPlayer*)(new AVPlayer("sbtvd-ts://dvr0.ts", true));
#endif

		avp->setOutWindow(w->getId());
		getchar();
		dm->clearWidgetPools();

	} else if (argc > 1 && strcmp(argv[1], "process") == 0) {
#if HAVE_MULTIPROCESS
		process = new Process(NULL);
		process->setProcessInfo(
				"/usr/local/etc/ginga/tools/loaders/players",
				"ImagePlayer");

		process->run();
		process->checkCom();

		process->sendMsg("createplayer,/root/img1.png,true::;::");
		process->sendMsg("setoutwindow," + itos(w->getId()) + "::;::");
		process->sendMsg("getwindowid::;::");
		process->sendMsg("play::;::");
		process->sendMsg("show::;::");

		getchar();
		process->forceKill();
#endif

	} else if (argc > 1 && strcmp(argv[1], "pprocess") == 0) {
#if HAVE_MULTIPROCESS
		cout << "CREATING PLAYER PROCESS A " << endl;

#if HAVE_COMPSUPPORT
		pprocessA = ((PlayerCreator*)(cm->getObject("PlayerProcess")))(
				"ImagePlayer", true);
#else
		pprocessA = new PlayerProcess("ImagePlayer");
#endif

		pprocessA->setMrl("/root/img1.png", true);
		pprocessA->setOutWindow(w->getId());
		pprocessA->play();
		getchar();

#if HAVE_COMPSUPPORT
		pprocessB = ((PlayerCreator*)(cm->getObject("PlayerProcess")))(
				"AVPlayer", true);
#else
		pprocessB = new PlayerProcess("AVPlayer");
#endif

		pprocessB->setMrl("/root/vid1.mpg", true);
		pprocessB->setOutWindow(ww->getId());
		pprocessB->play();
		getchar();

#if HAVE_COMPSUPPORT
		pprocessC = ((PlayerCreator*)(cm->getObject("PlayerProcess")))(
				"LinksPlayer", true);
#else
		pprocessC = new PlayerProcess("LinksPlayer");
#endif

		pprocessC->setMrl("/root/pag1.html", true);
		pprocessC->setOutWindow(www->getId());
		pprocessC->setPropertyValue("bounds", "120,120,400,400");
		pprocessC->play();
		getchar();

		pprocessC->stop();
		getchar();
		delete pprocessC;

		pprocessB->stop();
		getchar();
		delete pprocessB;
		getchar();
		pprocessA->stop();
		getchar();
		delete pprocessA;
#endif
	}

	//TODO: tests
	cout << "Player test done. press enter to continue" << endl;
	getchar();

	return 0;
}
