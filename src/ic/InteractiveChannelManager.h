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

#ifndef _InteractiveChannelManager_H_
#define _InteractiveChannelManager_H_

#include "IInteractiveChannelManager.h"

#include <pthread.h>

#include <map>
#include <iostream>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace core {
namespace ic {
  class InteractiveChannelManager : public IInteractiveChannelManager {
	private:
		set<IInteractiveChannel*>* ics;
		map<string, IInteractiveChannel*>* urisIcs;
		static InteractiveChannelManager* _instance;
		InteractiveChannelManager();

	public:
		~InteractiveChannelManager();
		bool hasInteractiveChannel();
		static InteractiveChannelManager* getInstance();

	private:
		set<IInteractiveChannel*>* getInteractiveChannels();

	public:
		IInteractiveChannel* createInteractiveChannel(string remoteUri);
		void releaseInteractiveChannel(IInteractiveChannel* ic);
		IInteractiveChannel* getInteractiveChannel(string remoteUri);
		void clearInteractiveChannelManager();

	private:
		void releaseInteractiveChannels();

	public:
		void performPendingUrls();

	private:
		static void* asyncPerform(void* thiz);
  };
}
}
}
}
}
}

#endif /*_InteractiveChannelManager_H_*/
