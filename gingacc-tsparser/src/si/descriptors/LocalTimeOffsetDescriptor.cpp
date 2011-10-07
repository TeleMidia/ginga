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

#include "tsparser/LocalTimeOffsetDescriptor.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {
namespace si {
namespace descriptors {

	LocalTimeOffsetDescriptor::LocalTimeOffsetDescriptor() {
		descriptorTag           = 0x58;
		descriptorLength        = 0;
		countryRegionId         = 0;
		localTimeOffset         = 0;
		localTimeOffsetPolarity = 0;
		nextTimeOffset          = 0;
		memset(countryCode, 0 , 3);
		memset(timeOfChange, 0, 5);

	}

	LocalTimeOffsetDescriptor::~LocalTimeOffsetDescriptor() {

	}

	unsigned char LocalTimeOffsetDescriptor::getDescriptorTag() {
		return descriptorTag;
	}

	unsigned int LocalTimeOffsetDescriptor::getDescriptorLength() {
		return (unsigned int)descriptorLength;
	}

	string LocalTimeOffsetDescriptor::getCountryCode() {
		string str;
		str.assign(countryCode, 3);

		return str;
	}

	unsigned char LocalTimeOffsetDescriptor::getCountryRegionId() {
		return countryRegionId;
	}

	unsigned char LocalTimeOffsetDescriptor::getLocalTimeOffsetPolarity() {
		return localTimeOffsetPolarity;
	}

	unsigned short LocalTimeOffsetDescriptor::getLocalTimeOffset(){
		return localTimeOffset;
	}

	string LocalTimeOffsetDescriptor::getTimeOfChange() {
		string str;
		str.assign(timeOfChange, 5);

		return str;
	}

	unsigned short LocalTimeOffsetDescriptor::getNextTimeOffset() {
		return nextTimeOffset;
	}
	void LocalTimeOffsetDescriptor::print() {
		clog << "LocalTimeOffsetDescriptor::print printing..."      << endl;
		clog << "CountryCode: "             << getCountryCode()     << endl;
		clog << "CountryRegionId: "         << (countryRegionId & 0xFF) << endl;
		clog << "LocalTimeOffsetPolarity: " << (localTimeOffsetPolarity & 0xFF);
		clog << endl;
		clog << "LocalTimeOffSet:"          << (localTimeOffset & 0xFF) << endl;
		clog << "TimeOfChange: "            << getTimeOfChange()    << endl;
		clog << "NextTimeOffset: "          << getNextTimeOffset()  << endl;
	}

	size_t LocalTimeOffsetDescriptor::process(char* data, size_t pos) {

		clog << " LocalTimeOffsetDescriptor::process" << endl;
		descriptorLength = data[pos+1];
		pos += 2;

		memcpy(countryCode, data+pos, 3);
		pos += 3;

		countryRegionId = ((data[pos] & 0xFC) >> 2);
		localTimeOffsetPolarity = data[pos] & 0x01;
		pos ++;

		localTimeOffset = ((((data[pos] & 0xFF) << 8) & 0xFF00)|
				(data[pos+1] & 0xFF));
		pos += 2;

		memcpy(timeOfChange, data+pos, 5);
		pos += 5;

		nextTimeOffset = ((((data[pos] & 0xFF) << 8) & 0xFF00)|
				(data[pos+1] & 0xFF));

		pos ++;

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
