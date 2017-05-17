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
  explicit NclComponentsParser (NclParser *nclparser);

  Node *parseMedia (DOMElement *parentElement);
  Node *createMedia (DOMElement *parentElement);

  void addAreaToMedia (ContentNode *media, Anchor *area);
  void addPropertyToMedia (ContentNode *media, Anchor *property);

  Node *parseContext (DOMElement *parentElement);
  void *posCompileContext2 (DOMElement *context_element, ContextNode *context);
  void *posCompileContext (DOMElement *parentElement, ContextNode *parentObject);

  Node *createContext (DOMElement *parentElement);


  void addMediaToContext (Entity *parentObject, Node *childObject);
  void addContextToContext (Entity *parentObject, Node *childObject);
  void addSwitchToContext (Entity *parentObject, Node *childObject);
  void addPropertyToContext (Entity *parentObject, Anchor *childObject);

private:
  void addLinkToContext (ContextNode *context, Link *link);
  void addNodeToContext (ContextNode *contextNode, Node *node);
  void addAnchorToMedia (ContentNode *contentNode, Anchor *anchor);
  void addPortToContext (Entity *parentObject, Port *childObject);

};

GINGA_NCLCONV_END

#endif /*NCLCOMPONENTSPARSER_H_*/
