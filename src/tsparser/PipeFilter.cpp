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
		this->dataReceived    = false;
		this->packetsReceived = 0;
		debugBytesWritten     = 0;

		this->srcIsAPipe      = false;
		this->srcUri          = "";
		this->dstUri          = "";
		this->dstPipeCreated  = false;

		this->running         = false;

		this->pids.clear();
		clog << "PipeFilter::PipeFilter all done" << endl;
	}

	PipeFilter::~PipeFilter() {
		pids.clear();
	}

	void PipeFilter::addPid(int pid) {
		clog << "PipeFilter::addPid '" << pid << "'" << endl;
		pids[pid] = 0;
	}

	bool PipeFilter::hasData() {
		return dataReceived;
	}

	void PipeFilter::receiveTSPacket(ITSPacket* pack) {
		int ppid;
		int contCounter;
		int bytesWritten = 0;
		char* packData;

		//memset(packData, 0, ITSPacket::TS_PACKET_SIZE);
		ppid = pack->getPid();

		if (!pids.empty()) {
			if (pids.count(ppid) == 0) {
				return;
			}

			contCounter = pids[ppid];
			pack->setContinuityCounter(contCounter);
			if (pack->getAdaptationFieldControl() != 2 &&
					pack->getAdaptationFieldControl() != 0) {

				if (contCounter == 15) {
					contCounter = -1;
				}
				pids[ppid] = contCounter + 1;
			}
		}

		pack->getPacketData(&packData);

		dataReceived = true;

		if (!dstPipeCreated) {
			if (!running && dstUri != "") {
				Thread::startThread();
			}

			while (!dstPipeCreated) {
				SystemCompat::uSleep(10000);
			}
		}

		bytesWritten = SystemCompat::writePipe(
				dstPd, packData, ITSPacket::TS_PACKET_SIZE);

		assert(bytesWritten == ITSPacket::TS_PACKET_SIZE);
	}

	void PipeFilter::receiveSection(
			char* buf, int len, IFrontendFilter* filter) {

	}

	void PipeFilter::receivePes(char* buf, int len, IFrontendFilter* filter) {

	}

	void PipeFilter::setSourceUri(string srcUri, bool isPipe) {
		this->srcUri     = srcUri;
		this->srcIsAPipe = isPipe;
	}

	string PipeFilter::setDestinationUri(string dstUri) {
		this->dstUri = SystemCompat::checkPipeName(dstUri);

		if (!running) {
			Thread::startThread();
		}

		return this->dstUri;
	}

	void PipeFilter::run() {
		FILE* fd;
		int rval;
		int buffSize = 188 * 1024;
		char* buff = new char[buffSize];
		string cmd;

		running = true;
		clog << "PipeFilter::run(" << this << ")" << endl;

		if (!SystemCompat::createPipe(dstUri, &dstPd)) {
			clog << "PipeFilter::run(" << this << ")";
			clog << " can't create '" << dstUri;
			clog << "'" << endl;

			running = false;
			delete[] buff;
			return;
		}

		dstPipeCreated  = true;

		clog << "PipeFilter::run(" << this << ") pipe '";
		clog << dstUri << "' created" << endl;

		if (srcIsAPipe) {
			clog << "PipeFilter::run(" << this << ") reader" << endl;

			if (!SystemCompat::openPipe(srcUri, &srcPd)) {
				clog << "PipeFilter::run(" << this << ")";
				clog << " can't open '" << srcUri;
				clog << "'" << endl;

				delete[] buff;
				return;
			}

			clog << "PipeFilter::run(" << this << ") '" << srcUri;
			clog << "' OPENED" << endl;

			while (srcIsAPipe) {
				rval = SystemCompat::readPipe(srcPd, buff, buffSize);
				if (rval > 0) {
					SystemCompat::writePipe(dstPd, buff, rval);
				}
			}

			clog << "PipeFilter::run(" << this << ") reader all done!" << endl;
		}

		running = false;
		delete[] buff;
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
