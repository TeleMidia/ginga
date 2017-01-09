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

#include "tsparser/HiperlinkDescriptor.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {
namespace si {
namespace descriptors {
//TODO: test this class - no use of this descriptor on TS files
	HiperlinkDescriptor::HiperlinkDescriptor() {
		descriptorTag    = 0xC5;
		descriptorLength = 0;
		selectorByte     = NULL;
		selectorLength   = 0;
	}

	HiperlinkDescriptor::~HiperlinkDescriptor() {
		if(selectorByte != NULL){
			delete selectorByte;
			selectorByte = NULL;
		}
	}
	unsigned char HiperlinkDescriptor::getDescriptorTag(){
		return descriptorTag;
	}
	unsigned int HiperlinkDescriptor::getDescriptorLength(){
		return (unsigned int)descriptorLength;
	}
	void HiperlinkDescriptor::print(){
		clog << "HiperlinkDescriptor::print..."<< endl;
	}
	size_t HiperlinkDescriptor::process(char* data, size_t pos){
		descriptorLength = data[pos+1];
		pos += 2;

		hyperLinkageType =  data[pos];
		pos++;

		linkDestinationType = data[pos];
		pos++;

		selectorLength = data[pos];
		if(selectorLength > 0){
			selectorByte = new char[selectorLength];
			memset(selectorByte, 0, selectorLength);
			memcpy(selectorByte, data+pos+1, selectorLength);
		}
		pos += selectorLength;
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
