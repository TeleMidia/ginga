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

#include "../include/TSSection.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {
	TSSection::TSSection(char* streamData, int len) {
		currentSize = 0;
		memset(section, 0, sizeof(section));
		process(streamData, len);
	}

	TSSection::TSSection() {
		currentSize = 0;
		memset(section, 0, sizeof(section));
	}

	TSSection::~TSSection() {
		
	}

	void TSSection::process(char* streamData, int len) {
		if (currentSize == 0) {
			processHeader(streamData, len);
		} else {
			addPayloadData(streamData, len);
		}
	}

	void TSSection::processHeader(char* streamData, int len) {
		tableId = (streamData[0] & 0xFF);
		sectionSyntaxIndicator = (streamData[1] & 0x80) >> 7;

		if (tableId == 0x00 || tableId == 0x02) {
			sectionLength = (((streamData[1] & 0x03) << 8) |
				    (streamData[2] & 0xFF));

		} else {
			sectionLength = (((streamData[1] & 0x0F) << 8) |
				    (streamData[2] & 0xFF));
		}

		idExtention = (((streamData[3] & 0xFF) << 8) | (streamData[4] & 0xFF));
		versionNumber = (streamData[5] & 0x3E) >> 1;
		currentNextIndicator = (streamData[5] & 0x01);
		sectionNumber = (streamData[6] & 0xFF);
		lastSectionNumber = (streamData[7] & 0xFF);

		//TODO: CRC32 and checksum

		/*// PAT
		if (tableId == 0x00) {
		// CAT
		} else if (tableId == 0x01) {
			//TODO: do we need CAT?

		// PMT
		} else if (tableId == 0x02) {
			//TODO

		// DSM-CC
		} else if ((tableId >= 0x38) && (tableId <= 0x3F)) {*/

		if (len >= (sectionLength + 3)) {
			memcpy((void*)&(section[0]),
				   (void*)(&streamData[0]), sectionLength + 3);

			currentSize = sectionLength + 3;

		} else {
			memcpy((void*)&(section[0]), (void*)(&streamData[0]), len);
			currentSize = len;
		}
	}

	void TSSection::addPayloadData(char* streamData, int len) {
		if (currentSize + len >= (sectionLength + 3)) {
			memcpy((void*)&(section[0]),
				    (void*)(&streamData[0]),
				    (sectionLength + 3 - currentSize));

			currentSize = sectionLength + 3;

		} else {
			memcpy((void*)&(section[currentSize]),
				    (void*)(&streamData[0]), len);

			currentSize += len;
		}
	}

	unsigned int TSSection::getTableId() {
		return tableId;
	}

	bool TSSection::getSectionSyntaxIndicator() {
		return sectionSyntaxIndicator;
	}

	unsigned int TSSection::getSectionLength() {
		return sectionLength;
	}

	unsigned int TSSection::getExtensionId() {
		return idExtention;
	}

	unsigned int TSSection::getVersionNumber() {
		return versionNumber;
	}

	bool TSSection::getCurrentNextIndicator() {
		return currentNextIndicator;
	}

	unsigned int TSSection::getSectionNumber() {
		return sectionNumber;
	}

	unsigned int TSSection::getLastSectionNumber() {
		return lastSectionNumber;
	}

	void* TSSection::getPayload() {
		return (void*)&section[8];
	}

	unsigned int TSSection::getPayloadSize() {
		if ((sectionLength - 9) != (currentSize - 12)) {
			cout << "Warning! Strange section size. CurrentSize = "
				 << currentSize << ", sectionLength = " << sectionLength
				 << endl;

			return 0;
		}

		return (currentSize - 12);
	}

	bool TSSection::isConsolidated() {
		if (currentSize == 0) {
			return false;
		}
		return ((sectionLength + 3) <= currentSize);
	}

	void TSSection::print() {
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
		memcpy(&payload[0], getPayload(), currentSize - 12);

		for (i=0; i < (currentSize - 12); i++) {
			cout << hex << (payload[i] & 0xFF) << " ";
		}
		cout << endl << endl;
	}
}
}
}
}
}
}
