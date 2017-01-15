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

#ifndef NOMINALEVENTMONITOR_H_
#define NOMINALEVENTMONITOR_H_

#include "system/Thread.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::thread;

#include "system/ITimeBaseListener.h"
#include "system/ITimeBaseProvider.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::time;

#include "player/IPlayer.h"
using namespace ::br::pucrio::telemidia::ginga::core::player;

#include "ncl/interfaces/ContentAnchor.h"
#include "ncl/interfaces/LambdaAnchor.h"
#include "ncl/interfaces/IntervalAnchor.h"
using namespace ::br::pucrio::telemidia::ncl::interfaces;

#include "gingancl/model/PresentationEvent.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::event;

#include "gingancl/model/EventTransition.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::event::transition;

#include "gingancl/model/ExecutionObject.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::components;

#include "IPlayerAdapter.h"

#include <vector>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace adapters {
   class NominalEventMonitor : public Thread, public ITimeBaseListener {
	private:
		double sleepTime;
		double expectedSleepTime;
		ExecutionObject* executionObject;
		IPlayerAdapter* adapter;
		int timeBaseId;
		ITimeBaseProvider* timeBaseProvider;

		static const double DEFAULT_SLEEP_TIME;
		static const double DEFAULT_ERROR;

		bool running;
		bool paused;
		bool stopped;

	public:
		NominalEventMonitor(ExecutionObject* obj, IPlayerAdapter* player);
		virtual ~NominalEventMonitor();

	private:
		void unregisterFromTimeBase();

   public:
		void setTimeBaseProvider(ITimeBaseProvider* timeBaseProvider);

	private:
		void prepareNptTransitionsEvents();

	public:
		void updateTimeBaseId(
				unsigned char oldContentId,
				unsigned char newContentId);

		void valueReached(unsigned char timeBaseId, double timeValue);

		void timeBaseNaturalEnd(unsigned char contentId, double timeValue);

		void loopDetected();

		void startMonitor();
		void pauseMonitor();
		void resumeMonitor();
		void stopMonitor();

		virtual void run();
   };
}
}
}
}
}
}

#endif /*NOMINALEVENTMONITOR_H_*/
