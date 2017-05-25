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

#ifndef PARSER_H
#define PARSER_H

GINGA_PRAGMA_DIAG_PUSH ()
GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)
GINGA_PRAGMA_DIAG_IGNORE (-Wundef)
#include <xercesc/dom/DOM.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/util/XercesDefs.hpp>
XERCES_CPP_NAMESPACE_USE
GINGA_PRAGMA_DIAG_POP ()

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

#include <vector>
using namespace std;

GINGA_FORMATTER_BEGIN

class NclParser : public ErrorHandler
{
public:
  NclParser ();
  ~NclParser ();

  Node *getNode (const string &id);
  NclDocument *importDocument (string &docLocation);

  string getPath ();
  string getDirName ();
  void setNclDocument (NclDocument *);
  NclDocument *getNclDocument ();

  void warning (const SAXParseException &);
  void error (const SAXParseException &);
  void fatalError (const SAXParseException &);
  void resetErrors () {};

  NclDocument *parse (const string &);

private:
  NclDocument *_ncl;             // NCL document
  string _path;                  // document's absolute path
  string _dirname;               // directory part of document's path
  bool _ownManager;

  NclDocument *parseRootElement (DOMElement *rootElement);

// STRUCTURE
private:
  NclDocument *parseNcl (DOMElement *parentElement);
  NclDocument *createNcl (DOMElement *parentElement);
  void parseHead (DOMElement *parentElement);
  ContextNode *parseBody (DOMElement *parentElement);
  ContextNode *createBody (DOMElement *parentElement);
  void *posCompileBody (DOMElement *parentElement, ContextNode *body);

  void solveNodeReferences (CompositeNode *composition);

// COMPONENTS
private:
  Node *parseMedia (DOMElement *parentElement);
  Node *createMedia (DOMElement *parentElement);
  Node *parseContext (DOMElement *parentElement);
  ContextNode *posCompileContext (DOMElement *ctx_element, ContextNode *ctx);
  Node *createContext (DOMElement *parentElement);

  void addNodeToContext (Entity *contextNode, Node *node);
  void addNodeToContext (ContextNode *contextNode, Node *node);
  void addPropertyToContext (Entity *parentObject, Anchor *childObject);
  void addLinkToContext (ContextNode *context, Link *link);
  void addAnchorToMedia (ContentNode *contentNode, Anchor *anchor);

// IMPORT
private:
  void parseImportedDocumentBase (DOMElement *importedDocBase_element);
  void addImportNCLToImportedDocumentBase (DOMElement *childObject);

// TRANSITION
private:
  TransitionBase *parseTransitionBase (DOMElement *transitionBase_element);
  Transition *parseTransition (DOMElement *transitionBase_element);
  void addImportBaseToTransitionBase (TransitionBase *, DOMElement *);

// CONNECTORS
private:
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

  void compileRoleInformation (Role *role, DOMElement *parentElement);
  void addAttributeAssessmentToAssessmentStatement (AssessmentStatement *parentObject, AttributeAssessment *childObject);
  void addImportBaseToConnectorBase (ConnectorBase *connectorBase, DOMElement *childObject);

// INTERFACES
private:
  SwitchPort *parseSwitchPort (DOMElement *switchPort_element, SwitchNode *switchNode);
  SwitchPort *createSwitchPort (DOMElement *switchPort_element, SwitchNode *switchNode);
  Port *parseMapping (DOMElement *parentElement, SwitchPort *switchPort);
  Anchor *parseArea (DOMElement *parentElement);
  PropertyAnchor *parseProperty (DOMElement *parentElement);
  Port *parsePort (DOMElement *parentElement, CompositeNode *objGrandParent);

  SpatialAnchor *createSpatialAnchor (DOMElement *areaElement);
  IntervalAnchor *createTemporalAnchor (DOMElement *areaElement);

// LAYOUT
private:
  LayoutRegion *parseRegion (DOMElement *region_element);
  LayoutRegion *createRegion (DOMElement *region_element);
  RegionBase *parseRegionBase (DOMElement *regionBase_element);
  RegionBase *createRegionBase (DOMElement *regionBase_element);
  void addImportBaseToRegionBase (RegionBase *regionBase, DOMElement *importBase_element);

// LINKING
private:
  Bind *parseBind (DOMElement *parentElement, Link *link);
  Bind *createBind (DOMElement *parentElement, Link *link);
  Parameter *parseLinkOrBindParam (DOMElement *parentElement);
  Link *parseLink (DOMElement *link_element, CompositeNode *compositeNode);
  Link *createLink (DOMElement *link_element, CompositeNode *compositeNode);

  Connector *_connectorLinkParsing = nullptr;
  CompositeNode *_composite = nullptr;

// PRESENTATION CONTROL
private:
  RuleBase *parseRuleBase (DOMElement *parentElement);
  RuleBase *createRuleBase (DOMElement *parentElement);
  void addImportBaseToRuleBase (RuleBase *ruleBase, DOMElement *importBase_element);
  SimpleRule *parseRule (DOMElement *parentElement);
  Node *parseSwitch (DOMElement *parentElement);
  void *posCompileSwitch (DOMElement *parentElement, SwitchNode *parentObject);
  Node *createSwitch (DOMElement *parentElement);
  CompositeRule *parseCompositeRule (DOMElement *parentElement);
  CompositeRule *createCompositeRule (DOMElement *parentElement);
  DescriptorSwitch *parseDescriptorSwitch (DOMElement *parentElement);
  DescriptorSwitch *createDescriptorSwitch (DOMElement *parentElement);
  vector<Node *> *getSwitchConstituents (SwitchNode *switchNode);

  map<string, map<string, Node *> *> _switchConstituents;
  void addNodeToSwitch (Node *switchNode, Node *node);
  void addBindRuleToSwitch (SwitchNode *parentObject, DOMElement *childObject);
  void addUnmappedNodesToSwitch (SwitchNode *switchNode);
  void addDefaultDescriptorToDescriptorSwitch (DescriptorSwitch *descriptorSwitch, DOMElement *defaultDescriptor);
  void addDefaultComponentToSwitch (SwitchNode *switchNode, DOMElement *defaultComponent);
  void addBindRuleToDescriptorSwitch (DescriptorSwitch *descriptorSwitch, DOMElement *bindRule_element);
  void addDescriptorToDescriptorSwitch (DescriptorSwitch *descriptorSwitch, GenericDescriptor *descriptor);

// PRESENTATION SPECIFICATION
private:
  Descriptor *parseDescriptor (DOMElement *descriptor_element);
  Descriptor *createDescriptor (DOMElement *descriptor_element);
  DescriptorBase *parseDescriptorBase (DOMElement *descriptorBase_element);
  DescriptorBase *createDescriptorBase (DOMElement *descriptorBase_element);
  void addImportBaseToDescriptorBase (DescriptorBase *descriptorBase, DOMElement *childObject);

};

GINGA_FORMATTER_END

#endif /*PARSER_H*/
