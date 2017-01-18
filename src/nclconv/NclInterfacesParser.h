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
#include "DocumentParser.h"
using namespace ::ginga::nclconv;

XERCES_CPP_NAMESPACE_USE

GINGA_NCLCONV_BEGIN

class NclInterfacesParser : public ModuleParser
{
public:
  NclInterfacesParser (DocumentParser *documentParser);
  void *parseSwitchPort (DOMElement *parentElement, void *objGrandParent);
  virtual void *createSwitchPort (DOMElement *parentElement,
                                  void *objGrandParent)
      = 0;

  virtual void addMappingToSwitchPort (void *parentObject, void *childObject)
      = 0;

  virtual void *createArea (DOMElement *parentElement, void *objGrandParent)
      = 0;

  virtual void *createProperty (DOMElement *parentElement,
                                void *objGrandParent)
      = 0;

  virtual void *createPort (DOMElement *parentElement, void *objGrandParent)
      = 0;

  void *parseMapping (DOMElement *parentElement, void *objGrandParent);
  virtual void *createMapping (DOMElement *parentElement, void *objGrandParent)
      = 0;

  void *parseArea (DOMElement *parentElement, void *objGrandParent);
  void *parseProperty (DOMElement *parentElement, void *objGrandParent);
  void *parsePort (DOMElement *parentElement, void *objGrandParent);
};

GINGA_NCLCONV_END

#endif /*NCLINTERFACESPARSER_H_*/
