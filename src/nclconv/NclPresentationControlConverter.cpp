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
#include "NclPresentationControlConverter.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace converter {
namespace ncl {
	NclPresentationControlConverter::NclPresentationControlConverter(
		    DocumentParser *documentParser) : NclPresentationControlParser(
		    	    documentParser) {

		switchConstituents = new map<string, map<string, NodeEntity*>*>;
	}

	NclPresentationControlConverter::~NclPresentationControlConverter() {
		map<string, map<string, NodeEntity*>*>::iterator i;

		if (switchConstituents != NULL) {
			i = switchConstituents->begin();
			while (i != switchConstituents->end()) {
				delete i->second;
				++i;
			}
			delete switchConstituents;
			switchConstituents = NULL;
		}
	}

	vector<Node*>* NclPresentationControlConverter::getSwitchConstituents(
		    SwitchNode *switchNode) {

		map<string, map<string, NodeEntity*>*>::iterator i;

		map<string, NodeEntity*>* hTable;
		map<string, NodeEntity*>::iterator j;

		vector<Node*>* ret = new vector<Node*>;

		i = switchConstituents->find(switchNode->getId());
		if (i != switchConstituents->end()) {
			hTable = i->second;

			j = hTable->begin();
			while (j != hTable->end()) {
				ret->push_back((Node*)j->second);
				++j;
			}
		}

		//Users: you have to delete this vector after using it
		return ret;
	}

	void NclPresentationControlConverter::addCompositeRuleToCompositeRule(
		    void *parentObject, void *childObject) {

		((CompositeRule*) parentObject)->addRule((Rule*) childObject);
	}

	void NclPresentationControlConverter::addCompositeRuleToRuleBase(
		    void *parentObject, void *childObject) {

		((RuleBase*) parentObject)->addRule((Rule*) childObject);
	}

	void NclPresentationControlConverter::addRuleToCompositeRule(
		    void *parentObject, void *childObject) {

		// adicionar regra
		((CompositeRule*)parentObject)->addRule((Rule*)childObject);
	}

	void NclPresentationControlConverter::addRuleToRuleBase(
		    void *parentObject, void *childObject) {

		((RuleBase*) parentObject)->addRule((Rule*) childObject);
	}

	void NclPresentationControlConverter::addSwitchPortToSwitch(
		    void *parentObject, void *childObject) {

		((SwitchNode*)parentObject)->addPort((Port*) childObject);
	}

	void *NclPresentationControlConverter::createCompositeRule(
		    DOMElement *parentElement, void *objGrandParent) {

		CompositeRule *compositePresentationRule;
		short ruleOp = CompositeRule::OP_AND;

		if (XMLString::compareIString(XMLString::transcode(
			    parentElement->getAttribute(XMLString::transcode("operator"))),
			    "and") == 0) {

			ruleOp = CompositeRule::OP_AND;

		} else if (XMLString::compareIString(XMLString::transcode(
			    parentElement->getAttribute(XMLString::transcode("operator"))),
			    "or") == 0) {

			ruleOp = CompositeRule::OP_OR;
		}

		// cria regra composta
		compositePresentationRule = new CompositeRule(XMLString::transcode(
			     parentElement->getAttribute(XMLString::transcode("id"))),
			     ruleOp);

		return compositePresentationRule;
	}

	void* NclPresentationControlConverter::createSwitch(
		    DOMElement *parentElement, void *objGrandParent) {

		string id;
		Node *node;
		string attValue;
		Entity *referNode;
		NclDocument *document;
		SwitchNode *switchNode;

		if (!parentElement->hasAttribute(XMLString::transcode("id"))) {
			clog << "NclPresentationControlConverter::createSwitch ";
			clog << "Error: a switch element was declared without an";
			clog << " id attribute." << endl;
			return NULL;
		}

		id = XMLString::transcode(parentElement->getAttribute(
			    XMLString::transcode("id")));

		node = ((NclDocumentConverter*) getDocumentParser())->getNode(id);
		if (node != NULL) {
			clog << "NclPresentationControlConverter::createSwitch ";
			clog << "Error: there is another node element previously";
			clog << " declared with the same " << id.c_str();
			clog << " id." << endl;
			return NULL;
		}

		if (parentElement->hasAttribute(XMLString::transcode("refer"))) {
			attValue = XMLString::transcode(parentElement->getAttribute(
				    XMLString::transcode("refer")));

			try {
				referNode = (SwitchNode*)((NclDocumentConverter*)
					    getDocumentParser())->getNode(attValue);

				if (referNode == NULL) {
					document = (NclDocument*)getDocumentParser()->getObject(
						    "return", "document");

					referNode = (SwitchNode*)document->getNode(attValue);
					if (referNode == NULL) {
						referNode = new reuse::ReferredNode(
								attValue, (void*)parentElement);
					}
				}

			} catch (...) {
				clog << "NclPresentationControlConverter::createSwitch ";
				clog << "Error: the switch element refers to ";
				clog << attValue;
				clog << " object, which is not a switch element" << endl;
				return NULL;
			}

			node = new ReferNode(id);
			((ReferNode*)node)->setReferredEntity(referNode);

			return node;
		}

		switchNode = new SwitchNode(id);
		(*switchConstituents)[switchNode->getId()] =
			    new map<string, NodeEntity*>;

		return switchNode;
	}

