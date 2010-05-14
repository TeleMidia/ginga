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
//jumping reserved_future_use (first 4 bits of data[pos])
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

#include "../../../include/DigitalCCDescriptor.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {
namespace si {
namespace descriptors {
	DigitalCCDescriptor::DigitalCCDescriptor() {
		descriptorTag               = 0xC1;
		descriptorLength            = 0;
		components                  = NULL;
		maximumBitrateFlag          = false;
		componentControlFlag        = false;
		copyControlType             = 0;
		digitalRecordingControlData = 0;
		maximumBitRate              = 0;
	}

	DigitalCCDescriptor::~DigitalCCDescriptor() {
		if(components != NULL){
			vector<Component*>::iterator i;
			for (i = components->begin(); i!= components->end(); ++i){
				if ((*i)!= NULL){
					delete (*i);
				}
			}
			delete components;
			components = NULL;
		}
	}
	unsigned char DigitalCCDescriptor::getDescriptorTag(){
		return descriptorTag;
	}
	unsigned int DigitalCCDescriptor::getDescriptorLength(){
		return (unsigned int)descriptorLength;
	}
	void DigitalCCDescriptor::print() {
		cout << "DigitalCCDescriptor::print printing..." << endl;
		if(components->size()> 0){
			vector<Component*>::iterator i;
			for (i = components->begin(); i!= components->end(); ++i) {
				cout << " -Component:";
				cout << " componentTag = " << (unsigned int)(*i)->componentTag;
				cout << " -copyControlType = ";
				cout << (unsigned int) (*i)->copyControlType ;
				if ((*i)->maximumBitrateFlag == true) {
					cout << " -maximumBitRate = ";
					cout << (unsigned int)((*i)->maximumBitrate) << endl;
				}
				else {
					cout << endl;
				}

			}
		}
	}
	size_t DigitalCCDescriptor::process(char* data, size_t pos){
		size_t remainingBytes = 0;
		struct Component* component;

		//cout << "DigitalCCDescriptor::process with pos = " << pos;
		descriptorLength = data[pos+1];
		//cout << " and length = " << (descriptorLength & 0xFF)<< endl;
		pos += 2;

		digitalRecordingControlData = ((data[pos] & 0xC0) >> 6); //2 bits
		maximumBitrateFlag =  ((data[pos] & 0x20 ) >> 5) ;//1 bit
		componentControlFlag = ((data[pos] & 0x10) >> 4 ); //1 bit
		//cout << "DCCD componentControlFlag = " << (componentControlFlag & 0xFF) << endl;
		copyControlType = ((data[pos] & 0x0C) >> 2);//2 bits

		if(copyControlType != 00){
			APSControlData = (data[pos] & 0x03); //2 bits
		}
		//cout << "DigitalCCD debug third byte = " << (data[pos] & 0xFF) << endl;
		pos++;

		if(maximumBitrateFlag == 1){
			maximumBitRate = data[pos];
			//cout << "DCCD maximumBitRate = " << maximumBitRate << endl;
			pos++;
		}
		if(componentControlFlag == 1){
			componentControlLength = data[pos];
			remainingBytes = componentControlLength;

			components = new vector<Component*>;
			while(remainingBytes > 0){

				pos++;

				component = new struct Component;
				component->componentTag = data[pos];
				pos++;

				remainingBytes -= 2; //2 bytes read

				component->digitalRecordingControlData =
						((data[pos] & 0xC0)>> 6);

				component->maximumBitrateFlag = ((data[pos] & 0x20 ) >> 5);
				component->copyControlType = ((data[pos] & 0x0C) >> 2);


				if(component->copyControlType != 00){
					component->APSControlData = (data[pos] & 0x03); //2 bits
				}

				if(maximumBitrateFlag == 1){
					pos++;
					remainingBytes--;
					component->maximumBitrate = data[pos];

				}
				components->push_back(component);
			}
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
