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

#include "NclInterfacesParser.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace converter {
namespace framework {
namespace ncl {
	NclInterfacesParser::NclInterfacesParser(
		    DocumentParser *documentParser) : ModuleParser(documentParser) {


	}

	void *NclInterfacesParser::parseSwitchPort(
		    DOMElement *parentElement, void *objGrandParent) {

		// Element: mapping.
		//  Content of: switchPort,
		clog << "parseSwitchPort" << endl;
		void *parentObject;
		DOMNodeList *elementNodeList;
		DOMElement *element;
		DOMNode *node;
		string elementTagName;
		void *elementObject;

		parentObject = createSwitchPort(parentElement, objGrandParent);
		if (parentObject == NULL) {
			clog << "create SwitchPort error for '";
			clog << "'" << elementTagName.c_str() << "'" << endl;
			return NULL;
		}

		elementNodeList = parentElement->getChildNodes();
		for (int i = 0; i < (int)elementNodeList->getLength(); i++) {
			node = elementNodeList->item(i);
			if (node->getNodeType()==DOMNode::ELEMENT_NODE) {
				element = (DOMElement*)node;
				elementTagName = XMLString::transcode(element->getTagName());
				clog << ">>" << elementTagName.c_str() << ": ";
				clog << XMLString::transcode(element->getAttribute(XMLString::transcode("id"))) << endl;

				if (XMLString::compareIString(
					    elementTagName.c_str(), "mapping") == 0) {

					elementObject = parseMapping(element, parentObject);
					if (elementObject != NULL) {
						addMappingToSwitchPort(parentObject, elementObject);
					}
				}
			}
		}

		return parentObject;
	}

	void *NclInterfacesParser::parseMapping(
		    DOMElement *parentElement, void *objGrandParent) {

		clog << "parseMapping" << endl;
		return createMapping(parentElement, objGrandParent);
	}

	void *NclInterfacesParser::parseArea(
		    DOMElement *parentElement, void *objGrandParent) {

		clog << "parseArea" << endl;
		return createArea(parentElement, objGrandParent);
	}

	void *NclInterfacesParser::parseProperty(
		    DOMElement *parentElement, void *objGrandParent) {

		clog << "parseProperty" << endl;
		return createProperty(parentElement, objGrandParent);
	}

	void *NclInterfacesParser::parsePort(
		    DOMElement *parentElement, void *objGrandParent) {

		clog << "parsePort" << endl;
		return createPort(parentElement, objGrandParent);
	}
}
}
}
}
}
}