	void *NclPresentationControlConverter::createRuleBase(
		    DOMElement *parentElement, void *objGrandParent) {

		RuleBase *ruleBase;
		ruleBase = new RuleBase(XMLString::transcode(parentElement->
			    getAttribute(XMLString::transcode("id"))));

		return ruleBase;
	}

	void *NclPresentationControlConverter::createRule(
		    DOMElement *parentElement, void *objGrandParent) {

		SimpleRule *simplePresentationRule;
		short ruleOp;

		ruleOp = convertComparator(XMLString::transcode(parentElement->
			    getAttribute(XMLString::transcode("comparator"))));

		char *var = XMLString::transcode(parentElement->getAttribute(
			    XMLString::transcode("var")));

		char *value = XMLString::transcode(parentElement->getAttribute(
			    XMLString::transcode("value")));

		XMLString::trim(var);
		XMLString::trim(value);
		simplePresentationRule = new SimpleRule(XMLString::transcode(
			    parentElement->getAttribute(XMLString::transcode("id"))),
			    var, ruleOp, value);

		return simplePresentationRule;
	}

	void *NclPresentationControlConverter::createDescriptorSwitch(
		    DOMElement *parentElement, void *objGrandParent) {

		DescriptorSwitch *descriptorSwitch = new DescriptorSwitch(
			    XMLString::transcode(parentElement->getAttribute(
			    	    XMLString::transcode("id"))));

		// vetores para conter componentes e regras do switch
		(*switchConstituents)[descriptorSwitch->getId()] = new
			    map<string, NodeEntity*>;

		return descriptorSwitch;
	}

	void NclPresentationControlConverter::addDescriptorToDescriptorSwitch(
		    void *parentObject, void *childObject) {

		map<string, NodeEntity*> *descriptors;
		try {
			if (switchConstituents->count(((DescriptorSwitch*)parentObject)->
				    getId() ) != 0) {

				descriptors = (*switchConstituents)[((DescriptorSwitch*)
					    parentObject)->getId()];

				if (descriptors->count(((GenericDescriptor*)childObject)->
					    getId()) == 0) {

					(*descriptors)[((GenericDescriptor*)childObject)->
						    getId()] = (NodeEntity*)childObject;
				}
			}

		} catch(...) {

		}
	}

	void NclPresentationControlConverter::addImportBaseToRuleBase(
		    void *parentObject, void *childObject) {

		string baseAlias, baseLocation;
		NclDocumentConverter* compiler;
		NclDocument *importedDocument;
		RuleBase *createdBase;

		// apanha o alias a localizacao da base
		baseAlias = XMLString::transcode(((DOMElement*)childObject)->
			   getAttribute(XMLString::transcode("alias")));

		baseLocation = XMLString::transcode(((DOMElement*)childObject)->
			   getAttribute(XMLString::transcode("documentURI")) );

		compiler = (NclDocumentConverter*)getDocumentParser();
		importedDocument = compiler->importDocument(&baseLocation);
		if (importedDocument == NULL) {
			return;
		}

		createdBase = importedDocument->getRuleBase();
		if (createdBase == NULL) {
			return;
		}

		// insere a base compilada na base do documento
		try {
			((RuleBase*) parentObject)->addBase(
				    createdBase, baseAlias, baseLocation);

		} catch (...) {

		}
	}

