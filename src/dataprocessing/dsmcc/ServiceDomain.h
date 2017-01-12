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

#ifndef SERVICEDOMAIN_H_
#define SERVICEDOMAIN_H_

#include "system/Thread.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::thread;

#include "system/SystemCompat.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;

#include "DownloadServerInitiate.h"
#include "DownloadInfoIndication.h"
#include "DownloadDataBlock.h"
#include "ObjectProcessor.h"
#include "Module.h"
#include "Biop.h"
#include "IServiceDomainListener.h"

#include <map>
#include <string>
#include <iostream>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace dataprocessing {
namespace carousel {
	class ServiceDomain : public Thread {
		private:
			bool mountingServiceDomain;
			bool hasServiceGateway;
			Ior* serviceGatewayIor;

			unsigned int carouselId;

			//mapping moduleId in module
			map<unsigned int, Module*> info;
			unsigned short blockSize;

			ObjectProcessor* processor;
			string mountPoint;
			IServiceDomainListener* sdl;
			bool mounted;

			pthread_mutex_t stlMutex;

		public:
			ServiceDomain(
					DownloadServerInitiate* dsi, DownloadInfoIndication* dii,
					unsigned short pid);

			virtual ~ServiceDomain();

			void setServiceDomainListener(IServiceDomainListener* sdl);
			void setObjectsListeners(set<IObjectListener*>* l);
			int receiveDDB(DownloadDataBlock* ddb);
			Module* getModuleById(unsigned int id);
			map<unsigned int, Module*>* getInfo();
			unsigned short getBlockSize();

			bool isMounted();

		private:
			Module* getModule(int position);
			void eraseModule(Module* module);
			bool hasModules();

		protected:
			virtual void run();
	};
}
}
}
}
}
}
}

#endif /*SERVICEDOMAIN_H_*/
