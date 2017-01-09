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

#include "tuner/Tuner.h"
#include "tsparser/Demuxer.h"
#include "tsparser/PipeFilter.h"
#include "dataprocessing/DataProcessor.h"

#include "system/time/Stc.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::time;

#include "tuner/ITuner.h"
using namespace ::br::pucrio::telemidia::ginga::core::tuning;

#include "tsparser/IDemuxer.h"
#include "tsparser/ITSFilter.h"
using namespace ::br::pucrio::telemidia::ginga::core::tsparser;

#include "dataprocessing/DataProcessor.h"
using namespace ::br::pucrio::telemidia::ginga::core::dataprocessing;

#include "dataprocessing/ncl/NCLSectionProcessor.h"
#include "dataprocessing/ncl/Metadata.h"
using namespace ::br::pucrio::telemidia::ginga::core::dataprocessing::ncl;

#include "dataprocessing/dsmcc/npt/DSMCCSectionPayload.h"
#include "dataprocessing/dsmcc/npt/NPTProcessor.h"
using namespace ::br::pucrio::telemidia::ginga::core::dataprocessing::dsmcc::npt;

#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char** argv) {
	vector<StreamData*>* streams;
	vector<StreamData*>::iterator i;
	vector<string>* files;
	vector<string>::iterator j;
	StreamData* streamData;
	INCLSectionProcessor* nsp;
	string baseUri, uri;
	NPTProcessor* nptProcessor;
	DSMCCSectionPayload* dsmccSection;
	TimeBaseClock* clk;

	ITuner* tuner;
	IDemuxer* demuxer;
	IDataProcessor* dataProcessor;
	FILE* fd;
	int rval;
	int buffSize = 188;
	char buf[buffSize];

	SystemCompat::setLogTo(SystemCompat::LOG_NULL);

	tuner         = new Tuner(0);
	demuxer       = new Demuxer(tuner);
	dataProcessor = new DataProcessor();

	nptProcessor  = new NPTProcessor(NULL);
	dsmccSection  = new DSMCCSectionPayload(NULL, 0);

	dataProcessor->setDemuxer(demuxer);

	if (argc == 1) {
		//encoding NCL sections
		baseUri = "/root/ncl/Tests/";
		clog << "gingacc-dataprocessing test: encoding streams (creating ";
		clog << baseUri << "metadata.xml)" << endl;

		nsp   = new NCLSectionProcessor();

		files = new vector<string>;
		files->push_back("01/test1.ncl");
		files->push_back("media/img/ship.gif");
		files->push_back("media/img/gray.gif");
		files->push_back("/root/ncl/Tests/media/img/green.gif");
		files->push_back("media/img/yellow.gif");

		streams = nsp->createNCLSections(
				"0x01.0x01", "nclApp", baseUri, files, NULL);

		//TODO: test map-event
		delete nsp;

		//removing files
		j = files->begin();
		while (j != files->end()) {
			uri = *j;
			if (uri.substr(0, 1) != SystemCompat::getIUriD()) {
				uri = baseUri + uri;
			}

			remove(uri.c_str());
			++j;
		}

		//decoding NCL sections
		clog << "gingacc-dataprocessing test: decoding streams print results: ";
		clog << endl;

		nsp = new NCLSectionProcessor();
		i = streams->begin();
		while (i != streams->end()) {
			streamData = *i;
			nsp->process(streamData->stream, streamData->size);
			++i;
		}

		nsp->mount();

		delete files;

	} else if (argc == 2 && strcmp(argv[1], "--time") == 0) {
		clog << "gingacc-dataprocessing test (argc = 2)";
		clog << endl;
		clk = new TimeBaseClock();
		clk->setReference(0);
		while (true) {
			clog << "stc in seconds: '" << clk->getBaseToSecond() << "'";
			clog << endl;
			SystemCompat::uSleep(1000000);
		}

	} else if (argc == 3 && strcmp(argv[1], "--decode-oc") == 0) {
		clog << "gingacc-dataprocessing test (argc = 3) decode-oc inside '";
		clog << argv[2] << "'";
		clog << endl;

		fd = fopen(argv[2], "rb");
		if (fd == NULL) {
			clog << "gingacc-dataprocessing test (argc = 3) Error! ";
			clog << " can't find '" << argv[2] << "'";
			clog << endl;
		}

		dataProcessor->createStreamTypeSectionFilter(STREAM_TYPE_DSMCC_TYPE_B);
		demuxer->setDestination(STREAM_TYPE_DSMCC_TYPE_B);

		do {
			rval = fread(buf, 1, buffSize, fd);
			if (rval != buffSize) {
				clog << "gingacc-dataprocessing(" << __LINE__ << ")";
				clog << " Warning! Can't read '" << buffSize << "'";
				clog << " bytes" << endl;
			}
			demuxer->receiveData(buf, rval);
		} while (rval > 0);

		clog << "gingacc-dataprocessing test decode-oc all done! press enter";
		clog << " to quit" << endl;
		getchar();
		delete dataProcessor;
		delete demuxer;
		delete tuner;

		fclose(fd);

	} else if (argc == 3 && strcmp(argv[1], "--ait") == 0) {
		clog << "gingacc-dataprocessing test (argc = 3) decode-oc inside '";
		clog << argv[2] << "'";
		clog << endl;

		fd = fopen(argv[2], "rb");
		if (fd < 0) {
			clog << "gingacc-dataprocessing test (argc = 3) Error! ";
			clog << " can't find '" << argv[2] << "'";
			clog << endl;
		}

		dataProcessor->createStreamTypeSectionFilter(
				STREAM_TYPE_PRIVATE_SECTION);

		do {
			rval = fread(buf, 1, buffSize, fd);
			if (rval != buffSize) {
				clog << "gingacc-dataprocessing(" << __LINE__ << ")";
				clog << " Warning! Can't read '" << buffSize << "'";
				clog << " bytes" << endl;
			}
			demuxer->receiveData(buf, rval);
		} while (rval > 0);

		clog << "gingacc-dataprocessing test decode-oc all done! press enter";
		clog << " to quit" << endl;
		getchar();
		delete dataProcessor;
		delete demuxer;
		delete tuner;

		fclose(fd);
	}

	clog << "gingacc-dataprocessing(" << __LINE__ << ")";
	clog << " all done!" << endl;
	//TODO: tests
	return 0;
}
