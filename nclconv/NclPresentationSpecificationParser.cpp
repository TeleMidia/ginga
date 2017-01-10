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

#include "NclPresentationSpecificationParser.h"
#include "NclPresentationControlParser.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace converter {
namespace framework {
namespace ncl {
	NclPresentationSpecificationParser::NclPresentationSpecificationParser(
		    DocumentParser *documentParser) : ModuleParser(documentParser) {


	}

	void *NclPresentationSpecificationParser::parseDescriptor(
		    DOMElement *parentElement, void *objGrandParent) {

		clog << "parseDescriptor" << endl;
		void *parentObject;
		DOMNodeList *elementNodeList;
		DOMElement *element;
		DOMNode *node;
		string elementTagName;
		void *elementObject;

		parentObject = createDescriptor(parentElement, objGrandParent);
		if (parentObject == NULL) {
			return NULL;
		}

		elementNodeList = parentElement->getChildNodes();
		for (int i = 0; i < (int)elementNodeList->getLength(); i++) {
			node = elementNodeList->item(i);
			if (node->getNodeType()==DOMNode::ELEMENT_NODE) {
				element = (DOMElement*)node;
				elementTagName = XMLString::transcode(element->getTagName());
				clog << ">>" << elementTagName.c_str() << ": ";
				clog << XMLString::transcode(element->getAttribute(
						XMLString::transcode("id"))) << endl;

				if (XMLString::compareIString(elementTagName.c_str(),
					    "descriptorParam") == 0) {

					elementObject = parseDescriptorParam(
						    element, parentObject);

					if (elementObject != NULL) {
						addDescriptorParamToDescriptor(
							    parentObject, elementObject);
					}
				}
			}
		}

		return parentObject;
	}

	void *NclPresentationSpecificationParser::parseDescriptorBase(
		    DOMElement *parentElement, void *objGrandParent) {

		clog << "parseDescriptorBase" << endl;
		void *parentObject;
		DOMNodeList *elementNodeList;
		DOMElement *element;
		DOMNode *node;
		string elementTagName;
		void *elementObject;

		parentObject = createDescriptorBase(parentElement, objGrandParent);
		if (parentObject == NULL) {
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

				if (XMLString::compareIString(elementTagName.c_str(),
					    "importBase") == 0) {

					elementObject = getImportParser()->parseImportBase(
						    element, parentObject);

					if (elementObject != NULL) {
						addImportBaseToDescriptorBase(
							   parentObject, elementObject);
					}

				} else if (XMLString::compareIString(
					    elementTagName.c_str(), "descriptorSwitch") == 0) {

					elementObject = ((NclPresentationControlParser*)
						     getPresentationControlParser())->
						     	    parseDescriptorSwitch(
						     	    	    element, parentObject);

					if (elementObject != NULL) {
						addDescriptorSwitchToDescriptorBase(
							    parentObject, elementObject);
					}

				} else if (XMLString::compareIString(
					    elementTagName.c_str(), "descriptor") == 0) {

					elementObject = parseDescriptor(element, parentObject);
					if (elementObject != NULL) {
						addDescriptorToDescriptorBase(
							    parentObject, elementObject);
					}
				}
			}
		}

		return parentObject;
	}

	void *NclPresentationSpecificationParser::parseDescriptorBind(
		    DOMElement *parentElement, void *objGrandParent) {

		clog << "parseDescriptorBind" << endl;
		return createDescriptorBind(parentElement, objGrandParent);
	}

	void *NclPresentationSpecificationParser::parseDescriptorParam(
		    DOMElement *parentElement, void *objGrandParent) {

		clog << "parseDescriptorParam" << endl;
		return createDescriptorParam(parentElement, objGrandParent);
	}

	NclImportParser *NclPresentationSpecificationParser::getImportParser() {
		return importParser;
	}

	void NclPresentationSpecificationParser::setImportParser(
		    NclImportParser *importParser) {

		this->importParser = importParser;
	}

	void *NclPresentationSpecificationParser::getPresentationControlParser() {
		return presentationControlParser;
	}

	void NclPresentationSpecificationParser::setPresentationControlParser(
		    void *presentationControlParser) {

		this->presentationControlParser = presentationControlParser;
	}
}
}
}
}
}
}
