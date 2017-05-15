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

#include "ncl/Parameter.h"
using namespace ::ginga::ncl;

#include "ncl/GenericDescriptor.h"
#include "ncl/Descriptor.h"
#include "ncl/DescriptorBase.h"
using namespace ::ginga::ncl;

#include "ncl/Rule.h"
#include "ncl/RuleBase.h"
using namespace ::ginga::ncl;

#include "ncl/LayoutRegion.h"
#include "ncl/RegionBase.h"
using namespace ::ginga::ncl;

#include "ncl/NclDocument.h"
using namespace ::ginga::ncl;

#include "ncl/FocusDecoration.h"
#include "ncl/KeyNavigation.h"
using namespace ::ginga::ncl;

#include "ncl/Transition.h"
#include "ncl/TransitionBase.h"
using namespace ::ginga::ncl;

#include "NclDocumentParser.h"
#include "ModuleParser.h"

#include "NclPresentationSpecificationParser.h"

#include "NclPresentationControlConverter.h"

GINGA_NCLCONV_BEGIN

class NclPresentationSpecConverter
    : public NclPresentationSpecificationParser
{
private:
  DeviceLayout *deviceLayout;

public:
  NclPresentationSpecConverter (NclDocumentParser *documentParser,
                                DeviceLayout *deviceLayout);

  void addDescriptorToDescriptorBase (void *parentObject,
                                      void *childObject);

  void addDescriptorSwitchToDescriptorBase (void *parentObject,
                                            void *childObject);

  void addDescriptorParamToDescriptor (void *parentObject,
                                       void *childObject);

  void addImportBaseToDescriptorBase (void *parentObject,
                                      void *childObject);

  void *createDescriptorParam (DOMElement *parentElement,
                               void *objGrandParent);

  void *createDescriptorBind (DOMElement *parentElement,
                              void *objGrandParent);

  void *createDescriptorBase (DOMElement *parentElement,
                              void *objGrandParent);

  void *createDescriptor (DOMElement *parentElement, void *objGrandParent);
};

GINGA_NCLCONV_END

#endif // NclPresentationSpecConverter_H
