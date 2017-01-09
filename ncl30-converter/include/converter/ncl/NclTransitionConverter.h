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

#ifndef NclTransitionConverter_H
#define NclTransitionConverter_H

#include "ncl/NclDocument.h"
using namespace ::br::pucrio::telemidia::ncl;

#include "ncl/transition/Transition.h"
#include "ncl/transition/TransitionBase.h"
#include "ncl/transition/TransitionUtil.h"
using namespace ::br::pucrio::telemidia::ncl::transition;

#include "../framework/DocumentParser.h"
using namespace ::br::pucrio::telemidia::converter::framework;

#include "../framework/ncl/NclTransitionParser.h"
using namespace ::br::pucrio::telemidia::converter::framework::ncl;

#include <map>
using namespace std;

#include "util/functions.h"
#include "util/Color.h"
using namespace ::br::pucrio::telemidia::util;

#include <xercesc/dom/DOM.hpp>
XERCES_CPP_NAMESPACE_USE

namespace br {
namespace pucrio {
namespace telemidia {
namespace converter {
namespace ncl {
  class NclTransitionConverter : public NclTransitionParser {
  	public:
		NclTransitionConverter(DocumentParser *documentParser);
		void addTransitionToTransitionBase(
			    void* parentObject, void* childObject);

		void* createTransitionBase(
			    DOMElement* parentElement, void* objGrandParent);

		void* createTransition(
			    DOMElement *parentElement, void *objGrandParent);

		void addImportBaseToTransitionBase(
			    void* parentObject, void* childObject);
  };
}
}
}
}
}

#endif // NclTransitionConverter_H
