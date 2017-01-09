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

#ifndef APPLICATION_H_
#define APPLICATION_H_

#include "IMpegDescriptor.h"
using namespace ::br::pucrio::telemidia::ginga::core::tsparser;

#include "ApplicationDescriptor.h"
#include "ApplicationLocationDescriptor.h"
#include "ApplicationNameDescriptor.h"
#include "PrefetchDescriptor.h"
#include "TransportProtocolDescriptor.h"

#include "IApplication.h"

#include "system/thread/Thread.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::thread;

#include <vector>
#include <iostream>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace tsparser {
namespace si {

	typedef struct {
		unsigned int organizationId;
		unsigned short applicationId;
	} ApplicationIdentifier;

	class Application : public IApplication {
		private:
			ApplicationIdentifier applicationId;
			unsigned short applicationControlCode;
			unsigned short appDescriptorsLoopLength;
			vector<IMpegDescriptor*> descriptors;
			unsigned short applicationLength;
			pthread_mutex_t stlMutex;

		public:
			Application();
			virtual ~Application();

			string getBaseDirectory();
			string getInitialClass();
			string getId();
			unsigned short getControlCode();
			unsigned short getLength();
			unsigned short getProfile(int profileNumber=0);
			unsigned short getTransportProtocolId();
			size_t process(char* data, size_t pos);
		};
}
}
}
}
}
}
}
#endif /* APPLICATION_H_ */
