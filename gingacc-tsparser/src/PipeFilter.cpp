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
	PipeFilter::PipeFilter(unsigned int pid) {
		this->pid             = pid;
		this->dataReceived    = false;
		this->packetsReceived = 0;

		this->pids.clear();
		this->buffers.clear();
	}

	PipeFilter::~PipeFilter() {
		vector<TSBuffer*>::iterator i;

		pids.clear();

		i = buffers.begin();
		while (i != buffers.end()) {
			PipeFilter::releaseTSBuffer(*i);
			++i;
		}
		buffers.clear();
	}

	void PipeFilter::releaseTSBuffer(TSBuffer* buffer) {
		delete buffer->data;
		delete buffer;
	}

	TSBuffer* PipeFilter::createTSBuffer(char* data, int dataSize) {
		TSBuffer* buffer = new TSBuffer;
		buffer->data     = new char[dataSize];
		buffer->dataSize = dataSize;

		memcpy(buffer->data, data, dataSize);

		return buffer;
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
		char packData[ITSPacket::TS_PACKET_SIZE];

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

		pack->getPacketData(packData);
		if (ppid == 0x00 && !pids.empty()) {
			Pat::resetPayload(packData + 4, pack->getPayloadSize());
		}

		buffers.push_back(PipeFilter::createTSBuffer(
				packData, ITSPacket::TS_PACKET_SIZE));

		dataReceived = true;
	}

	void PipeFilter::receiveSection(
			char* buf, int len, IFrontendFilter* filter) {

	}

	void PipeFilter::receivePes(char* buf, int len, IFrontendFilter* filter) {
		buffers.push_back(PipeFilter::createTSBuffer(buf, len));
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
