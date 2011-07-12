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

#include "tsparser/TSPacket.h"
#include "tsparser/Demuxer.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {
	TSPacket::TSPacket(char* packetData) {
		//DEBUG
		/*printf("NEW ====================\n");
			for (int j = 0; j <= 188; j++) {
					printf("%02hhX ", (char) packetData[j]);
			}
		printf("\n====================\n");*/
		memset(payload, 0, sizeof(payload));
		constructionFailed = !create(packetData);
	}

	TSPacket::~TSPacket() {
		// Nothing to do.
	}

	bool TSPacket::isConstructionFailed() {
		return constructionFailed;
	}

	// Create TSPacket.  Returs true if successful, otherwise returns
	// false.
	bool TSPacket::create(char* data) {
		unsigned int aflen;

		payloadOffset = TS_PACKET_SIZE - TS_PAYLOAD_SIZE;

		// Header data.
		if ((syncByte = (data[0] & 0xFF)) != TS_PACKET_SYNC_BYTE) {
			cout << "TSPacket::create: Warning! syncByte != 0x47";
			cout << endl;
			return false;
		}

		transportErrorIndication   = (data[1] & 0x80) >> 7;
		payloadUnitStartIndicator  = (data[1] & 0x40) >> 6;
		transportPriority          = (data[1] & 0x20) >> 5;
		pid                        = ((data[1] & 0x1F) << 8) |(data[2] & 0xFF);
		transportScramblingControl = (data[3] & 0xC0) >> 6;
		adaptationFieldControl     = (data[3] & 0x30) >> 4;
		continuityCounter          = (data[3] & 0x0F);
		payloadSize                = 0;
		pointerField               = 0;

		// No adaptation field (payload only).
		if (getAdaptationFieldControl() == PAYLOAD_ONLY) {
			// Nothing to do.

		// Adaptation field followed by payload.
		} else if (getAdaptationFieldControl() == ADAPT_PAYLOAD) {
			// Specifies the number of bytes in the adaptationField
			// following this byte.  Maximum value 183 (184 minus
			// adaptation field length byte)
			aflen = data[4] & 0xFF;
			if (aflen < 0 || aflen > 183) {
				cout << "TSPacket::create: Warning! ";
				cout << "Invalid adaptationFieldLength: ";
				cout << aflen << endl;
				return false;
			}

			// adaptation field length is 0 then we have to skip it
			if (aflen == 0) {
				aflen = 1;
			}
			payloadOffset += aflen;

		// Adaptation field only.
		} else if (getAdaptationFieldControl() == NO_PAYLOAD) {
			// Nothing to do.
		}

		if (getStartIndicator()) {
			//checking if it is section or pes
			unsigned int pesStartCode = (((data[payloadOffset] & 0xFF) << 16) |
					((data[payloadOffset + 1] & 0xFF) << 8) |
					(data[payloadOffset + 2] & 0xFF));

			if (pesStartCode == 0x01 && pid != 0x00) {
				pointerField = 0;

			} else {
				// Update pointerField (the first payload byte).
				pointerField = (data[payloadOffset] & 0xFF);

				// TODO: Pat, Pmp and SectionFilter code
				// cannot handle pointerField as the first payload
				// byte.  For compatibility we're removing the
				// pointerField from the payload before this ``bad''
				// code is called.  This is only a workaround, the real
				// solution is fix all external code.
				payloadOffset++;
			}
		}

		payloadSize = TS_PACKET_SIZE - payloadOffset;
		memcpy(
				(void*)(&payload[0]),
				(void*)(&data[TS_PACKET_SIZE - TS_PAYLOAD_SIZE]),
				TS_PAYLOAD_SIZE);

		return true;
	}

	unsigned int TSPacket::getPid() {
		return pid;
	}

	void TSPacket::getPacketData(char streamData[TS_PACKET_SIZE]) {
		int payload_offset = TS_PACKET_SIZE - TS_PAYLOAD_SIZE;

		// Header data.
		streamData[0] = TS_PACKET_SYNC_BYTE;
		streamData[1] = ((transportErrorIndication << 7) & 0x80) |
				((payloadUnitStartIndicator << 6) & 0x40) |
				((transportPriority << 5) & 0x20) |
				(((pid & 0xFF00) >> 8) & 0x1F);

		streamData[2] = (pid & 0x00FF);
		streamData[3] = ((transportScramblingControl << 6) & 0xC0) |
				((adaptationFieldControl << 4) & 0x30) |
				(continuityCounter & 0x0F);

		// Payload.
		memcpy(
				(void*)(&streamData[payload_offset]),
				(void*)(&payload[0]),
				TS_PAYLOAD_SIZE);
	}

	void TSPacket::getPayload(char streamData[184]) {
		memcpy(
				(void*)(&streamData[0]),
				(void*)(&payload[payloadOffset - 4]),
				payloadSize);
	}

	unsigned int TSPacket::getPayloadSize() {
		return payloadSize;
	}

	bool TSPacket::getStartIndicator() {
		return payloadUnitStartIndicator;
	}

	unsigned int TSPacket::getPointerField() {
		return pointerField;
	}

	unsigned int TSPacket::getAdaptationFieldControl() {
		return adaptationFieldControl;
	}

	unsigned int TSPacket::getContinuityCounter() {
		return continuityCounter;
	}

	void TSPacket::setContinuityCounter(unsigned int counter) {
		continuityCounter = counter;
	}

	void TSPacket::print() {
		unsigned int i;
		cout << "TS PACK" << endl;
		cout << "sync = " << hex << (syncByte & 0xFF) << endl;
		cout << "pid = " << hex << pid << endl;

		cout << "payloadSize = " << payloadSize << endl;

		cout << "transportErrorIndication = "
		     << transportErrorIndication << endl;

		cout << "payloadUnitStartIndicator = "
		     << hex << payloadUnitStartIndicator << endl;

		cout << "transportPriority = " << transportPriority << endl;

		cout << "transportScramblingControl = "
		     << hex << transportScramblingControl << endl;

		cout << "adaptationFieldControl = " << hex
		     << adaptationFieldControl << endl;

		cout << "continuityCounter = " << hex << continuityCounter
		     << endl;

		cout << "payload: " << endl;
		for (i=0; i < 184; i++) {
			cout << (payload[i] & 0xFF) << " ";
		}
		cout << endl << endl;
	}
}
}
}
}
}
}
