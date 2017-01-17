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

#ifndef EventTransitionManager_H_
#define EventTransitionManager_H_

#include "system/Thread.h"
using namespace ::br::pucrio::telemidia::ginga::core::system::thread;

#include "ncl/interfaces/RelativeTimeIntervalAnchor.h"
#include "ncl/interfaces/SampleIntervalAnchor.h"
using namespace ::br::pucrio::telemidia::ncl::interfaces;

#include "ncl/connectors/EventUtil.h"
using namespace ::br::pucrio::telemidia::ncl::connectors;

#include "player/IPlayer.h"
using namespace ::br::pucrio::telemidia::ginga::core::player;

#include "PresentationEvent.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::event;

#include "BeginEventTransition.h"
#include "EndEventTransition.h"

#include <map>
#include <vector>
using namespace std;

BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_EVENT_TRANSITION_BEGIN

  class EventTransitionManager {
	private:
		map<short int, int> currentTransitionIndex;
		map<short int, int> startTransitionIndex;
		map<short int, vector<EventTransition*>*> transTable;
		pthread_mutex_t transMutex;

	public:
		EventTransitionManager();
		virtual ~EventTransitionManager();

	private:
		short int getType(PresentationEvent* event);
		vector<EventTransition*>* getTransitionEvents(short int type);

	public:
		void addPresentationEvent(PresentationEvent* event);

	private:
		void addEventTransition(EventTransition* transition, short int type);

	public:
		void removeEventTransition(PresentationEvent* event);

		void resetTimeIndex();
		void resetTimeIndexByType(short int type);
		void prepare(bool wholeContent, double startTime, short int type);
		void start(double offsetTime);
		void stop(double endTime, bool applicationType=false);
		void abort(double endTime, bool applicationType=false);

		void timeBaseNaturalEnd(
				int64_t timeValue,
				FormatterEvent* mainEvent,
				short int transType);

		void updateTransitionTable(
				double timeValue,
				IPlayer* player,
				FormatterEvent* mainEvent,
				short int transType);

		set<double>* getTransitionsValues(short int transType);
		EventTransition* getNextTransition(FormatterEvent* mainEvent);
  };

BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_EVENT_TRANSITION_END
#endif /*EventTransitionManager_H_*/
