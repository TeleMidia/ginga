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

#ifndef NCLLINKINGPARSER_H_
#define NCLLINKINGPARSER_H_

#include "ModuleParser.h"
#include "DocumentParser.h"

XERCES_CPP_NAMESPACE_USE

GINGA_NCLCONV_BEGIN

class NclLinkingParser : public ModuleParser
{
public:
  NclLinkingParser (DocumentParser *documentParser);
  void *parseBind (DOMElement *parentElement, void *objGrandParent);
  virtual void *createBind (DOMElement *parentElement, void *objGrandParent)
      = 0;

  virtual void addBindParamToBind (void *parentObject, void *childObject)
      = 0;

  void *parseLinkParam (DOMElement *parentElement, void *objGrandParent);
  virtual void *createLinkParam (DOMElement *parentElement,
                                 void *objGrandParent)
      = 0;

  void *parseBindParam (DOMElement *parentElement, void *objGrandParent);
  virtual void *createBindParam (DOMElement *parentElement,
                                 void *objGrandParent)
      = 0;

  void *parseLink (DOMElement *parentElement, void *objGrandParent);
  virtual void *createLink (DOMElement *parentElement, void *objGrandParent)
      = 0;

  virtual void addLinkParamToLink (void *parentObject, void *childObject)
      = 0;

  virtual void addBindToLink (void *parentObject, void *childObject) = 0;
};

GINGA_NCLCONV_END

#endif /*NCLLINKINGPARSER_H_*/
