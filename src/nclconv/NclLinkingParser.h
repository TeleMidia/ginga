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

#include "ncl/Parameter.h"
#include "ncl/GenericDescriptor.h"
#include "ncl/CausalConnector.h"
#include "ncl/Connector.h"
#include "ncl/Role.h"
#include "ncl/CompositeNode.h"
#include "ncl/NodeEntity.h"
#include "ncl/CausalLink.h"
#include "ncl/Bind.h"
#include "ncl/Link.h"
#include "ncl/ValueAssessment.h"
#include "ncl/Comparator.h"
#include "ncl/NclDocument.h"
using namespace ::ginga::ncl;

#include "ModuleParser.h"
#include "NclDocumentParser.h"

GINGA_NCLCONV_BEGIN

class NclLinkingParser : public ModuleParser
{
public:
  NclLinkingParser (NclDocumentParser *documentParser,
                    NclConnectorsParser *connCompiler);

  void *parseBind (DOMElement *parentElement, void *objGrandParent);
  void *createBind (DOMElement *parentElement, void *objGrandParent);

  void addBindParamToBind (void *parentObject, void *childObject);

  void *parseLinkParam (DOMElement *parentElement, void *objGrandParent);
  void *createLinkParam (DOMElement *parentElement, void *objGrandParent);

  void *parseBindParam (DOMElement *parentElement, void *objGrandParent);
  void *createBindParam (DOMElement *parentElement, void *objGrandParent);

  void *parseLink (DOMElement *parentElement, void *objGrandParent);
  void *createLink (DOMElement *parentElement, void *objGrandParent);

  void addLinkParamToLink (void *parentObject, void *childObject);

  void addBindToLink (void *parentObject, void *childObject);

private:
  Connector *connector;
  CompositeNode *composite;

  string getId (DOMElement *element);
};

GINGA_NCLCONV_END

#endif /*NCLLINKINGPARSER_H_*/
