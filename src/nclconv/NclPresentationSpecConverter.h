/* Copyright (C) 2006-2017 PUC-Rio/Laboratorio TeleMidia

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

#ifndef NclPresentationSpecConverter_H
#define NclPresentationSpecConverter_H

#include <vector>
#include <string>
using namespace std;

#include "util/Color.h"

#include "ncl/Parameter.h"
using namespace ::br::pucrio::telemidia::ncl;

#include "ncl/descriptor/GenericDescriptor.h"
#include "ncl/descriptor/Descriptor.h"
#include "ncl/descriptor/DescriptorBase.h"
using namespace ::br::pucrio::telemidia::ncl::descriptor;

#include "ncl/switches/Rule.h"
#include "ncl/switches/RuleBase.h"
using namespace ::br::pucrio::telemidia::ncl::switches;

#include "ncl/layout/LayoutRegion.h"
#include "ncl/layout/RegionBase.h"
using namespace ::br::pucrio::telemidia::ncl::layout;

#include "ncl/NclDocument.h"
using namespace ::br::pucrio::telemidia::ncl;

#include "ncl/navigation/FocusDecoration.h"
#include "ncl/navigation/KeyNavigation.h"
using namespace ::br::pucrio::telemidia::ncl::navigation;

#include "ncl/transition/Transition.h"
#include "ncl/transition/TransitionBase.h"
using namespace ::br::pucrio::telemidia::ncl::transition;

#include "DocumentParser.h"
#include "ModuleParser.h"
using namespace ::br::pucrio::telemidia::converter::framework;

#include "NclPresentationSpecificationParser.h"
using namespace ::br::pucrio::telemidia::converter::framework::ncl;

#include "NclImportConverter.h"
#include "NclPresentationControlConverter.h"

#include <xercesc/dom/DOM.hpp>
XERCES_CPP_NAMESPACE_USE

BR_PUCRIO_TELEMIDIA_CONVERTER_NCL_BEGIN

  class NclPresentationSpecConverter
		: public NclPresentationSpecificationParser {

	private:
		IDeviceLayout* deviceLayout;

	public:
		NclPresentationSpecConverter(
				DocumentParser *documentParser, IDeviceLayout* deviceLayout);

		void addDescriptorToDescriptorBase(
			    void *parentObject, void *childObject);

		void addDescriptorSwitchToDescriptorBase(
			    void *parentObject, void *childObject);

		void addDescriptorParamToDescriptor(
			    void *parentObject, void *childObject);

		void addImportBaseToDescriptorBase(
			    void *parentObject, void *childObject);

		void *createDescriptorParam(
			    DOMElement *parentElement, void *objGrandParent);

		void *createDescriptorBind(
			    DOMElement *parentElement, void *objGrandParent);

		void *createDescriptorBase(
			    DOMElement *parentElement, void *objGrandParent);

		void *createDescriptor(
			    DOMElement *parentElement, void *objGrandParent);
  };

BR_PUCRIO_TELEMIDIA_CONVERTER_NCL_END
#endif //NclPresentationSpecConverter_H
