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
#include "tsparser/AudioComponentDescriptor.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {
namespace si{
namespace descriptors{

	AudioComponentDescriptor::AudioComponentDescriptor() {
		descriptorTag      = 0xC4;
		ESMultiLingualFlag = false;
		componentTag       = 0;
		componentType      = 0;
		descriptorLength   = 0;
		mainComponentFlag  = false;
		qualityIndicator   = 0;
		samplingRate       = 0;
		simulcastGroupTag  = 0;
		streamContent      = 0;
		streamType         = 0;
		textLength         = 0;
		languageCode[0]    = 0;
		languageCode[1]    = 0;
		languageCode2      = NULL;
		textChar           = NULL;
	}

	AudioComponentDescriptor::~AudioComponentDescriptor() {
		if (textChar != NULL) {
			delete textChar;
			textChar = NULL;
		}

		if (languageCode2 != NULL) {
			delete languageCode2;
			languageCode2 = NULL;
		}
	}

	unsigned char AudioComponentDescriptor::getDescriptorTag (){
		return descriptorTag;
	}

	unsigned int AudioComponentDescriptor::getDescriptorLength(){
		return (unsigned int)descriptorLength;
	}

	unsigned char AudioComponentDescriptor::getStreamContent (){
		return streamContent;
	}

	string AudioComponentDescriptor::getLanguageCode() {
		string str;

		if(languageCode == NULL){
			return "";
		}
		str.append(languageCode, 3);
		return str;
	}

	string AudioComponentDescriptor::getLanguageCode2(){
		string str;

		if(languageCode2 == NULL){
			return "";
		}
		str.append(languageCode2, 3);
		return str;
	}

	string AudioComponentDescriptor::getTextChar() {
		string str;

		if(textChar == NULL){
			return "";
		}

		str.append(textChar, textLength);

		return str;
	}

	unsigned char AudioComponentDescriptor::getComponentTag() {
		return componentTag;
	}

	unsigned char AudioComponentDescriptor::getComponentType() {
		return componentType;
	}

	unsigned char AudioComponentDescriptor::getStreamType() {
		return streamType;
	}

	unsigned char AudioComponentDescriptor::getSimulcastGroupTag() {
		return simulcastGroupTag;
	}

	bool AudioComponentDescriptor::getESMultiLingualFlag() {
		return ESMultiLingualFlag;
	}

	bool AudioComponentDescriptor::getMainComponentFlag() {
		return mainComponentFlag;
	}

	unsigned char AudioComponentDescriptor::getQualityIndicator() {
		return qualityIndicator;
	}

	unsigned char AudioComponentDescriptor::getSamplingRate() {
		return samplingRate;
	}

	void AudioComponentDescriptor::print() {
		cout << "AudioComponentDescriptor::print" << endl;
		cout << " -languageCode = " << getLanguageCode() << endl;
		if (ESMultiLingualFlag) {
			cout << " -languageCode2 = " << getLanguageCode2() << endl;
		}
	}

	size_t AudioComponentDescriptor::process(char* data, size_t pos) {
		//cout << "Audio Component process beginning with pos =  " << pos;

		descriptorLength = data[pos+1];
		//cout <<" and length = " << (descriptorLength & 0xFF) <<endl;
		if (descriptorLength == 0){
			pos++;
			return pos;
		}

		pos += 2; //pos = 20
		//jumping reserved_future_use (first 4 bits of data[pos]
		streamContent = (data[pos] & 0x0F); //last 4 bits of data[pos]
		pos += 1; //pos = 21

		componentType = data[pos];
		pos ++; //pos = 22

		componentTag = data[pos];
		pos++; //pos=23

		streamType = data[pos];
		pos ++; //pos=24

		simulcastGroupTag = data[pos];
		pos ++; //25

		ESMultiLingualFlag = ((data[pos] & 0x80) >> 7);
		mainComponentFlag  = ((data[pos] & 0x40) >> 6);
		qualityIndicator   = ((data[pos] & 0x30) >> 4);
		samplingRate       = ((data[pos] & 0x0C));
		pos++;//pos= 26

		memcpy(languageCode, data+pos, 3);
		pos += 3; //pos = 29

		if (ESMultiLingualFlag) {
			if (languageCode2 != NULL) {
				delete languageCode2;
			}

			languageCode2 = new char[3];
			memcpy(languageCode2, data+pos, 3);
			pos+=3;
			textLength = descriptorLength - 12;

		} else {
			textLength = descriptorLength - 9;
			//9 bytes since descriptorLenght until languageCode
		}

		if (textLength > 0) {
			if (textChar != NULL) {
				delete textChar;
			}
			textChar = new char[textLength];
			memset(textChar, 0, textLength);
			memcpy(textChar, data+pos, textLength);
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
