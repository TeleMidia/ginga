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

#ifndef NCLIMPORTPARSER_H_
#define NCLIMPORTPARSER_H_

#include "ModuleParser.h"

GINGA_NCLCONV_BEGIN

class NclImportParser : public ModuleParser
{
public:
  NclImportParser (NclDocumentParser *documentParser);

  void *parseImportedDocumentBase (DOMElement *parentElement,
                                   void *objGrandParent);
  void *createImportedDocumentBase (DOMElement *parentElement,
                                    void *objGrandParent);

  void *parseImportNCL (DOMElement *parentElement, void *objGrandParent);
  void *createImportNCL (DOMElement *parentElement, void *objGrandParent);

  void addImportNCLToImportedDocumentBase (void *parentObject,
                                           void *childObject);

  void *parseImportBase (DOMElement *parentElement, void *objGrandParent);
  void *createImportBase (DOMElement *parentElement, void *objGrandParent);

};

GINGA_NCLCONV_END

#endif /*NCLIMPORTPARSER_H_*/
