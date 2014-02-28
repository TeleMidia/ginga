/******************************************************************************
Este arquivo eh parte da implementacao do ambiente declarativo do middleware
Ginga (Ginga-NCL).

Direitos Autorais Reservados (c) 1989-2007 PUC-Rio/Laboratorio TeleMidia

Este programa eh software livre; voce pode redistribui-lo e/ou modificah-lo sob
os termos da Licenca Publica Geral GNU versao 2 conforme publicada pela Free
Software Foundation.

Este programa eh distribuido na expectativa de que seja util, porem, SEM
NENHUMA GARANTIA; nem mesmo a garantia implicita de COMERCIABILIDADE OU
ADEQUACAO A UMA FINALIDADE ESPECIFICA. Consulte a Licenca Publica Geral do
GNU versao 2 para mais detalhes.

Voce deve ter recebido uma copia da Licenca Publica Geral do GNU versao 2 junto
com este programa; se nao, escreva para a Free Software Foundation, Inc., no
endereco 59 Temple Street, Suite 330, Boston, MA 02111-1307 USA.

Para maiores informacoes:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
******************************************************************************
This file is part of the declarative environment of middleware Ginga (Ginga-NCL)

Copyright: 1989-2007 PUC-RIO/LABORATORIO TELEMIDIA, All Rights Reserved.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License version 2 as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License version 2 for more
details.

You should have received a copy of the GNU General Public License version 2
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

For further information contact:
ncl @ telemidia.puc-rio.br
http://www.ncl.org.br
http://www.ginga.org.br
http://www.telemidia.puc-rio.br
*******************************************************************************/

#include "converter/framework/ncl/NclStructureParser.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace converter {
namespace framework {
namespace ncl {
	NclStructureParser::NclStructureParser(
		    DocumentParser *documentParser) :
		    	    ModuleParser(documentParser) {


	}

	void *NclStructureParser::parseBody(
		    DOMElement *parentElement, void *objGrandParent) {

		void *parentObject;
		DOMNodeList *elementNodeList;
		int i, size;
		DOMNode *node;
		DOMElement *element;
		string elementTagName;
		void *elementObject;

		parentObject = createBody(parentElement, objGrandParent);
		if (parentObject == NULL) {
			return NULL;
		}

		elementNodeList = parentElement->getChildNodes();
		size = elementNodeList->getLength();

		for (i = 0; i < size; i++) {
			node = elementNodeList->item(i);
			if(node->getNodeType()==DOMNode::ELEMENT_NODE){
				element = (DOMElement*)node;
				elementTagName = XMLString::transcode(element->getTagName());
				if (XMLString::compareIString(
					    elementTagName.c_str(), "media")==0) {

					elementObject = getComponentsParser()->parseMedia(
						    element, parentObject);

					if (elementObject != NULL) {
						addMediaToBody(parentObject, elementObject);
					}
				}
				else if (XMLString::compareIString(
					    elementTagName.c_str(), "context")==0) {

					elementObject = getComponentsParser()->parseContext(
						    element, parentObject);

					if (elementObject != NULL) {
						addContextToBody(parentObject, elementObject);
					}
				}
				else if (XMLString::compareIString(
					    elementTagName.c_str(), "switch")==0) {

					elementObject = getPresentationControlParser()->
						    parseSwitch(element, parentObject);

					if (elementObject != NULL) {
						addSwitchToBody(parentObject, elementObject);
					}
				}
			}
		}

		for (i = 0; i < size; i++) {
			node = elementNodeList->item(i);
			if (node->getNodeType()==DOMNode::ELEMENT_NODE &&
				    XMLString::compareIString(((DOMElement*)node)->
				    	    getTagName(),
				    	    XMLString::transcode("property"))==0) {

				elementObject = getInterfacesParser()->parseProperty(
					    (DOMElement*)node, parentObject);

				if (elementObject != NULL) {
					addPropertyToBody(parentObject, elementObject);
				}
			}
		}

		return parentObject;
	}

	void *NclStructureParser::posCompileBody(
		    DOMElement *parentElement, void *parentObject) {

		return getComponentsParser()->posCompileContext(
			    parentElement, parentObject);
	}

