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

#ifndef NCLLAYOUTPARSER_H_
#define NCLLAYOUTPARSER_H_

#include "../ModuleParser.h"
#include "../DocumentParser.h"
using namespace ::br::pucrio::telemidia::converter::framework;

#include <xercesc/dom/DOM.hpp>
XERCES_CPP_NAMESPACE_USE

#include <iostream>
#include <string>
using namespace std;

#include "NclImportParser.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace converter {
namespace framework {
namespace ncl {
  class NclLayoutParser : public ModuleParser {
	private:
		//Ref Classes
		NclImportParser *importParser;
	public:
		NclLayoutParser(DocumentParser *documentParser);
		void *parseRegion(DOMElement *parentElement, void *objGrandParent);
		virtual void *createRegion(
			    DOMElement *parentElement, void *objGrandParent)=0;

		virtual void addRegionToRegion(void *parentObject, void *childObject)=0;
		void *parseRegionBase(DOMElement *parentElement, void *objGrandParent);
		virtual void  *createRegionBase(
			    DOMElement *parentElement, void *objGrandParent)=0;

		virtual void addImportBaseToRegionBase(
			    void *parentObject, void *childObject)=0;

		virtual void addRegionToRegionBase(
			    void *parentObject, void *childObject)=0;

		NclImportParser *getImportParser();
		void setImportParser(NclImportParser *importParser);
  };
}
}
}
}
}
}

#endif /*NCLLAYOUTPARSER_H_*/
