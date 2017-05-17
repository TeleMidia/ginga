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
#include "NclParser.h"

GINGA_NCLCONV_BEGIN

class NclLinkingParser : public ModuleParser
{
public:
  explicit NclLinkingParser (NclParser *nclParser);

  Bind *parseBind (DOMElement *parentElement, Link *objGrandParent);
  Bind *createBind (DOMElement *parentElement, Link *objGrandParent);

  void addBindParamToBind (Bind *parentObject, Parameter *childObject);

  Parameter *parseLinkParam (DOMElement *parentElement);
  Parameter *parseBindParam (DOMElement *parentElement);

  Link *parseLink (DOMElement *parentElement, void *objGrandParent);
  Link *createLink (DOMElement *parentElement, void *objGrandParent);

  void addLinkParamToLink (Link *parentObject, Parameter *childObject);

  void addBindToLink (Link *parentObject, Bind *childObject);

private:
  Connector *_connector;
  CompositeNode *_composite;

  string getId (DOMElement *element);
};

GINGA_NCLCONV_END

#endif /*NCLLINKINGPARSER_H_*/
