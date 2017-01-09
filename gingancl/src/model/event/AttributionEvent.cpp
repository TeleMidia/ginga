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

#include "ncl/components/NodeEntity.h"
#include "ncl/components/ContentNode.h"
using namespace ::br::pucrio::telemidia::ncl::components;

#include "gingancl/model/AttributionEvent.h"

#include "gingancl/model/ExecutionObject.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::components;

namespace br {
namespace pucrio {
namespace telemidia {
namespace ginga {
namespace ncl {
namespace model {
namespace event {
	AttributionEvent::AttributionEvent(
			string id,
			void* executionObject,
	        PropertyAnchor* anchor,
	        IPresentationContext* presContext) :
					FormatterEvent(id, executionObject) {

		Entity* entity;
		NodeEntity* dataObject;

		typeSet.insert("AttributionEvent");

		this->anchor          = anchor;
		this->valueMaintainer = NULL;
		this->settingNode     = false;
		this->presContext     = presContext;

		dataObject = (NodeEntity*)(((ExecutionObject*)executionObject)->
				getDataObject());

		if (dataObject->instanceOf("ContentNode") &&
			    ((ContentNode*)dataObject)->isSettingNode()) {

			settingNode = true;
		}

		if (dataObject->instanceOf("ReferNode")) {
			if (((ReferNode*)dataObject)->getInstanceType() == "instSame") {
				entity = ((ReferNode*)dataObject)->getDataEntity();
				if (entity->instanceOf("ContentNode") &&
						((ContentNode*)entity)->isSettingNode()) {

					settingNode = true;
				}
			}
		}
	}

	AttributionEvent::~AttributionEvent() {
		removeInstance(this);

		assessments.clear();
	}

	PropertyAnchor* AttributionEvent::getAnchor() {
		return anchor;
	}

	string AttributionEvent::getCurrentValue() {
		string propName;
		string maintainerValue = "";

		if (settingNode) {
			propName = anchor->getPropertyName();
			if (propName != "") {
				maintainerValue = presContext->getPropertyValue(propName);
			}

		} else {
			if (valueMaintainer != NULL) {
				maintainerValue = valueMaintainer->getPropertyValue(this);
			}

			if (maintainerValue == "") {
				maintainerValue = anchor->getPropertyValue();
			}
		}

		return maintainerValue;
	}

	bool AttributionEvent::setValue(string newValue) {
/*		if ((value == "" && newValue != "") ||
			    (newValue == "" && value != "") ||
			    (newValue != "" && value != "" && (newValue != value))) {

			value = newValue;
			return true;
		}

		return false;*/

		if (anchor->getPropertyValue() != newValue) {
			anchor->setPropertyValue(newValue);
			return true;
		}
		return false;
	}

	void AttributionEvent::setValueMaintainer(
		    IAttributeValueMaintainer* valueMaintainer) {

		this->valueMaintainer = valueMaintainer;
	}

	IAttributeValueMaintainer* AttributionEvent::getValueMaintainer() {
		return this->valueMaintainer;
	}

	void AttributionEvent::setImplicitRefAssessmentEvent(
			string roleId, FormatterEvent* event) {

		assessments[roleId] = event;
	}

	FormatterEvent* AttributionEvent::getImplicitRefAssessmentEvent(
			string roleId) {

		if (assessments.count(roleId) == 0) {
			return NULL;
		}

		return assessments[roleId];
	}
}
}
}
}
}
}
}
