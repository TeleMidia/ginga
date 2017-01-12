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

#include "LinkTransitionTriggerCondition.h"
#include "ExecutionObject.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::components;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace link {
	LinkTransitionTriggerCondition::LinkTransitionTriggerCondition(
		    FormatterEvent* event,
		    short transition,
			Bind* bind) : LinkTriggerCondition() {

		typeSet.insert("LinkTransitionTriggerCondition");

		this->bind       = bind;
		this->event      = NULL;
		this->transition = transition;

		if (FormatterEvent::hasInstance(event, false)) {
			this->event = event;
			this->event->addEventListener(this);

		} else {
			clog << "LinkTransitionTriggerCondition::";
			clog << "LinkTransitionTriggerCondition Warning! ";
			clog << "creating a link with NULL event" << endl;
		}
	}

	LinkTransitionTriggerCondition::~LinkTransitionTriggerCondition() {
		isDeleting = true;
		listener   = NULL;
		bind       = NULL;

		if (FormatterEvent::hasInstance(event, false)) {
			event->removeEventListener(this);
			event = NULL;
		}
	}

	Bind* LinkTransitionTriggerCondition::getBind() {
		return bind;
	}

	void LinkTransitionTriggerCondition::eventStateChanged(
		    void* someEvent, short transition, short previousState) {

		if (this->transition == transition) {
			notifyConditionObservers(
				    LinkTriggerListener::EVALUATION_STARTED);

			LinkTriggerCondition::conditionSatisfied(this);
		}
	}

	short LinkTransitionTriggerCondition::getPriorityType() {
		return IEventListener::PT_LINK;
	}

	FormatterEvent* LinkTransitionTriggerCondition::getEvent() {
		return event;
	}

	short LinkTransitionTriggerCondition::getTransition() {
		return transition;
	}

	vector<FormatterEvent*>* LinkTransitionTriggerCondition::getEvents() {
		if (!FormatterEvent::hasInstance(event, false)) {
			return NULL;
		}

		vector<FormatterEvent*>* events = new vector<FormatterEvent*>;

		events->push_back(event);
		return events;
	}
}
}
}
}
}
}
}
