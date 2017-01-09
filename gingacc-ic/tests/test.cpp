/* Copyright (C) 1989-2017 PUC-Rio/Laboratorio TeleMidia

This file is part of Ginga (Ginga-NCL).

Ginga is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

Ginga is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
License for more details.

You should have received a copy of the GNU General Public License
along with Ginga.  If not, see <http://www.gnu.org/licenses/>.  */

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
