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

#include "tsparser/ComponentGroupDescriptor.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {
namespace si {
namespace descriptors {
//TODO: test this class - no use of this descriptor on TS files
	ComponentGroupDescriptor::ComponentGroupDescriptor() {
		descriptorTag    = 0xD9;
		descriptorLength = 0;
		group            = NULL;
		numOfGroup       = 0;
		totalBitRateFlag = 0;
	}
	ComponentGroupDescriptor::~ComponentGroupDescriptor() {
		ComponentGroup* cg;
		CAUnit* unit;

		if (group != NULL) {
			for (int i = 0; i < numOfGroup; i++) {
				cg = (ComponentGroup*)group[i];
				for (int j = 0; j < cg->numOfCAUnit; i++) {
					unit = (CAUnit*)cg->units[i];
					delete unit->components;
					unit->components == NULL;
					delete unit;
					unit = NULL;
				}
				delete (cg->units);
				cg->units == NULL;
				delete cg;
			}
			delete group;
			group = NULL;
		}
	}
	unsigned char ComponentGroupDescriptor::getDescriptorTag(){
		return descriptorTag;
	}
	unsigned int ComponentGroupDescriptor::getDescriptorLength(){
		return (unsigned int)descriptorLength;
	}
	void ComponentGroupDescriptor::print(){
		cout << "ComponentGroupDescriptor::print..."<< endl;
	}
	size_t ComponentGroupDescriptor::process(char* data, size_t pos){
		struct ComponentGroup* cg;
		struct CAUnit* unit;

		descriptorLength = data[pos+1];
		pos += 2;

		componentGroupByte = ((data[pos] >> 5) & 0x07);
		totalBitRateFlag = ((data[pos] >> 4) & 0x01);
		numOfGroup = (data[pos] & 0x0F);
		/* with numOfGroup == 0, there is no more data to read; last byte
		* is pos (componentGroupByte, totalBitRateFlag and numOfGroup).

	     /* group contains at least 1 componentGroup, witch contains at least 1
		 *  CAUnit, witch contains at least 1 componentTag
		 */
		for(int i = 0; i < numOfGroup; i++){
			pos++;
			if(group == NULL){
				group = new struct ComponentGroup[numOfGroup];
			}
			cg = new struct ComponentGroup;
			cg->componentGroupId = ((data[pos] >> 4) & 0x0F);
			cg->numOfCAUnit = (data[pos] & 0x0F);
			cg->units =  new struct Unit[cg->numOfCAUnit];
			//pos++;
			for (int j = 0; i < cg->numOfCAUnit; i++){
				pos++;
				unit = new struct CAUnit;
				unit->CAUnitId = ((data[pos] >> 4) & 0x0F);
				unit->numOfComponent = (data[pos] & 0x0F);
				//pos++;
				unit->components = new unsigned char[cg->numOfCAUnit];
				for (int k = 0; i < unit->numOfComponent; i++){
					pos++;
					unit->components[k] = data[pos];
					//pos++;
				}
				cg->units[j] = unit;
			}
			if(totalBitRateFlag == 1){
				pos ++;
				cg->totalBitRate = data[pos];

			}
			pos ++;
			cg->textLength = data[pos];
			if(cg->textLength > 0){
				cg->textChar = new char[cg->textLength];
				memset(cg->textChar, 0, cg->textLength);
				memcpy(cg->textChar, data+pos+1, cg->textLength);
				group[i] = cg;
				pos += cg->textLength;
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
