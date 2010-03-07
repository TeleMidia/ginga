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

#if HAVE_COMPSUPPORT
#include "cm/IComponentManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::cm;

#include "system/io/ILocalDeviceManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::io;
#endif

#include "system/process/SpawnedProcess.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::process;

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "../include/IPlayer.h"
using namespace ::br::pucrio::telemidia::ginga::core::player;

#include <vector>
#include <iostream>
using namespace std;

class PlayerSpawnedProcess : public SpawnedProcess {
  private:
#if HAVE_COMPSUPPORT
	IComponentManager* cm;
#endif

	ILocalDeviceManager* dm;
	IPlayer* player;
	IWindow* window;

  public:
	PlayerSpawnedProcess(string objectName, string wCom, string rCom) :
			SpawnedProcess(objectName, wCom, rCom) {

		dm     = NULL;
		player = NULL;

#if HAVE_COMPSUPPORT
		cm = IComponentManager::getCMInstance();
		dm = ((LocalDeviceManagerCreator*)(cm->getObject(
				"LocalDeviceManager")))();

		dm->createDevice("systemScreen(0)");
#endif

		sendMessage("ready");
	}

	void messageReceived(string msg) {
		vector<string>* cmds;
		vector<string>* vMsg;
		vector<string>::iterator i;
		int size;
		ISurface* surface;

		SpawnedProcess::messageReceived(msg);

		cmds = split(msg, "::;::");
		i = cmds->begin();
		while (i != cmds->end()) {
			vMsg = split(*i, ",");
			if ((*vMsg)[0] == "createplayer") {
#if HAVE_COMPSUPPORT
				player = ((PlayerCreator*)(cm->getObject(objectName)))(
						(*vMsg)[1].c_str(), (*vMsg)[2] == "true");
#endif

			} else if ((*vMsg)[0] == "createwindow") {
#if HAVE_COMPSUPPORT
				window = ((WindowCreator*)(cm->getObject("Window")))(
						stof((*vMsg)[1]),
						stof((*vMsg)[2]),
						stof((*vMsg)[3]),
						stof((*vMsg)[4]));

				window->setCaps(window->getCap("ALPHACHANNEL"));
				window->draw();

				window->renderFrom(player->getSurface());
#endif

			} else if ((*vMsg)[0] == "setpropertyvalue") {
				size = vMsg->size();
				if ((*vMsg)[1] == "bounds" && size >= 5) {
					if (size == 5) {
						window->setBounds(
								stof((*vMsg)[2]),
								stof((*vMsg)[3]),
								stof((*vMsg)[4]),
								stof((*vMsg)[5]));

					} else if (size == 6) {
						//TODO: animation
						window->setBounds(
								stof((*vMsg)[2]),
								stof((*vMsg)[3]),
								stof((*vMsg)[4]),
								stof((*vMsg)[5]));
					}

				} else if ((*vMsg)[1] == "location" && size >= 3) {
					if (size == 3) {
						window->moveTo(
								stof((*vMsg)[2]),
								stof((*vMsg)[3]));

					} else if (size == 4) {
						//TODO: animation
						window->moveTo(
								stof((*vMsg)[2]),
								stof((*vMsg)[3]));
					}

				} else if ((*vMsg)[1] == "size" && size >= 3) {
					if (size == 3) {
						window->resize(
								stof((*vMsg)[2]),
								stof((*vMsg)[3]));

					} else if (size == 4) {
						//TODO: animation
						window->resize(
								stof((*vMsg)[2]),
								stof((*vMsg)[3]));
					}

				} else if (size == 3) {
					player->setPropertyValue((*vMsg)[1], (*vMsg)[2]);

				} else if (size == 4) {
					//TODO: animation
					player->setPropertyValue(
							(*vMsg)[1], (*vMsg)[2], stof((*vMsg)[3]));
				}

				//zindex
				//mediatime
				//scope
				//currentScope
				//keyHandler

			} else if ((*vMsg)[0] == "getmediatime") {
				sendMessage("mediatime=" + itos(player->getMediaTime()));

			} else if ((*vMsg)[0] == "getwindowid") {
				if (window != NULL) {
					sendMessage("windowid=" + itos(window->getId()));
				}

			} else if ((*vMsg)[0] == "play") {
				player->play();

			} else if ((*vMsg)[0] == "stop") {
				player->stop();

			} else if ((*vMsg)[0] == "show") {
				window->show();

			} else if ((*vMsg)[0] == "hide") {
				window->hide();

			} else if ((*vMsg)[0] == "pause") {
				player->pause();

			} else if ((*vMsg)[0] == "resume") {
				player->resume();

			} else if ((*vMsg)[0] == "abort") {
				player->abort();

			} else if ((*vMsg)[0] == "getvpts") {
				sendMessage(itos(player->getVPts()));
			}

			delete vMsg;
			++i;
		}
		delete cmds;
	}
};

int main(int argc, char *argv[], char* envp[]) {
	string rName, wName, objName;
	PlayerSpawnedProcess* psp;

	objName = argv[0];
	wName   = argv[1];
	rName   = argv[2];

	/*cout << "PlayerLoader::main oName = '" << objName << "'" << endl;
	cout << "PlayerLoader::main wName = '" << wName << "'" << endl;
	cout << "PlayerLoader::main rName = '" << rName << "'" << endl;*/

	setLogToNullDev();

	psp = new PlayerSpawnedProcess(objName, wName, rName);
    psp->waitSignal();
	return 0;
}
