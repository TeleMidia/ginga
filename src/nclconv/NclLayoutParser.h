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

#ifndef NCLLAYOUTPARSER_H_
#define NCLLAYOUTPARSER_H_

#include "ModuleParser.h"
#include "NclDocumentParser.h"

#include "NclImportParser.h"

GINGA_NCLCONV_BEGIN

class NclLayoutParser : public ModuleParser
{
private:
  DeviceLayout *deviceLayout;

public:
  NclLayoutParser (NclDocumentParser *documentParser,
                   DeviceLayout *deviceLayout);

  LayoutRegion *parseRegion (DOMElement *parentElement);
  LayoutRegion *createRegion (DOMElement *parentElement);

  void addRegionToRegion (LayoutRegion *parentObject,
                          LayoutRegion *childObject);

  RegionBase *parseRegionBase (DOMElement *parentElement);
  RegionBase *createRegionBase (DOMElement *parentElement);

  void addImportBaseToRegionBase (RegionBase *parentObject,
                                  DOMElement *childObject);

  void addRegionToRegionBase (RegionBase *parentObject,
                              LayoutRegion *childObject);

private:
  double getPercentValue (const string &value);
  int getPixelValue (const string &value);
  bool isPercentValue (const string &value);
};

GINGA_NCLCONV_END

#endif /*NCLLAYOUTPARSER_H_*/
