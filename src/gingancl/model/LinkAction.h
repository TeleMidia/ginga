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

#ifndef _LINKACTION_H_
#define _LINKACTION_H_

#include "FormatterEvent.h"
#include "AttributionEvent.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::event;

#include "LinkActionProgressionListener.h"

#include "system/Thread.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::thread;

#include "system/SystemCompat.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::compat;

extern "C" {
#include <pthread.h>
}

#include <set>
#include <string>
#include <vector>
#include <iostream>
using namespace std;

BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_LINK_BEGIN

	class LinkAction : public Thread {
		protected:
			set<string> typeSet;
			void* satisfiedCondition;

		private:
			double delay;
			pthread_mutex_t plMutex;
			vector<LinkActionProgressionListener*>* progressionListeners;

		public:
			LinkAction();
			LinkAction(double delay);

		private:
			void initLinkAction(double delay);

		public:
			virtual ~LinkAction();
			bool instanceOf(string s);
			double getWaitDelay();
			void setWaitDelay(double delay);
			bool hasDelay();

			void addActionProgressionListener(
				    LinkActionProgressionListener* listener);

			void removeActionProgressionListener(
				    LinkActionProgressionListener* listener);

			void notifyProgressionListeners(bool start);
			virtual vector<FormatterEvent*>* getEvents()=0;
			virtual vector<LinkAction*>* getImplicitRefRoleActions()=0;

			void setSatisfiedCondition(void* satisfiedCondition);
			void run(void* satisfiedCondition);

		protected:
			virtual void run();

		private:
			bool tryLock();
	};

BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_LINK_END
#endif //_LINKACTION_H_
