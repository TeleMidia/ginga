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

#include "tuner/Tuner.h"
#include "tuner/ITunerListener.h"
using namespace ::br::pucrio::telemidia::ginga::core::tuning;

#include <sys/types.h>
#include <stdio.h>

#include <iostream>
#include <string>
using namespace std;

class TestTunerListener : public ITunerListener {
	private:
		string url;
		FILE* fd;

	public:
		TestTunerListener(string url) {
			this->url = url;
			fd = fopen(url.c_str(), "w+b");
		}

		void receiveData(char* buff, unsigned int size) {
			if (fd != NULL) {
				fwrite(buff, 1, size, fd);
			}

			delete[] buff;
		}

		void updateChannelStatus(short newStatus, IChannel* channel) {

		}
};

int main(int argc, char** argv, char** envp) {
	string name = "", protocol = "", address = "";
	TestTunerListener* ttl = NULL;
	ITuner* tuner;

	if (argc > 3) {
		if (strcmp(argv[1], "udp") == 0 || strcmp(argv[1], "rtp") == 0 ||
				strcmp(argv[1], "http") == 0) {

			name = "ip";
			protocol.assign(argv[1]);

			if (strcmp(argv[2], "multicast") == 0) {
				protocol = protocol + "_multicast";

			} else if (strcmp(argv[2], "unicast") == 0) {
				protocol = protocol + "_unicast";
			}

		} else if (strcmp(argv[1], "isdbt") == 0 || strcmp(argv[1], "sbtvdt")) {
			name = "sbtvd";
			protocol = "terrestrial";

		} else if (strcmp(argv[1], "fs")) {
			name = "fs";
			protocol = "local";
		}

		address.assign(argv[3]);
	}

	if (argc > 4) {
		ttl = new TestTunerListener(argv[4]);
	}

	tuner = new Tuner(0, name, protocol, address);

	if (ttl != NULL) {
		tuner->setTunerListener(ttl);
	}

	tuner->tune();

	getchar();
	tuner->channelUp();

	getchar();
	tuner->channelUp();

	getchar();

	if (ttl != NULL) {
		tuner->setTunerListener(NULL);
		delete ttl;
	}

	delete tuner;

	clog << "gingacc-tuner test all done. press enter to exit." << endl;
	getchar();

	//TODO: more tests
	return 0;
}
