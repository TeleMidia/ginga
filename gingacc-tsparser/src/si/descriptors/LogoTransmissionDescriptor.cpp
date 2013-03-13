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

#include "tsparser/LogoTransmissionDescriptor.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {
namespace si {
namespace descriptors {
//TODO: test this class - no use of this descriptor on TS files.
	LogoTransmissionDescriptor::LogoTransmissionDescriptor() {
		descriptorTag    = 0xCF;
		descriptorLength = 0;
		downloadDataId   = 0;
		logoId           = 0;
		logoName         = "";
		logoType         = 0;
		logoVersion      = 0;
	}

	LogoTransmissionDescriptor::~LogoTransmissionDescriptor() {

	}

	unsigned int LogoTransmissionDescriptor::getDescriptorLength() {
		return (unsigned int)descriptorLength;
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

	unsigned int LogoTransmissionDescriptor::getNameLength() {
		return logoName.length();
	}

	unsigned char LogoTransmissionDescriptor::getDescriptorTag() {
		return descriptorTag;
	}

	void LogoTransmissionDescriptor::print() {

	}

	size_t LogoTransmissionDescriptor::process (char* data, size_t pos){
		descriptorLength = data[pos+1];
		//clog << "Descriptor length: ";
		//clog << (ltd->getDescriptorLength() & 0xFF) << endl;
		pos += 2;

		setType(data[pos]);
		//clog << "Type: " << (ltd->getType() & 0xFF) << endl;
		pos ++;
		if (logoType == 0x01) { // scheme 1
			logoId = (((data[pos] << 8) & 0x0100) |
					(data[pos+1] & 0xFF));

			pos += 2;
			logoVersion = (((data[pos] << 8) & 0x0100) |
					(data[pos+1] & 0xFF));
			pos += 2;
			downloadDataId = (((data[pos] << 8) &
						0xFF00) | (data[pos+1] & 0xFF));
			pos += 2;
		}
		else if (logoType == 0x02) { // scheme 2
			logoId = (((data[pos] << 8) & 0x0100) |
								(data[pos+1] & 0xFF));
			pos += 2;
		}
		else if (logoType == 0x03) { // simple logo system

			logoName = "";
			logoName.append(data+pos, descriptorLength-1);
			pos += (descriptorLength - 1);
			//clog << "Simple logo system: " << ltd->getName() << endl;
		}
		else {
			pos += (descriptorLength-1);
			//clog << "Unrecognized Logo Transmission Type: "
					//	<< logoType << endl;
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

