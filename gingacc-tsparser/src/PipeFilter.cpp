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

#include "../include/PipeFilter.h"

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

		Thread::start();
	}

	PipeFilter::~PipeFilter() {
		if (pids != NULL) {
			delete pids;
			pids = NULL;
		}
	}

	void PipeFilter::addPid(int pid) {
		cout << "PipeFilter::addPid '" << pid << "'" << endl;
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
		while (!fifoCreated) {
			::usleep(10000);
		}

		lock();
		if (pids->count(ppid) == 0) {
			unlock();
			return;
		}
		unlock();

		contCounter = (*pids)[ppid];
		pack->setContinuityCounter(contCounter);
		if (pack->getContinuityCounter() != 2 &&
				pack->getContinuityCounter() != 0) {

			(*pids)[ppid] = contCounter + 1;
		}

		pack->getPacketData(packData);
		ret = write(pipeFd, (void*)packData, ITSPacket::TS_PACKET_SIZE);
		if (ret == ITSPacket::TS_PACKET_SIZE) {
			dataReceived = true;
		}
	}

	void PipeFilter::receiveSection(
			char* buf, int len, IFrontendFilter* filter) {

	}

	void PipeFilter::receivePes(char* buf, int len, IFrontendFilter* filter) {
		if (secondFd > 0) {
			write(secondFd, buf, len);
		}

		if (pipeFd > 0) {
			write(pipeFd, buf, len);
		}
	}

	bool PipeFilter::addDestination(unsigned int dest) {
		cout << "PipeFilter::addDestination '" << dest << "'" << endl;
		secondPid = dest;
		return true;
	}

	void PipeFilter::run() {
		int rval, fd;
		string fifoName;
		int buffSize = 188 * 1024;
		char buff[buffSize];
		string cmd;

		if (dvrReader) {
			cout << "PipeFilter::run reader" << endl;

			fd = open(dvrName.c_str(), O_RDONLY | O_NONBLOCK);
			//fd = open(dvrName.c_str(), O_RDONLY);
			if (fd < 0) {
				cout << "PipeFilter::run can't open '" << dvrName;
				cout << "'" << endl;
				return;
			}

			while (dvrReader) {
				rval = read(fd, buff, buffSize);
				if (rval > 0) {
					receivePes(buff, rval, NULL);
				}
			}

			cout << "PipeFilter::run reader all done!" << endl;

		} else {
			fifoName = "dvr" + itos(pid) + ".ts";

			rval = mkfifo(fifoName.c_str(), S_IFIFO);
			fifoCreated = true;

			pipeFd = open(fifoName.c_str(), O_WRONLY);
			if (pipeFd == -1) {
				cout << "error opening pipe '" << pid << "'" << endl;

			} else {
				if (secondPid > 0) {
					fifoName = "dvr" + itos(secondPid) + ".ts";
					mkfifo(fifoName.c_str(), S_IFIFO);
					secondFd = open(fifoName.c_str(), O_WRONLY);
					cout << "secpipe '" << fifoName << "' opened" << endl;
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
