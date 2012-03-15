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

#include "tsparser/TransportSection.h"

#include "tsparser/IDemuxer.h"
using namespace ::br::pucrio::telemidia::ginga::core::tsparser;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {

	static bool initTab = false;
	static unsigned int crcTab[256] = {};

	TransportSection::TransportSection() {
		initialize();
	}

	TransportSection::TransportSection(
			char* sectionBytes, unsigned int size) {

		initialize();
		constructionFailed = create(sectionBytes, size);
	}

	TransportSection::~TransportSection() {

	}

	void TransportSection::initialize() {
		unsigned int i = 0;
		unsigned int j = 0;
		unsigned int c = 0;

		if (!initTab) {
			initTab = true;

			for (i = 0; i < 0x100; i++)	{
				c = (i << 24);

				for (j = 0; j < 8; j++)	{
					if (c & 0x80000000)	{
						c = (c << 1) ^ 0x04C11DB7;
					} else {
						c = (c << 1);
					}
				}

				crcTab[i] = c;
			}
		}

		constructionFailed = true;
		pid                = 0;
		sectionLength      = 0;
		sectionName        = "";
		currentSize        = 0;

		memset(section, 0, sizeof(section));
	}

	bool TransportSection::isConstructionFailed() {
		return constructionFailed;
	}

	bool TransportSection::create(char *sectionBytes, unsigned int size){
		// Verifies the size to protect the memcpy call
		if (size > ARRAY_SIZE(section)) {
			clog << "TransportSection::create Warning! ";
			clog << "Invalid section size " << size << " ";
			clog << "truncating..." << endl;
			size = ARRAY_SIZE(section);
		}

		memcpy((void*)&(section[0]), (void*)&(sectionBytes[0]), size);

		// Header data.
		tableId                = section[0];
		sectionSyntaxIndicator = (section[1] & 0x80) >> 7;

		//sectionLength = (((section[1] & 0x0F) << 8) | (section[2] & 0xFF));
		if (tableId == 0x00 || tableId == 0x02) {
			sectionLength = (((section[1] & 0x03) << 8) |
					(section[2] & 0xFF));

		} else {
			sectionLength = (((section[1] & 0x0F) << 8) | (section[2] & 0xFF));

		}

		if (sectionSyntaxIndicator == 0) {
			//when sectionSyntaxIndicator is 0 the last field of the header is
			// sectionLength

			idExtention          = 0;
			versionNumber        = 0;
			currentNextIndicator = 0;
			sectionNumber        = 0;
			lastSectionNumber    = 0;

		} else {
			//when sectionSyntaxIndicator is 1 the header section is bigger.

			idExtention   = (((section[3] & 0xFF) << 8) | (section[4] & 0xFF));

			versionNumber        = (section[5] & 0x3E) >> 1;
			currentNextIndicator = (section[5] & 0x01);
			sectionNumber        = (section[6] & 0xFF);
			lastSectionNumber    = (section[7] & 0xFF);
		}

		// Update current section size.
		if (size >= sectionLength + 3) {
			currentSize = sectionLength + 3;
			//section header has 3 bytes including the sectionLength field.

		} else {
			currentSize = size;
		}

		// Verifies Invalid Section .. Discard TransportSection
		if (sectionLength > 4093) {
			clog << "TransportSection::new: Invalid section size=";
			clog << sectionLength << endl;
			return false;
		}

		return true;
	}

	void TransportSection::setESId(unsigned int id) {
		this->pid = id;
		setSectionName();
	}

	unsigned int TransportSection::getESId() {
		return this->pid;
	}

	void TransportSection::addData(char* bytes, unsigned int size) {
		unsigned int freespace = sectionLength + 3 - currentSize;

		if (sectionLength == 0) {
			constructionFailed = create(bytes, size);
			return;
		}

		if (isConsolidated()) {
			clog << "TransportSection::addData: Warning! ";
			clog << "Trying to add " << size << " ";
			clog << "bytes in a consolidated section." << endl;
			return;
		}

		// Invalid size.
		if (size > freespace) {
			clog << "TransportSection::addData: size (";
			clog << size << ") is larger than the available space (";
			clog << freespace << "), truncating..." << endl;
			size = freespace;
		}

		/*printf("### currSize=%d, secLen=%d, freesp=%d, size=%d secNum=%d\n",
			currentSize, sectionLength, freespace, size, sectionNumber);

		fflush(stdout);*/

		memcpy((void*)(section + currentSize), (void*)bytes, size);

		/*if (pid == 0x384) {
		//DEBUG
			printf("ADDDATA====================currsize=%d\n", currentSize + size);
			for (int i = 0; i < currentSize + size; i++) {
				printf("%02hhX ", (char) section[i]);
			}
			printf("\n====================\n");
		}*/

		currentSize += size;
	}

	void TransportSection::setSectionName() {
		if (tableId == SDT_TID || tableId == EIT_TID || tableId == CDT_TID ||
		 						(tableId >= 0x50 && tableId <= 0x5F )) {

			sectionName = "epg/data/" + itos(pid) +
			    itos(tableId) + itos(idExtention) + itos(versionNumber);

		} else {
			sectionName = "carousel/modules/" + itos(pid) +
			    itos(tableId) + itos(idExtention) + itos(versionNumber);
		}
	}

	string TransportSection::getSectionName() {
		return sectionName;
	}

	unsigned int TransportSection::getTableId() {
		return tableId;
	}

	bool TransportSection::getSectionSyntaxIndicator() {
		return sectionSyntaxIndicator;
	}

	unsigned int TransportSection::getSectionLength() {
		return sectionLength;
	}

	unsigned int TransportSection::getExtensionId() {
		return idExtention;
	}

	unsigned int TransportSection::getVersionNumber() {
		return versionNumber;
	}

	bool TransportSection::getCurrentNextIndicator() {
		return currentNextIndicator;
	}

	unsigned int TransportSection::getSectionNumber() {
		return sectionNumber;
	}

	unsigned int TransportSection::getLastSectionNumber() {
		return lastSectionNumber;
	}

	void* TransportSection::getPayload() {
		unsigned int size = this->getPayloadSize();
		char *buffer = new char[size];

		if (sectionSyntaxIndicator == 0) {
			memcpy((void*)buffer,(void*)&section[3], size);

		} else {
			memcpy((void*)buffer,(void*)&section[8], size);
		}

		return (void*)(buffer);
	}

	unsigned int TransportSection::getCurrentSize() {
		return currentSize;
	}

	unsigned int TransportSection::getPayloadSize() {
		// Skipping header.
		if (sectionSyntaxIndicator == 0) {
			//when 0, the payload starts after the sectionLength field and the
			//section does not have CRC.

			return sectionLength;

		} else {
			//when 1, header has 5 bytes after sectionLength field and has the
			//4 bytes for CRC.
			return sectionLength - 9;
		}
	}

	bool TransportSection::isConsolidated() {
		return ((sectionLength + 3) == currentSize);
	}

	unsigned int TransportSection::crc32(char* data, unsigned int len) {
		unsigned int i   = 0;
		unsigned int crc = 0xFFFFFFFF;

		for (i = 0; i < len; i++) {
			crc = (crc << 8) ^ crcTab[((crc >> 24) ^ data[i]) & 0xFF];
		}

		return crc;
	}

	void TransportSection::print() {
		unsigned int i;
		clog << "TS SECTION " << endl;
		clog << "tableid = "           << getTableId() << endl;
		clog << "syntax indicator = "  << getSectionSyntaxIndicator() << endl;
		clog << "section length = "    << getSectionLength() << endl;

		if (sectionSyntaxIndicator == 1) {
			clog << "extendion Id = "           << getExtensionId() << endl;
			clog << "section version = "        << getVersionNumber() << endl;
			clog << "current next indicator = " << getCurrentNextIndicator();
			clog << endl;
			clog << "section number = "         << getSectionNumber() << endl;
			clog << "last section number = "    << getLastSectionNumber();
			clog << endl;
		}

		clog << (char*)getPayload() << endl;
		return;

		unsigned int payloadLength;

		if(sectionSyntaxIndicator == 0) {
			i = 3;
			payloadLength = sectionLength;

		} else {
			//char payload[currentSize - 12];
			i = 8;
			payloadLength = currentSize - 12;
		}

		//memcpy((void*)&(payload[0]), getPayload(), currentSize - 12);

		for (; i < (payloadLength); i++) {
			clog << (section[i] & 0xFF) << " ";
		}
		clog << endl << endl;
	}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::tsparser::ITransportSection*
		createTSSection(char* sectionBytes, unsigned int size) {

	if (sectionBytes == NULL) {
		return new ::br::pucrio::telemidia::ginga::core::tsparser::
			TransportSection();

	} else {
		return new ::br::pucrio::telemidia::ginga::core::tsparser::
			TransportSection(sectionBytes, size);
	}
}
