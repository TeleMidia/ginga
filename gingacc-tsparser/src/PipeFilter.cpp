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

#include "tsparser/PipeFilter.h"

#include "tsparser/Pat.h"
using namespace ::br::pucrio::telemidia::ginga::core::tsparser::si;

#include <stdio.h>
#include <signal.h>
#include <string.h>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {
	PipeFilter::PipeFilter(unsigned int pid) : Thread() {
		this->pid             = pid;
		this->secondPid       = 0;
		this->dataReceived    = false;
		this->pipeFd          = 0;
		this->secondFd        = 0;
		this->packetsReceived = 0;
		this->fifoCreated     = false;
		this->dvrReader       = false;
		this->dvrName         = "";
		this->pids            = new map<int, int>;
	}

	PipeFilter::~PipeFilter() {
		if (pids != NULL) {
			delete pids;
			pids = NULL;
		}
	}

	void PipeFilter::setDestName(string name) {
		fifoName = name;
		Thread::start();

		while (!fifoCreated) {
			SystemCompat::uSleep(10000);
		}
	}

	void PipeFilter::addPid(int pid) {
		clog << "PipeFilter::addPid '" << pid << "'" << endl;
		lock();
		(*pids)[pid] = 0;
		unlock();
	}

	bool PipeFilter::hasData() {
		return dataReceived;
	}

	bool PipeFilter::isDVRReader() {
		return dvrReader;
	}

	void PipeFilter::setDVRReader(int fd, bool reader, string dvrName) {
		this->readerFd  = fd;
		this->dvrReader = reader;
		this->dvrName   = dvrName;
	}

	void PipeFilter::receiveTSPacket(ITSPacket* pack) {
		int ret;
		int ppid;
		int contCounter;
		char packData[ITSPacket::TS_PACKET_SIZE];

		ppid = pack->getPid();

		if (!pids->empty()) {
			lock();
			if (pids->count(ppid) == 0) {
				unlock();
				return;
			}
			unlock();

			contCounter = (*pids)[ppid];
			pack->setContinuityCounter(contCounter);
			if (pack->getAdaptationFieldControl() != 2 &&
					pack->getAdaptationFieldControl() != 0) {

				if (contCounter == 15) {
					contCounter = -1;
				}
				(*pids)[ppid] = contCounter + 1;
			}
		}

		pack->getPacketData(packData);
		if (ppid == 0x00 && !pids->empty()) {
			Pat::resetPayload(packData + 4, pack->getPayloadSize());
		}

		if (pipeFd > 0) {
		    try {
				ret = write(pipeFd, (void*)packData, ITSPacket::TS_PACKET_SIZE);
				if (ret == ITSPacket::TS_PACKET_SIZE) {
					dataReceived = true;
				}

		    } catch (const char *except) {
		    	clog << "PipeFilter::receiveTSPacket catch: " << except << endl;
		    	SystemCompat::uSleep(100000);
		    }
		}
	}

	void PipeFilter::receiveSection(
			char* buf, int len, IFrontendFilter* filter) {

	}

	void PipeFilter::receivePes(char* buf, int len, IFrontendFilter* filter) {
		if (secondFd > 0) {
			try {
				write(secondFd, buf, len);

		    } catch (const char *except) {
		    	clog << "PipeFilter::receivePes p2 catch: " << except << endl;
		    }
		}

		if (pipeFd > 0) {
			try {
				write(pipeFd, buf, len);

			} catch (const char *except) {
				clog << "PipeFilter::receivePes p1 catch: " << except << endl;
			}
		}
	}

	bool PipeFilter::addDestination(unsigned int dest) {
		clog << "PipeFilter::addDestination '" << dest << "'" << endl;
		secondPid = dest;
		return true;
	}

	void PipeFilter::run() {
		FILE* fd;
		int rval;
		int buffSize = 188 * 1024;
		char buff[buffSize];
		string cmd;

		clog << "PipeFilter::run(" << this << ")" << endl;

		if (dvrReader) {
			clog << "PipeFilter::run(" << this << ") reader" << endl;

			fd = fopen(dvrName.c_str(), "rb");
			//fd = fopen(dvrName.c_str(), "rb");
			if (fd < 0) {
				fd = fopen(dvrName.c_str(), "rb");
				if (fd < 0) {
					clog << "PipeFilter::run(" << this << ")";
					clog << " can't open '" << dvrName;
					clog << "'" << endl;
					perror("PipeFilter::run can't open file");
					return;
				}
			}

			clog << "PipeFilter::run(" << this << ") '" << dvrName;
			clog << "' OPENED" << endl;

			while (dvrReader) {
				rval = fread(buff, 1, buffSize, fd);
				if (rval > 0) {
					receivePes(buff, rval, NULL);
				}
			}

			clog << "PipeFilter::run(" << this << ") reader all done!" << endl;

		} else {
			rval = mkfifo(fifoName.c_str(), S_IFIFO);
			fifoCreated = true;

			pipeFd = open(fifoName.c_str(), O_WRONLY);
			if (pipeFd == -1) {
				clog << "PipeFilter::run error opening pipe '";
				clog << fifoName << "'";
				clog << endl;

			} else {
				if (secondPid > 0) {
					fifoName = "dvr" + itos(secondPid) + ".ts";
					mkfifo(fifoName.c_str(), S_IFIFO);
					secondFd = open(fifoName.c_str(), O_WRONLY);
					clog << "secpipe '" << fifoName << "' opened" << endl;
				}
				fifoCreated = true;
			}
		}
	}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::tsparser::ITSFilter*
		createPipeFilter(int pid) {

	return new ::br::pucrio::telemidia::ginga::core::tsparser::PipeFilter(pid);
}

extern "C" void destroyPipeFilter(
		::br::pucrio::telemidia::ginga::core::tsparser::ITSFilter* f) {

	delete f;
}
