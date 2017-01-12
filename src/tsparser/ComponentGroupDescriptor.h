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

#ifndef COMPONENTGROUPDESCRIPTOR_H_
#define COMPONENTGROUPDESCRIPTOR_H_

#include "IMpegDescriptor.h"
using namespace ::br::pucrio::telemidia::ginga::core::tsparser;

#include <string.h>

struct CAUnit{
	unsigned char CAUnitId;
	unsigned numOfComponent;
	unsigned char* components;
};

struct ComponentGroup{
	unsigned char componentGroupId;
	unsigned char numOfCAUnit;
	struct CAunit* units;
	unsigned char totalBitRate;
	unsigned char textLength;
	char* textChar;
};

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {
namespace si {
namespace descriptors {

	class ComponentGroupDescriptor : public IMpegDescriptor{
		protected:
			unsigned char componentGroupByte;
			unsigned char totalBitRateFlag;
			unsigned char numOfGroup;
			ComponentGroup* group;
		public:
			ComponentGroupDescriptor();
			virtual ~ComponentGroupDescriptor();
			unsigned char getDescriptorTag();
			unsigned int getDescriptorLength();
			size_t process(char* data, size_t pos);
			void print ();

		};

}
}
}
}
}
}
}
}

#endif /* COMPONENTGROUPDESCRIPTOR_H_ */
