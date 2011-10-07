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

#include "dataprocessing/dsmcc/npt/DSMCCSectionPayload.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace dataprocessing {
namespace dsmcc {
namespace npt {
DSMCCSectionPayload::DSMCCSectionPayload(char* data, unsigned int length) {
	payload            = data;
	payloadSize        = length;
	privateDataByte    = NULL;
	dsmccDescritorList = new vector<Descriptor*>;
	privateDataLength  = 0;
	checksum           = 0;

	processSectionPayload();
}

DSMCCSectionPayload::~DSMCCSectionPayload() {
	//clearDsmccDescritor();
	if (dsmccDescritorList != NULL) {
		delete (dsmccDescritorList);
	}
}

int DSMCCSectionPayload::processSectionPayload() {
	unsigned int pos;
	unsigned char descriptorTag;
	unsigned short descriptorSize;
	NPTReference* nptRef;
	StreamMode* strMode;

	pos = 0;

	//dsmccDescriptorList()
	while (pos < payloadSize) {
		descriptorTag  = payload[pos] & 0xFF;
		descriptorSize = (payload[pos + 1] & 0xFF) + 2;

		switch (descriptorTag) {
			case 0x01: // NPT Reference
				nptRef = new NPTReference();
				nptRef->addData(payload + pos, descriptorSize);
				addDsmccDescriptor(nptRef);
				break;

			case 0x03: // Stream Mode
				strMode = new StreamMode();
				strMode->addData(payload + pos, descriptorSize);
				addDsmccDescriptor(strMode);
				break;

			case 0x04: // Stream Event
				clog << "DSMCCSectionPayload::processSectionPayload";
				clog << " stream event. " << endl;

			default:
				clog << "DSMCCSectionPayload::processSectionPayload";
				clog << "Descriptor unrecognized. ";
				clog << (descriptorTag & 0xFF) << endl;
		}
		pos = pos + descriptorSize;
	}

	return pos;
}

int DSMCCSectionPayload::updateStream() {
	/*int pos;

	if (sectionSyntaxIndicator) {
		privateIndicator = 0x00;
	} else {
		privateIndicator = 0x01;
	}

	pos = PrivateSection::updateStream();

	Descriptor* desc;
	int streamLen;
	char* dataStream;

	if (tableId == 0x3A) {
		//LLCSNAP()
	} else if (tableId == 0x3B) {
		//userNetworkMessage()
	} else if (tableId == 0x3C) {
		//downloadDataMessage()
	} else if (tableId == 0x3D) {
		vector<Descriptor*>::iterator i;
		if ((dsmccDescritorList != NULL) &&
				(!dsmccDescritorList->empty())) {
			i = dsmccDescritorList->begin();
			while (i != dsmccDescritorList->end()) {
				desc = *i;
				streamLen = desc->getStream(&dataStream);
				if ((pos + streamLen + 4) <= MAX_SECTION_SIZE) {
					memcpy(stream + pos, dataStream, streamLen);
					pos += streamLen;
				} else {
					break;
				}
				++i;
			}
		}
	} else if (tableId == 0x3E) {
		//private_data_byte
	}

	if (!sectionSyntaxIndicator) {
		//TODO: checksum
		stream[pos++] = (checksum >> 24) & 0xFF;
		stream[pos++] = (checksum >> 16) & 0xFF;
		stream[pos++] = (checksum >> 8) & 0xFF;
		stream[pos++] = checksum & 0xFF;
	} else {
		//crc32
		Crc32 crc;
		unsigned int value = crc.crc(stream, pos);
		crc32 = value;
		stream[pos++] = (crc32 >> 24) & 0xFF;
		stream[pos++] = (crc32 >> 16) & 0xFF;
		stream[pos++] = (crc32 >> 8) & 0xFF;
		stream[pos++] = crc32 & 0xFF;
	}
	return pos;*/
	return 0;
}

int DSMCCSectionPayload::calculateSectionSize() {
	/*unsigned int pos = PrivateSection::calculateSectionSize();
	Descriptor* desc;
	int streamLen;
	if (tableId == 0x3A) {
		//LLCSNAP()
	} else if (tableId == 0x3B) {
		//userNetworkMessage()
	} else if (tableId == 0x3C) {
		//downloadDataMessage()
	} else if (tableId == 0x3D) {
		vector<Descriptor*>::iterator i;
		if ((dsmccDescritorList != NULL) &&
				(!dsmccDescritorList->empty())) {
			i = dsmccDescritorList->begin();
			while (i != dsmccDescritorList->end()) {
				desc = *i;
				streamLen = desc->getStreamSize();
				if ((pos + streamLen + 4) <= MAX_SECTION_SIZE) {
					pos += streamLen;
				} else {
					break;
				}
				++i;
			}
		}
	} else if (tableId == 0x3E) {
		//private_data_byte
	}
	return pos + 4;*/
	return 0;
}

vector<Descriptor*>* DSMCCSectionPayload::getDsmccDescritorList() {
	return dsmccDescritorList;
}

unsigned int DSMCCSectionPayload::getChecksum() {
	return checksum;
}

void DSMCCSectionPayload::setChecksum(unsigned int cs) {
	checksum = cs;
}

int DSMCCSectionPayload::getPrivateDataByte(char** dataStream) {
	if (privateDataByte != NULL) {
		*dataStream = privateDataByte;
		return privateDataLength;
	} else {
		return 0;
	}
}

int DSMCCSectionPayload::setPrivateDataByte(char* data, unsigned short length) {
	if (privateDataByte != NULL) {
		delete (privateDataByte);
	}
	try {
		privateDataByte = new char[length];
	} catch(...) {
		return -1;
	}
	memcpy(privateDataByte, data, length);
	privateDataLength = length;
	return privateDataLength;
}

void DSMCCSectionPayload::addDsmccDescriptor(Descriptor* d) {
	dsmccDescritorList->push_back(d);
}

void DSMCCSectionPayload::removeDsmccDescriptor(unsigned char descriptorTag) {
	Descriptor* desc;
	vector<Descriptor*>::iterator i;
	if ((dsmccDescritorList != NULL) && (!dsmccDescritorList->empty())) {
		i = dsmccDescritorList->begin();
		while (i != dsmccDescritorList->end()) {
			desc = *i;
			if (desc->getDescriptorTag() == descriptorTag) {
				delete (desc);
				dsmccDescritorList->erase(i);
				break;
			}
			++i;
		}
	}
}

void DSMCCSectionPayload::clearDsmccDescritor() {
	Descriptor* desc;
	vector<Descriptor*>::iterator i;
	if ((dsmccDescritorList != NULL) && (!dsmccDescritorList->empty())) {
		i = dsmccDescritorList->begin();
		while (i != dsmccDescritorList->end()) {
			desc = *i;
			delete (desc);
			++i;
		}
		dsmccDescritorList->clear();
	}
}

}
}
}
}
}
}
}
}
