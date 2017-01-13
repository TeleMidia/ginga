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
#include "LinkCompoundTriggerCondition.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace link {
	LinkCompoundTriggerCondition::LinkCompoundTriggerCondition() :
		    LinkTriggerCondition() {

		typeSet.insert("LinkCompoundTriggerCondition");
	}

	LinkCompoundTriggerCondition::~LinkCompoundTriggerCondition() {
		vector<LinkCondition*>::iterator i;
		LinkCondition* condition;

		isDeleting = true;
		for (i = conditions.begin(); i != conditions.end(); ++i) {
			condition = (LinkCondition*)(*i);
			if (condition != NULL) {
				delete condition;
				condition = NULL;
			}
		}

		conditions.clear();
	}

	void LinkCompoundTriggerCondition::addCondition(LinkCondition* condition) {
		if (condition == NULL) {
			return;
		}

		conditions.push_back(condition);
		if (condition->instanceOf("LinkTriggerCondition")) {
			((LinkTriggerCondition*)condition)->setTriggerListener(this);
		}
	}

	vector<FormatterEvent*>* LinkCompoundTriggerCondition::getEvents() {
		vector<FormatterEvent*>* condEvents;
		vector<FormatterEvent*>* events;
		vector<LinkCondition*>::iterator i;
		vector<FormatterEvent*>::iterator j;
		LinkCondition* condition;

		if (conditions.empty()) {
			return NULL;
		}

		events = new vector<FormatterEvent*>;

		for (i = conditions.begin(); i != conditions.end(); ++i) {
			condition = (LinkCondition*)(*i);
			condEvents = condition->getEvents();
			if (condEvents != NULL) {
				for (j = condEvents->begin(); j != condEvents->end(); ++j) {
					events->push_back(*j);
				}
				delete condEvents;
				condEvents = NULL;
			}
		}

		if (events->begin() == events->end()) {
			delete events;
			return NULL;
		}

		return events;
	}

	void LinkCompoundTriggerCondition::conditionSatisfied(void *condition) {
		/*int i, size;
		LinkCondition *childCondition;

		size = conditions.size();
		for (i = 0; i < size; i++) {
			childCondition = (LinkCondition*)(*conditions)[i];
			if (childCondition != (LinkCondition*)condition &&
				    childCondition->instanceOf("LinkStatement")) {

				if (op == CompoundCondition::OP_OR) {
					break;
				}
				else {
					if (!((LinkStatement*)childCondition)->evaluate()) {
						return;
					}
				}
			}
		}*/
		LinkTriggerCondition::conditionSatisfied(condition);
	}

	void LinkCompoundTriggerCondition::evaluationStarted() {
		notifyConditionObservers(LinkTriggerListener::EVALUATION_STARTED);
	}

	void LinkCompoundTriggerCondition::evaluationEnded() {
		notifyConditionObservers(LinkTriggerListener::EVALUATION_ENDED);
	}
}
}
}
}
}
}
}
