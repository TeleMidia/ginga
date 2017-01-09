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

#include "tuner/ITuner.h"
using namespace ::br::pucrio::telemidia::ginga::core::tuning;

#include "tsparser/PipeFilter.h"
#include "tsparser/Demuxer.h"
using namespace ::br::pucrio::telemidia::ginga::core::tsparser;

#include "tsparser/AIT.h"
using namespace ::br::pucrio::telemidia::ginga::core::tsparser::si;

#include "tuner/Tuner.h"

#include <sys/types.h>
#include <stdio.h>

int main(int argc, char** argv, char** envp) {
	IAIT* ait             = new AIT();
	IDemuxer* demuxer     = NULL;
	ITSFilter* pipeFilter = NULL;
	ITuner* tuner         = NULL;
	FILE* fdPipe          = NULL;
	FILE* fd              = NULL;
	int ret;
	string pipeName, fileName;
	char buffer[188];

	pipeName = "dtv_channel.ts";
	fileName = "/tmp/TSPARSERTEST.ts";

	tuner = new Tuner(0);
	demuxer = new Demuxer(tuner);

	((ITuner*)tuner)->tune();
	((IDemuxer*)demuxer)->waitProgramInformation();

	pipeFilter = new PipeFilter(0);
	((IDemuxer*)demuxer)->addPesFilter(PFT_DEFAULTTS, pipeFilter);

	if (argc == 2 && strcmp(argv[1], "save-pipe") == 0) {
		fdPipe = fopen(pipeName.c_str(), "rb");
		fd = fopen(fileName.c_str(), "w+b");

		while (true) {
			ret = fread(buffer, 1, ITSPacket::TS_PACKET_SIZE, fdPipe);
			if (ret == ITSPacket::TS_PACKET_SIZE) {
				fwrite(buffer, 1, ITSPacket::TS_PACKET_SIZE, fd);

			} else {
				break;
			}
		}
	}
	//TODO: tests

	clog << "gingacc-tsparser test all done. press enter to exit" << endl;
	getchar();
	return 0;
}
