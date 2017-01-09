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

#include "tsparser/LDTLinkageDescriptor.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {
namespace si {
namespace descriptors {
//TODO: test this class - no use of this descriptor on TS files
	LDTLinkageDescriptor::LDTLinkageDescriptor() {
		descriptorTag      = 0xDC;
		descriptorLength   = 0;
		descriptions       = NULL;
		originalNetworkdId = 0;
		originalServiceId  = 0;
		transportStreamId  = 0;
	}

	LDTLinkageDescriptor::~LDTLinkageDescriptor() {
		if(descriptions != NULL){
			for(int i = 0; i < descriptorLength; ++i){
				if((*descriptions)[i] != NULL) {
					delete (*descriptions)[i];
				}
				delete descriptions;
				descriptions = NULL;
			}
		}
	}
	unsigned char LDTLinkageDescriptor::getDescriptorTag(){
		return descriptorTag;
	}
	unsigned int LDTLinkageDescriptor::getDescriptorLength(){
		return (unsigned int)descriptorLength;
	}
	vector<Description*>* LDTLinkageDescriptor::getDescriptions() {
		return descriptions;
	}

	unsigned char LDTLinkageDescriptor::getDescriptorType(struct Description*
			description) {

		return (description->descriptorType);
	}
	unsigned char LDTLinkageDescriptor::getUserDefined(struct Description*
			description) {

		return (description->userDefined);
	}
	/*
	unsigned char* LDTLinkageDescriptor::getAllDescriptorType() {
    	unsigned char* allDescriptorType;

    	if(descriptionsLength == 0){
    		return NULL;
    	}
    	allDescriptorType = new unsigned char[descriptionsLength];
    	for(int i = 0; i < descriptionsLength; ++i){
    		allDescriptorType[i] = ((Description*)
    				(description[i]))->descriptorType;
    	}

    	return allDescriptorType;
	}
    unsigned char* LDTLinkageDescriptor::getAllUserDefined(){
    	unsigned char* allUserDefined;

    	if(descriptionsLength == 0){
    		return NULL;
    	}
    	allUserDefined = new unsigned char[descriptionsLength];
    	for(int i = 0; i < descriptionsLength; ++i){
    		allUserDefined[i] = ((Description*)(description[i]))->userDefined;
    	}

    	return allUserDefined;
    }
    */
	void LDTLinkageDescriptor::print(){
		clog << "LDTLinkageDescriptor::print..."<< endl;
	}
	size_t LDTLinkageDescriptor::process(char* data, size_t pos){
		struct Description* description;
		unsigned char descriptionsLength;

		descriptorLength = data[pos+1];
		pos += 2;

		originalServiceId = (((data[pos] << 8) & 0xFF00) ||
				(data[pos+1] & 0xFF));
		pos += 2;

		transportStreamId = (((data[pos] << 8) & 0xFF00) ||
				(data[pos+1] & 0xFF));
		pos += 2;

		originalNetworkdId = (((data[pos] << 8) & 0xFF00) ||
				(data[pos+1] & 0xFF));
		pos ++;

		descriptionsLength = (descriptorLength - 6) / 4;
		for(int i = 0; i < descriptionsLength; i++){
			if(descriptions == NULL){
				//descriptions = new struct Description[descriptionCount];
				descriptions = new vector<Description*>;
			}
			description = new Description;

			pos++;
			description->descriptionId = (((data[pos] << 8) & 0xFF00) ||
					(data[pos+1] & 0xFF));
			pos += 2;
			description->descriptorType = (data[pos] & 0x0F);

			pos ++;
			description->userDefined = data[pos];

			//descriptions[i] =  description;
			descriptions->push_back(description);
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
