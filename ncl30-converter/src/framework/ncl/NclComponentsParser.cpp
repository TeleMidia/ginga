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

#include "converter/framework/ncl/NclComponentsParser.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace converter {
namespace framework {
namespace ncl {
	NclComponentsParser::NclComponentsParser(
		    DocumentParser *documentParser) : ModuleParser(documentParser) {

	}

	void *NclComponentsParser::parseMedia(
		    DOMElement *parentElement, void *objGrandParent) {

		clog << "parseMedia" << endl;
		void *parentObject;
		DOMNodeList *elementNodeList;
		int i, size;
		DOMNode *node;
		DOMElement *element;
		string elementTagName;
		void *elementObject = NULL;

		parentObject = createMedia(parentElement, objGrandParent);
		if (parentObject == NULL) {
			return NULL;
		}

		elementNodeList = parentElement->getChildNodes();
		size = elementNodeList->getLength();

		for (i = 0; i < size; i++) {
			node = elementNodeList->item(i);
			if(node->getNodeType()==DOMNode::ELEMENT_NODE){
				element = (DOMElement*)node;
				elementTagName = XMLString::transcode(
					    element->getTagName());

				if (XMLString::compareIString(
					    elementTagName.c_str(), "area") == 0) {

					elementObject = getInterfacesParser()->parseArea(
						    element, parentObject);

					if (elementObject != NULL) {
						addAreaToMedia(parentObject, elementObject);
					}

				} else if (XMLString::compareIString(
					    elementTagName.c_str(), "property") == 0) {

					elementObject = getInterfacesParser()->parseProperty(
						    element, parentObject);

					if (elementObject != NULL) {
						addPropertyToMedia(parentObject, elementObject);
					}
				}
			}
		}

		return parentObject;
	}

	void *NclComponentsParser::parseContext(
		    DOMElement *parentElement, void *objGrandParent) {

		void *parentObject;
		DOMNodeList *elementNodeList;
		int i, size;
		DOMNode *node;
		DOMElement *element;
		string elementTagName;
		void *elementObject = NULL;

		parentObject = createContext(parentElement, objGrandParent);
		if (parentObject == NULL) {
			return NULL;
		}

		elementNodeList = parentElement->getChildNodes();
		size = elementNodeList->getLength();

		for (i = 0; i < size; i++) {
			node = elementNodeList->item(i);
			if(node->getNodeType()==DOMNode::ELEMENT_NODE) {
				element = (DOMElement*)node;
				elementTagName = XMLString::transcode(element->getTagName());
				if (XMLString::compareIString(
					    elementTagName.c_str(), "media") == 0) {

					elementObject = parseMedia(element, parentObject);
					if (elementObject != NULL) {
						addMediaToContext(parentObject, elementObject);
					}

				} else if (XMLString::compareIString(
					    elementTagName.c_str(), "context") == 0) {

					elementObject = parseContext(element, parentObject);
					if (elementObject != NULL) {
						addContextToContext(parentObject, elementObject);
					}
				} else if (XMLString::compareIString(
					    elementTagName.c_str(), "switch") == 0) {

					elementObject = getPresentationControlParser()->
						    parseSwitch(element, parentObject);

					if (elementObject != NULL) {
						addSwitchToContext(parentObject, elementObject);
					}
				}
			}
		}

		for (i = 0; i < size; i++) {
			node = elementNodeList->item(i);
			if (node->getNodeType()==DOMNode::ELEMENT_NODE &&
				    XMLString::compareIString(
				    	    ((DOMElement*)node)->getTagName(),
				    	    XMLString::transcode("property")) == 0) {

				elementObject = getInterfacesParser()->
					    parseProperty((DOMElement*)node, parentObject);

				if (elementObject != NULL) {
					addPropertyToContext(parentObject, elementObject);
				}
			}
		}

		return parentObject;
	}

	void *NclComponentsParser::posCompileContext(
		    DOMElement *parentElement, void *parentObject) {

		clog << "posCompileContext" << endl;
		DOMNodeList *elementNodeList;
		int i, size;
		DOMNode *node;
		void *elementObject;

		elementNodeList = parentElement->getChildNodes();
		size = elementNodeList->getLength();

		for (i = 0; i < size; i++) {
			node = elementNodeList->item(i);
			if (node->getNodeType()==DOMNode::ELEMENT_NODE &&
				    XMLString::compareIString(
				    	    ((DOMElement*)node)->getTagName(),
				    	    XMLString::transcode("link")) == 0){

				elementObject = getLinkingParser()->parseLink(
					    (DOMElement*)node, parentObject);

				if (elementObject != NULL) {
					addLinkToContext(parentObject, elementObject);
				}
			}
		}

		for (i = 0; i < size; i++) {
			node = elementNodeList->item(i);
			if (node->getNodeType()==DOMNode::ELEMENT_NODE &&
				    XMLString::compareIString(
				    	    ((DOMElement*)node)->getTagName(),
				    	    XMLString::transcode("port")) == 0) {

				elementObject = getInterfacesParser()->parsePort(
					    (DOMElement*)node, parentObject);

				if (elementObject != NULL) {
					addPortToContext(parentObject, elementObject);
				}
			}
		}

		return parentObject;
	}

	NclPresentationSpecificationParser*
		    NclComponentsParser::getPresentationSpecificationParser() {

		return presentationSpecificationParser;
	}

	void NclComponentsParser::setPresentationSpecificationParser(
		    NclPresentationSpecificationParser*
		    	    presentationSpecificationParser) {

		this->presentationSpecificationParser = presentationSpecificationParser;
	}

	NclLinkingParser *NclComponentsParser::getLinkingParser() {
		return linkingParser;
	}

	void NclComponentsParser::setLinkingParser(
		    NclLinkingParser *linkingParser) {

		this->linkingParser = linkingParser;
	}

	NclInterfacesParser *NclComponentsParser::getInterfacesParser() {
		return interfacesParser;
	}

	void NclComponentsParser::setInterfacesParser(
		    NclInterfacesParser *interfacesParser) {

		this->interfacesParser = interfacesParser;
	}

	NclPresentationControlParser*
		    NclComponentsParser::getPresentationControlParser() {

		return presentationControlParser;
	}

	void NclComponentsParser::setPresentationControlParser(
		    NclPresentationControlParser* presentationControlParser) {

		this->presentationControlParser = presentationControlParser;
	}
}
}
}
}
}
}
