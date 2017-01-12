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

#include "LinkAssessmentStatement.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace link {
	LinkAssessmentStatement::LinkAssessmentStatement(
		    short comparator,
		    LinkAttributeAssessment* mainAssessment,
		    LinkAssessment* otherAssessment) : LinkStatement() {

		this->comparator = comparator;
		this->mainAssessment = mainAssessment;
		this->otherAssessment = otherAssessment;
		typeSet.insert("LinkAssessmentStatement");
	}

	LinkAssessmentStatement::~LinkAssessmentStatement() {
		if (mainAssessment != NULL) {
			delete mainAssessment;
			mainAssessment = NULL;
		}

		if (otherAssessment != NULL) {
			delete otherAssessment;
			otherAssessment = NULL;
		}
	}

	vector<FormatterEvent*> *LinkAssessmentStatement::getEvents() {
		vector<FormatterEvent*> *events;

		events = new vector<FormatterEvent*>;
		events->push_back(((LinkAttributeAssessment*)mainAssessment)->
			    getEvent());

		if (otherAssessment->instanceOf("LinkAttributeAssessment")) {
			events->push_back(((LinkAttributeAssessment*)otherAssessment)->
				    getEvent());
		}
		return events;
	}

	short LinkAssessmentStatement::getComparator() {
		return comparator;
	}

	void LinkAssessmentStatement::setComparator(short comp) {
		comparator = comp;
	}

	LinkAssessment *LinkAssessmentStatement::getMainAssessment() {
		return mainAssessment;
	}

	void
	LinkAssessmentStatement::setMainAssessment(LinkAssessment *assessment) {
		mainAssessment = assessment;
	}

	LinkAssessment* LinkAssessmentStatement::getOtherAssessment() {
		return otherAssessment;
	}

	void
	LinkAssessmentStatement::setOtherAssessment(LinkAssessment* assessment) {
		otherAssessment = assessment;
	}

	bool LinkAssessmentStatement::evaluate() {
		if (mainAssessment == NULL || otherAssessment == NULL) {
			return false;
		}

		clog << "LinkAssessmentStatement::evaluate() ";
		clog << "mainAssessment(" << mainAssessment << ") = '";
		clog << mainAssessment->getValue() << "' ";
		clog << "otherAssessment(" << otherAssessment << ") = '";
		clog << otherAssessment->getValue() << "' comparator = '";
		clog << comparator << "'";
		clog << endl;

		return Comparator::evaluate(
			    mainAssessment->getValue(),
			    otherAssessment->getValue(),
			    comparator);
	}
}
}
}
}
}
}
}
