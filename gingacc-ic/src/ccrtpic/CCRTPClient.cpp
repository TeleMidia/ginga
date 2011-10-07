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

#include "ic/ccrtpic/CCRTPClient.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace ic {
	CCRTPClient::CCRTPClient(InetMcastAddress& ima, tpport_t port) :
			RTPSession(ima, port) {

		hasSession = false;
		rtpIC      = NULL;
	}

	CCRTPClient::CCRTPClient(InetHostAddress& ia, tpport_t port) :
			RTPSession(ia, port) {

		hasSession = false;
		rtpIC      = NULL;
	}

	CCRTPClient::~CCRTPClient() {
		hasSession = false;
		rtpIC      = NULL;
	}

	void CCRTPClient::setInteractiveChannel(IInteractiveChannel* ic) {
		rtpIC = ic;
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
		clog << "CCRTPClient::printSSRC ";
		clog << "My SSRC identifier is: " << hex << getLocalSSRC() << endl;
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
		clog << "CCRTPClient::printADU ";
		clog << "I got an app. data unit - ";
		clog << adu->getSize();
		clog << " payload octets (";
		clog << "pt " << (int)adu->getType();
		clog << ") from ";
		clog << hex << (int)adu->getSource().getID();
		clog << "@" << dec;
		clog << adu->getSource().getNetworkAddress();
		clog << ":";
		clog << adu->getSource().getDataTransportPort();
		clog << endl;
	}

	/*************************************************************************/
	/******************** redefined functions from libccrtp*******************/
	/*************************************************************************/

	// redefined from IncomingDataQueue
	void CCRTPClient::onNewSyncSource(const SyncSource& src) {
		clog << "CCRTPClient::onNewSyncSource ";
		clog << "* New synchronization source: ";
		clog << hex << (int)src.getID() << endl;
	}

	// redefined from QueueRTCPManager
	void CCRTPClient::onGotSR(
			SyncSource& source, SendReport& SR, uint8 blocks) {

		RTPSession::onGotSR(source,SR,blocks);
		clog << "CCRTPClient::onGotSR ";
		clog << "I got an SR RTCP report from ";
		clog << hex << (int)source.getID() << "@";
		clog << dec;
		clog << source.getNetworkAddress() << ":";
		clog << source.getControlTransportPort() << endl;
	}

	// redefined from QueueRTCPManager
	void CCRTPClient::onGotRR(
			SyncSource& source, RecvReport& RR, uint8 blocks) {

		RTPSession::onGotRR(source,RR,blocks);
		clog << "CCRTPClient::onGotRR ";
		clog << "I got an RR RTCP report from ";
		clog << hex << (int)source.getID() << "@";
		clog << dec;
		clog << source.getNetworkAddress() << ":";
		clog << source.getControlTransportPort() << endl;
	}

	// redefined from QueueRTCPManager
	bool CCRTPClient::onGotSDESChunk(
			SyncSource& source, SDESChunk& chunk, size_t len) {

		bool result = RTPSession::onGotSDESChunk(source, chunk, len);
		clog << "CCRTPClient::onGotSDESChunk ";
		clog << "I got a SDES chunk from ";
		clog << hex << (int)source.getID() << "@";
		clog << dec;
		clog << source.getNetworkAddress() << ":";
		clog << source.getControlTransportPort();
		clog << " (";
		clog << source.getParticipant()->getSDESItem(SDESItemTypeCNAME);
		clog << ") " << endl;
		return result;
	}

	void CCRTPClient::onGotGoodbye(
			const SyncSource& source, const std::string& reason) {

		hasSession = true;

		if (rtpIC != NULL) {
			rtpIC->releaseUrl();
		}

		clog << "CCRTPClient::onGotGoodbye ";
		clog << "I got a Goodbye packet from ";
		clog << hex << (int)source.getID() << "@";
		clog << dec;
		clog << source.getNetworkAddress() << ":";
		clog << source.getControlTransportPort() << endl;
		clog << "   Goodbye reason: \"" << reason << "\"" << endl;
	}
}
}
}
}
}
}
