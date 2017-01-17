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
#include "NclConnectorsParser.h"

GINGA_NCLCONV_BEGIN

	NclConnectorsParser::NclConnectorsParser(
		   DocumentParser *documentParser) : ModuleParser(documentParser) {

	}

	void *NclConnectorsParser::parseSimpleCondition(
		   DOMElement *parentElement, void *objGrandParent) {

		clog << "parseSimpleCondition" << endl;
		return createSimpleCondition(parentElement, objGrandParent);
	}

	void *NclConnectorsParser::parseCompoundCondition(
		    DOMElement *parentElement, void *objGrandParent) {

		clog << "parseCompoundCondition" << endl;
		void *parentObject = NULL;
		DOMNodeList *elementNodeList;
		DOMElement *element;
		DOMNode *node;
		string elementTagName = "";
		void *elementObject = NULL;

		parentObject = createCompoundCondition(parentElement, objGrandParent);
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
				clog << XMLString::transcode(element->getAttribute(XMLString::transcode("id") )) << endl;

				if (XMLString::compareIString(elementTagName.c_str(),
					    "simpleCondition")==0) {

					elementObject = parseSimpleCondition(element, parentObject);
					if (elementObject != NULL) {
						addSimpleConditionToCompoundCondition(parentObject,
							    elementObject);
					}

				} else if (XMLString::compareIString(elementTagName.c_str(),
					    "assessmentStatement") == 0) {

					elementObject = parseAssessmentStatement(
						    element, parentObject);

					if (elementObject != NULL) {
						addAssessmentStatementToCompoundCondition(
							    parentObject, elementObject);
					}

				} else if(XMLString::compareIString(elementTagName.c_str(),
					    "compoundCondition") == 0) {

					elementObject = parseCompoundCondition(
						    element, parentObject);

					if (elementObject != NULL) {
						addCompoundConditionToCompoundCondition(
							    parentObject, elementObject);
					}

				} else if(XMLString::compareIString(elementTagName.c_str(),
					    "compoundStatement") == 0) {

					elementObject = parseCompoundStatement(
						    element, parentObject);

					if (elementObject != NULL) {
						addCompoundStatementToCompoundCondition(
							    parentObject, elementObject);
					}
				}
			}
		}

		return parentObject;
	}

	void *NclConnectorsParser::parseAssessmentStatement(
		     DOMElement *parentElement, void *objGrandParent) {

		clog << "parseAssessmentStatement" << endl;
		void *parentObject = NULL;
		DOMNodeList *elementNodeList;
		DOMElement *element;
		DOMNode *node;
		string elementTagName = "";
		void *elementObject = NULL;

		parentObject = createAssessmentStatement(parentElement, objGrandParent);
		if (parentObject == NULL) {
			return NULL;
		}

		elementNodeList = parentElement->getChildNodes();
		for (int i = 0; i < (int)elementNodeList->getLength(); i++) {
			node = elementNodeList->item(i);
			if (node->getNodeType()==DOMNode::ELEMENT_NODE){
				element = (DOMElement*)node;
				elementTagName = XMLString::transcode( element->getTagName() );
				if (XMLString::compareIString(elementTagName.c_str(),
					    "attributeAssessment") == 0) {

					elementObject = parseAttributeAssessment(
						    element, parentObject);

					if (elementObject != NULL) {
						addAttributeAssessmentToAssessmentStatement(
							    parentObject, elementObject);
					}

				} else if (XMLString::compareIString(elementTagName.c_str(),
					    "valueAssessment") == 0) {

					elementObject = parseValueAssessment(element, parentObject);
					if (elementObject != NULL) {
						addValueAssessmentToAssessmentStatement(
							    parentObject, elementObject);
					}
				}
			}
		}

		return parentObject;
	}

	void *NclConnectorsParser::parseAttributeAssessment(
		    DOMElement *parentElement, void *objGrandParent) {

		clog << "parseAttributeAssessment" << endl;
		return createAttributeAssessment(parentElement, objGrandParent);
	}

	void *NclConnectorsParser::parseValueAssessment(
		    DOMElement *parentElement, void *objGrandParent) {

		clog << "parseValueAssessment" << endl;
		return createValueAssessment(parentElement, objGrandParent);
	}

	void *NclConnectorsParser::parseCompoundStatement(
		    DOMElement *parentElement, void *objGrandParent) {

		clog << "parseCompoundStatement" << endl;
		void *parentObject = NULL;
	 	DOMNodeList *elementNodeList;
		DOMElement *element;
		DOMNode *node;
		string elementTagName = "";
		void *elementObject = NULL;

		parentObject = createCompoundStatement(parentElement, objGrandParent);
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
				clog << XMLString::transcode(element->getAttribute(XMLString::transcode("id"))) << endl;

				if (XMLString::compareIString(elementTagName.c_str(),
					    "assessmentStatement") == 0) {

					elementObject = parseAssessmentStatement(
						    element, parentObject);

					if (elementObject != NULL) {
						addAssessmentStatementToCompoundStatement(
							    parentObject, elementObject);
					}

				} else if (XMLString::compareIString(elementTagName.c_str(),
					    "compoundStatement") == 0) {

					elementObject = parseCompoundStatement(
						    element, parentObject);

					if (elementObject != NULL) {
						addCompoundStatementToCompoundStatement(
							    parentObject, elementObject);
					}
				}
			}
		}

		return parentObject;
	}

	void *NclConnectorsParser::parseSimpleAction(
		    DOMElement *parentElement, void *objGrandParent) {

		clog << "parseSimpleAction" << endl;
		return createSimpleAction(parentElement, objGrandParent);
	}

	void *NclConnectorsParser::parseCompoundAction(
		     DOMElement *parentElement, void *objGrandParent) {

		clog << "parseCompoundAction" << endl;
		void *parentObject = NULL;
		DOMNodeList *elementNodeList;
		DOMElement *element;
		DOMNode *node;
		string elementTagName = "";
		void *elementObject = NULL;

		parentObject = createCompoundAction(parentElement, objGrandParent);
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
				clog << XMLString::transcode(element->getAttribute(XMLString::transcode("id"))) << endl;

				if (XMLString::compareIString(elementTagName.c_str(),
					    "simpleAction")==0) {

					elementObject = parseSimpleAction(element, parentObject);
					if (elementObject != NULL) {
						addSimpleActionToCompoundAction(
							    parentObject, elementObject);
					}

				} else if(XMLString::compareIString(
					    elementTagName.c_str(), "compoundAction")==0) {

					elementObject = parseCompoundAction(element, parentObject);
					if (elementObject != NULL) {
						addCompoundActionToCompoundAction(
							    parentObject, elementObject);
					}
				}
			}
		}

		return parentObject;
	}

	void *NclConnectorsParser::parseConnectorParam(
		    DOMElement *parentElement, void *objGrandParent) {

		clog << "parseConnectorParam" << endl;
		return createConnectorParam(parentElement, objGrandParent);
	}

	void *NclConnectorsParser::parseCausalConnector(
		    DOMElement *parentElement, void *objGrandParent) {

		clog << "parseCausalConnector" << endl;
		void *parentObject = NULL;
		DOMNodeList *elementNodeList;
		DOMElement *element;
		DOMNode *node;
		string elementTagName = "";
		void *elementObject = NULL;

		//pre-compile attributes

		parentObject = createCausalConnector(parentElement, objGrandParent);
		if (parentObject == NULL) {
			return NULL;
		}

		elementNodeList = parentElement->getChildNodes();
		for (int i = 0; i < (int)elementNodeList->getLength(); i++) {
			node = elementNodeList->item(i);
			if (node->getNodeType()==DOMNode::ELEMENT_NODE) {
				element = (DOMElement*)node;
				elementTagName = XMLString::transcode( element->getTagName());
					clog << ">>" << elementTagName.c_str() << ": ";
					clog << XMLString::transcode(element->getAttribute(XMLString::transcode("id"))) << endl;

				if (XMLString::compareIString(elementTagName.c_str(),
					    "simpleCondition") == 0) {

					elementObject = parseSimpleCondition(
						    element, parentObject);

					if (elementObject != NULL) {
						addSimpleConditionToCausalConnector(
							   parentObject, elementObject);
					}

				} else if (XMLString::compareIString(
					    elementTagName.c_str(), "simpleAction") == 0) {

					elementObject = parseSimpleAction(
						   element, parentObject);

					if (elementObject != NULL) {
						addSimpleActionToCausalConnector(
							     parentObject, elementObject);
					}

				} else if (XMLString::compareIString(
					    elementTagName.c_str(), "compoundAction") == 0) {

					elementObject = parseCompoundAction(
						    element, parentObject);

					if (elementObject != NULL) {
						addCompoundActionToCausalConnector(
							    parentObject, elementObject);
					}

				} else if (XMLString::compareIString(
					    elementTagName.c_str(), "connectorParam") == 0) {

					elementObject = parseConnectorParam(
						    element, parentObject);

					if (elementObject != NULL) {
						addConnectorParamToCausalConnector(
							    parentObject, elementObject);
					}

				} else if (XMLString::compareIString(
					    elementTagName.c_str(), "compoundCondition") == 0) {

					elementObject = parseCompoundCondition(
						    element, parentObject);

					if (elementObject != NULL) {
						addCompoundConditionToCausalConnector(
							    parentObject, elementObject);
					}
				}
			}
		}

		return parentObject;
	}


	void *NclConnectorsParser::parseConnectorBase(
		    DOMElement *parentElement, void *objGrandParent) {

		clog << "parseConnectorBase" << endl;
		void *parentObject = NULL;
		DOMNodeList *elementNodeList;
		DOMElement *element;
		DOMNode *node;
		string elementTagName = "";
		void *elementObject = NULL;

		parentObject = createConnectorBase(parentElement, objGrandParent);
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
				clog << XMLString::transcode(element->getAttribute(XMLString::transcode("id") )) << endl;

				if (XMLString::compareIString(
					    elementTagName.c_str(), "importBase") == 0) {

					elementObject = getImportParser()->parseImportBase(
						    element, parentObject);

					if (elementObject != NULL) {
						addImportBaseToConnectorBase(
							    parentObject, elementObject);
					}

				} else if(XMLString::compareIString(
					    elementTagName.c_str(), "causalConnector") == 0) {

					elementObject = parseCausalConnector(element, parentObject);
					if (elementObject != NULL) {
						addCausalConnectorToConnectorBase(
							   parentObject, elementObject);
					}
				}
			}
		}

		return parentObject;
	}

	NclImportParser *NclConnectorsParser::getImportParser() {
		return importParser;
	}

	void NclConnectorsParser::setImportParser(
		    NclImportParser *importParser) {

		this->importParser = importParser;
	}

GINGA_NCLCONV_END