	void NclPresentationControlConverter::addBindRuleToDescriptorSwitch(
		    void *parentObject, void *childObject) {

		DescriptorSwitch *descriptorSwitch;
		DOMElement *bindRule;
		map<string, NodeEntity*> *descriptors;
		GenericDescriptor *descriptor;
		NclDocument *document;
		Rule *ncmRule;

		descriptorSwitch = (DescriptorSwitch*) parentObject;
		bindRule = (DOMElement*) childObject;

		if( switchConstituents->count( descriptorSwitch->getId() )==0 ) {
			return;
		}
		descriptors = (*switchConstituents) [descriptorSwitch->getId()];
		if (descriptors->count( XMLString::transcode(bindRule->getAttribute(
			    XMLString::transcode("constituent")))) == 0) {

			return;
		}

		descriptor = (GenericDescriptor*)(*descriptors)[XMLString::transcode(
			    bindRule->getAttribute(XMLString::transcode("constituent")))];

		if (descriptor == NULL) {
			return;
		}

		document = (NclDocument*)getDocumentParser()->getObject(
			    "return", "document");

		ncmRule = document->getRule(XMLString::transcode(bindRule->
			    getAttribute(XMLString::transcode("rule"))));

		if (ncmRule == NULL) {
			return;
		}

		descriptorSwitch->addDescriptor(descriptor, ncmRule);
	}

	void NclPresentationControlConverter::addBindRuleToSwitch(
		    void* parentObject, void* childObject) {

		SwitchNode* switchNode;
		DOMElement* bindRule;
		map<string, NodeEntity*>* nodes;
		Node* node;
		NclDocument* document;
		Rule* ncmRule;

		switchNode = (SwitchNode*)parentObject;
		bindRule   = (DOMElement*)childObject;

		if (switchConstituents->count(switchNode->getId()) == 0) {
			return;
		}

		nodes = (*switchConstituents)[switchNode->getId()];
		if (nodes->count(XMLString::transcode(bindRule->getAttribute(
			    XMLString::transcode("constituent")))) == 0) {

			return;
		}

		node = (NodeEntity*)(*nodes)[XMLString::transcode(bindRule->
			    getAttribute(XMLString::transcode("constituent")))];

		if (node == NULL) {
			return;
		}

		document = (NclDocument*)getDocumentParser()->getObject(
			    "return", "document");

		ncmRule = document->getRule(XMLString::transcode(bindRule->
			    getAttribute(XMLString::transcode("rule"))));

		if (ncmRule == NULL) {
			return;
		}

		switchNode->addNode(node, ncmRule);
	}

	void NclPresentationControlConverter::addUnmappedNodesToSwitch(
			void *parentObject) {

		SwitchNode* switchNode;

		map<string, NodeEntity*>* nodes;
		map<string, NodeEntity*>::iterator i;

		switchNode = (SwitchNode*)parentObject;

		if (switchConstituents->count(switchNode->getId()) == 0) {
			return;
		}

		nodes = (*switchConstituents)[switchNode->getId()];
		i = nodes->begin();
		while (i != nodes->end()) {
			if (switchNode->getNode(i->second->getId()) == NULL) {
				switchNode->addNode(i->second, new Rule("fake"));

			} else {
				i->second->setParentComposition(switchNode);
			}
			++i;
		}
	}

	void NclPresentationControlConverter::addDefaultComponentToSwitch(
		    void *parentObject, void *childObject) {

		SwitchNode *switchNode;
		DOMElement *defaultComponent;
		map<string, NodeEntity*> *nodes;
		NodeEntity *node;

		switchNode = (SwitchNode*) parentObject;
		defaultComponent = (DOMElement*) childObject;

		if (switchConstituents->count(switchNode->getId()) == 0) {
			return;
		}

		nodes = (*switchConstituents)[switchNode->getId()];
		if (nodes->count(XMLString::transcode(defaultComponent->getAttribute(
			    XMLString::transcode("component")))) == 0) {

			return;
		}

		node = (NodeEntity*)(*nodes)[XMLString::transcode(
			    defaultComponent->getAttribute(
			    	   XMLString::transcode("component")))];

		if (node == NULL) {
			return;
		}

		switchNode->setDefaultNode(node);
	}

	void NclPresentationControlConverter::
		    addDefaultDescriptorToDescriptorSwitch(
		    	   void *parentObject, void *childObject) {

		DescriptorSwitch *descriptorSwitch;
		DOMElement *defaultDescriptor;
		map<string, NodeEntity*> *descriptors;
		GenericDescriptor *descriptor;

		descriptorSwitch = (DescriptorSwitch*) parentObject;
		defaultDescriptor = (DOMElement*) childObject;

		if(switchConstituents->count(descriptorSwitch->getId())==0) {
			return;
		}

		descriptors = (*switchConstituents)[descriptorSwitch->getId()];
		if(descriptors->count( XMLString::transcode(defaultDescriptor->
			    getAttribute(XMLString::transcode("descriptor")))) == 0) {

			return;
		}

		descriptor = (GenericDescriptor*)(*descriptors)[XMLString::transcode(
			    defaultDescriptor->getAttribute(
			    	   XMLString::transcode("descriptor")))];

		if (descriptor == NULL) {
			return;
		}

		descriptorSwitch->setDefaultDescriptor(descriptor);
	}

