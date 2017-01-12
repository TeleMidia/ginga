/* Copyright (C) 2006-2017 PUC-Rio/Laboratorio TeleMidia

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

#ifndef CONTENTDESCRIPTOR_H_
#define CONTENTDESCRIPTOR_H_

#include "IMpegDescriptor.h"
using namespace br::pucrio::telemidia::ginga::core::tsparser;

#include <string.h>

#include <vector>
using namespace std;

struct Content {
	unsigned char contentNibbleLevel1;
	unsigned char contentNibbleLevel2;
	unsigned char userNibble1;
	unsigned char userNibble2;
};

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {
namespace si{
namespace descriptors{
	class ContentDescriptor : public IMpegDescriptor {
		protected:
			//unsigned char contentNibbleLevel1;
			//unsigned char contentNibbleLevel2;
			//unsigned char userNibble1;
			//unsigned char userNibble2;
			vector<Content*>* contents;
		public:
			ContentDescriptor();
			virtual ~ContentDescriptor();
			unsigned char getDescriptorTag();
			unsigned int getDescriptorLength();
			unsigned short getContentNibble1(struct Content* content);
			unsigned short getContentNibble2(struct Content* content);
			unsigned short getUserNibble1(struct Content* content);
			unsigned short getUserNibble2(struct Content* content);
			vector<Content*>* getContents();
			void print();
			size_t process (char* data, size_t pos);
	};

}
}
}
}
}
}
}
}
#endif /* CONTENTDESCRIPTOR_H_ */
