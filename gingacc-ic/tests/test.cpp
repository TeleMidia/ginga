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

#include "ic/InteractiveChannelManager.h"
#include "ic/curlic/CurlInteractiveChannel.h"

#include "system/compat/SystemCompat.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;

#include "ic/IInteractiveChannelManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::ic;

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <string>
#include <iostream>
using namespace std;

int main(int argc, char** argv) {
	IInteractiveChannel* ic;
	IInteractiveChannelManager* icm;

	FILE* fd;
	string localPath = "/tmp/gingaTests/";
	string localFile = localPath + "CurlInteractiveChannelTest.xml";
	string remoteUri = "http://apps.club.ncl.org.br/78/main.ncl";

	clog << "gingacc-ic main test: begin" << endl;
	SystemCompat::setLogTo(SystemCompat::LOG_FILE);
	SystemCompat::makeDir(localPath.c_str(), 0666);

	icm = InteractiveChannelManager::getInstance();

	if (argc == 3 && strcmp(argv[1], "--curl") == 0) {
		if (strcmp(argv[2], "fd") == 0) {
			fd = fopen(localFile.c_str(), "w+b");
		}

		if (icm != NULL) {
#if HAVE_CURL
			ic = new CurlInteractiveChannel();
			if (ic != NULL && ic->hasConnection()) {
				if (fd > 0) {
					ic->setTarget(fd);
				}
				ic->setSourceTarget(localFile);
				ic->reserveUrl(remoteUri, NULL);
				ic->performUrl();
			}

			delete ic;
#endif
		}

	} else if (argc == 3 && strcmp(argv[1], "--ccrtp") == 0) {
	}

#if HAVE_CCRTP
	delete listener;
#endif
	clog << "gingacc-ic main test: end - check '" << localFile << "'" << endl;
	return 0;
}
