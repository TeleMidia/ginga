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

#ifndef ModuleParser_H
#define ModuleParser_H

#include "ncl/NclDocument.h"
#include "ncl/TransitionUtil.h"
#include "ncl/Rule.h"
#include "ncl/SimpleRule.h"
#include "ncl/CompositeRule.h"
#include "ncl/ContentNode.h"
#include "ncl/AbsoluteReferenceContent.h"
#include "ncl/CausalConnector.h"
#include "ncl/ValueAssessment.h"
#include "ncl/SwitchNode.h"
#include "ncl/SpatialAnchor.h"
#include "ncl/TextAnchor.h"
#include "ncl/LabeledAnchor.h"
#include "ncl/SampleIntervalAnchor.h"
#include "ncl/RectangleSpatialAnchor.h"
#include "ncl/CausalLink.h"
#include "ncl/ReferredNode.h"
#include "ncl/Comparator.h"
#include "ncl/Descriptor.h"
using namespace ginga::ncl;

GINGA_NCLCONV_BEGIN

class NclParser;

class ParserModule
{
protected:
  NclParser *_nclParser;

public:
  explicit ParserModule (NclParser *nclParser) { this->_nclParser = nclParser; }
  virtual ~ParserModule () { }

  NclParser *getNclParser () { return _nclParser; }
};

#define PARSER_MODULE_BEGIN(X) \
  class X : public ParserModule \
  { \
    public: \
      explicit X (NclParser *nclParser) : ParserModule (nclParser) { };

#define PARSER_MODULE_END \
  };

PARSER_MODULE_BEGIN (NclStructureParser)
public:
  NclDocument *parseNcl (DOMElement *parentElement);
  NclDocument *createNcl (DOMElement *parentElement);

  void parseHead (DOMElement *parentElement);

  ContextNode *parseBody (DOMElement *parentElement);
  ContextNode *createBody (DOMElement *parentElement);
  void *posCompileBody (DOMElement *parentElement, ContextNode *body);

private:
  void solveNodeReferences (CompositeNode *composition);
PARSER_MODULE_END

PARSER_MODULE_BEGIN (NclComponentsParser)
public:
  Node *parseMedia (DOMElement *parentElement);
  Node *createMedia (DOMElement *parentElement);

  Node *parseContext (DOMElement *parentElement);
  ContextNode *posCompileContext (DOMElement *ctx_element, ContextNode *ctx);

  Node *createContext (DOMElement *parentElement);

  void addNodeToContext (Entity *contextNode, Node *node);
  void addNodeToContext (ContextNode *contextNode, Node *node);
  void addPropertyToContext (Entity *parentObject, Anchor *childObject);

private:
  void addLinkToContext (ContextNode *context, Link *link);
  void addAnchorToMedia (ContentNode *contentNode, Anchor *anchor);
PARSER_MODULE_END

PARSER_MODULE_BEGIN (NclImportParser)
public:
  void parseImportedDocumentBase (DOMElement *importedDocBase_element);
  void addImportNCLToImportedDocumentBase (DOMElement *childObject);
PARSER_MODULE_END

PARSER_MODULE_BEGIN (NclMetainformationParser)
public:
  Meta *parseMeta (DOMElement *parentElement);
  Metadata *parseMetadata (DOMElement *parentElement);
PARSER_MODULE_END

PARSER_MODULE_BEGIN (NclTransitionParser)
public:
  TransitionBase *parseTransitionBase (DOMElement *transitionBase_element);
  Transition *parseTransition (DOMElement *transitionBase_element);

  void addImportBaseToTransitionBase (TransitionBase *, DOMElement *);
PARSER_MODULE_END

PARSER_MODULE_BEGIN (NclConnectorsParser)
public:
  SimpleCondition *parseSimpleCondition (DOMElement *simpleCond_element);
  SimpleAction *parseSimpleAction (DOMElement *simpleAction_element);

  CompoundCondition *parseCompoundCondition (DOMElement *compoundCond_element);
  CompoundCondition *createCompoundCondition (DOMElement *compoundCond_element);

  AssessmentStatement *parseAssessmentStatement (DOMElement *assessmentStatement_element);
  AssessmentStatement *createAssessmentStatement (DOMElement *assessmentStatement_element);

  AttributeAssessment *parseAttributeAssessment (DOMElement *attributeAssessment_element);
  ValueAssessment *parseValueAssessment (DOMElement *valueAssessment_element);

  CompoundStatement *parseCompoundStatement (DOMElement *compoundStatement_element);
  CompoundStatement *createCompoundStatement (DOMElement *compoundStatement_element);

  CompoundAction *parseCompoundAction (DOMElement *compoundAction_element);
  CompoundAction *createCompoundAction (DOMElement *compoundAction_element);

  ConnectorBase *parseConnectorBase (DOMElement *connBase_element);
  ConnectorBase *createConnectorBase (DOMElement *connBase_element);

  Parameter *parseConnectorParam (DOMElement *connectorParam_element);

  CausalConnector *parseCausalConnector (DOMElement *causalConnector_element);
  CausalConnector *createCausalConnector (DOMElement *causalConnector_element);

