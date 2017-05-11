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

#ifndef NclLayoutConverter_H
#define NclLayoutConverter_H

#include "util/functions.h"
using namespace ::ginga::util;

#include "ncl/LayoutRegion.h"
using namespace ::ginga::ncl;

#include "ncl/NclDocument.h"
using namespace ::ginga::ncl;

#include "DocumentParser.h"
#include "ModuleParser.h"

#include "NclLayoutParser.h"

#include "NclImportConverter.h"

GINGA_NCLCONV_BEGIN

class NclLayoutConverter : public NclLayoutParser
{
private:
  DeviceLayout *deviceLayout;

public:
  NclLayoutConverter (DocumentParser *documentParser,
                      DeviceLayout *deviceLayout);

  void addImportBaseToRegionBase (void *parentObject, void *childObject);
  void addRegionToRegion (void *parentObject, void *childObject);
  void addRegionToRegionBase (void *parentObject, void *childObject);
  void *createRegionBase (DOMElement *parentElement, void *objGrandParent);
  void *createRegion (DOMElement *parentElement, void *objGrandParent);

private:
  double getPercentualValue (const string &value);
  int getPixelValue (const string &value);
  bool isPercentualValue (const string &value);
};

GINGA_NCLCONV_END

#endif
