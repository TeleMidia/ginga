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

#ifndef NCLDOCUMENTPARSER_H_
#define NCLDOCUMENTPARSER_H_

#include "../DocumentParser.h"
using namespace ::br::pucrio::telemidia::converter::framework;

#include "NclConnectorsParser.h"
#include "NclImportParser.h"
#include "NclTransitionParser.h"
#include "NclPresentationControlParser.h"
#include "NclComponentsParser.h"
#include "NclStructureParser.h"
#include "NclPresentationSpecificationParser.h"
#include "NclLayoutParser.h"
#include "NclInterfacesParser.h"
#include "NclLinkingParser.h"
#include "NclMetainformationParser.h"

#include "ncl/NclDocument.h"
using namespace ::br::pucrio::telemidia::ncl;

#include <xercesc/dom/DOM.hpp>
XERCES_CPP_NAMESPACE_USE

#include <iostream>
#include <string>
using namespace std;

namespace br {
namespace pucrio {
namespace telemidia {
namespace converter {
namespace framework {
namespace ncl {
   class NclDocumentParser : public DocumentParser {
	protected:
		NclConnectorsParser *connectorsParser;
		NclImportParser *importParser;
		NclTransitionParser* transitionParser;
		NclPresentationControlParser *presentationControlParser;
		NclComponentsParser *componentsParser;
		NclStructureParser *structureParser;
		NclPresentationSpecificationParser *presentationSpecificationParser;
		NclLayoutParser *layoutParser;
		NclInterfacesParser *interfacesParser;
		NclLinkingParser *linkingParser;
		NclMetainformationParser* metainformationParser;
		IDeviceLayout* deviceLayout;

	public:
		NclDocumentParser();
		virtual ~NclDocumentParser();

		void setDeviceLayout(IDeviceLayout* deviceLayout);

	protected:
		void setDependencies();

	public:
		NclTransitionParser* getTransitionParser();
		NclConnectorsParser *getConnectorsParser();
		void setConnectorsParser(NclConnectorsParser *connectorsParser);
		NclImportParser *getImportParser();
		void setBaseReuseParser(NclImportParser *importParser);
		NclPresentationControlParser *getPresentationControlParser();
		void setPresentationControlParser(
			    NclPresentationControlParser *presentationControlParser);

		NclComponentsParser *getComponentsParser();
		void setComponentsParser(NclComponentsParser *componentsParser);
		NclStructureParser *getStructureParser();
		void setStructureParser(NclStructureParser *structureParser);
		NclPresentationSpecificationParser*
			    getPresentationSpecificationParser();

		void setPresentationSpecificationParser(
			    NclPresentationSpecificationParser*
			    	    presentationSpecificationParser);

		NclLayoutParser *getLayoutParser();
		void setLayoutParser(NclLayoutParser *layoutParser);
		NclInterfacesParser *getInterfacesParser();
		void setInterfacesParser(NclInterfacesParser *interfacesParser);
		NclMetainformationParser* getMetainformationParser();
		void setMetainformationParser(
			     NclMetainformationParser* metainformationParser);

		NclLinkingParser *getLinkingParser();
		void setLinkingParser(NclLinkingParser *linkingParser);

	protected:
		virtual void* parseRootElement(DOMElement *rootElement);
		virtual void initialize()=0;
   };
}
}
}
}
}
}

#endif /*NCLDOCUMENTPARSER_H_*/
