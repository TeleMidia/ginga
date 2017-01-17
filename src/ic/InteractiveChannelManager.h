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

#ifndef _InteractiveChannelManager_H_
#define _InteractiveChannelManager_H_

#include "CurlInteractiveChannel.h"

#include <pthread.h>

#include <set>
#include <string>
#include <map>
#include <iostream>
using namespace std;

BR_PUCRIO_TELEMIDIA_GINGA_CORE_IC_BEGIN

  class InteractiveChannelManager {
	private:
		set<CurlInteractiveChannel*>* ics;
		map<string, CurlInteractiveChannel*>* urisIcs;
		static InteractiveChannelManager* _instance;
		InteractiveChannelManager();

	public:
		~InteractiveChannelManager();
		bool hasInteractiveChannel();
		static InteractiveChannelManager* getInstance();

	private:
		set<CurlInteractiveChannel*>* getInteractiveChannels();

	public:
		CurlInteractiveChannel* createInteractiveChannel(string remoteUri);
		void releaseInteractiveChannel(CurlInteractiveChannel* ic);
		CurlInteractiveChannel* getInteractiveChannel(string remoteUri);
		void clearInteractiveChannelManager();

	private:
		void releaseInteractiveChannels();

	public:
		void performPendingUrls();

	private:
		static void* asyncPerform(void* thiz);
  };

BR_PUCRIO_TELEMIDIA_GINGA_CORE_IC_END
#endif /*_InteractiveChannelManager_H_*/
