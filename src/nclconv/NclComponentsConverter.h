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

#ifndef NclComponentsConverter_H
#define NclComponentsConverter_H

#include "ncl/ReferNode.h"
#include "ncl/ReferredNode.h"
using namespace ::ginga::ncl;

#include "ncl/SwitchNode.h"
using namespace ::ginga::ncl;

#include "ncl/GenericDescriptor.h"
using namespace ::ginga::ncl;

#include "ncl/Anchor.h"
#include "ncl/Port.h"
using namespace ::ginga::ncl;

#include "ncl/NodeEntity.h"
#include "ncl/ContextNode.h"
#include "ncl/ContentNode.h"
#include "ncl/AbsoluteReferenceContent.h"
using namespace ::ginga::ncl;

#include "ncl/Link.h"
using namespace ::ginga::ncl;

#include "ncl/Role.h"
using namespace ::ginga::ncl;

#include "ncl/NclDocument.h"
using namespace ::ginga::ncl;

#include "ModuleParser.h"
#include "NclDocumentParser.h"

#include "NclComponentsParser.h"

#include "NclPresentationSpecConverter.h"
#include "NclLinkingConverter.h"
#include "NclPresentationControlConverter.h"

GINGA_NCLCONV_BEGIN

class NclComponentsConverter : public NclComponentsParser
{
  /*tabela com a lista dos elementos q devem ser compilados ao fim do
   * processamento da composicao, ja' que referenciam outros elementos
   * TODO deve ser feito o mesmo para linkBase, mas como esse elemento
   * costuma
   * vir ao final da composicao, foi ignorado pelo momento
   * para reaproveitar:
   * 1) copiar e inicializar as vars. abaixo
   * 2) criar addbinddescriptor...2 para representar o add original
   * 3) criar addcompositeinterface...2 para representar o add original
   * 4) copiar o addbinddescriptor... addcompositeinterface...
   *    posCreateCompositionContentGroup e posCompileCompositionContentGroup
   *
   * 5)chamar o posCreateCompositionContentGroup do createcomposition
   * 6) chamar o posCreateCompositionContentGroup na criacao do body
   * 7) chamar o posCompileCompositionContentGroup no poscompile do body
   * 8) criar createBindDescriptorGroup2 e fazer o antigo retornar o
   * elemento
   * 9) criar createCompositeInterfaceElementGroup2 e fazer o antigo
   * retornar
   *    o elemento
   */

public:
  NclComponentsConverter (NclDocumentParser *documentParser);
  void addPortToContext (void *parentObject, void *childObject);
  void addPropertyToContext (void *parentObject, void *childObject);
  void addContextToContext (void *parentObject, void *childObject);
  void addMediaToContext (void *parentObject, void *childObject);
  void addLinkToContext (void *parentObject, void *childObject);

private:
  void addNodeToContext (ContextNode *contextNode, NodeEntity *node);
  void addAnchorToMedia (ContentNode *contentNode, Anchor *anchor);

public:
  void addAreaToMedia (void *parentObject, void *childObject);
  void addPropertyToMedia (void *parentObject, void *childObject);
  void addSwitchToContext (void *parentObject, void *childObject);
  void *createContext (DOMElement *parentElement, void *objGrandParent);
  void *posCompileContext (DOMElement *parentElement, void *parentObject);
  void *createMedia (DOMElement *parentElement, void *objGrandParent);
};

GINGA_NCLCONV_END

#endif // NclComponentsConverter
