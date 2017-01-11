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

#include "model/FormatterCausalLink.h"

#include "model/CompositeExecutionObject.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::components;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace link {
	FormatterCausalLink::FormatterCausalLink(
		    LinkTriggerCondition* condition,
		    LinkAction* action,
		    Link* ncmLink,
			void* parentObject) :
				FormatterLink(ncmLink, parentObject) {

		typeSet.insert("FormatterCausalLink");
		this->condition = condition;
		this->action = action;

		if (this->condition != NULL) {
			this->condition->setTriggerListener(this);
		}

		if (this->action != NULL) {
			this->action->addActionProgressionListener(this);
		}
	}

	FormatterCausalLink::~FormatterCausalLink() {
		if (condition != NULL) {
			delete condition;
			condition = NULL;
		}

		if (action != NULL) {
			delete action;
			action = NULL;
		}
	}

    LinkAction* FormatterCausalLink::getAction() {
    	return action;
    }

    LinkTriggerCondition* FormatterCausalLink::getTriggerCondition() {
    	return condition;
    }

    void FormatterCausalLink::conditionSatisfied(void* condition) {
    	if (!suspend) {
    		action->run(condition);
    	}
    }

    vector<FormatterEvent*>* FormatterCausalLink::getEvents() {
    	vector<FormatterEvent*>* events;
    	vector<FormatterEvent*>* actEvents;
    	vector<FormatterEvent*>::iterator i;

    	events = condition->getEvents();
		actEvents = action->getEvents();

		if (actEvents == NULL) {
			return events;
		}

    	if (events == NULL) {
    		return actEvents;
    	}

		for (i = actEvents->begin(); i != actEvents->end(); ++i) {
			events->push_back(*i);
		}

		delete actEvents;
		actEvents = NULL;

		if (events->empty()) {
			delete events;
			return NULL;
		}

		return events;
    }

    void FormatterCausalLink::evaluationStarted() {
		/*clog << endl;
		clog << "FormatterCausalLink::evaluationStarted(" << ncmLink->getId();
		clog << ")" << endl << endl;*/
		((CompositeExecutionObject*)parentObject)->linkEvaluationStarted(this);
	}

	void FormatterCausalLink::evaluationEnded() {
		/*clog << endl;
		clog << "FormatterCausalLink::evaluationEnded(" << ncmLink->getId();
		clog << ")" << endl << endl;*/
		((CompositeExecutionObject*)parentObject)->
			    linkEvaluationFinished(this, false);
	}

	void FormatterCausalLink::actionProcessed(bool start) {
		/*clog << endl;
		clog << "FormatterCausalLink::actionProcessed(";
		if (!start) {
			clog << "start:" << ncmLink->getId();
		} else {
			clog << "nostart:" << ncmLink->getId();
		}
		clog << ")" << endl << endl;*/
		((CompositeExecutionObject*)parentObject)->
			    linkEvaluationFinished(this, start);
	}
}
}
}
}
}
}
}
