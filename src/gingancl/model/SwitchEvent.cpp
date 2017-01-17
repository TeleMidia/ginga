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

#include "config.h"
#include "SwitchEvent.h"

BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_SWITCHES_BEGIN

	SwitchEvent::SwitchEvent(
		    string id,
		    void* executionObjectSwitch,
		    InterfacePoint* interfacePoint,
		    int eventType,
		    string key) : FormatterEvent(id, executionObjectSwitch) {

		this->interfacePoint = interfacePoint;
		this->eventType      = eventType;
		this->key            = key;
		this->mappedEvent    = NULL;

		typeSet.insert("SwitchEvent");
	}

	SwitchEvent::~SwitchEvent() {
		if (FormatterEvent::hasInstance(mappedEvent, false)) {
			mappedEvent->removeEventListener(this);
			mappedEvent = NULL;
		}
	}

	InterfacePoint* SwitchEvent::getInterfacePoint() {
		return interfacePoint;
	}

	short SwitchEvent::getEventType() {
		return eventType;
	}

	string SwitchEvent::getKey() {
		return key;
	}

	void SwitchEvent::setMappedEvent(FormatterEvent* event) {
		if (mappedEvent != NULL) {
			mappedEvent->removeEventListener(this);
		}

		mappedEvent = event;
		if (mappedEvent != NULL) {
			mappedEvent->addEventListener(this);
		}
	}

	FormatterEvent* SwitchEvent::getMappedEvent() {
		return mappedEvent;
	}

	void SwitchEvent::eventStateChanged(
		    void* someEvent, short transition, short previousState) {

		changeState(getNewState(transition), transition);
	}

	short SwitchEvent::getPriorityType() {
		return IEventListener::PT_LINK;
	}

BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_SWITCHES_END
