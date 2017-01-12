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

#include "ExecutionObjectSwitch.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace switches {
	ExecutionObjectSwitch::ExecutionObjectSwitch(
		    string id,
		    Node* switchNode,
		    bool handling,
		    ILinkActionListener* seListener) : CompositeExecutionObject(
		    		id, switchNode, handling, seListener) {

		selectedObject = NULL;
		typeSet.insert("ExecutionObjectSwitch");
	}

	ExecutionObject* ExecutionObjectSwitch::getSelectedObject() {
		return selectedObject;
	}

	void ExecutionObjectSwitch::select(ExecutionObject* executionObject) {
		vector<FormatterEvent*>* eventsVector;
		vector<FormatterEvent*>::iterator i;
		SwitchEvent* switchEvent;

		if (executionObject != NULL &&
				containsExecutionObject(executionObject->getId())) {

			clog << "ExecutionObjectSwitch::select '";
			clog << executionObject->getId() << "'" << endl;

			selectedObject = executionObject;

		} else {
			selectedObject = NULL;
			eventsVector = getEvents();
			if (eventsVector != NULL) {
				i = eventsVector->begin();
				while (i != eventsVector->end()) {
					switchEvent = (SwitchEvent*)(*i);
					switchEvent->setMappedEvent(NULL);
					++i;
				}
				delete eventsVector;
				eventsVector = NULL;
			}
		}
	}

	bool ExecutionObjectSwitch::addEvent(FormatterEvent* event) {
		if (event->instanceOf("PresentationEvent") &&
				(((PresentationEvent*)event)->getAnchor())->
					    instanceOf("LambdaAnchor")) {

			ExecutionObject::wholeContent = (PresentationEvent*)event;
			return true;

		} else {
			return ExecutionObject::addEvent(event);
		}
	}
}
}
}
}
}
}
}
