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

#ifndef NCLSTRUCTUREPARSER_H_
#define NCLSTRUCTUREPARSER_H_

#include "ModuleParser.h"
#include "NclDocumentParser.h"

#include "NclTransitionParser.h"
#include "NclPresentationSpecificationParser.h"
#include "NclComponentsParser.h"
#include "NclLinkingParser.h"
#include "NclLayoutParser.h"
#include "NclInterfacesParser.h"
#include "NclPresentationControlParser.h"
#include "NclConnectorsParser.h"
#include "NclImportParser.h"
#include "NclMetainformationParser.h"

GINGA_NCLCONV_BEGIN

class NclStructureParser : public ModuleParser
{
public:
  NclStructureParser (NclDocumentParser *documentParser);

  ContextNode *parseBody ( DOMElement *parentElement, NclDocument *nclDoc );

  void *posCompileBody (DOMElement *parentElement, ContextNode *body);

  ContextNode *createBody (DOMElement *parentElement, void *objGrandParent);

  void parseHead (DOMElement *parentElement);

  void addImportedDocumentBaseToHead (void *parentObject, void *childObject);

  NclDocument *parseNcl (DOMElement *parentElement);

  NclDocument *createNcl (DOMElement *parentElement);

private:
  void solveNodeReferences (CompositeNode *composition);
};

GINGA_NCLCONV_END

#endif /*NCLSTRUCTUREPARSER_H_*/