	void *NclStructureParser::parseHead(
		    DOMElement *parentElement, void *objGrandParent) {

		void *parentObject = NULL;
		DOMNodeList *elementNodeList;
		int i, size;
		DOMNode *node;
		void *elementObject = NULL;

		parentObject = createHead(parentElement, objGrandParent);
		if (parentObject == NULL) {
			return NULL;
		}

		elementNodeList = parentElement->getChildNodes();
		size = elementNodeList->getLength();

		for (i = 0; i < size; i++) {
		node = elementNodeList->item(i);
		if (node->getNodeType()==DOMNode::ELEMENT_NODE &&
			    XMLString::compareIString(
			    	    ((DOMElement*)node)->getTagName(),
			    	    XMLString::transcode("importedDocumentBase"))
			    	    	    == 0) {

				elementObject = getImportParser()->
					    parseImportedDocumentBase(
					    	    (DOMElement*)node, parentObject);

				if (elementObject != NULL) {
					addImportedDocumentBaseToHead(
						    parentObject, elementObject);

					break;
				}
			}
		}

		for (i = 0; i < size; i++) {
			node = elementNodeList->item(i);
			if (node->getNodeType()==DOMNode::ELEMENT_NODE &&
				    XMLString::compareIString(
				    	    ((DOMElement*)node)->getTagName(),
				    	    XMLString::transcode("regionBase") )==0) {

				elementObject = getLayoutParser()->
					    parseRegionBase((DOMElement*)node, parentObject);

				if (elementObject != NULL) {
					addRegionBaseToHead(parentObject, elementObject);
				}
			}
		}

		for (i = 0; i < size; i++) {
			node = elementNodeList->item(i);
			if (node->getNodeType()==DOMNode::ELEMENT_NODE &&
				    XMLString::compareIString(
				    	    ((DOMElement*)node)->getTagName(),
				    	    XMLString::transcode("ruleBase") )==0){

				elementObject = getPresentationControlParser()->
					    parseRuleBase((DOMElement*)node, parentObject);

				if (elementObject != NULL) {
					addRuleBaseToHead(parentObject, elementObject);
					break;
				}
			}
		}

		for (i = 0; i < size; i++) {
			node = elementNodeList->item(i);
			if (node->getNodeType()==DOMNode::ELEMENT_NODE &&
				    XMLString::compareIString(
				    	    ((DOMElement*)node)->getTagName(),
				    	    XMLString::transcode("transitionBase") )==0) {

				clog << "NclStructureParser::parseHead ";
				clog << "transitionBase i = '" << i << "'" << endl;
				elementObject = getTransitionParser()->
					    parseTransitionBase((DOMElement*)node, parentObject);

				if (elementObject != NULL) {
					addTransitionBaseToHead(parentObject, elementObject);
					break;
				}
			}
		}

		for (i = 0; i < size; i++) {
			node = elementNodeList->item(i);
			if (node->getNodeType()==DOMNode::ELEMENT_NODE &&
				    XMLString::compareIString(
				    	    ((DOMElement*)node)->getTagName(),
				    	    XMLString::transcode("descriptorBase") )==0) {

				elementObject = getPresentationSpecificationParser()->
					    parseDescriptorBase((DOMElement*)node, parentObject);

				if (elementObject != NULL) {
					addDescriptorBaseToHead(parentObject, elementObject);
					break;
				}
			}
		}

		for (i = 0; i < size; i++) {
			node = elementNodeList->item(i);
			if (node->getNodeType()==DOMNode::ELEMENT_NODE &&
				    XMLString::compareIString(
				    	    ((DOMElement*)node)->getTagName(),
				    	    XMLString::transcode("connectorBase") )==0) {

				elementObject = getConnectorsParser()->parseConnectorBase(
					    (DOMElement*)node, parentObject);

				if (elementObject != NULL) {
					addConnectorBaseToHead(parentObject, elementObject);
					break;
				}
			}
		}

		for (i = 0; i < size; i++) {
			node = elementNodeList->item(i);
			if (node->getNodeType()==DOMNode::ELEMENT_NODE &&
				    XMLString::compareIString(
				    	    ((DOMElement*)node)->getTagName(),
				    	    XMLString::transcode("meta") )==0) {

				elementObject = getMetainformationParser()->
					    parseMeta((DOMElement*)node, parentObject);

				if (elementObject != NULL) {
					addMetaToHead(parentObject, elementObject);
					break;
				}
			}
		}

		for (i = 0; i < size; i++) {
			node = elementNodeList->item(i);
			if (node->getNodeType()==DOMNode::ELEMENT_NODE &&
				    XMLString::compareIString(
				    	    ((DOMElement*)node)->getTagName(),
				    	    XMLString::transcode("metadata") )==0) {

				elementObject = getMetainformationParser()->
					    parseMetadata((DOMElement*)node, parentObject);

				if (elementObject != NULL) {
					addMetadataToHead(parentObject, elementObject);
					break;
				}
			}
		}

		return parentObject;
	}

