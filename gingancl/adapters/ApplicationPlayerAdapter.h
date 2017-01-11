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

#ifndef _APPLICATIONPLAYERADAPTER_H_
#define _APPLICATIONPLAYERADAPTER_H_

#include "gingancl/model/ApplicationExecutionObject.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::components;

#include "FormatterPlayerAdapter.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::adapters;

#include "player/IPlayerListener.h"
using namespace ::br::pucrio::telemidia::ginga::core::player;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace adapters {
namespace application {

	typedef struct {
		short code;
		string param;
		short type;
		string value;
	} ApplicationStatus;

	class ApplicationPlayerAdapter :
			public FormatterPlayerAdapter, public Thread {

		private:
			IPlayerListener* editingCommandListener;
			pthread_mutex_t eventMutex;
			pthread_mutex_t eventsMutex;
			bool running;

		protected:
			vector<ApplicationStatus*> notes;
			map<string, FormatterEvent*> preparedEvents;
			FormatterEvent* currentEvent;

		public:
			ApplicationPlayerAdapter();
			virtual ~ApplicationPlayerAdapter();

			void setNclEditListener(IPlayerListener* listener);

		protected:
			void checkPlayerSurface(ExecutionObject* obj);
			virtual void createPlayer();

			virtual bool hasPrepared();
			virtual bool prepare(
					ExecutionObject* object, FormatterEvent* mainEvent);

		private:
			void prepare(FormatterEvent* event);

		public:
			virtual bool start();
			virtual bool stop();
			virtual bool pause();
			virtual bool resume();
			virtual bool abort();

		private:
			virtual bool unprepare();

		public:
			virtual void naturalEnd();
			virtual void updateStatus(
					short code,
					string parameter="",
					short type=10,
					string value="");

		private:
			void notificationHandler(
					short code,
					string parameter,
					short type,
					string value);

			void run();

		public:
			virtual bool setAndLockCurrentEvent(FormatterEvent* event)=0;
			virtual void unlockCurrentEvent(FormatterEvent* event)=0;

		private:
			bool checkEvent(FormatterEvent* event, short type);

		protected:
			bool startEvent(string anchorId, short type, string value);
			bool stopEvent(string anchorId, short type, string value);
			bool abortEvent(string anchorId, short type);
			bool pauseEvent(string anchorId, short type);
			bool resumeEvent(string anchorId, short type);

			void lockEvent();
			void unlockEvent();

			void lockPreparedEvents();
			void unlockPreparedEvents();

		public:
			virtual void flip(){};
	};
}
}
}
}
}
}
}

#endif //_ApplicationPlayerAdapter_H_
