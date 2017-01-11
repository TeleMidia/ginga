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

#ifndef DSMCCSectionPayload_H_
#define DSMCCSectionPayload_H_

#include <string.h>
#include <stdint.h>

#include "Crc.h"
#include "MpegDescriptor.h"

#include "NPTReference.h"
#include "NPTEndpoint.h"
#include "StreamMode.h"

#include <map>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace dataprocessing {
namespace dsmcc {
namespace npt {

class DSMCCSectionPayload {
	protected:
		unsigned short privateDataLength;
		char* privateDataByte;

		vector<MpegDescriptor*>* dsmccDescritorList;
		unsigned int checksum;

		char* payload;
		unsigned int payloadSize;

		int processSectionPayload();
		int updateStream();
		int calculateSectionSize();

		void clearDsmccDescritor();
		void deleteDescriptor(MpegDescriptor* desc);

		/*
		 * if (tableId == 3D) {
		 *     tableIdExtension = dataEventId & eventMsgGroupId
		 * }
		 */
	public:
		DSMCCSectionPayload(char* data, unsigned int length);
		virtual ~DSMCCSectionPayload();

		vector<MpegDescriptor*>* getDsmccDescritorList();
		unsigned int getChecksum();
		void setChecksum(unsigned int cs);
		int getPrivateDataByte(char** dataStream);
		int setPrivateDataByte(char* data, unsigned short length);

		void addDsmccDescriptor(MpegDescriptor* d);
		void removeDsmccDescriptor(unsigned char descriptorTag);
};

}
}
}
}
}
}
}
}

#endif /* DSMCCSectionPayload_H_ */
