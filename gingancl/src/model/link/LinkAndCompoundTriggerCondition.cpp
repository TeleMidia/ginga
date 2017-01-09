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
#include "gingancl/model/LinkAndCompoundTriggerCondition.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace link {
	LinkAndCompoundTriggerCondition::LinkAndCompoundTriggerCondition() :
		    LinkCompoundTriggerCondition() {

		typeSet.insert("LinkAndCompoundTriggerCondition");
	}

	LinkAndCompoundTriggerCondition::~LinkAndCompoundTriggerCondition() {
		vector<LinkCondition*>::iterator i;
		LinkCondition* l;

		isDeleting = true;
		unsatisfiedConditions.clear();

		i = statements.begin();
		while (i != statements.end()) {
			l = *i;
			if (l != NULL) {
				delete l;
				l = NULL;
			}
			++i;
		}

		statements.clear();
	}

	void LinkAndCompoundTriggerCondition::addCondition(
		    LinkCondition* condition) {

		if (condition == NULL) {
			return;
		}

		if (condition->instanceOf("LinkTriggerCondition")) {
			unsatisfiedConditions.push_back(condition);
			LinkCompoundTriggerCondition::addCondition(condition);

		} else if (condition->instanceOf("LinkStatement")) {
			statements.push_back(condition);

		} else {
			clog << "LinkAndCompoundTriggerCondition::";
			clog << "addCondition Warning! statements ";
			clog << "trying to add a condition !instanceOf(LinkStatement)";
			clog << " and !instanceOf(LinkTriggerCondition)";
			clog << endl;
		}
	}

	void LinkAndCompoundTriggerCondition::conditionSatisfied(
		    void* condition) {

		vector<LinkCondition*>::iterator i;
		i = unsatisfiedConditions.begin();
		while (i != unsatisfiedConditions.end()) {
			if ((*i) == (LinkCondition*)condition) {
				unsatisfiedConditions.erase(i);
				if (unsatisfiedConditions.empty()) {
					break;
				} else {
					i = unsatisfiedConditions.begin();
				}
			} else {
				++i;
			}
		}

		if (unsatisfiedConditions.empty()) {
			for (i = conditions.begin(); i != conditions.end(); ++i) {
				unsatisfiedConditions.push_back(*i);
			}

			for (i = statements.begin(); i != statements.end(); ++i) {
				if (!(((LinkStatement*)(*i))->evaluate())) {
					notifyConditionObservers(
						    LinkTriggerListener::EVALUATION_ENDED);

					return;
				}
			}

			LinkTriggerCondition::conditionSatisfied(condition);

		} else {
			notifyConditionObservers(LinkTriggerListener::EVALUATION_ENDED);
		}
	}

	vector<FormatterEvent*>* LinkAndCompoundTriggerCondition::getEvents() {
		vector<FormatterEvent*>* events;
		vector<FormatterEvent*>* eventsToAdd;

		vector<LinkCondition*>::iterator i;
		vector<FormatterEvent*>::iterator j;

		events = LinkCompoundTriggerCondition::getEvents();
		if (events != NULL) {
			for (i = statements.begin(); i != statements.end(); ++i) {
				eventsToAdd = (*i)->getEvents();
				if (eventsToAdd != NULL) {
					for (j = eventsToAdd->begin();
						    j != eventsToAdd->end(); ++j) {

						events->push_back(*j);
					}
					delete eventsToAdd;
					eventsToAdd = NULL;
				}
			}
		}

		return events;
	}
}
}
}
}
}
}
}
