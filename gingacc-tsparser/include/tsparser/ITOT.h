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

#ifndef ITOT_H_
#define ITOT_H_

#include "IMpegDescriptor.h"
using namespace br::pucrio::telemidia::ginga::core::tsparser;

#include <string>
#include <vector>
using namespace std;

#include <time.h>

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {
namespace si {
	class ITOT {
		public:
			static const unsigned char DT_LOCAL_TIME_OFFSET = 0x58;
		public:
			virtual ~ITOT(){};
			virtual struct tm getUTC3TimeTm()=0;
			virtual void process(void* payload, unsigned int payloadSize)=0;
			virtual void print()=0;
			virtual vector<IMpegDescriptor*>* getDescriptors()=0;
	};
}
}
}
}
}
}
}

typedef ::br::pucrio::telemidia::ginga::core::tsparser::si::ITOT* TOTCreator();

typedef void TOTDestroyer(
		::br::pucrio::telemidia::ginga::core::tsparser::si::ITOT* tot);

#endif /* ITOT_H_ */
