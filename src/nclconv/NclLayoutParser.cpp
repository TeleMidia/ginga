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
#include "NclLayoutParser.h"

GINGA_NCLCONV_BEGIN

	NclLayoutParser::NclLayoutParser(
		    DocumentParser *documentParser) : ModuleParser(documentParser) {


	}

	void *NclLayoutParser::parseRegion(
		    DOMElement *parentElement, void *objGrandParent) {

		clog << "parseRegion" << endl;
		void *parentObject;
		DOMNodeList *elementNodeList;
		DOMElement *element;
		DOMNode *node;
		string elementTagName;
		void *elementObject;

		parentObject = createRegion(parentElement, objGrandParent);
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
				clog << XMLString::transcode(element->getAttribute(XMLString::transcode("id")));

				if (XMLString::compareIString(
					    elementTagName.c_str(), "region") == 0) {

					elementObject = parseRegion(element, parentObject);
					if (elementObject != NULL) {
						addRegionToRegion(parentObject, elementObject);
					}
				}
			}
		}

		return parentObject;
	}

	void *NclLayoutParser::parseRegionBase(
		    DOMElement *parentElement, void *objGrandParent) {

		clog << "parseRegionBase" << endl;
		void *parentObject;
		DOMNodeList *elementNodeList;
		DOMElement *element;
		DOMNode *node;
		string elementTagName;
		void *elementObject;

		parentObject = createRegionBase(parentElement, objGrandParent);
		if (parentObject == NULL) {
			return NULL;
		}

		elementNodeList = parentElement->getChildNodes();
		for (int i = 0; i < (int)elementNodeList->getLength(); i++) {
			node = elementNodeList->item(i);
			if(node->getNodeType()==DOMNode::ELEMENT_NODE){
				element = (DOMElement*)node;
				elementTagName = XMLString::transcode( element->getTagName() );
				clog << ">>" << elementTagName.c_str() << ": ";
				clog << XMLString::transcode(element->getAttribute(XMLString::transcode("id")));

				if (XMLString::compareIString(
					    elementTagName.c_str(), "importBase") == 0) {

					elementObject = getImportParser()->parseImportBase(
						    element, parentObject);

					if (elementObject != NULL) {
						addImportBaseToRegionBase(parentObject, elementObject);
					}

				} else if (XMLString::compareIString(
					    elementTagName.c_str(), "region") == 0) {

					elementObject = parseRegion(element, parentObject);
					if (elementObject != NULL) {
						addRegionToRegionBase(parentObject, elementObject);
					}
				}
			}
		}

		return parentObject;
	}

	NclImportParser *NclLayoutParser::getImportParser() {
		return importParser;
	}

	void NclLayoutParser::setImportParser(NclImportParser *importParser) {
		this->importParser = importParser;
	}

GINGA_NCLCONV_END
