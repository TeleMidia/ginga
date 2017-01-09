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
