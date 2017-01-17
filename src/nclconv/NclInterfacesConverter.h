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

#ifndef NclInterfacesConverter_H
#define NclInterfacesConverter_H

#include "ncl/switches/SwitchNode.h"
using namespace ::br::pucrio::telemidia::ncl::switches;

#include "ncl/interfaces/Anchor.h"
#include "ncl/interfaces/PropertyAnchor.h"
#include "ncl/interfaces/IntervalAnchor.h"
#include "ncl/interfaces/LabeledAnchor.h"
#include "ncl/interfaces/RectangleSpatialAnchor.h"
#include "ncl/interfaces/RelativeTimeIntervalAnchor.h"
#include "ncl/interfaces/SampleIntervalAnchor.h"
#include "ncl/interfaces/SpatialAnchor.h"
#include "ncl/interfaces/TextAnchor.h"
#include "ncl/interfaces/SwitchPort.h"
#include "ncl/interfaces/Port.h"
using namespace ::br::pucrio::telemidia::ncl::interfaces;

#include "ncl/components/CompositeNode.h"
#include "ncl/components/NodeEntity.h"
using namespace ::br::pucrio::telemidia::ncl::components;

#include "DocumentParser.h"
using namespace ::br::pucrio::telemidia::converter::framework;

#include "NclInterfacesParser.h"
using namespace ::br::pucrio::telemidia::converter::framework::ncl;

#include "util/functions.h"
using namespace ::ginga::util;

#include <string>
using namespace std;

#include <xercesc/dom/DOM.hpp>
XERCES_CPP_NAMESPACE_USE

BR_PUCRIO_TELEMIDIA_CONVERTER_NCL_BEGIN

  class NclInterfacesConverter : public NclInterfacesParser {
	public:
		NclInterfacesConverter(DocumentParser *documentParser);
		void *createPort(DOMElement *parentElement, void *objGrandParent);

	private:
		SpatialAnchor *createSpatialAnchor(DOMElement *areaElement);
		IntervalAnchor *createTemporalAnchor(DOMElement *areaElement);

	public:
		void *createProperty(DOMElement *parentElement, void *objGrandParent);
		void *createArea(DOMElement *parentElement, void *objGrandParent);
		void *createMapping(DOMElement *parentElement, void *objGrandParent);
		void *createSwitchPort(DOMElement *parentElement, void *objGrandParent);
		void addMappingToSwitchPort(void *parentObject, void *childObject);
  };

BR_PUCRIO_TELEMIDIA_CONVERTER_NCL_END
#endif