private:
  Connector *connector;

  void compileRoleInformation (Role *role, DOMElement *parentElement);
  void addAttributeAssessmentToAssessmentStatement (
      AssessmentStatement *parentObject, AttributeAssessment *childObject);
  void addImportBaseToConnectorBase (ConnectorBase *connectorBase,
                                     DOMElement *childObject);
PARSER_MODULE_END

PARSER_MODULE_BEGIN (NclInterfacesParser)
public:
  SwitchPort *parseSwitchPort (DOMElement *switchPort_element,
                               SwitchNode *switchNode);
  SwitchPort *createSwitchPort (DOMElement *switchPort_element,
                                SwitchNode *switchNode);

  Port *parseMapping (DOMElement *parentElement, SwitchPort *switchPort);
  Anchor *parseArea (DOMElement *parentElement);
  PropertyAnchor *parseProperty (DOMElement *parentElement);
  Port *parsePort (DOMElement *parentElement, CompositeNode *objGrandParent);

private:
  SpatialAnchor *createSpatialAnchor (DOMElement *areaElement);
  IntervalAnchor *createTemporalAnchor (DOMElement *areaElement);
PARSER_MODULE_END

PARSER_MODULE_BEGIN (NclLayoutParser)
public:
  LayoutRegion *parseRegion (DOMElement *region_element);
  LayoutRegion *createRegion (DOMElement *region_element);

  RegionBase *parseRegionBase (DOMElement *regionBase_element);
  RegionBase *createRegionBase (DOMElement *regionBase_element);

  void addImportBaseToRegionBase (RegionBase *regionBase,
                                  DOMElement *importBase_element);
PARSER_MODULE_END

PARSER_MODULE_BEGIN (NclLinkingParser)
public:
  Bind *parseBind (DOMElement *parentElement, Link *link);
  Bind *createBind (DOMElement *parentElement, Link *link);

  Parameter *parseLinkOrBindParam (DOMElement *parentElement);

  Link *parseLink (DOMElement *link_element, CompositeNode *compositeNode);
  Link *createLink (DOMElement *link_element, CompositeNode *compositeNode);

private:
  Connector *_connector = NULL;
  CompositeNode *_composite = NULL;
PARSER_MODULE_END

PARSER_MODULE_BEGIN (NclPresentationControlParser)
public:
  virtual ~NclPresentationControlParser();

  DOMElement *parseBindRule (DOMElement *bindRule_element);

  RuleBase *parseRuleBase (DOMElement *parentElement);
  RuleBase *createRuleBase (DOMElement *parentElement);

  void addImportBaseToRuleBase (RuleBase *ruleBase, DOMElement *importBase_element);

  SimpleRule *parseRule (DOMElement *parentElement);

  Node *parseSwitch (DOMElement *parentElement);
  void *posCompileSwitch (DOMElement *parentElement, SwitchNode *parentObject);

  DOMElement *parseDefaultComponent (DOMElement *parentElement);
  DOMElement *parseDefaultDescriptor (DOMElement *parentElement);

  Node *createSwitch (DOMElement *parentElement);

  CompositeRule *parseCompositeRule (DOMElement *parentElement);
  CompositeRule *createCompositeRule (DOMElement *parentElement);

  DescriptorSwitch *parseDescriptorSwitch (DOMElement *parentElement);
  DescriptorSwitch *createDescriptorSwitch (DOMElement *parentElement);

  vector<Node *> *getSwitchConstituents (SwitchNode *switchNode);

private:
  map<string, map<string, Node *> *> switchConstituents;

  void addNodeToSwitch (Node *switchNode, Node *node);
  void addBindRuleToSwitch (SwitchNode *parentObject, DOMElement *childObject);
  void addUnmappedNodesToSwitch (SwitchNode *switchNode);
  void addDefaultDescriptorToDescriptorSwitch (DescriptorSwitch *descriptorSwitch,
                                               DOMElement *defaultDescriptor);
  void addDefaultComponentToSwitch (SwitchNode *switchNode,
                                    DOMElement *defaultComponent);
  void addBindRuleToDescriptorSwitch (DescriptorSwitch *descriptorSwitch,
                                      DOMElement *bindRule_element);
  void addDescriptorToDescriptorSwitch (DescriptorSwitch *descriptorSwitch,
                                        GenericDescriptor *descriptor);
PARSER_MODULE_END

PARSER_MODULE_BEGIN (NclPresentationSpecificationParser)
public:
  Descriptor *parseDescriptor (DOMElement *descriptor_element);
  Descriptor *createDescriptor (DOMElement *descriptor_element);

  DescriptorBase *parseDescriptorBase (DOMElement *descriptorBase_element);
  DescriptorBase *createDescriptorBase (DOMElement *descriptorBase_element);

  void addImportBaseToDescriptorBase (DescriptorBase *descriptorBase,
                                      DOMElement *childObject);

  DOMElement *parseDescriptorBind (DOMElement *descriptorBind_element);

  DOMElement *parseDescriptorParam (DOMElement *descriptorParam_element);
PARSER_MODULE_END

GINGA_NCLCONV_END

#endif
