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
