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

#include "player/NTPPlayer.h"

#include <iostream>
using namespace std;

#if !HAVE_COMPONENTS
#include "mb/LocalScreenManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::mb;
#endif

int main(int argc, char *argv[]) {
	NTPPlayer* ntpPlayer;

	GingaScreenID screen;
	ILocalScreenManager* dm;

	SystemCompat::setLogTo(SystemCompat::LOG_NULL);

#if HAVE_COMPONENTS
	dm = ((LocalScreenManagerCreator*)(cm->getObject("LocalScreenManager")))();

#else
	dm = LocalScreenManager::getInstance();
#endif

	screen = dm->createScreen(argc, argv);

	ntpPlayer = new NTPPlayer(screen, "200.160.7.186");
	if (ntpPlayer->updateTime()) {
		string nclTime = "2014:08:22:21:30:00.000";
		cout << ntpPlayer->getTimeString() << endl;
		cout << ntpPlayer->elapsedTime(nclTime) << endl;
		cout << "Process done." << endl;

	} else {
		cout << "An error occurred during the process!" << endl;
	}

	delete ntpPlayer;

	cout << "gingacc-player test has shown ImagePlayers. ";
	cout << "press enter to automatic release";
	cout << endl;
	getchar();

	dm->clearWidgetPools(screen);
	dm->releaseScreen(screen);
	dm->releaseMB(screen);

	delete dm;

	cout << "Player test done. press enter to continue" << endl;
	getchar();

	return 0;
}
