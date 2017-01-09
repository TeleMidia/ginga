/* Copyright (C) 1989-2017 PUC-Rio/Laboratorio TeleMidia

This file is part of Ginga (Ginga-NCL).

Ginga is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

Ginga is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
License for more details.

You should have received a copy of the GNU General Public License
along with Ginga.  If not, see <http://www.gnu.org/licenses/>.  */

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
		//TODO: WORSE CODE EVER MADE: FIX IT
/*		if (group != NULL) {
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
		}*/
	}
	unsigned char ComponentGroupDescriptor::getDescriptorTag(){
		return descriptorTag;
	}
	unsigned int ComponentGroupDescriptor::getDescriptorLength(){
		return (unsigned int)descriptorLength;
	}
	void ComponentGroupDescriptor::print(){
		clog << "ComponentGroupDescriptor::print..."<< endl;
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
			//TODO: WORSE CODE EVER MADE: FIX IT
//			cg->units =  new struct Unit[cg->numOfCAUnit];
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
//TODO: WORSE CODE EVER MADE: FIX IT
//				cg->units[j] = unit;
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
				//TODO: WORSE CODE EVER MADE: FIX IT
//				group[i] = cg;
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
