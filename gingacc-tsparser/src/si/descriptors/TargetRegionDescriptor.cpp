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

#include "tsparser/TargetRegionDescriptor.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {
namespace si {
namespace descriptors {
	TargetRegionDescriptor::TargetRegionDescriptor() {
		descriptorLength = 0xC6;
		descriptorTag    = 0;
		regionSpecType   = 0;

	}

	TargetRegionDescriptor::~TargetRegionDescriptor() {

	}

	unsigned char TargetRegionDescriptor::getDescriptorTag() {
		return descriptorTag;
	}

	unsigned int TargetRegionDescriptor::getDescriptorLength() {
		return descriptorLength;
	}

	void TargetRegionDescriptor::print() {
		clog << "TargetRegionDescriptor::print regionSpecType = " <<
			(regionSpecType & 0xFF) << endl;
	}

	size_t TargetRegionDescriptor::process(char* data, size_t pos) {
		descriptorLength =  data[pos+1];
		pos += 2;
		regionSpecType = data[pos];

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
