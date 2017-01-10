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

#ifndef NclLayoutConverter_H
#define NclLayoutConverter_H

#include "util/functions.h"
using namespace ::br::pucrio::telemidia::util;

#include "ncl/layout/LayoutRegion.h"
using namespace ::br::pucrio::telemidia::ncl::layout;

#include "ncl/NclDocument.h"
using namespace ::br::pucrio::telemidia::ncl;

#include "DocumentParser.h"
#include "ModuleParser.h"
using namespace ::br::pucrio::telemidia::converter::framework;

#include "NclLayoutParser.h"
using namespace ::br::pucrio::telemidia::converter::framework::ncl;

#include "NclImportConverter.h"

#include <string>
#include <vector>
using namespace std;

#include <xercesc/dom/DOM.hpp>
XERCES_CPP_NAMESPACE_USE

namespace br {
namespace pucrio {
namespace telemidia {
namespace converter {
namespace ncl {
  class NclLayoutConverter : public NclLayoutParser {
	private:
		IDeviceLayout* deviceLayout;

	public:
		NclLayoutConverter(
				DocumentParser *documentParser, IDeviceLayout* deviceLayout);

		void addImportBaseToRegionBase(void *parentObject, void *childObject);
		void addRegionToRegion(void *parentObject, void *childObject);
		void addRegionToRegionBase(void *parentObject, void *childObject);
		void *createRegionBase(DOMElement *parentElement, void *objGrandParent);
		void *createRegion(DOMElement *parentElement, void *objGrandParent);

	private:
		float getPercentualValue(string value);
		int getPixelValue(string value);
		bool isPercentualValue(string value);
  };
}
}
}
}
}

#endif
