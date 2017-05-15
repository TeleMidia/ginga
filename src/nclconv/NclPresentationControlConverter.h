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

#ifndef NclPresentationControlConverter_H
#define NclPresentationControlConverter_H

#include "ncl/ReferNode.h"
#include "ncl/ReferredNode.h"
using namespace ::ginga::ncl;

#include "ncl/Descriptor.h"
using namespace ::ginga::ncl;

#include "ncl/DescriptorSwitch.h"
#include "ncl/SwitchNode.h"
#include "ncl/CompositeRule.h"
#include "ncl/SimpleRule.h"
#include "ncl/Rule.h"
#include "ncl/RuleBase.h"
using namespace ::ginga::ncl;

#include "ncl/Port.h"
using namespace ::ginga::ncl;

#include "ncl/Comparator.h"
using namespace ::ginga::ncl;

#include "ncl/NclDocument.h"
using namespace ::ginga::ncl;

#include "ModuleParser.h"
#include "NclDocumentParser.h"

#include "NclPresentationControlParser.h"

#include "NclPresentationSpecConverter.h"
#include "NclComponentsConverter.h"

GINGA_NCLCONV_BEGIN

class NclPresentationControlConverter : public NclPresentationControlParser
{
  // tabelas com componentes e regras de cada switch
  // para reusar logica
  // 1 copiar tabelas abaixo e inicializar
  // 2 copiar os metodos:
  // addBindRuleGroupToContentControlGroup,
  // addCompositionContentGroupToContentControlGroup
  // addMediaContentGroupToContentControlGroup
  // createBindRuleGroup
  // addContentControlGroupToContentControlGroup
  // posCreateContentControlGroup
  // addDescriptorGroupToDescriptorSwitchGroup
  // addBindRuleGroupToDescriptorSwitchGroup
  // 3 adicionar posCreateDescriptorSwitchGroup no
  // createDescriptorSwitchGroup
  // 3 adicionar posCreateContentControlGroup no createContentControlGroup
  // 4 copiar adaptando posCompileContentControlGroup
  // 4 copiar adaptando posCompileDescriptorSwitchGroup
private:
  map<string, map<string, NodeEntity *> *> *switchConstituents;

public:
  NclPresentationControlConverter (NclDocumentParser *documentParser);
  virtual ~NclPresentationControlConverter ();

  vector<Node *> *getSwitchConstituents (SwitchNode *switchNode);
  void addCompositeRuleToCompositeRule (void *parentObject,
                                        void *childObject);

  void addCompositeRuleToRuleBase (void *parentObject, void *childObject);
  void addRuleToCompositeRule (void *parentObject, void *childObject);
  void addRuleToRuleBase (void *parentObject, void *childObject);
  void addSwitchPortToSwitch (void *parentObject, void *childObject);
  void *createCompositeRule (DOMElement *parentElement,
                             void *objGrandParent);

  void *createSwitch (DOMElement *parentElement, void *objGrandParent);
  void *createRuleBase (DOMElement *parentElement, void *objGrandParent);
  void *createRule (DOMElement *parentElement, void *objGrandParent);
  void *createDescriptorSwitch (DOMElement *parentElement,
                                void *objGrandParent);

  void addDescriptorToDescriptorSwitch (void *parentObject,
                                        void *childObject);

  void addImportBaseToRuleBase (void *parentObject, void *childObject);
  void addBindRuleToDescriptorSwitch (void *parentObject,
                                      void *childObject);

  void addBindRuleToSwitch (void *parentObject, void *childObject);
  void addDefaultComponentToSwitch (void *parentObject, void *childObject);
  void addDefaultDescriptorToDescriptorSwitch (void *parentObject,
                                               void *childObject);

  void addContextToSwitch (void *parentObject, void *childObject);
  void addMediaToSwitch (void *parentObject, void *childObject);
  void addSwitchToSwitch (void *parentObject, void *childObject);
  void addUnmappedNodesToSwitch (void *parentObject);

private:
  void addNodeToSwitch (SwitchNode *switchNode, NodeEntity *node);

public:
  void *createBindRule (DOMElement *parentElement, void *objGrandParent);
  void *createDefaultComponent (DOMElement *parentElement,
                                void *objGrandParent);

  void *createDefaultDescriptor (DOMElement *parentElement,
                                 void *objGrandParent);

  void *posCompileSwitch (DOMElement *parentElement, void *parentObject);

private:
  short convertComparator (const string &comparator);
};

GINGA_NCLCONV_END

#endif
