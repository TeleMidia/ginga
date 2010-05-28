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
		descriptorTag    = 0x4D;
		descriptorLength = 0;
		eventNameLength  = 0;
		eventNameChar    = NULL;
		textLength       = 0;
		textChar         = NULL;

	}

	ShortEventDescriptor::~ShortEventDescriptor() {
		if (eventNameChar != NULL) {
			delete eventNameChar;
			eventNameChar = NULL;
		}
		if (textChar != NULL) {
			delete textChar;
			textChar = NULL;
		}
	}

	unsigned char ShortEventDescriptor::getDescriptorTag() {
		return descriptorTag;
	}

	unsigned int ShortEventDescriptor::getDescriptorLength() {
		return (unsigned int)descriptorLength;
	}

	unsigned int ShortEventDescriptor::getEventNameLength() {
		return (unsigned int)eventNameLength;
	}

	unsigned int ShortEventDescriptor::getTextLength() {
		return (unsigned int)textLength;
	}

	string ShortEventDescriptor::getLanguageCode() {
		string str;

		str.append(languageCode, 3);
		return str;
	}

	string ShortEventDescriptor::getEventName() {
		string str;

		if(eventNameChar == NULL){
			return "";
		}
		str.append(eventNameChar, eventNameLength);
		return str;
	}

	string ShortEventDescriptor::getTextChar() {
		string str;

		if(textChar == NULL){
			return "";
		}
		str.append(textChar, textLength);
		return textChar;
	}

	void ShortEventDescriptor::print() {
		cout << "ShortEventDescriptor::print printing...." << endl;
		cout << " -languageCode: "  << getLanguageCode()   << endl;
		cout << " -eventNameChar: " << getEventName()      << endl;
		cout << " -textChar: "      << getTextChar()       << endl;
 	}

	size_t ShortEventDescriptor::process(char* data, size_t pos) {
		unsigned char len = 0;
		//cout << "ShortEventDescriptor::process with pos = " << pos << endl;

		descriptorLength = data[pos+1];
		pos += 2;

		memcpy(languageCode, data+pos, 3);
		pos += 3;

		eventNameLength = data[pos];

		if (eventNameLength > 0) {
			eventNameChar = new char[eventNameLength];

			if (eventNameChar == NULL) {
				cout << "ShortEvent::process error allocating memory" << endl;
				return -1;
			}
			memset(eventNameChar, 0, eventNameLength);
			memcpy(eventNameChar, data+pos+1, eventNameLength);
		}
		pos += eventNameLength + 1;

		textLength = data[pos];
		if (textLength) {

			textChar = new char[textLength];
			if (textChar == NULL) {
				//cout << "ShortEvent::process error allocating memory" << endl;
				return -1;
			}
			memset(textChar, 0, textLength);
			memcpy(textChar, data+pos+1, textLength);

		}
		pos += textLength;

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

