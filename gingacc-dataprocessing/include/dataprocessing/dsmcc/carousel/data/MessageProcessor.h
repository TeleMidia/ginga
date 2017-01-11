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

#ifndef MESSAGEPROCESSOR_H_
#define MESSAGEPROCESSOR_H_

#include "system/Thread.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::thread;

#include "tsparser/ITransportSection.h"
using namespace ::br::pucrio::telemidia::ginga::core::tsparser;

#include "../ServiceDomain.h"
#include "DsmccMessageHeader.h"
#include "DownloadServerInitiate.h"
#include "DownloadInfoIndication.h"
#include "DownloadDataBlock.h"

#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace dataprocessing {
namespace carousel {
	class MessageProcessor {
		private:
			ServiceDomain* sd;
			DownloadServerInitiate* dsi;
			DownloadInfoIndication* dii;
			vector<DsmccMessageHeader*> msgs;
			pthread_mutex_t msgMutex;
			unsigned short pid;

		public:
			MessageProcessor(unsigned short pid);
			virtual ~MessageProcessor();

			ServiceDomain* pushMessage(DsmccMessageHeader* hdr);

		private:
			ServiceDomain* processDSIMessage(DsmccMessageHeader* message);
			ServiceDomain* processDIIMessage(DsmccMessageHeader* message);
			void processDDBMessages();

		public:
			ServiceDomain* getServiceDomain();
			void checkTasks();
	};
}
}
}
}
}
}
}

#endif /*MESSAGEPROCESSOR_H_*/
