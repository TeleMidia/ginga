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

#ifndef IAIT_H_
#define IAIT_H_

#include "IMpegDescriptor.h"
using namespace br::pucrio::telemidia::ginga::core::tsparser;

#include "IApplication.h"

#include <string>
#include <vector>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {
namespace si {
	class IAIT {
		public:
			virtual ~IAIT(){};
			virtual string getSectionName()=0;
			virtual void setSectionName(string secName)=0;
			virtual void setApplicationType(unsigned int type)=0;
			virtual void process(void* payload, unsigned int payloadSize)=0;
			virtual vector<IMpegDescriptor*>* copyDescriptors()=0;
			virtual vector<IApplication*>* copyApplications()=0;
	};
}
}
}
}
}
}
}

typedef ::br::pucrio::telemidia::ginga::core::tsparser::si::IAIT* AITCreator();

#endif /* IAIT_H_ */
