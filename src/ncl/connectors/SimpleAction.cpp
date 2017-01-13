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
#include "ncl/connectors/SimpleAction.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace connectors {
	SimpleAction::SimpleAction(string role) :
		    Action(), Role() {

		SimpleAction::setLabel(role);
		qualifier = CompoundAction::OP_PAR;
		repeat = "0";
		repeatDelay = "0";
		value = "";
		animation = NULL;
		typeSet.insert("SimpleAction");
		typeSet.insert("Role");
	}

	SimpleAction::~SimpleAction() {
		if (animation != NULL) {
			delete animation;
			animation = NULL;
		}
	}

	void SimpleAction::setLabel(string id) {
		label = id;
		string upLabel = upperCase(label);

		if (upLabel == upperCase("start")) {
			actionType = ACT_START;
			eventType = EventUtil::EVT_PRESENTATION;

		} else if (upLabel == upperCase("stop")) {
			actionType = ACT_STOP;
			eventType = EventUtil::EVT_PRESENTATION;

		} else if (upLabel == upperCase("set")) {
			actionType = ACT_START;
			eventType = EventUtil::EVT_ATTRIBUTION;

		} else if (upLabel == upperCase("abort")) {
			actionType = ACT_ABORT;
			eventType = EventUtil::EVT_PRESENTATION;

		} else if (upLabel == upperCase("pause")) {
			actionType = ACT_PAUSE;
			eventType = EventUtil::EVT_PRESENTATION;

		} else if (upLabel == upperCase("resume")) {
			actionType = ACT_RESUME;
			eventType = EventUtil::EVT_PRESENTATION;
		}
	}

	short SimpleAction::getQualifier() {
		return qualifier;
	}

	void SimpleAction::setQualifier(short qualifier) {
		this->qualifier = qualifier;
	}

	string SimpleAction::getRepeat() {
		return repeat;
	}

	string SimpleAction::getRepeatDelay() {
		return repeatDelay;
	}

	void SimpleAction::setRepeatDelay(string time) {
		repeatDelay = time;
	}

	void SimpleAction::setRepeat(string newRepetitions) {
		repeat = newRepetitions;
	}

	short SimpleAction::getActionType() {
		return actionType;
	}

	void SimpleAction::setActionType(short action) {
		actionType = action;
	}

	string SimpleAction::getValue() {
		return value;
	}

	void SimpleAction::setValue(string value) {
		this->value = value;
	}

	Animation* SimpleAction::getAnimation() {
		return animation;
	}

	void SimpleAction::setAnimation(Animation* animation) {
		this->animation = animation;
	}
}
}
}
}
}