	void NclPresentationControlConverter::addContextToSwitch(
		   void *parentObject, void *childObject) {

		addNodeToSwitch((SwitchNode*)parentObject, (NodeEntity*)childObject);
	}

	void NclPresentationControlConverter::addMediaToSwitch(
		   void *parentObject, void *childObject) {

		addNodeToSwitch((SwitchNode*)parentObject, (NodeEntity*)childObject);
	}

	void NclPresentationControlConverter::addSwitchToSwitch(
		    void *parentObject, void *childObject) {

		addNodeToSwitch((SwitchNode*)parentObject, (NodeEntity*)childObject);
	}

	void NclPresentationControlConverter::addNodeToSwitch(
		    SwitchNode* switchNode, NodeEntity* node) {

		map<string, NodeEntity*>* nodes;

		if (switchConstituents->count(switchNode->getId()) == 0) {
			(*switchConstituents)[switchNode->getId()] = new
				    map<string, NodeEntity*>;
		}

		nodes = (*switchConstituents) [switchNode->getId()];
		if (nodes->count(node->getId()) == 0) {
			(*nodes)[node->getId()] = node;
		}
	}

	void *NclPresentationControlConverter::createBindRule(
		    DOMElement *parentElement, void *objGrandParent) {

		return parentElement;
	}

	void *NclPresentationControlConverter::createDefaultComponent(
		    DOMElement *parentElement, void *objGrandParent) {

		return parentElement;
	}

	void *NclPresentationControlConverter::createDefaultDescriptor(
		    DOMElement *parentElement, void *objGrandParent) {

		return parentElement;
	}

	void* NclPresentationControlConverter::posCompileSwitch(
		    DOMElement *parentElement, void *parentObject) {

		clog << "posCompileSwitch" << endl;
		DOMNodeList *elementNodeList;
		int i, size;
		DOMNode *node;
		DOMElement *element;
		string elementTagName;
		Node *elementObject;

		elementNodeList = parentElement->getChildNodes();
		size = elementNodeList->getLength();

		for (i = 0; i < size; i++) {
			node = elementNodeList->item(i);
			if (node->getNodeType()==DOMNode::ELEMENT_NODE) {
				element = (DOMElement*) node;
				elementTagName = XMLString::transcode(element->getTagName());
				if (XMLString::compareIString(
					    elementTagName.c_str(), "context") == 0) {

					elementObject = (Node*)(((NclDocumentConverter*)
						    getDocumentParser())->getNode(
						    	    XMLString::transcode(element->getAttribute(
						    	    	    XMLString::transcode("id")))));

					if (elementObject->instanceOf("ContextNode")) {
						((NclComponentsParser*)NclPresentationControlParser::
							    getComponentsParser())->
							    	   posCompileContext(
							    	   	    element,elementObject);
					}

				} else if (XMLString::compareIString(elementTagName.c_str(),
					    "switch") == 0) {

					elementObject = ((NclDocumentConverter*)
						    getDocumentParser())->getNode(XMLString::transcode(
						    	    element->getAttribute(XMLString::transcode(
						    	    	    "id"))));

					if (elementObject == NULL) {
						clog << "NclPresentationControlConverter::";
						clog << "posCompileSwitch Error can't find '";
						clog << XMLString::transcode(
								element->getAttribute(
										XMLString::transcode("id")));

						clog << "' (switch)";
						clog << endl;

					} else if (elementObject->instanceOf("SwitchNode")) {
						posCompileSwitch(element, elementObject);
					}
				}
			}
		}

		return NclPresentationControlParser::posCompileSwitch(
			    parentElement, parentObject);
	}

	short NclPresentationControlConverter::convertComparator(
		    string comparator) {

		if (comparator=="eq")
			return Comparator::CMP_EQ;

		else if (comparator=="ne")
			return Comparator::CMP_NE;

		else if (comparator=="gt")
			return Comparator::CMP_GT;

		else if (comparator=="lt")
			return Comparator::CMP_LT;

		else if (comparator=="ge")
			return Comparator::CMP_GTE;

		else if (comparator=="le")
			return Comparator::CMP_LTE;

		return -1;
	}
}
}
}
}
}
