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

#ifndef _APPLICATIONEXECUTIONOBJECT_H_
#define _APPLICATIONEXECUTIONOBJECT_H_

#include "CompositeExecutionObject.h"
#include "ExecutionObject.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace components {
  class ApplicationExecutionObject : public ExecutionObject {
    private:
    	map<string, FormatterEvent*> preparedEvents;
    	FormatterEvent* currentEvent;
    	pthread_mutex_t eventsMutex;

	public:
		ApplicationExecutionObject(
				string id,
				Node* node,
				bool handling,
				ILinkActionListener* seListener);

		ApplicationExecutionObject(
				string id,
				Node* node,
				GenericDescriptor* descriptor,
				bool handling,
				ILinkActionListener* seListener);

		ApplicationExecutionObject(
				string id,
				Node* node,
				CascadingDescriptor* descriptor,
				bool handling,
				ILinkActionListener* seListener);

		virtual ~ApplicationExecutionObject();

	private:
		void initializeApplicationObject();

	public:
		bool isSleeping();
		bool isPaused();
		FormatterEvent* getCurrentEvent();
		bool hasPreparedEvent(FormatterEvent* event);
		void setCurrentEvent(FormatterEvent* event);
		bool prepare(FormatterEvent* event, double offsetTime);
		bool start();
		EventTransition* getNextTransition();
		bool stop();
		bool abort();
		bool pause();
		bool resume();
		bool unprepare();

	private:
		void unprepareEvents();
		void removeEventListeners();
		void removeParentObject(Node* parentNode, void* parentObject);
		void removeParentListenersFromEvent(FormatterEvent* event);
		void lockEvents();
		void unlockEvents();
  };
}
}
}
}
}
}
}

#endif //_APPLICATIONEXECUTIONOBJECT_H_
