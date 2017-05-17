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

#ifndef NCLINTERFACESPARSER_H_
#define NCLINTERFACESPARSER_H_

#include "ModuleParser.h"
#include "NclParser.h"

#include "ncl/Anchor.h"
#include "ncl/PropertyAnchor.h"
#include "ncl/IntervalAnchor.h"
#include "ncl/LabeledAnchor.h"
#include "ncl/RectangleSpatialAnchor.h"
#include "ncl/RelativeTimeIntervalAnchor.h"
#include "ncl/SampleIntervalAnchor.h"
#include "ncl/SpatialAnchor.h"
#include "ncl/TextAnchor.h"
#include "ncl/SwitchNode.h"
#include "ncl/SwitchPort.h"
#include "ncl/Port.h"
#include "ncl/CompositeNode.h"
#include "ncl/NodeEntity.h"
using namespace ::ginga::ncl;

GINGA_NCLCONV_BEGIN

class NclInterfacesParser : public ModuleParser
{
public:
  explicit NclInterfacesParser (NclParser *nclParser);

  SwitchPort *parseSwitchPort (DOMElement *parentElement,
                               SwitchNode *objGrandParent);

  SwitchPort *createSwitchPort (DOMElement *parentElement,
                                SwitchNode *objGrandParent);

  void addMappingToSwitchPort (SwitchPort *parentObject, Port *childObject);

  void *createPort (DOMElement *parentElement, void *objGrandParent);
  Port *parseMapping (DOMElement *parentElement, SwitchPort *objGrandParent);
  Anchor *parseArea (DOMElement *parentElement, void *objGrandParent);
  PropertyAnchor *parseProperty (DOMElement *parentElement, Node *objGrandParent);
  Port *parsePort (DOMElement *parentElement, void *objGrandParent);

private:
  SpatialAnchor *createSpatialAnchor (DOMElement *areaElement);
  IntervalAnchor *createTemporalAnchor (DOMElement *areaElement);
};

GINGA_NCLCONV_END

#endif /*NCLINTERFACESPARSER_H_*/
