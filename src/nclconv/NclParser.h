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

#ifndef NCLDOCUMENTPARSER_H_
#define NCLDOCUMENTPARSER_H_

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

#include "formatter/PrivateBaseContext.h" //FIXME: This is an architectural problem!
using namespace ::ginga::formatter;

#include <vector>
using namespace std;

GINGA_NCLCONV_BEGIN

// Get the DOMElement tagname as a std::string and free resources allocated by
// Xerces
static string
dom_element_tagname (const DOMElement *el)
{
  char *tagname = XMLString::transcode (el->getTagName ());
  string tagname_str (tagname);
  XMLString::release(&tagname);

  return tagname_str;
}

// Leak free check if DOMElement* has an attribute
static bool
dom_element_has_attr (const DOMElement *el, const string &attr)
{
  XMLCh *attr_xmlch = XMLString::transcode(attr.c_str());
  bool result = el->hasAttribute(attr_xmlch);
  XMLString::release(&attr_xmlch);

  return result;
}

// Gets the value of an attribute of DOMElement* as a std::string and free
// resources allocated by Xerces.  Similar to Xerces, it returns an empty
// string if there is no attribute.
static string
dom_element_get_attr (const DOMElement *element, const string &attr)
{
  XMLCh *attr_xmlch = XMLString::transcode(attr.c_str());
  char *attr_value_ch = XMLString::transcode(element->getAttribute (attr_xmlch));
  string attr_value_str(attr_value_ch);

  XMLString::release(&attr_xmlch);
  XMLString::release(&attr_value_ch);

  return attr_value_str;
}

// If attribute exists in DOMElement *, gets its value in gotAttr variable and
// returns true.  Otherwise, does not change gotAttr and returns false.
static bool
dom_element_try_get_attr (string &gotAttr,
                          const DOMElement *element,
                          const string &attr)
{
  bool has_attr = false;
  XMLCh *attr_xmlch = XMLString::transcode(attr.c_str());
  if (element->hasAttribute(attr_xmlch))
    {
      has_attr = true;
      char *attr_value_ch =
          XMLString::transcode(element->getAttribute (attr_xmlch));
      gotAttr = attr_value_ch;
      XMLString::release(&attr_value_ch);
    }
  XMLString::release(&attr_xmlch);

  return has_attr;
}

// Sets the value of an attribute of DOMElement* free resources allocated by
// Xerces.
static void
dom_element_set_attr (DOMElement *element,
                      const string &attr, const string &value)
{
  XMLCh *attr_name = XMLString::transcode (attr.c_str());
  XMLCh *attr_value = XMLString::transcode(value.c_str ());

  element->setAttribute(attr_name, attr_value);

  XMLString::release(&attr_name);
  XMLString::release(&attr_value);
}

// Removes the atribute from DOMElement *
static void
dom_element_remove_attr (DOMElement *element, const string &attr)
{
  XMLCh *attr_name = XMLString::transcode (attr.c_str());
  element->removeAttribute(attr_name);
  XMLString::release(&attr_name);
}

#define FOR_EACH_DOM_ELEM_CHILD(X, Y) \
  for ( X = Y->getFirstElementChild(); \
        X != nullptr; \
        X = X->getNextElementSibling() )

static vector <DOMElement *>
dom_element_children(DOMElement *el)
{
  vector <DOMElement *> vet;

  DOMElement *child;
  FOR_EACH_DOM_ELEM_CHILD(child, el)
    {
      vet.push_back(child);
    }

  return vet;
}

static vector <DOMElement *>
dom_element_children_by_tagname(DOMElement *el, const string &tagname)
{
  vector <DOMElement *> vet;

  DOMElement *child;
  FOR_EACH_DOM_ELEM_CHILD(child, el)
    {
      if (dom_element_tagname(child) == tagname)
        {
          vet.push_back(child);
        }
    }

  return vet;
}

static vector <DOMElement *>
dom_element_children_by_tagnames(DOMElement *el, const vector<string> &tagnames)
{
  vector <DOMElement *> vet;

  DOMElement *child;
  FOR_EACH_DOM_ELEM_CHILD(child, el)
    {
      if (std::find(tagnames.begin(), tagnames.end(), dom_element_tagname(child))
          != tagnames.end() )
        {
          vet.push_back(child);
        }
    }

  return vet;
}

class NclParser : public ErrorHandler
{
public:
  explicit NclParser (PrivateBaseContext *pbc, DeviceLayout *deviceLayout);
  virtual ~NclParser ();

  Node *getNode (const string &id);
  PrivateBaseContext *getPrivateBaseContext ();
  NclDocument *importDocument (string &docLocation);

  string getPath ();
  string getDirName ();
  void setNclDocument (NclDocument *);
  NclDocument *getNclDocument ();
  DeviceLayout *getDeviceLayout();

  void warning (const SAXParseException &);
  void error (const SAXParseException &);
  void fatalError (const SAXParseException &);
  void resetErrors () {};

