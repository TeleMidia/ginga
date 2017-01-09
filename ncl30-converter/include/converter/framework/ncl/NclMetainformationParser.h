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

#ifndef NCLMETAINFORMATIONPARSER_H_
#define NCLMETAINFORMATIONPARSER_H_

#include "../ModuleParser.h"
#include "../DocumentParser.h"
using namespace ::br::pucrio::telemidia::converter::framework;

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
	class NclMetainformationParser : public ModuleParser {
		public:
			NclMetainformationParser(DocumentParser *documentParser);
			void *parseMeta(DOMElement *parentElement, void *objGrandParent);

			virtual void* createMeta(
				    DOMElement *parentElement, void *objGrandParent)=0;

			void *parseMetadata(
				    DOMElement *parentElement, void *objGrandParent);

			virtual void* createMetadata(
				    DOMElement *parentElement, void *objGrandParent)=0;
	};
}
}
}
}
}
}

#endif //NCLMETAINFORMATIONPARSER_H_
