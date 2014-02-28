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

#include "converter/framework/ncl/NclPresentationSpecificationParser.h"
#include "converter/framework/ncl/NclPresentationControlParser.h"

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
