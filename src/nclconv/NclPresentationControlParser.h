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

#ifndef NCLPRESENTATIONCONTROLPARSER_H_
#define NCLPRESENTATIONCONTROLPARSER_H_

#include "ncl/ReferNode.h"
#include "ncl/ReferredNode.h"
#include "ncl/Descriptor.h"
#include "ncl/DescriptorSwitch.h"
#include "ncl/SwitchNode.h"
#include "ncl/CompositeRule.h"
#include "ncl/SimpleRule.h"
#include "ncl/Rule.h"
#include "ncl/RuleBase.h"
#include "ncl/Port.h"
#include "ncl/Comparator.h"
#include "ncl/NclDocument.h"
using namespace ::ginga::ncl;


#include "ModuleParser.h"
#include "NclParser.h"

#include "NclPresentationSpecificationParser.h"
#include "NclInterfacesParser.h"
#include "NclImportParser.h"

GINGA_NCLCONV_BEGIN

class NclPresentationControlParser : public ModuleParser
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
  NclPresentationControlParser (NclParser *nclParser);
  virtual ~NclPresentationControlParser();

  DOMElement *parseBindRule (DOMElement *parentElement);

  RuleBase *parseRuleBase (DOMElement *parentElement);
  RuleBase *createRuleBase (DOMElement *parentElement);

  void addImportBaseToRuleBase (void *parentObject, void *childObject);

  void addRuleToRuleBase (void *parentObject, void *childObject);

  void addCompositeRuleToRuleBase (void *parentObject, void *childObject);

  SimpleRule *parseRule (DOMElement *parentElement);

  Node *parseSwitch (DOMElement *parentElement);
  void *posCompileSwitch (DOMElement *parentElement, void *parentObject);
  void *posCompileSwitch2 (DOMElement *parentElement, void *parentObject);

  DOMElement *parseDefaultComponent (DOMElement *parentElement);

  void addDefaultComponentToSwitch (void *parentObject, void *childObject);

  DOMElement *parseDefaultDescriptor (DOMElement *parentElement);

  void addDefaultDescriptorToDescriptorSwitch (void *parentObject,
                                               void *childObject);

  Node *createSwitch (DOMElement *parentElement);

  void addSwitchPortToSwitch (void *parentObject, void *childObject);

  void addMediaToSwitch (void *parentObject, void *childObject);

  void addContextToSwitch (void *parentObject, void *childObject);

  void addSwitchToSwitch (void *parentObject, void *childObject);

  void addBindRuleToSwitch (SwitchNode *parentObject, DOMElement *childObject);

  void addUnmappedNodesToSwitch (SwitchNode *switchNode);

  CompositeRule *parseCompositeRule (DOMElement *parentElement);
  CompositeRule *createCompositeRule (DOMElement *parentElement);

  void addRuleToCompositeRule (void *parentObject, void *childObject);

  void addCompositeRuleToCompositeRule (void *parentObject, void *childObject);

  DescriptorSwitch *parseDescriptorSwitch (DOMElement *parentElement);

  DescriptorSwitch *createDescriptorSwitch (DOMElement *parentElement);

  void addBindRuleToDescriptorSwitch (void *parentObject, void *childObject);

  void addDescriptorToDescriptorSwitch (void *parentObject, void *childObject);

  vector<Node *> *getSwitchConstituents (SwitchNode *switchNode);

private:
  void addNodeToSwitch (SwitchNode *switchNode, NodeEntity *node);
};

GINGA_NCLCONV_END

#endif /*NCLPRESENTATIONCONTROLPARSER_H_*/
