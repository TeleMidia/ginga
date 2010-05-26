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

#include "../../include/ccrtpic/CCRTPClient.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace ic {
	CCRTPClient::CCRTPClient(InetMcastAddress& ima, tpport_t port) :
			RTPSession(ima, port) {

		hasSession = false;
	}

	CCRTPClient::CCRTPClient(InetHostAddress& ia, tpport_t port) :
			RTPSession(ia, port) {

		hasSession = false;
	}

	CCRTPClient::~CCRTPClient() {
		hasSession = false;
	}

	void CCRTPClient::openSession() {
		hasSession = true;

		defaultApplication().setSDESItem(SDESItemTypeTOOL, "gingacc-ic");
		//setSchedulingTimeout(20000);
		setExpireTimeout(1000000);

		setPayloadFormat(StaticPayloadFormat(sptMP2T));
		startRunning();
	}

	void CCRTPClient::printSSRC() {
		cout << "CCRTPClient::printSSRC ";
		cout << "My SSRC identifier is: " << hex << getLocalSSRC() << endl;
	}

	char* CCRTPClient::getPacketData(int* size) {
		const AppDataUnit* adu = NULL;
		char* buffer;

		if (!hasSession) {
			*size = 0;
			return NULL;
		}

		while (adu == NULL) {
			adu = getData(getFirstTimestamp());
			if (adu != NULL) {
				*size  = adu->getSize();
				buffer = new char[*size];
				memcpy(buffer, adu->getData(), *size);
				//printADU(adu);
				delete adu;

				return buffer;
			}
			Thread::sleep(7);
		}

		*size = 0;
		return NULL;
	}

	void CCRTPClient::printADU(const AppDataUnit* adu) {
		cout << "CCRTPClient::printADU ";
		cout << "I got an app. data unit - ";
		cout << adu->getSize();
		cout << " payload octets (";
		cout << "pt " << (int)adu->getType();
		cout << ") from ";
		cout << hex << (int)adu->getSource().getID();
		cout << "@" << dec;
		cout << adu->getSource().getNetworkAddress();
		cout << ":";
		cout << adu->getSource().getDataTransportPort();
		cout << endl;
	}

	/*************************************************************************/
	/******************** redefined functions from libccrtp*******************/
	/*************************************************************************/

	// redefined from IncomingDataQueue
	void CCRTPClient::onNewSyncSource(const SyncSource& src) {
		cout << "CCRTPClient::onNewSyncSource ";
		cout << "* New synchronization source: ";
		cout << hex << (int)src.getID() << endl;
	}

	// redefined from QueueRTCPManager
	void CCRTPClient::onGotSR(
			SyncSource& source, SendReport& SR, uint8 blocks) {

		RTPSession::onGotSR(source,SR,blocks);
		cout << "CCRTPClient::onGotSR ";
		cout << "I got an SR RTCP report from ";
		cout << hex << (int)source.getID() << "@";
		cout << dec;
		cout << source.getNetworkAddress() << ":";
		cout << source.getControlTransportPort() << endl;
	}

	// redefined from QueueRTCPManager
	void CCRTPClient::onGotRR(
			SyncSource& source, RecvReport& RR, uint8 blocks) {

		RTPSession::onGotRR(source,RR,blocks);
		cout << "CCRTPClient::onGotRR ";
		cout << "I got an RR RTCP report from ";
		cout << hex << (int)source.getID() << "@";
		cout << dec;
		cout << source.getNetworkAddress() << ":";
		cout << source.getControlTransportPort() << endl;
	}

	// redefined from QueueRTCPManager
	bool CCRTPClient::onGotSDESChunk(
			SyncSource& source, SDESChunk& chunk, size_t len) {

		bool result = RTPSession::onGotSDESChunk(source, chunk, len);
		cout << "CCRTPClient::onGotSDESChunk ";
		cout << "I got a SDES chunk from ";
		cout << hex << (int)source.getID() << "@";
		cout << dec;
		cout << source.getNetworkAddress() << ":";
		cout << source.getControlTransportPort();
		cout << " (";
		cout << source.getParticipant()->getSDESItem(SDESItemTypeCNAME);
		cout << ") " << endl;
		return result;
	}

	void CCRTPClient::onGotGoodbye(
			const SyncSource& source, const std::string& reason) {

		hasSession = true;

		cout << "CCRTPClient::onGotGoodbye ";
		cout << "I got a Goodbye packet from ";
		cout << hex << (int)source.getID() << "@";
		cout << dec;
		cout << source.getNetworkAddress() << ":";
		cout << source.getControlTransportPort() << endl;
		cout << "   Goodbye reason: \"" << reason << "\"" << endl;
	}
}
}
}
}
}
}
