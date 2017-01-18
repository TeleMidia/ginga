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

#include "ncl/SwitchNode.h"
using namespace ::ginga::ncl;

#include "ncl/Anchor.h"
#include "ncl/PropertyAnchor.h"
#include "ncl/IntervalAnchor.h"
#include "ncl/LabeledAnchor.h"
#include "ncl/RectangleSpatialAnchor.h"
#include "ncl/RelativeTimeIntervalAnchor.h"
#include "ncl/SampleIntervalAnchor.h"
#include "ncl/SpatialAnchor.h"
#include "ncl/TextAnchor.h"
#include "ncl/SwitchPort.h"
#include "ncl/Port.h"
using namespace ::ginga::ncl;

#include "ncl/CompositeNode.h"
#include "ncl/NodeEntity.h"
using namespace ::ginga::ncl;

#include "DocumentParser.h"
using namespace ::ginga::nclconv;

#include "NclInterfacesParser.h"
using namespace ::ginga::nclconv;

#include "util/functions.h"
using namespace ::ginga::util;

#include <string>
using namespace std;

#include <xercesc/dom/DOM.hpp>
XERCES_CPP_NAMESPACE_USE

GINGA_NCLCONV_BEGIN

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

GINGA_NCLCONV_END
#endif
