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

#include "tsparser/PartialReceptionDescriptor.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {
namespace si {
namespace descriptors {
//TODO: test this class - no use of this descriptor on TS files
	PartialReceptionDescriptor::PartialReceptionDescriptor() {
		descriptorTag    = 0xFB;
		descriptorLength = 0;
		services         = NULL;
	}

	PartialReceptionDescriptor::~PartialReceptionDescriptor() {
		if (services != NULL){
			delete services;
			services == NULL;
		}
	}
	unsigned char PartialReceptionDescriptor::getDescriptorLength() {
		return descriptorLength;
	}
	unsigned char PartialReceptionDescriptor::getDescriptorTag() {
		return descriptorTag;
	}
	void ParentalRatingDescriptor::print() {
		clog << "ParentalRatingDescriptor::print printing..." << endl;
	}
	size_t ParentalRatingDescriptor::process(char* data, size_t pos) {
		size_t servicesNumber;

		descriptorLength = data[pos+1];
		servicesNumber  = descriptorLength / 2;
		pos++;

		services = new unsigned char[servicesNumber];
		for (int i = 0 ; i < servicesNumber; ++i) {
			pos ++;
			services[i] = (((data[pos] << 8)& 0xFF00) ||
					(data[pos+1] & OxFF));
			pos++;
		}
		return pos;

	}
}
}
}
}
}
}
}
}