	void *NclStructureParser::parseNcl(
		    DOMElement *parentElement,
		    void *objGrandParent) {

		void *parentObject = NULL;
		DOMNodeList *elementNodeList;
		int i, size;
		DOMNode *node;
		void *elementObject = NULL;

		parentObject = createNcl(parentElement, objGrandParent);
		if (parentObject == NULL) {
			return NULL;
		}

		elementNodeList = parentElement->getChildNodes();
		size = elementNodeList->getLength();

		for (i = 0; i < size; i++) {
			node = elementNodeList->item(i);
			if (node->getNodeType()==DOMNode::ELEMENT_NODE &&
				    XMLString::compareIString(((DOMElement*)node)->
				    	    getTagName(), XMLString::transcode("head") )==0) {

				elementObject = parseHead((DOMElement*)node, parentObject);;
				if (elementObject != NULL) {
					addHeadToNcl(parentObject, elementObject);
					break;
				}
			}
		}

		for (i = 0; i < size; i++) {
			node = elementNodeList->item(i);
			if (node->getNodeType()==DOMNode::ELEMENT_NODE &&
				    XMLString::compareIString(((DOMElement*)node)->
				    	    getTagName(), XMLString::transcode("body") )==0) {

				elementObject = parseBody((DOMElement*)node, parentObject);
		      	if (elementObject != NULL) {
					posCompileBody((DOMElement*)node, elementObject);
					addBodyToNcl(parentObject, elementObject);
					break;
				}
			}
		}

		return parentObject;
	}

	NclTransitionParser* NclStructureParser::getTransitionParser() {
		return transitionParser;
	}

	void NclStructureParser::setTransitionParser(
		    NclTransitionParser* transitionParser) {

		this->transitionParser = transitionParser;
	}

	NclPresentationSpecificationParser *NclStructureParser::
		    getPresentationSpecificationParser() {

		return presentationSpecificationParser;
	}

	void NclStructureParser::setPresentationSpecificationParser(
		    NclPresentationSpecificationParser *
		    	    presentationSpecificationParser) {

		this->presentationSpecificationParser =
			    presentationSpecificationParser;
	}

	NclComponentsParser *NclStructureParser::getComponentsParser() {
		return componentsParser;
	}

	void NclStructureParser::setComponentsParser(
		    NclComponentsParser *componentsParser) {

		this->componentsParser = componentsParser;
	}

	NclLinkingParser *NclStructureParser::getLinkingParser() {
		return linkingParser;
	}

	void NclStructureParser::setLinkingParser(
		    NclLinkingParser *linkingParser) {

		this->linkingParser = linkingParser;
	}

	NclLayoutParser *NclStructureParser::getLayoutParser() {
		return layoutParser;
	}

	void NclStructureParser::setLayoutParser(
		    NclLayoutParser *layoutParser) {

		this->layoutParser = layoutParser;
	}

	NclMetainformationParser*
		    NclStructureParser::getMetainformationParser() {

		return metainformationParser;
	}

	NclInterfacesParser *NclStructureParser::getInterfacesParser() {
		return interfacesParser;
	}

	void NclStructureParser::setInterfacesParser(
		    NclInterfacesParser *interfacesParser) {

		this->interfacesParser = interfacesParser;
	}

	NclPresentationControlParser *NclStructureParser::
		    getPresentationControlParser() {

		return presentationControlParser;
	}

	void NclStructureParser::setPresentationControlParser(
		    NclPresentationControlParser *presentationControlParser) {

		this->presentationControlParser = presentationControlParser;
	}

	NclConnectorsParser *NclStructureParser::getConnectorsParser() {
		return connectorsParser;
	}

	void NclStructureParser::setConnectorsParser(
		    NclConnectorsParser *connectorsParser) {

		this->connectorsParser = connectorsParser;
	}

	NclImportParser *NclStructureParser::getImportParser() {
		return importParser;
	}

	void NclStructureParser::setImportParser(
		    NclImportParser *importParser) {

		this->importParser = importParser;
	}

	void NclStructureParser::setMetainformationParser(
		    NclMetainformationParser* metainformationParser) {

		this->metainformationParser = metainformationParser;
	}
}
}
}
}
}
}
