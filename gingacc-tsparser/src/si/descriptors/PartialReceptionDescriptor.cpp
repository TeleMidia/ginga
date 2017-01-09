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

#include "tsparser/PartialReceptionDescriptor.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {
namespace si {
namespace descriptors {
//TODO: test this class - no use of this descriptor on TS files
	PartialReceptionDescriptor::PartialReceptionDescriptor() {
		descriptorTag    = 0xFB;
		descriptorLength = 0;
		services         = NULL;
	}

	PartialReceptionDescriptor::~PartialReceptionDescriptor() {
		if (services != NULL){
			delete services;
			services == NULL;
		}
	}

	unsigned int PartialReceptionDescriptor::getDescriptorLength() {
		return descriptorLength;
	}

	unsigned char PartialReceptionDescriptor::getDescriptorTag() {
		return descriptorTag;
	}

	void PartialReceptionDescriptor::print() {
		clog << "ParentalRatingDescriptor::print printing..." << endl;
	}

	size_t PartialReceptionDescriptor::process(char* data, size_t pos) {
		size_t servicesNumber;

		descriptorLength = data[pos+1];
		servicesNumber  = descriptorLength / 2;
		pos++;

		services = new unsigned char[servicesNumber];
		for (int i = 0 ; i < servicesNumber; ++i) {
			pos ++;
			services[i] = (((data[pos] << 8)& 0xff00) ||
					(data[pos+1] & 0xff));
			pos++;
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
