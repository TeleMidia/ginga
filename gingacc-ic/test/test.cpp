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
#else
#include "../include/InteractiveChannelManager.h"
#include "../include/curlic/CurlInteractiveChannel.h"
#endif

#include "../include/IInteractiveChannelManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::ic;

#include <sys/types.h>
#include <fcntl.h>
#include <string>
#include <iostream>
using namespace std;

int main() {
	IInteractiveChannel* ic;
#if HAVE_COMPSUPPORT
	IComponentManager* cm = IComponentManager::getCMInstance();
	IInteractiveChannelManager* icm = ((ICMCreator*)(cm->getObject(
			"InteractiveChannelManager")))();
#else
	IInteractiveChannelManager* icm = InteractiveChannelManager::getInstance();
#endif

	string localPath = "/tmp/gingaTests/";
	string localFile = localPath + "CurlInteractiveChannelTest.xml";
	string remoteUri = "http://apps.club.ncl.org.br/78/main.ncl";

	cout << "gingacc-ic main test: begin" << endl;
	/*mkdir(localPath.c_str(), 0666);

	fd = open(localFile.c_str(), O_CREAT | O_WRONLY | O_LARGEFILE, 0644);
	if (fd > 0) {*/
		if (icm != NULL) {
#if HAVE_COMPSUPPORT
			ic = ((ICCreator*)(cm->getObject("CurlInteractiveChannel")))();
#else
			ic = new CurlInteractiveChannel();
#endif
			if (ic != NULL && ic->hasConnection()) {
				//ic->setTarget(fd);
				ic->setSourceTarget(localFile);
				ic->reserveUrl(remoteUri, NULL);
				ic->performUrl();
			}
		}

	/*} else {
		cout << "gingacc-ic main test: can't create file" << endl;
	}*/

	cout << "gingacc-ic main test: end - check '" << localFile << "'" << endl;
	return 0;
}
