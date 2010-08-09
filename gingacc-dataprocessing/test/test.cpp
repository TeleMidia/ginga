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
#include "../../gingacc-tuner/include/Tuner.h"
#include "../../gingacc-tsparser/include/Demuxer.h"
#include "../../gingacc-tsparser/include/PipeFilter.h"
#include "../../gingacc-dataprocessing/include/DataProcessor.h"
#include "../../gingacc-system/include/io/interface/output/dfb/DFBWindow.h"
#endif

#include "tuner/ITuner.h"
using namespace ::br::pucrio::telemidia::ginga::core::tuning;

#include "tsparser/IDemuxer.h"
#include "tsparser/ITSFilter.h"
using namespace ::br::pucrio::telemidia::ginga::core::tsparser;

#include "../include/DataProcessor.h"
using namespace ::br::pucrio::telemidia::ginga::core::dataprocessing;

#include "../include/ncl/NCLSectionProcessor.h"
#include "../include/ncl/Metadata.h"
using namespace ::br::pucrio::telemidia::ginga::core::dataprocessing::ncl;

#include "../include/dsmcc/npt/DSMCCSectionPayload.h"
#include "../include/dsmcc/npt/NPTProcessor.h"
#include "../include/dsmcc/npt/Stc.h"
using namespace ::br::pucrio::telemidia::ginga::core::dataprocessing::dsmcc::npt;

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

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
	int fd, rval;
	int buffSize = 188;
	char buf[buffSize];

	setLogToNullDev();

#if HAVE_COMPSUPPORT
	IComponentManager* cm = IComponentManager::getCMInstance();
#endif

#if HAVE_COMPSUPPORT
	tuner         = ((TunerCreator*)(cm->getObject("Tuner")))();
	demuxer       = ((demCreator*)(cm->getObject("Demuxer")))((ITuner*)tuner);
	dataProcessor = ((dpCreator*)(cm->getObject("DataProcessor")))();

#else
	tuner         = new Tuner();
	demuxer       = new Demuxer(tuner);
	dataProcessor = new DataProcessor();
#endif

	nptProcessor  = new NPTProcessor(NULL);
	dsmccSection  = new DSMCCSectionPayload(NULL, 0);

	dataProcessor->setDemuxer(demuxer);

	if (argc == 1) {
		//encoding NCL sections
		baseUri = "/root/ncl/Tests/";
		cout << "gingacc-dataprocessing test: encoding streams (creating ";
		cout << baseUri << "metadata.xml)" << endl;

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
			if (uri.substr(0, 1) != "/") {
				uri = baseUri + uri;
			}

			remove(uri.c_str());
			++j;
		}

		//decoding NCL sections
		cout << "gingacc-dataprocessing test: decoding streams print results: ";
		cout << endl;

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
		cout << "gingacc-dataprocessing test (argc = 2)";
		cout << endl;
		clk = new TimeBaseClock();
		clk->setReference(0);
		while (true) {
			cout << "stc in seconds: '" << clk->getBaseToSecond() << "'";
			cout << endl;
			::usleep(1000000);
		}

	} else if (argc == 3 && strcmp(argv[1], "--decode-oc") == 0) {
		cout << "gingacc-dataprocessing test (argc = 3) decode-oc inside '";
		cout << argv[2] << "'";
		cout << endl;

		fd = open(argv[2], O_RDONLY | O_LARGEFILE);
		if (fd < 0) {
			cout << "gingacc-dataprocessing test (argc = 3) Error! ";
			cout << " can't find '" << argv[2] << "'";
			cout << endl;
		}

		dataProcessor->createStreamTypeSectionFilter(STREAM_TYPE_DSMCC_TYPE_B);
		demuxer->setDestination(STREAM_TYPE_DSMCC_TYPE_B);

		do {
			rval = read(fd, buf, buffSize);
			if (rval != buffSize) {
				cout << "gingacc-dataprocessing(" << __LINE__ << ")";
				cout << " Warning! Can't read '" << buffSize << "'";
				cout << " bytes" << endl;
			}
			demuxer->receiveData(buf, rval);
		} while (rval > 0);

		cout << "gingacc-dataprocessing test decode-oc all done! press enter";
		cout << " to quit" << endl;
		getchar();
		delete dataProcessor;
		delete demuxer;
		delete tuner;

		close(fd);

	} else if (argc == 3 && strcmp(argv[1], "--ait") == 0) {
		cout << "gingacc-dataprocessing test (argc = 3) decode-oc inside '";
		cout << argv[2] << "'";
		cout << endl;

		fd = open(argv[2], O_RDONLY | O_LARGEFILE);
		if (fd < 0) {
			cout << "gingacc-dataprocessing test (argc = 3) Error! ";
			cout << " can't find '" << argv[2] << "'";
			cout << endl;
		}

		dataProcessor->createStreamTypeSectionFilter(
				STREAM_TYPE_PRIVATE_SECTION);

		do {
			rval = read(fd, buf, buffSize);
			if (rval != buffSize) {
				cout << "gingacc-dataprocessing(" << __LINE__ << ")";
				cout << " Warning! Can't read '" << buffSize << "'";
				cout << " bytes" << endl;
			}
			demuxer->receiveData(buf, rval);
		} while (rval > 0);

		cout << "gingacc-dataprocessing test decode-oc all done! press enter";
		cout << " to quit" << endl;
		getchar();
		delete dataProcessor;
		delete demuxer;
		delete tuner;

		close(fd);
	}

	cout << "gingacc-dataprocessing(" << __LINE__ << ")";
	cout << " all done!" << endl;
	//TODO: tests
	return 0;
}
