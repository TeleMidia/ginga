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

#include "../../../include/ShortEventDescriptor.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {
namespace si {
namespace descriptors {
	ShortEventDescriptor::ShortEventDescriptor() {
		descriptorTag = 0x4D;
		eventNameLength = 0;
		descriptionLength = 0;
	}

	ShortEventDescriptor::~ShortEventDescriptor() {

	}

	unsigned char ShortEventDescriptor::getDescriptorTag() {
		return 0x4D;
	}

	void ShortEventDescriptor::setLanguageCode(string language) {
		strncpy(languageCode, language.c_str(), 3);
	}

	void ShortEventDescriptor::setEventName(string name, unsigned char len) {
		eventNameLength = len;
		memset(eventName, 0, 255);
		memcpy(eventName, name.c_str(), eventNameLength);
		descriptorLength = (eventNameLength + descriptionLength + 5);
	}

	void ShortEventDescriptor::setDescription(string text, unsigned char len) {
		descriptionLength = len;
		memset(description, 0, 255);
		memcpy(description, text.c_str(), descriptionLength);
		descriptorLength = (eventNameLength + descriptionLength + 5);
	}

	unsigned char ShortEventDescriptor::getDescriptorLength() {
		return descriptorLength;
	}

	unsigned char ShortEventDescriptor::getEventNameLength() {
		return eventNameLength;
	}

	unsigned char ShortEventDescriptor::getDescriptionLength() {
		return descriptionLength;
	}

	char * ShortEventDescriptor::getLanguageCode() {
		return languageCode;
	}

	char * ShortEventDescriptor::getEventName() {
		return eventName;
	}

	char * ShortEventDescriptor::getDescription() {
		return description;
	}

	void ShortEventDescriptor::setDescriptorLength(unsigned char length) {
		descriptorLength = length;
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
		IShortEventDescriptor* createSED() {

	return (new ::br::pucrio::telemidia::ginga::core::tsparser::si::
			descriptors::ShortEventDescriptor());
}

extern "C" void destroySED(
		::br::pucrio::telemidia::ginga::core::tsparser::si::descriptors::
		IShortEventDescriptor* sed) {

	delete sed;
}