  NclDocument *parse (const string &);


protected:
  NclDocument *_ncl;             // NCL document
  string _path;                  // document's absolute path
  string _dirname;               // directory part of document's path

  DeviceLayout *_deviceLayout;

  NclDocument *parseRootElement (DOMElement *rootElement);

private:
  PrivateBaseContext *_privateBaseContext;
  bool _ownManager;


// STRUCTURE
public:
  NclDocument *parseNcl (DOMElement *parentElement);
  NclDocument *createNcl (DOMElement *parentElement);
  void parseHead (DOMElement *parentElement);
  ContextNode *parseBody (DOMElement *parentElement);
  ContextNode *createBody (DOMElement *parentElement);
  void *posCompileBody (DOMElement *parentElement, ContextNode *body);
private:
  void solveNodeReferences (CompositeNode *composition);

// COMPONENTS
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

// IMPORT
public:
  void parseImportedDocumentBase (DOMElement *importedDocBase_element);
  void addImportNCLToImportedDocumentBase (DOMElement *childObject);

// META INFORMATION
public:
  Meta *parseMeta (DOMElement *parentElement);
  Metadata *parseMetadata (DOMElement *parentElement);

// TRANSITION
public:
  TransitionBase *parseTransitionBase (DOMElement *transitionBase_element);
  Transition *parseTransition (DOMElement *transitionBase_element);
  void addImportBaseToTransitionBase (TransitionBase *, DOMElement *);

// CONNECTORS
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
  Connector *_connector;
  void compileRoleInformation (Role *role, DOMElement *parentElement);
  void addAttributeAssessmentToAssessmentStatement (AssessmentStatement *parentObject, AttributeAssessment *childObject);
  void addImportBaseToConnectorBase (ConnectorBase *connectorBase, DOMElement *childObject);

// INTERFACES
public:
  SwitchPort *parseSwitchPort (DOMElement *switchPort_element, SwitchNode *switchNode);
  SwitchPort *createSwitchPort (DOMElement *switchPort_element, SwitchNode *switchNode);
  Port *parseMapping (DOMElement *parentElement, SwitchPort *switchPort);
  Anchor *parseArea (DOMElement *parentElement);
  PropertyAnchor *parseProperty (DOMElement *parentElement);
  Port *parsePort (DOMElement *parentElement, CompositeNode *objGrandParent);
private:
  SpatialAnchor *createSpatialAnchor (DOMElement *areaElement);
  IntervalAnchor *createTemporalAnchor (DOMElement *areaElement);

// LAYOUT
public:
  LayoutRegion *parseRegion (DOMElement *region_element);
  LayoutRegion *createRegion (DOMElement *region_element);
  RegionBase *parseRegionBase (DOMElement *regionBase_element);
  RegionBase *createRegionBase (DOMElement *regionBase_element);
  void addImportBaseToRegionBase (RegionBase *regionBase, DOMElement *importBase_element);

// LINKING
public:
  Bind *parseBind (DOMElement *parentElement, Link *link);
  Bind *createBind (DOMElement *parentElement, Link *link);
  Parameter *parseLinkOrBindParam (DOMElement *parentElement);
  Link *parseLink (DOMElement *link_element, CompositeNode *compositeNode);
  Link *createLink (DOMElement *link_element, CompositeNode *compositeNode);
private:
  Connector *_connectorLinkParsing = nullptr;
  CompositeNode *_composite = nullptr;

// PRESENTATION CONTROL
public:
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
private:
  map<string, map<string, Node *> *> _switchConstituents;
  void addNodeToSwitch (Node *switchNode, Node *node);
  void addBindRuleToSwitch (SwitchNode *parentObject, DOMElement *childObject);
  void addUnmappedNodesToSwitch (SwitchNode *switchNode);
  void addDefaultDescriptorToDescriptorSwitch (DescriptorSwitch *descriptorSwitch, DOMElement *defaultDescriptor);
  void addDefaultComponentToSwitch (SwitchNode *switchNode, DOMElement *defaultComponent);
  void addBindRuleToDescriptorSwitch (DescriptorSwitch *descriptorSwitch, DOMElement *bindRule_element);
  void addDescriptorToDescriptorSwitch (DescriptorSwitch *descriptorSwitch, GenericDescriptor *descriptor);

// PRESENTATION SPECIFICATION
public:
  Descriptor *parseDescriptor (DOMElement *descriptor_element);
  Descriptor *createDescriptor (DOMElement *descriptor_element);
  DescriptorBase *parseDescriptorBase (DOMElement *descriptorBase_element);
  DescriptorBase *createDescriptorBase (DOMElement *descriptorBase_element);
  void addImportBaseToDescriptorBase (DescriptorBase *descriptorBase, DOMElement *childObject);

};

GINGA_NCLCONV_END

#endif /*NCLDOCUMENTPARSER_H_*/
