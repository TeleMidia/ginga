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

#include "model/LinkAttributeAssessment.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace link {
	LinkAttributeAssessment::LinkAttributeAssessment(
		    FormatterEvent* ev, short attrType) : LinkAssessment() {

		event = ev;
		attributeType = attrType;
		offset = "";
		typeSet.insert("LinkAttributeAssessment");
	}

	FormatterEvent *LinkAttributeAssessment::getEvent() {
		return event;
	}

	void LinkAttributeAssessment::setOffset(string offset) {
		this->offset = offset;
	}

	string LinkAttributeAssessment::getOffset() {
		return offset;
	}

	void LinkAttributeAssessment::setEvent(FormatterEvent *ev) {
		event = ev;
	}

	short LinkAttributeAssessment::getAttributeType() {
		return attributeType;
	}

	void LinkAttributeAssessment::setAttributeType(short attrType) {
		attributeType = attrType;
	}

	string LinkAttributeAssessment::getValue() {
		string value = "";

		switch (attributeType) {
			case EventUtil::ATT_NODE_PROPERTY:
				if (event->instanceOf("AttributionEvent")) {
					value = getAssessmentWithOffset(
							((AttributionEvent*)event)->getCurrentValue());
				}
				break;

			case EventUtil::ATT_STATE:
				value = FormatterEvent::getStateName(event->getCurrentState());
				break;

			case EventUtil::ATT_OCCURRENCES:
				value = getAssessmentWithOffset(itos(event->getOccurrences()));
				break;

			case EventUtil::ATT_REPETITIONS:
				if (event->instanceOf("PresentationEvent")) {
					value = getAssessmentWithOffset(itos(
						    ((PresentationEvent*)event)->getRepetitions()));
				}
				break;
		}

		return value;
	}

	string LinkAttributeAssessment::getAssessmentWithOffset(
		    string assessmentValue) {

		if (offset != "" && isNumeric((void*)(assessmentValue.c_str()))) {
			return itos(util::stof(
				    assessmentValue) + util::stof(offset));
		}

		return assessmentValue;
		/*
		if (!(assessmentValue->instanceof Double) || offset == null ||
			    !(offset instanceof Double)) {

			return assessmentValue;
		}
		else {
			return new Double(((Double)assessmentValue).doubleValue() +
				    ((Double)offset).doubleValue());
		}
		*/
	}
}
}
}
}
}
}
}
