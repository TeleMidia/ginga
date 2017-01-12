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

#ifndef _PRESENTATIONCONTEXT_H_
#define _PRESENTATIONCONTEXT_H_

#include "util/Observable.h"

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "ctxmgmt/IContextManager.h"
#include "ctxmgmt/ISystemInfo.h"
using namespace ::br::pucrio::telemidia::ginga::core::contextmanager;

#include "config.h"

#if HAVE_MULTIDEVICE
# include "multidevice/IDeviceDomain.h"
using namespace ::br::pucrio::telemidia::ginga::core::multidevice;
#endif

#include "system/Thread.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::thread;

#include "ContextBase.h"
#include "IPresentationContext.h"

#include "math.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace adaptation {
namespace context {
	class PresentationContext : public IPresentationContext,
				public Observable, public IContextListener {

		private:
			GingaScreenID myScreen;
			map<string, string> contextTable;
			static IContextManager* contextManager;
			IContextListener* globalVarListener;
			pthread_mutex_t attrMutex;

#if HAVE_MULTIDEVICE
			IRemoteDeviceListener* devListener;
#else
			void* devListener;
#endif //HAVE_MULTIDEVICE

		public:
			PresentationContext(GingaScreenID screenId);
			virtual ~PresentationContext();

			void setPropertyValue(string propertyName, string value);
			void incPropertyValue(string propertyName);
			void decPropertyValue(string propertyName);
			set<string>* getPropertyNames();
			string getPropertyValue(string attributeId);

		private:
			void initializeUserContext();
			void initializeUserInfo(int currentUserId);
			void initializeSystemValues();

		public:
			void save();
			void setGlobalVarListener(IContextListener* listener);
			void receiveGlobalAttribution(string propertyName, string value);

#if HAVE_MULTIDEVICE
			void setRemoteDeviceListener(IRemoteDeviceListener* rdl);
#endif //HAVE_MULTIDEVICE
	};
}
}
}
}
}
}
}

#endif //_PRESENTATIONCONTEXT_H_
