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

#include "../../../include/LogoTransmissionDescriptor.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {
namespace si {
namespace descriptors {
	LogoTransmissionDescriptor::LogoTransmissionDescriptor() {
		descriptorTag = 0xCF;
		descriptorLength = 1;
	}

	LogoTransmissionDescriptor::~LogoTransmissionDescriptor() {

	}

	void LogoTransmissionDescriptor::setDescriptorLength(unsigned short length) {
		descriptorLength = length;
	}

	unsigned char LogoTransmissionDescriptor::getDescriptorLength() {
		return descriptorLength;
	}

	void LogoTransmissionDescriptor::setType(unsigned char type) {
		logoType = type;
		switch (logoType) {
		case 0x01:
			descriptorLength = 7;
			break;
		case 0x02:
			descriptorLength = 3;
			break;
		default:
			break;
		}
	}

	unsigned char LogoTransmissionDescriptor::getType() {
		return logoType;
	}

	void LogoTransmissionDescriptor::setLogoId(unsigned short id) {
		logoId = id;
	}

	unsigned short LogoTransmissionDescriptor::getLogoId() {
		return logoId;
	}

	void LogoTransmissionDescriptor::setLogoVersion(unsigned short version) {
		logoVersion = version;
	}

	unsigned short LogoTransmissionDescriptor::getLogoVersion() {
		return logoVersion;
	}

	void LogoTransmissionDescriptor::setDownloadDataId(unsigned short id) {
		downloadDataId = id;
	}

	unsigned short LogoTransmissionDescriptor::getDownloadDataId() {
		return downloadDataId;
	}

	void LogoTransmissionDescriptor::setName(string name) {
		logoName = name;
		descriptorLength = (logoName.length() + 1);
		logoType = 0x03;
	}

	string LogoTransmissionDescriptor::getName() {
		return logoName;
	}

	unsigned char LogoTransmissionDescriptor::getNameLength() {
		return logoName.length();
	}

	unsigned char LogoTransmissionDescriptor::getDescriptorTag() {
		return descriptorTag;
	}
}
}
}
}
}
}
}
}

extern "C" ::br::pucrio::telemidia::ginga::core::tsparser::si::descriptors::
		ILogoTransmissionDescriptor* createLTD() {

	return (new
			::br::pucrio::telemidia::ginga::core::tsparser::si::descriptors::
			LogoTransmissionDescriptor());
}

extern "C" void destroyLTD(
		::br::pucrio::telemidia::ginga::core::tsparser::si::descriptors::
		ILogoTransmissionDescriptor* ltd) {

	delete ltd;
}
