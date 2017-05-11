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

#ifndef NclLinkingConverter_H
#define NclLinkingConverter_H

#include "ncl/Parameter.h"
using namespace ::ginga::ncl;

#include "ncl/GenericDescriptor.h"
using namespace ::ginga::ncl;

#include "ncl/CausalConnector.h"
#include "ncl/Connector.h"
#include "ncl/Role.h"
using namespace ::ginga::ncl;

#include "ncl/CompositeNode.h"
#include "ncl/NodeEntity.h"
using namespace ::ginga::ncl;

#include "ncl/CausalLink.h"
#include "ncl/Bind.h"
#include "ncl/Link.h"
using namespace ::ginga::ncl;

#include "ncl/NclDocument.h"
using namespace ::ginga::ncl;

#include "DocumentParser.h"
#include "ModuleParser.h"

#include "NclLinkingParser.h"

#include "NclConnectorsConverter.h"

GINGA_NCLCONV_BEGIN

class NclLinkingConverter : public NclLinkingParser
{
private:
  Connector *connector;
  CompositeNode *composite;

public:
  NclLinkingConverter (DocumentParser *documentParser,
                       NclConnectorsConverter *connCompiler);

  void addBindToLink (void *parentObject, void *childObject);
  void addBindParamToBind (void *parentObject, void *childObject);
  void addLinkParamToLink (void *parentObject, void *childObject);
  void *createBind (DOMElement *parentElement, void *objGrandParent);
  void *createLink (DOMElement *parentElement, void *objGrandParent);
  void *createBindParam (DOMElement *parentElement, void *objGrandParent);
  void *createLinkParam (DOMElement *parentElement, void *objGrandParent);

private:
  string getId (DOMElement *element);
};

GINGA_NCLCONV_END

#endif // NclLinkingConverter_H
