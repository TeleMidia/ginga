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

#ifndef NCLCOMPONENTSPARSER_H_
#define NCLCOMPONENTSPARSER_H_

#include "ncl/AbsoluteReferenceContent.h"
#include "ncl/ContentNode.h"
using namespace ::ginga::ncl;

#include "ModuleParser.h"

#include "NclPresentationSpecificationParser.h"
#include "NclLinkingParser.h"
#include "NclInterfacesParser.h"
#include "NclPresentationControlParser.h"

GINGA_NCLCONV_BEGIN

class NclComponentsParser : public ModuleParser
{
public:
  NclComponentsParser (NclDocumentParser *PARSERS_EXPORT);

  void *parseMedia (DOMElement *parentElement, void *objGrandParent);
  void *createMedia (DOMElement *parentElement, void *objGrandParent);

  void addAreaToMedia (void *parentObject, void *childObject);
  void addPropertyToMedia (void *parentObject, void *childObject);

  void *parseContext (DOMElement *parentElement, void *objGrandParent);
  void *posCompileContext2 (DOMElement *parentElement, void *parentObject);
  void *posCompileContext (DOMElement *parentElement, void *parentObject);

  void *createContext (DOMElement *parentElement, void *objGrandParent);

  void addPortToContext (void *parentObject, void *childObject);

  void addPropertyToContext (void *parentObject, void *childObject);

  void addMediaToContext (void *parentObject, void *childObject);
  void addContextToContext (void *parentObject, void *childObject);

  void addLinkToContext (void *parentObject, void *childObject);
  void addSwitchToContext (void *parentObject, void *childObject);

private:
  void addNodeToContext (ContextNode *contextNode, NodeEntity *node);
  void addAnchorToMedia (ContentNode *contentNode, Anchor *anchor);

};

GINGA_NCLCONV_END

#endif /*NCLCOMPONENTSPARSER_H_*/
