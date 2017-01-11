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

#include "model/LinkAssignmentAction.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace link {
	LinkAssignmentAction::LinkAssignmentAction(
			FormatterEvent* event, short actionType, string value) :
					LinkRepeatAction(event, actionType) {

		this->value     = value;
		this->animation = NULL;
		typeSet.insert("LinkAssignmentAction");
	}

	LinkAssignmentAction::~LinkAssignmentAction() {
		isDeleting = true;

		if (animation != NULL) {
			delete animation;
			animation = NULL;
		}
	}

	string LinkAssignmentAction::getValue() {
		return value;
	}

	void LinkAssignmentAction::setValue(string value) {
		this->value = value;
	}

	Animation* LinkAssignmentAction::getAnimation() {
		return animation;
	}

	void LinkAssignmentAction::setAnimation(Animation* animation) {
		this->animation = animation;
	}
}
}
}
}
}
}
}
