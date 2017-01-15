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

#ifndef REMOTEEVENTSERVICE_H_
#define REMOTEEVENTSERVICE_H_

#include "TcpSocketService.h"

#include <pthread.h>
#include <map>
#include <vector>
#include <string>
using namespace std;

#include "util/functions.h"
#include "util/Base64.h"

using namespace ::br::pucrio::telemidia::util;

#include "system/SystemCompat.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;

#include "ctxmgmt/ContextManager.h"
using namespace ::br::pucrio::telemidia::ginga::core::contextmanager;

using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace multidevice {
	class RemoteEventService : public IRemoteDeviceListener {
		private:
			static RemoteEventService* _instance;
			pthread_mutex_t groupsMutex;
			map<int,TcpSocketService*> groups;
			static const int DEFAULT_PORT = 22222;
			static ContextManager* contextManager;
			string base_device_ncl_path;

		public:
			RemoteEventService();
			virtual ~RemoteEventService();

			static RemoteEventService* getInstance();

			void addDeviceClass(unsigned int id);
			void setBaseDeviceNCLPath(string base_device_ncl_path);
			void addDevice(
					unsigned int device_class,
					unsigned int device_id,
					char* addr,
					int srvPort,
					bool isLocalConnection);

			void addDocument(unsigned int device_class, char* name, char* body);
			void startDocument(unsigned int device_class, char* name);
			void stopDocument(unsigned int device_class, char* name);

			bool newDeviceConnected(int newDevClass, int w, int h);
			void connectedToBaseDevice(unsigned int domainAddr);

			bool receiveRemoteContent(
					int remoteDevClass,
					string contentUri);

			bool receiveRemoteContent(
					int remoteDevClass,
					char *stream, int streamSize);

			bool receiveRemoteContentInfo(
					string contentId, string contentUri);

			bool receiveRemoteEvent(
					int remoteDevClass,
					int eventType,
					string eventContent);

	};
}
}
}
}
}
}

#endif /* REMOTEEVENTSERVICE_H_ */
