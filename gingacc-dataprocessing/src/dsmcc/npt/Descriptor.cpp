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

#include "dataprocessing/dsmcc/npt/Descriptor.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace dataprocessing {
namespace dsmcc {
namespace npt {

Descriptor::Descriptor() {
	stream = NULL;
	descriptorTag = 0;
	descriptorLength = 0;
	currentSize = 0;
}

Descriptor::~Descriptor() {
	if (stream != NULL) {
		delete (stream);
	}
}

Descriptor::Descriptor(unsigned char tag) {
	stream = NULL;
	descriptorTag = tag;
	descriptorLength = 0;
	currentSize = 0;
}

char Descriptor::addData(char* data, unsigned short length) {
	unsigned short rbytes;

	if (currentSize == 0) {

		if (length < 2) {
			return -1; //error
		}

		descriptorTag = data[0] & 0xFF;
		descriptorLength = data[1] & 0xFF;

		if (stream != NULL) {
			delete (stream);
		}
		try {
			stream = new char[descriptorLength + 2];
		} catch(...) {
			return -1;
		}
	}

	if (currentSize + length >= MAX_DESCRIPTOR_SIZE) {
		rbytes = MAX_DESCRIPTOR_SIZE - currentSize;
	} else {
		rbytes = length;
	}

	memcpy(stream + currentSize, data, rbytes);

	currentSize = currentSize + rbytes;

	if (isConsolidated()) {
		if (rbytes > 0) {
			process();
		}
		return 1;
	} else {
		return 0;
	}
}

int Descriptor::process() {
	return 2;
}

int Descriptor::updateStream() {
	unsigned int len;

	if (stream != NULL) {
		delete (stream);
	}
	try {
		len = calculateDescriptorSize();
		stream = new char[len];
	} catch(...) {
		return -1;
	}

	descriptorLength = len - 2;

	stream[0] = descriptorTag & 0xFF;
	stream[1] = descriptorLength & 0xFF;
	return 2;
}

unsigned int Descriptor::calculateDescriptorSize() {
	return 2;
}

unsigned char Descriptor::isConsolidated() {
	unsigned short len = descriptorLength + 2;
	return (len <= currentSize);
}

unsigned char Descriptor::getDescriptorTag() {
	return descriptorTag;
}

unsigned char Descriptor::getDescriptorLength() {
	return descriptorLength;
}

void Descriptor::setDescriptorTag(unsigned char tag) {
	descriptorTag = tag;
}

int Descriptor::getStreamSize() {
	return calculateDescriptorSize();
}

int Descriptor::getStream(char** dataStream) {
	int slen = updateStream();
	if (slen >= 0) {
		*dataStream = stream;
		return slen;
	} else {
		return -1;
	}
}

Descriptor* Descriptor::getDescriptor(
		vector<Descriptor*>* descriptors, unsigned char Tag) {
	vector<Descriptor*>::iterator dit;
	dit = descriptors->begin();
	while (dit != descriptors->end()) {
		if ((*dit)->getDescriptorTag() == Tag) {
			return *dit;
		}
		dit++;
	}
	return NULL;
}

vector<Descriptor*>* Descriptor::getDescriptors(
		vector<Descriptor*>* descriptors, unsigned char Tag) {
	vector<Descriptor*>* result;
	vector<Descriptor*>::iterator dit;
	result = new vector<Descriptor*>;
	dit = descriptors->begin();
	while (dit != descriptors->end()) {
		if ((*dit)->getDescriptorTag() == Tag) {
			result->push_back(*dit);
		}
		dit++;
	}
	if (result->size() == 0) {
		delete result;
		result = NULL;
	}
	return result;
}

int Descriptor::getDescriptorsLength(vector<Descriptor*>* descriptors) {
	int len = 0;
	vector<Descriptor*>::iterator dit;
	dit = descriptors->begin();
	while (dit != descriptors->end()) {
		len = len + (*dit)->getDescriptorLength() + 2;
		dit++;
	}
	return len;
}

}
}
}
}
}
}
}
}
