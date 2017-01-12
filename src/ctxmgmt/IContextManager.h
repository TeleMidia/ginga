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

#ifndef _IContextManager_H_
#define _IContextManager_H_

#include "ISystemInfo.h"
#include "IGingaUser.h"

#include "IContextListener.h"

#include <map>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace contextmanager {
  class IContextManager {
	public:
		virtual ~IContextManager(){};
		virtual void addContextVar(
				int userId, string varName, string varValue)=0;

		virtual void addUser(IGingaUser* newUser)=0;
		virtual void saveUsersAccounts()=0;
		virtual void saveUsersProfiles()=0;

		virtual void setCurrentUserId(int userId)=0;
		virtual int getCurrentUserId()=0;
		virtual IGingaUser* getUser(int userId)=0;
		virtual map<string, string>* getUserProfile(int userId)=0;
		virtual map<string, string>* getUsersNames()=0;
		virtual ISystemInfo* getSystemInfo()=0;
		virtual void addContextListener(IContextListener* listener)=0;
		virtual void removeContextListener(IContextListener* listener)=0;
		virtual void setGlobalVar(string varName, string varValue)=0;
  };
}
}
}
}
}
}

typedef ::br::pucrio::telemidia::ginga::core::contextmanager::IContextManager*
		ContextManagerCreator();

typedef void ContextManagerDestroyer(
		::br::pucrio::telemidia::ginga::core::contextmanager::IContextManager*
		cm);

#endif /*_IContextManager_H_*/
