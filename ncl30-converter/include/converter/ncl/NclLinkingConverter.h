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

#ifndef NclLinkingConverter_H
#define NclLinkingConverter_H

#include "ncl/Parameter.h"
using namespace ::br::pucrio::telemidia::ncl;

#include "ncl/descriptor/GenericDescriptor.h"
using namespace ::br::pucrio::telemidia::ncl::descriptor;

#include "ncl/connectors/CausalConnector.h"
#include "ncl/connectors/Connector.h"
#include "ncl/connectors/Role.h"
using namespace ::br::pucrio::telemidia::ncl::connectors;

#include "ncl/components/CompositeNode.h"
#include "ncl/components/NodeEntity.h"
using namespace ::br::pucrio::telemidia::ncl::components;

#include "ncl/link/CausalLink.h"
#include "ncl/link/Bind.h"
#include "ncl/link/Link.h"
using namespace ::br::pucrio::telemidia::ncl::link;

#include "ncl/NclDocument.h"
using namespace ::br::pucrio::telemidia::ncl;

#include "../framework/DocumentParser.h"
#include "../framework/ModuleParser.h"
using namespace ::br::pucrio::telemidia::converter::framework;

#include "../framework/ncl/NclLinkingParser.h"
using namespace ::br::pucrio::telemidia::converter::framework::ncl;

#include "NclConnectorsConverter.h"

#include <string>
using namespace std;

#include <xercesc/dom/DOM.hpp>
XERCES_CPP_NAMESPACE_USE

namespace br {
namespace pucrio {
namespace telemidia {
namespace converter {
namespace ncl {
  class NclLinkingConverter : public NclLinkingParser {
	private:
		Connector *connector;
		CompositeNode *composite;

	public:
		NclLinkingConverter(
			    DocumentParser *documentParser,
			    NclConnectorsConverter *connCompiler);

		void addBindToLink(void *parentObject, void *childObject);
		void addBindParamToBind(void *parentObject, void *childObject);
		void addLinkParamToLink(void *parentObject, void *childObject);
		void *createBind(DOMElement *parentElement, void *objGrandParent);
		void *createLink(DOMElement *parentElement, void *objGrandParent);
		void *createBindParam(DOMElement *parentElement, void *objGrandParent);
		void *createLinkParam(DOMElement *parentElement, void *objGrandParent);

	private:
		string getId(DOMElement *element);
  };
}
}
}
}
}

#endif //NclLinkingConverter_H
