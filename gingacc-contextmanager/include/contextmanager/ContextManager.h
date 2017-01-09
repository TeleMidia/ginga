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

#ifndef _ContextManager_H_
#define _ContextManager_H_

#include "IContextManager.h"

#include "IContextListener.h"

#include "system/compat/SystemCompat.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;

#include "system/thread/Thread.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::thread;

#include <set>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace contextmanager {
  class ContextManager : public IContextManager {
	private:
		map<int, IGingaUser*> users;
		map<int, map<string, string>*> contexts;
		set<IContextListener*> ctxListeners;
		string usersUri, contextsUri;
		int curUserId;
		ISystemInfo* systemInfo;
		static IContextManager* _instance;
		ContextManager();

		pthread_mutex_t groupsMutex;

	public:
		~ContextManager();
		static IContextManager* getInstance();

	private:
		void initializeUsers();
		void initializeContexts();

	public:
		void addContextVar(int userId, string varName, string varValue);
		void addUser(IGingaUser* newUser);
		void saveUsersAccounts();
		void saveUsersProfiles();
		void addContextListener(IContextListener* listener);
		void removeContextListener(IContextListener* listener);
		void setGlobalVar(string varName, string varValue);

	private:
		void saveProfile(FILE* fd, int userId, map<string, string>* profile);

	public:
		void setCurrentUserId(int userId);
		int getCurrentUserId();
		IGingaUser* getUser(int userId);

	private:
		map<string,string>* getUserMap(int userId);

	public:
		map<string,string>* getUserProfile(int userId);
		map<string, string>* getUsersNames();
		ISystemInfo* getSystemInfo();

	private:
		void listUsersNicks();
  };
}
}
}
}
}
}

#endif /*_ContextManager_H_*/
