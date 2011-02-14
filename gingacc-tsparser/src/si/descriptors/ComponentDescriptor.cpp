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
#include "tsparser/ComponentDescriptor.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {
namespace si{
namespace descriptors{
	ComponentDescriptor::ComponentDescriptor() {
		descriptorTag    = 0x50;
		descriptorLength = 0;
		streamContent    = 0;
		componentType    = 0;
		componentTag     = 0;
		textChar         = NULL;
	}

	ComponentDescriptor::~ComponentDescriptor() {
		if (textChar != NULL) {
			delete textChar;
			textChar == NULL;
		}
	}

	unsigned char ComponentDescriptor::getDescriptorTag(){
			return descriptorTag;
	}

	unsigned int ComponentDescriptor::getDescriptorLength(){
		return (unsigned int)descriptorLength;
	}

	string ComponentDescriptor::getTextChar() {
		string str;

		if(textChar == NULL){
			return "";
		}
		str.append(textChar, textLength);
		return str;
	}

	void ComponentDescriptor::print() {
		cout << "ComponentDescriptor::print printing..." << endl;
		cout << " -descriptorLength = "   << (unsigned int)descriptorLength;
		cout << endl;
		cout << " -streamContent = " << (unsigned int)streamContent;
		cout << endl;
		cout << " -componentType = " << (unsigned int)componentType;
		cout << endl;
		/*if (textLength > 0){
			cout << " and textChar = " ;
			for(int i = 0; i < textLength; ++i){
				cout << textChar[i];
			}
			cout << endl;
		}
		*/
	}

	size_t ComponentDescriptor::process(char* data, size_t pos){
		//cout << "ComponentDescriptor::process beginning with pos =  " << pos;
		descriptorLength = data[pos+1];

		//cout <<" and length = " << (descriptorLength & 0xFF) <<endl;

		pos += 2;
		//jumping reserved_future_use (first 4 bits of data[pos])
		streamContent = (data[pos] & 0x0F); //last 4 bits of data[pos]
		//cout <<"Componenet streamContent = " << (streamContent & 0xFF) <<endl;
		pos += 1;

		componentType = data[pos];
		//cout <<"Component componentType = " << (componentType & 0xFF) <<endl;
		pos ++;

		componentTag = data[pos];
		//cout << "Componenet component tag = "<< (componentTag & 0xFF) << endl;

		pos++; //pos=23
		memcpy(languageCode, data+pos, 3);

		pos += 3;
		textLength = descriptorLength - 6;
		/*the si standard do not define textLenght for this descriptor
		 *for this reason, it has to be calculated. It was kept as
		 *a class atribute to maintain conformity with others classes
		 */
		if (textLength > 0) {
			if (textChar != NULL) {
				delete textChar;
			}
			//setTextChar(data+pos, textLength);
			textChar = new char[textLength];
			memset(textChar, 0, textLength);
			memcpy(textChar, data+pos, textLength);

			/*
			cout << "ComponentDescriptor::process text char = ";
			for (int i = 0; i < textLength; i++){
				cout << (textChar[i]);
			}
			cout << endl;
			*/
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

