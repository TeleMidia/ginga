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

#ifndef MOSAICDESCRIPTOR_H_
#define MOSAICDESCRIPTOR_H_

#include "tsparser/IMpegDescriptor.h"
using namespace ::br::pucrio::telemidia::ginga::core::tsparser;

struct Cell{
	unsigned char logicalCellId;
	unsigned char logicalCellPresInfo;
	unsigned char elemCellFieldLength;
	unsigned char* elemCellsId;
	unsigned char cellLinkageInfo;
	unsigned short bouquetId;
	unsigned short originalNetworkId;
	unsigned short transportStreamId;
	unsigned short serviceId;
	unsigned short eventId;

};

#include <vector>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {
namespace si {
namespace descriptors {
	class MosaicDescriptor : public IMpegDescriptor {
		protected:
			unsigned char mosaicEntryPoint;
			unsigned char numberHorizontalCells;
			unsigned char numberVerticalCells;
			vector<Cell*>* cells;

		public:
			MosaicDescriptor();
			virtual ~MosaicDescriptor();
			unsigned char getDescriptorTag();
			unsigned int getDescriptorLength();
			size_t process(char* data, size_t pos);
			void print();
	};

}
}
}
}
}
}
}
}

#endif /* MOSAICDESCRIPTOR_H_ */
