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
#include "ncl/components/ContentNode.h"

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

BR_PUCRIO_TELEMIDIA_NCL_COMPONENTS_BEGIN

	ContentNode::ContentNode(string uid, Content* someContent)
		    : NodeEntity(uid, someContent) {

		initialize("");
	}

	ContentNode::ContentNode(string uid, Content* someContent, string type)
		    : NodeEntity(uid, someContent) {

		initialize(type);
	}

	void ContentNode::initialize(string type) {
		typeSet.insert("ContentNode");
		typeSet.insert("DocumentNode");

		this->type = type;

		// must set to false before a new isSettingNode call
		isSettingNodeType = false;
		isSettingNodeType = isSettingNode();

		// must set to false before a new isTimeNode call
		isTimeNodeType = false;
		isTimeNodeType = isTimeNode();
	}

	bool ContentNode::isSettingNode() {
		string upNodeType;
		string nodeType = getNodeType();

		if (isSettingNodeType) {
			return true;
		}

		if (nodeType == "") {
			return false;
		}

		//W3C (and RFC2045) type value isn't sensitive
		upNodeType = upperCase(nodeType);
		if (upNodeType == "APPLICATION/X-GINGA-SETTINGS" ||
				upNodeType == "APPLICATION/X-NCL-SETTINGS") {

			return true;
		}

		return false;
	}

	bool ContentNode::isTimeNode() {
		string upNodeType;
		string nodeType = getNodeType();

		if (isTimeNodeType) {
			return true;
		}

		if (nodeType == "") {
			return false;
		}

		//W3C (and RFC2045) type value isn't sensitive
		upNodeType = upperCase(nodeType);
		if (upNodeType == "APPLICATION/X-GINGA-TIME" ||
				upNodeType == "APPLICATION/X-NCL-TIME") {

			return true;
		}

		return false;
	}

	string ContentNode::getTypeValue() {
		return type;
	}

	string ContentNode::getNodeType() {
		if (type != "") {
			return type;

		} else if (content != NULL) {
			return content->getType();

		} else {
			return "";
		}
	}

	void ContentNode::setNodeType(string type) {
		this->type        = type;

		// must set to false before a new isSettingNode call
		isSettingNodeType = false;
		isSettingNodeType = isSettingNode();
	}

BR_PUCRIO_TELEMIDIA_NCL_COMPONENTS_END
