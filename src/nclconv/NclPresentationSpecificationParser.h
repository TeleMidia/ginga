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

#ifndef NCLPRESENTATIONSPECIFICATIONPARSER_H_
#define NCLPRESENTATIONSPECIFICATIONPARSER_H_

#include "ncl/Parameter.h"
#include "ncl/GenericDescriptor.h"
#include "ncl/Descriptor.h"
#include "ncl/DescriptorBase.h"
#include "ncl/Rule.h"
#include "ncl/RuleBase.h"
#include "ncl/LayoutRegion.h"
#include "ncl/RegionBase.h"
#include "ncl/NclDocument.h"
#include "ncl/FocusDecoration.h"
#include "ncl/KeyNavigation.h"
#include "ncl/Transition.h"
#include "ncl/TransitionBase.h"
using namespace ::ginga::ncl;


#include "ModuleParser.h"
#include "NclParser.h"

#include "NclImportParser.h"

GINGA_NCLCONV_BEGIN

class NclPresentationSpecificationParser : public ModuleParser
{
private:
  DeviceLayout *deviceLayout;

public:
  explicit NclPresentationSpecificationParser (NclParser *nclParser,
                                               DeviceLayout *deviceLayout);

  Descriptor *parseDescriptor (DOMElement *descriptor_element);
  Descriptor *createDescriptor (DOMElement *descriptor_element);

  DescriptorBase *parseDescriptorBase (DOMElement *descriptorBase_element);
  DescriptorBase *createDescriptorBase (DOMElement *descriptorBase_element);

  void addImportBaseToDescriptorBase (DescriptorBase *descriptorBase,
                                      DOMElement *childObject);

  DOMElement *parseDescriptorBind (DOMElement *parentElement);

  DOMElement *parseDescriptorParam (DOMElement *parentElement);
};

GINGA_NCLCONV_END

#endif /*NCLPRESENTATIONSPECIFICATIONPARSER_H_*/
