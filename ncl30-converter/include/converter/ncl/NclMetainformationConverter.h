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

#ifndef NclMetainformationConverter_H
#define NclMetainformationConverter_H

#include "ncl/metainformation/Meta.h"
#include "ncl/metainformation/Metadata.h"
using namespace ::br::pucrio::telemidia::ncl::metainformation;

#include "../framework/DocumentParser.h"
using namespace ::br::pucrio::telemidia::converter::framework;

#include "../framework/ncl/NclMetainformationParser.h"
using namespace ::br::pucrio::telemidia::converter::framework::ncl;

#include <map>
#include <string>
using namespace std;

#include <xercesc/dom/DOM.hpp>
XERCES_CPP_NAMESPACE_USE

namespace br {
namespace pucrio {
namespace telemidia {
namespace converter {
namespace ncl {
   class NclMetainformationConverter : public NclMetainformationParser {
  	public:
		NclMetainformationConverter(DocumentParser* documentParser);
		void* createMeta(
			    DOMElement* parentElement, void* objGrandParent);

		void* createMetadata(
			    DOMElement *parentElement, void *objGrandParent);
	};
}
}
}
}
}

#endif //NclMetainformationConverter_H
