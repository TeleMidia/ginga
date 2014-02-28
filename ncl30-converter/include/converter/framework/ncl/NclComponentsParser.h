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

#ifndef NCLCOMPONENTSPARSER_H_
#define NCLCOMPONENTSPARSER_H_

#include "../ModuleParser.h"
#include "../DocumentParser.h"
using namespace ::br::pucrio::telemidia::converter::framework;

#include <xercesc/dom/DOM.hpp>
XERCES_CPP_NAMESPACE_USE

#include <iostream>
#include <string>
using namespace std;

#include "NclPresentationSpecificationParser.h"
#include "NclLinkingParser.h"
#include "NclInterfacesParser.h"
#include "NclPresentationControlParser.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace converter {
namespace framework {
namespace ncl {
  class NclComponentsParser : public ModuleParser {
	//Ref Classes
	private:
		NclPresentationSpecificationParser *presentationSpecificationParser;
		NclLinkingParser *linkingParser;
		NclInterfacesParser *interfacesParser;
		NclPresentationControlParser *presentationControlParser;

	public:
		NclComponentsParser(DocumentParser *documentParser);

		void *parseMedia(DOMElement *parentElement, void *objGrandParent);
		virtual void* createMedia(
			    DOMElement *parentElement, void *objGrandParent)=0;

		virtual void addAreaToMedia(void *parentObject, void *childObject)=0;
		virtual void addPropertyToMedia(
			    void *parentObject, void *childObject)=0;

		void *parseContext(DOMElement *parentElement, void *objGrandParent);
		virtual void *posCompileContext(
			    DOMElement *parentElement, void *parentObject);

		virtual void *createContext(
			    DOMElement *parentElement, void * objGrandParent)=0;

		virtual void addPortToContext(void * parentObject, void *childObject)=0;
		virtual void addPropertyToContext(
			    void *parentObject, void *childObject)=0;

		virtual void addMediaToContext(void *parentObject, void *childObject)=0;
		virtual void addContextToContext(
			    void *parentObject, void *childObject)=0;

		virtual void addLinkToContext(void *parentObject, void *childObject)=0;
		virtual void addSwitchToContext(
			    void *parentObject, void *childObject)=0;

		NclPresentationSpecificationParser*
			    getPresentationSpecificationParser();

		void setPresentationSpecificationParser(
			    NclPresentationSpecificationParser*
			    	    presentationSpecificationParser);

		NclLinkingParser *getLinkingParser();
		void setLinkingParser(NclLinkingParser *linkingParser);
		NclInterfacesParser *getInterfacesParser();
		void setInterfacesParser(NclInterfacesParser *interfacesParser);
		NclPresentationControlParser *getPresentationControlParser();
		void setPresentationControlParser(
			    NclPresentationControlParser *presentationControlParser);
  };
}
}
}
}
}
}

#endif /*NCLCOMPONENTSPARSER_H_*/
