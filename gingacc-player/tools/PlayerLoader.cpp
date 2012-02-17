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

#if HAVE_COMPSUPPORT
#include "cm/IComponentManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::cm;

#include "mb/ILocalScreenManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::mb;
#endif

#include "system/process/SpawnedProcess.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::process;

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "player/IPlayer.h"
#include "player/IPlayerListener.h"
using namespace ::br::pucrio::telemidia::ginga::core::player;

#include <vector>
#include <iostream>
using namespace std;

class PlayerSpawnedProcess : public SpawnedProcess, public IPlayerListener {
  private:
#if HAVE_COMPSUPPORT
	IComponentManager* cm;
#endif

	ILocalScreenManager* dm;
	IPlayer* player;
	IWindow* window;

  public:
	PlayerSpawnedProcess(string objectName, string wCom, string rCom) :
			SpawnedProcess(objectName, wCom, rCom) {

		dm     = NULL;
		player = NULL;

		clog << "PlayerSpawnedProcess(" << objectName << ") creating GFX";
		clog << " stuffs" << endl;

#if HAVE_COMPSUPPORT
		cm = IComponentManager::getCMInstance();
		dm = ((LocalScreenManagerCreator*)(
				cm->getObject("LocalScreenManager")))();

		dm->createDevice("systemScreen(0)");
#endif

		sendMessage("ready");
		clog << "PlayerSpawnedProcess(" << objectName << ") READY" << endl;
	}

	void updateStatus(
			short code, string parameter, short type, string value) {

		string param = parameter;

		if (param == "") {
			param = "NULL";
		}
		sendMessage(
				"updatestatus," + itos(code) + "," + param + "," + itos(type));
	}

	void messageReceived(string msg) {
		vector<string>* cmds;
		vector<string>* vMsg;
		vector<string>::iterator i;
		int size;
		ISurface* surface;
		bool auxBool;
		string auxStr;

		//SpawnedProcess::messageReceived(msg);

		cmds = split(msg, "::;::");
		i = cmds->begin();
		while (i != cmds->end()) {
			vMsg = split(*i, ",");

			clog << "PlayerLoader::messageReceived '";
			clog << (*vMsg)[0] << "'" << endl;

			if ((*vMsg)[0] == "createplayer") {
#if HAVE_COMPSUPPORT
				player = ((PlayerCreator*)(cm->getObject(objectName)))(
						(*vMsg)[1].c_str(), (*vMsg)[2] == "true");

				player->addListener(this);
#endif

			} else if (player == NULL) {
				clog << "PlayerLoader::messageReceived Warning! ";
				clog << "PLAYER IS NULL" << endl;

			} else if ((*vMsg)[0] == "setoutwindow") {
				player->setOutWindow(GingaWindowID((*vMsg)[1]));

			} else if ((*vMsg)[0] == "setpropertyvalue") {
				size = vMsg->size();
				if ((*vMsg)[1] == "bounds" && size == 6) {
					auxStr = (*vMsg)[2] + "," + (*vMsg)[3] + "," +
							(*vMsg)[4] + "," + (*vMsg)[5];

					clog << "PlayerLoader::messageReceived setproperty '";
					clog << (*vMsg)[1] << "' = '" << auxStr << "'" << endl;

					player->setPropertyValue((*vMsg)[1], auxStr);

				} else if ((*vMsg)[1] == "location" && size == 4) {
					auxStr = (*vMsg)[2] + "," + (*vMsg)[3];
					player->setPropertyValue((*vMsg)[1], auxStr);

				} else if ((*vMsg)[1] == "size" && size == 4) {
					auxStr = (*vMsg)[2] + "," + (*vMsg)[3];
					player->setPropertyValue((*vMsg)[1], auxStr);

				} else if (size == 3) {
					player->setPropertyValue((*vMsg)[1], (*vMsg)[2]);
				}

				//zindex
				//mediatime
				//scope
				//currentScope
				//keyHandler

			} else if ((*vMsg)[0] == "getmediatime") {
				sendMessage("mediatime=" + itos(player->getMediaTime()));

			} else if ((*vMsg)[0] == "setmediatime" && vMsg->size() == 2) {
				player->setMediaTime(stof((*vMsg)[1]));

			} else if ((*vMsg)[0] == "play") {
				player->play();

			} else if ((*vMsg)[0] == "stop") {
				player->stop();
				player->setOutWindow(NULL);

			} else if ((*vMsg)[0] == "pause") {
				player->pause();

			} else if ((*vMsg)[0] == "resume") {
				player->resume();

			} else if ((*vMsg)[0] == "abort") {
				player->abort();
				player->setOutWindow(NULL);

			} else if ((*vMsg)[0] == "getvpts") {
				sendMessage("vpts=" + itos(player->getVPts()));

			} else if ((*vMsg)[0] == "getpropertyvalue" && vMsg->size() == 2) {
				sendMessage(
						"propertyvalue" + (*vMsg)[1] + "=" +
								player->getPropertyValue((*vMsg)[1]));

			} else if ((*vMsg)[0] == "setkeyhandler" && vMsg->size() == 2) {
				auxBool = player->setKeyHandler((*vMsg)[1] == "true");
				auxStr  = "true";
				if (!auxBool) {
					auxStr = "false";
				}
				sendMessage("iskeyhandler=" + auxStr);

			} else if ((*vMsg)[0] == "setscope" && vMsg->size() == 5) {
				player->setScope(
						(*vMsg)[1],
						stof((*vMsg)[2]),
						stof((*vMsg)[3]),
						stof((*vMsg)[4]));

			} else if ((*vMsg)[0] == "setcurrentscope" && vMsg->size() == 2) {
				player->setCurrentScope((*vMsg)[1]);

			} else if ((*vMsg)[0] == "timeshift" && vMsg->size() == 2) {
				player->timeShift((*vMsg)[1]);
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

	setLogToNullDev();

	objName = argv[0];
	wName   = argv[1];
	rName   = argv[2];

	clog << "PlayerLoader::main oName = '" << objName << "'" << endl;
	clog << "PlayerLoader::main wName = '" << wName << "'" << endl;
	clog << "PlayerLoader::main rName = '" << rName << "'" << endl;

	psp = new PlayerSpawnedProcess(objName, wName, rName);
    psp->waitSignal();
	return 0;
}
