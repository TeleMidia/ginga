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

#include "converter/framework/ncl/NclDocumentParser.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace converter {
namespace framework {
namespace ncl {
	NclDocumentParser::NclDocumentParser(IDeviceLayout* deviceLayout) :
			DocumentParser() {

		this->deviceLayout                    = deviceLayout;
		this->presentationSpecificationParser = NULL;
		this->structureParser                 = NULL;
		this->componentsParser                = NULL;
		this->connectorsParser                = NULL;
		this->linkingParser                   = NULL;
		this->interfacesParser                = NULL;
		this->layoutParser                    = NULL;
		this->presentationControlParser       = NULL;
		this->importParser                    = NULL;
	}

	NclDocumentParser::~NclDocumentParser() {

	}

	void NclDocumentParser::setDependencies() {
		getConnectorsParser()->setImportParser(importParser);
		getPresentationControlParser()->setPresentationSpecificationParser(
			    presentationSpecificationParser);

		getPresentationControlParser()->setComponentsParser(componentsParser);
		getPresentationControlParser()->setInterfacesParser(interfacesParser);
		getPresentationControlParser()->setImportParser(importParser);
		getComponentsParser()->setPresentationSpecificationParser(
			    presentationSpecificationParser);

		getComponentsParser()->setLinkingParser(linkingParser);
		getComponentsParser()->setInterfacesParser(interfacesParser);
		getComponentsParser()->setPresentationControlParser(
			    presentationControlParser);

		getStructureParser()->setPresentationSpecificationParser(
			    presentationSpecificationParser);

		getStructureParser()->setComponentsParser(componentsParser);
		getStructureParser()->setLinkingParser(linkingParser);
		getStructureParser()->setLayoutParser(layoutParser);
		getStructureParser()->setInterfacesParser(interfacesParser);
		getStructureParser()->setPresentationControlParser(
			    presentationControlParser);

		getStructureParser()->setConnectorsParser(connectorsParser);
		getStructureParser()->setImportParser(importParser);
		getStructureParser()->setTransitionParser(transitionParser);
		getStructureParser()->setMetainformationParser(metainformationParser);
		getPresentationSpecificationParser()->setImportParser(importParser);
		getPresentationSpecificationParser()->setPresentationControlParser(
			    presentationControlParser);

		getLayoutParser()->setImportParser(importParser);
		getTransitionParser()->setImportParser(importParser);
	}

	NclTransitionParser* NclDocumentParser::getTransitionParser() {
		return transitionParser;
	}

	NclConnectorsParser *NclDocumentParser::getConnectorsParser() {
		return connectorsParser;
	}

	void NclDocumentParser::setConnectorsParser(
		    NclConnectorsParser *connectorsParser) {

		this->connectorsParser = connectorsParser;
	}

	NclImportParser *NclDocumentParser::getImportParser() {
		return importParser;
	}

	void NclDocumentParser::setBaseReuseParser(
		    NclImportParser *importParser) {

		this->importParser = importParser;
	}

	NclPresentationControlParser*
		    NclDocumentParser::getPresentationControlParser() {

		return presentationControlParser;
	}

	void NclDocumentParser::setPresentationControlParser(
		    NclPresentationControlParser *presentationControlParser) {

		this->presentationControlParser = presentationControlParser;
	}

	NclComponentsParser *NclDocumentParser::getComponentsParser() {
		return componentsParser;
	}

	void NclDocumentParser::setComponentsParser(
		    NclComponentsParser *componentsParser) {

		this->componentsParser = componentsParser;
	}

	NclStructureParser *NclDocumentParser::getStructureParser() {
		return structureParser;
	}

	void NclDocumentParser::setStructureParser(
		    NclStructureParser *structureParser) {

		this->structureParser = structureParser;
	}

	NclPresentationSpecificationParser*
		    NclDocumentParser::getPresentationSpecificationParser() {

		return presentationSpecificationParser;
	}

	void NclDocumentParser::setPresentationSpecificationParser(
		    NclPresentationSpecificationParser*
		    	    presentationSpecificationParser) {

		this->presentationSpecificationParser = presentationSpecificationParser;
	}

	NclLayoutParser *NclDocumentParser::getLayoutParser() {
		return layoutParser;
	}

	void NclDocumentParser::setLayoutParser(NclLayoutParser *layoutParser) {
		this->layoutParser = layoutParser;
	}

	NclInterfacesParser *NclDocumentParser::getInterfacesParser() {
		return interfacesParser;
	}

	void NclDocumentParser::setInterfacesParser(
		    NclInterfacesParser *interfacesParser) {

		this->interfacesParser = interfacesParser;
	}

	NclMetainformationParser*
		    NclDocumentParser::getMetainformationParser() {

		return metainformationParser;
	}

	void NclDocumentParser::setMetainformationParser(
		    NclMetainformationParser* metainformationParser) {

		this->metainformationParser = metainformationParser;
	}

	NclLinkingParser* NclDocumentParser::getLinkingParser() {
		return linkingParser;
	}

	void NclDocumentParser::setLinkingParser(
		    NclLinkingParser *linkingParser) {

		this->linkingParser = linkingParser;
	}

	void* NclDocumentParser::parseRootElement(DOMElement* rootElement) {
		string tagName;
		tagName = XMLString::transcode(rootElement->getTagName());
		if (tagName == "ncl") {
			return getStructureParser()->parseNcl(rootElement, NULL);

		} else {
			clog << "NclDocumentParser::parseRootElement Warning!";
			clog << " Trying to parse a non NCL document. rootElement";
			clog << "->getTagName = '" << tagName.c_str() << "'" << endl;
			return NULL;
		}
	}
}
}
}
}
}
}
