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

#ifndef MPEGDESCRIPTOR_H_
#define MPEGDESCRIPTOR_H_

#include <string.h>
#include <iostream>
#include <vector>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace dataprocessing {
namespace dsmcc {
namespace npt {

class MpegDescriptor {
	public:
		static const unsigned short MAX_DESCRIPTOR_SIZE = 257;

	private:

	protected:
		unsigned char descriptorTag;
		unsigned char descriptorLength;
		char* stream;

		unsigned short currentSize;

		virtual int process();
		virtual int updateStream();

		virtual unsigned int calculateDescriptorSize();

	public:
		MpegDescriptor();
		MpegDescriptor(unsigned char tag);
		virtual ~MpegDescriptor();

		char addData(char* data, unsigned short length);
		unsigned char isConsolidated();

		unsigned char getDescriptorTag();
		unsigned char getDescriptorLength();

		int getStreamSize();
		int getStream(char** dataStream);

		void setDescriptorTag(unsigned char tag);

		static MpegDescriptor* getDescriptor(
				vector<MpegDescriptor*>* descriptors, unsigned char Tag);

		static vector<MpegDescriptor*>* getDescriptors(
				vector<MpegDescriptor*>* descriptors, unsigned char Tag);

		static int getDescriptorsLength(vector<MpegDescriptor*>* descriptors);
};

}
}
}
}
}
}
}
}

#endif /*MPEGDESCRIPTOR_H_*/
