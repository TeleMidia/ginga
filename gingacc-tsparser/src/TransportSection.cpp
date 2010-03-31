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

#include "../include/TransportSection.h"

#include "tsparser/IDemuxer.h"
using namespace ::br::pucrio::telemidia::ginga::core::tsparser;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {
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
		constructionFailed = true;
		pid                = 0;
		sectionLength      = 0;
		sectionName        = "";
	}

	bool TransportSection::isConstructionFailed() {
		return constructionFailed;
	}

	bool TransportSection::create(char *sectionBytes, unsigned int size){
		// Verifies the size to protect the memcpy call
		if (size > ARRAY_SIZE(section)) {
			cout << "TransportSection::create Warning! ";
			cout << "Invalid section size " << size << " ";
			cout << "truncating..." << endl;
			size = ARRAY_SIZE(section);
		}

		memcpy((void*)&(section[0]), (void*)&(sectionBytes[0]), size);

		// Header data.
		tableId                = section[0];
		sectionSyntaxIndicator = (section[1] & 0x80) >> 7;

		sectionLength = (((section[1] & 0x0F) << 8) | (section[2] & 0xFF));
		idExtention   = (((section[3] & 0xFF) << 8) | (section[4] & 0xFF));

		versionNumber          = (section[5] & 0x3E) >> 1;
		currentNextIndicator   = (section[5] & 0x01);
		sectionNumber          = (section[6] & 0xFF);
		lastSectionNumber      = (section[7] & 0xFF);

		// Update current section size.
		if (size >= sectionLength + 3) {
			currentSize = sectionLength + 3;

		} else {
			currentSize = size;
		}

		// Verifies Invalid Section .. Discard TransportSection
		if (sectionLength > 4093) {
			_warn(
					"TransportSection::new: Invalid section size=%d\n",
					sectionLength);
			return false;
		}

		/*_debug("NEW====================currSize=%d , secLen=%d secNum=%d\n",
				currentSize, sectionLength,sectionNumber);*/
		//DEBUG
		/*printf("NEW====================currsize=%d , secLen=%d\n", currentSize, sectionLength);
		for (int i = 0; i < currentSize; i++) {
			printf("%02hhX ", (char) section[i]);
		}
		printf("\n====================\n");*/

		return true;
	}

	void TransportSection::setESId(unsigned int id) {
		this->pid = id;
		setSectionName();
	}

	unsigned int TransportSection::getESId() {
		return this->pid;
	}

	void TransportSection::addData(char bytes[184], unsigned int size) {
		unsigned int freespace = sectionLength + 3 - currentSize;

		if (sectionLength == 0) {
			constructionFailed = create(bytes, size);
			return;
		}

		if (isConsolidated()) {
			cout << "TransportSection::addData: Warning! ";
			cout << "Trying to add " << size << " ";
			cout << "bytes in a consolidated section." << endl;
			return;
		}

		// Invalid size.
		if (size > freespace) {
			cout << "TransportSection::addData: size (";
			cout << size << ") is larger thant the available space (";
			cout << freespace << "), truncating..." << endl;
			size = freespace;
		}

		/*_debug("### currSize=%d, secLen=%d, freesp=%d, size=%d secNum=%d\n",
			currentSize, sectionLength, freespace, size, sectionNumber);*/

		memcpy((void*)&(section[currentSize]), (void*)bytes, size);
		currentSize += size;

		//DEBUG
//		printf("ADDDATA====================currsize=%d\n", currentSize);
//		for (int i = 0; i < currentSize; i++) {
//			printf("%02hhX ", (char) section[i]);
//		}
//		printf("\n====================\n");
	}

	void TransportSection::setSectionName() {
		if (tableId == SDT_TID || tableId == EIT_TID || tableId == CDT_TID) {
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
		memcpy((void*)buffer,
						   (void*)&section[8], size);
		return (void*)(buffer);
	}

	unsigned int TransportSection::getCurrentSize() {
		return currentSize;
	}

	unsigned int TransportSection::getPayloadSize() {
		// Skipping header.
		return sectionLength - 9;
	}

	bool TransportSection::isConsolidated() {
		return ((sectionLength + 3) == currentSize);
	}

	void TransportSection::print() {
		unsigned int i;
		cout << "TS SECTION " << endl;
		cout << "tableid = " << getTableId() << endl;
		cout << "syntax indicator = " << getSectionSyntaxIndicator() << endl;
		cout << "section length = " << getSectionLength() << endl;
		cout << "extendion Id = " << getExtensionId() << endl;
		cout << "section version = " << getVersionNumber() << endl;
		cout << "current next indicator = " << getCurrentNextIndicator() << endl;
		cout << "section number = " << getSectionNumber() << endl;
		cout << "last secion number = " << getLastSectionNumber() << endl;

		char payload[currentSize - 12];
		memcpy((void*)&(payload[0]), getPayload(), currentSize - 12);

		for (i=0; i < (currentSize - 12); i++) {
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
