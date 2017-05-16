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

#include "ginga.h"
#include "NclPresentationControlParser.h"
#include "NclComponentsParser.h"

#include "NclParser.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_NCLCONV_BEGIN

NclPresentationControlParser::NclPresentationControlParser (
    NclParser *nclParser)
    : ModuleParser (nclParser)
{
  this->switchConstituents = new map<string, map<string, NodeEntity *> *>;
}

DOMElement *
NclPresentationControlParser::parseBindRule (DOMElement *parentElement)
{
  return parentElement; // ?
}

RuleBase *
NclPresentationControlParser::parseRuleBase (DOMElement *parentElement)
{
  RuleBase *ruleBase = createRuleBase (parentElement);
  g_assert_nonnull (ruleBase);

  DOMNodeList *elementNodeList = parentElement->getChildNodes ();
  for (int i = 0; i < (int)elementNodeList->getLength (); i++)
    {
      DOMNode *node = elementNodeList->item (i);
      if (node->getNodeType () == DOMNode::ELEMENT_NODE)
        {
          DOMElement *element = (DOMElement *)node;
          string tagname = dom_element_tagname(element);

          if (XMLString::compareIString (tagname.c_str (), "importBase") == 0)
            {
              DOMElement *elementObject = _nclParser->getImportParser ()
                      ->parseImportBase (element);

              if (elementObject)
                {
                  addImportBaseToRuleBase (ruleBase, elementObject);
                }
            }
          else if (XMLString::compareIString (tagname.c_str (), "rule") == 0)
            {
              SimpleRule *rule = parseRule (element);
              if (rule)
                {
                  addRuleToRuleBase (ruleBase, rule);
                }
            }
          else if (XMLString::compareIString (tagname.c_str (), "compositeRule")
                   == 0)
            {
              CompositeRule *compositeRule = parseCompositeRule (element);
              if (compositeRule)
                {
                  addCompositeRuleToRuleBase (ruleBase, compositeRule);
                }
            }
        }
    }

  return ruleBase;
}

SimpleRule *
NclPresentationControlParser::parseRule (DOMElement *parentElement)
{
  short ruleOp = Comparator::fromString(
        dom_element_get_attr(parentElement, "comparator") );

  string var = dom_element_get_attr(parentElement, "var");
  string value = dom_element_get_attr(parentElement, "value");

//  XMLString::trim (var);
//  XMLString::trim (value);
  SimpleRule *simplePresentationRule
      = new SimpleRule ( dom_element_get_attr(parentElement, "id"),
                        var, ruleOp, value);

  return simplePresentationRule;
}

Node *
NclPresentationControlParser::parseSwitch (DOMElement *parentElement)
{
  Node *switch_node = createSwitch (parentElement);
  if (unlikely (switch_node == NULL))
    {
      syntax_error ("switch: bad parent '%s'",
                    string (XMLString::transcode
                            (parentElement->getNodeName ())).c_str ());
    }

  DOMNodeList *elementNodeList = parentElement->getChildNodes ();
  int size = (int) elementNodeList->getLength ();

  for (int i = 0; i < size; i++)
    {
      DOMNode *node = elementNodeList->item (i);
      if (node->getNodeType () == DOMNode::ELEMENT_NODE)
        {
          DOMElement *element = (DOMElement *)node;
          string tagname = dom_element_tagname(element);
          if (XMLString::compareIString (tagname.c_str (), "media") == 0)
            {
              Node *media = _nclParser->getComponentsParser ()
                              ->parseMedia (element);

              if (media)
                {
                  addMediaToSwitch (switch_node, media);
                }
            }
          else if (XMLString::compareIString (tagname.c_str (), "context") == 0)
            {
              Node *ctx = _nclParser->getComponentsParser ()
                  ->parseContext (element);

              if (ctx)
                {
                  addContextToSwitch (switch_node, ctx);
                }
            }
          else if (XMLString::compareIString (tagname.c_str (), "switch") == 0)
            {
              Node *elementObject = parseSwitch (element);
              if (elementObject)
                {
                  addSwitchToSwitch (switch_node, elementObject);
                }
            }
        }
    }

  for (int i = 0; i < size; i++)
    {
      DOMNode *node = elementNodeList->item (i);
      if (node->getNodeType () == DOMNode::ELEMENT_NODE)
        {
          DOMElement *element = (DOMElement *)node;
          string tagname = dom_element_tagname(element);

          if (XMLString::compareIString (tagname.c_str (), "bindRule") == 0)
            {
              DOMElement *elementObject = parseBindRule ((DOMElement *)node);

              if (elementObject)
                {
                  addBindRuleToSwitch ((SwitchNode *)switch_node,
                                       elementObject);
                }
            }
          else if (XMLString::compareIString (tagname.c_str (),
                                              "defaultComponent")
                   == 0)
            {
              DOMElement *elementObject =
                  parseDefaultComponent ((DOMElement *)node);

              if (elementObject)
                {
                  addDefaultComponentToSwitch (switch_node, elementObject);
                }
            }
        }
    }

  addUnmappedNodesToSwitch ((SwitchNode *)switch_node);

  return switch_node;
}

void *
NclPresentationControlParser::posCompileSwitch2 (DOMElement *parentElement,
                                                 void *parentObject)
{
  DOMNodeList *elementNodeList = parentElement->getChildNodes ();
  int size = (int) elementNodeList->getLength ();

  for (int i = 0; i < size; i++)
    {
      DOMNode *node = elementNodeList->item (i);
      if (node->getNodeType () == DOMNode::ELEMENT_NODE)
        {
          DOMElement *element = (DOMElement *)node;
          string tagname = dom_element_tagname(element);
          if (XMLString::compareIString (tagname.c_str(), "switchPort") == 0)
            {
              void *elementObject = _nclParser->getInterfacesParser ()
                  ->parseSwitchPort (element, (SwitchNode*) parentObject);

              if (elementObject)
                {
                  addSwitchPortToSwitch (parentObject, elementObject);
                }
            }
        }
    }

  return parentObject;
}

DOMElement *
NclPresentationControlParser::parseDefaultComponent (DOMElement *parentElement)
{
  return parentElement; // ?
}

DOMElement *
NclPresentationControlParser::parseDefaultDescriptor (DOMElement *parentElement)
{
  return parentElement; // ?
}

CompositeRule *
NclPresentationControlParser::parseCompositeRule (DOMElement *parentElement)
{
  CompositeRule *compositeRule = createCompositeRule (parentElement);
  g_assert_nonnull (compositeRule);

  DOMNodeList *elementNodeList = parentElement->getChildNodes ();
  for (int i = 0; i < (int)elementNodeList->getLength (); i++)
    {
      DOMNode *node = elementNodeList->item (i);
      if (node->getNodeType () == DOMNode::ELEMENT_NODE)
        {
          DOMElement *element = (DOMElement *)node;
          string tagname = dom_element_tagname(element);

          if (XMLString::compareIString (tagname.c_str (), "rule")
              == 0)
            {
              SimpleRule *rule = parseRule (element);
              if (rule)
                {
                  addRuleToCompositeRule (compositeRule, rule);
                }
            }
          else if (XMLString::compareIString (tagname.c_str (), "compositeRule")
                   == 0)
            {
              CompositeRule *child_compositeRule = parseCompositeRule (element);

              if (child_compositeRule)
                {
                  addCompositeRuleToCompositeRule (compositeRule,
                                                   child_compositeRule);
                }
            }
        }
    }

  return compositeRule;
}

DescriptorSwitch *
NclPresentationControlParser::parseDescriptorSwitch (DOMElement *parentElement)
{
  DescriptorSwitch *descriptorSwitch = createDescriptorSwitch (parentElement);
  g_assert_nonnull (descriptorSwitch);

  DOMNodeList *elementNodeList = parentElement->getChildNodes ();
  int size = (int) elementNodeList->getLength ();

  for (int i = 0; i < size; i++)
    {
      DOMNode *node = elementNodeList->item (i);
      if (node->getNodeType () == DOMNode::ELEMENT_NODE)
        {
          DOMElement *element = (DOMElement *)node;
          string tagname = dom_element_tagname(element);

          if (XMLString::compareIString (tagname.c_str(), "descriptor") == 0)
            {
              Descriptor* desc
                  = _nclParser->getPresentationSpecificationParser ()
                      ->parseDescriptor ((DOMElement *)node);

              if (desc)
                {
                  addDescriptorToDescriptorSwitch (descriptorSwitch, desc);
                }
            }
        }
    }

  for (int i = 0; i < size; i++)
    {
      DOMNode *node = elementNodeList->item (i);
      if (node->getNodeType () == DOMNode::ELEMENT_NODE)
        {
          DOMElement *element = (DOMElement *)node;
          string tagname = dom_element_tagname(element);
          if (XMLString::compareIString (tagname.c_str (), "bindRule") == 0)
            {
              DOMElement *elementObject = parseBindRule ((DOMElement *)node);

              if (elementObject)
                {
                  addBindRuleToDescriptorSwitch (descriptorSwitch,
                                                 elementObject);
                }
            }
          else if (XMLString::compareIString (tagname.c_str (),
                                              "defaultDescriptor")
                   == 0)
            {
              DOMElement *elementObject =
                  parseDefaultDescriptor ((DOMElement *)node);

              if (elementObject)
                {
                  addDefaultDescriptorToDescriptorSwitch (descriptorSwitch,
                                                          elementObject);
                }
            }
        }
    }

  return descriptorSwitch;
}

NclPresentationControlParser::~NclPresentationControlParser ()
{
  map<string, map<string, NodeEntity *> *>::iterator i;

  if (switchConstituents != NULL)
    {
      i = switchConstituents->begin ();
      while (i != switchConstituents->end ())
        {
          delete i->second;
          ++i;
        }
      delete switchConstituents;
      switchConstituents = NULL;
    }
}

vector<Node *> *
NclPresentationControlParser::getSwitchConstituents (SwitchNode *switchNode)
{
  map<string, map<string, NodeEntity *> *>::iterator i;

  map<string, NodeEntity *> *hTable;
  map<string, NodeEntity *>::iterator j;

  vector<Node *> *ret = new vector<Node *>;

  i = switchConstituents->find (switchNode->getId ());
  if (i != switchConstituents->end ())
    {
      hTable = i->second;

      j = hTable->begin ();
      while (j != hTable->end ())
        {
          ret->push_back ((Node *)j->second);
          ++j;
        }
    }

  // Users: you have to delete this vector after using it
  return ret;
}

void
NclPresentationControlParser::addCompositeRuleToCompositeRule (
    void *parentObject, void *childObject)
{
  ((CompositeRule *)parentObject)->addRule ((Rule *)childObject);
}

void
NclPresentationControlParser::addCompositeRuleToRuleBase (
    void *parentObject, void *childObject)
{
  ((RuleBase *)parentObject)->addRule ((Rule *)childObject);
}

void
NclPresentationControlParser::addRuleToCompositeRule (void *parentObject,
                                                      void *childObject)
{
  // adicionar regra
  ((CompositeRule *)parentObject)->addRule ((Rule *)childObject);
}

void
NclPresentationControlParser::addRuleToRuleBase (void *parentObject,
                                                 void *childObject)
{
  ((RuleBase *)parentObject)->addRule ((Rule *)childObject);
}

void
NclPresentationControlParser::addSwitchPortToSwitch (void *parentObject,
                                                     void *childObject)
{
  ((SwitchNode *)parentObject)->addPort ((Port *)childObject);
}

CompositeRule *
NclPresentationControlParser::createCompositeRule (DOMElement *parentElement)
{
  CompositeRule *compositePresentationRule;
  short ruleOp = CompositeRule::OP_AND;

  string op = dom_element_get_attr(parentElement, "operator");
  if (XMLString::compareIString (op.c_str(), "and") == 0)
    {
      ruleOp = CompositeRule::OP_AND;
    }
  else if (XMLString::compareIString (op.c_str(), "or") == 0)
    {
      ruleOp = CompositeRule::OP_OR;
    }

  compositePresentationRule = new CompositeRule (
        dom_element_get_attr(parentElement, "id"),
        ruleOp);

  return compositePresentationRule;
}

Node *
NclPresentationControlParser::createSwitch (DOMElement *parentElement)
{
  string id;
  Node *node;
  string attValue;
  Entity *referNode;
  NclDocument *document;
  SwitchNode *switchNode;

  if (unlikely (!dom_element_has_attr(parentElement, "id")))
    syntax_error ("switch: missing id");

  id = dom_element_get_attr(parentElement, "id");

  node = getNclParser ()->getNode (id);
  if (unlikely (node != NULL))
    syntax_error ("switch '%s': duplicated id", id.c_str ());

  if (dom_element_has_attr(parentElement, "refer"))
    {
      attValue =  dom_element_get_attr(parentElement, "refer");

      try
        {
          referNode
              = (SwitchNode *) getNclParser ()->getNode (attValue);

          if (referNode == NULL)
            {
              document = getNclParser ()->getNclDocument ();
              referNode = (SwitchNode *)document->getNode (attValue);
              if (referNode == NULL)
                {
                  referNode
                      = new ReferredNode (attValue, (void *)parentElement);
                }
            }
        }
      catch (...)
        {
          syntax_error ("switch '%s': bad refer '%s'",
                        id.c_str (), attValue.c_str ());
        }

      node = new ReferNode (id);
      ((ReferNode *)node)->setReferredEntity (referNode);

      return node;
    }

  switchNode = new SwitchNode (id);
  (*switchConstituents)[switchNode->getId ()]
      = new map<string, NodeEntity *>;

  return switchNode;
}

RuleBase *
NclPresentationControlParser::createRuleBase (DOMElement *parentElement)
{
  RuleBase *ruleBase;
  ruleBase = new RuleBase (dom_element_get_attr(parentElement, "id"));

  return ruleBase;
}

DescriptorSwitch *
NclPresentationControlParser::createDescriptorSwitch (DOMElement *parentElement)
{
  DescriptorSwitch *descriptorSwitch =
      new DescriptorSwitch (dom_element_get_attr(parentElement, "id"));

  // vetores para conter componentes e regras do switch
  (*switchConstituents)[descriptorSwitch->getId ()]
      = new map<string, NodeEntity *>;

  return descriptorSwitch;
}

void
NclPresentationControlParser::addDescriptorToDescriptorSwitch (
    void *parentObject, void *childObject)
{
  map<string, NodeEntity *> *descriptors;
  try
    {
      if (switchConstituents->count (
              ((DescriptorSwitch *)parentObject)->getId ())
          != 0)
        {
          descriptors
              = (*switchConstituents)[((DescriptorSwitch *)parentObject)
                                          ->getId ()];

          if (descriptors->count (
                  ((GenericDescriptor *)childObject)->getId ())
              == 0)
            {
              (*descriptors)[((GenericDescriptor *)childObject)->getId ()]
                  = (NodeEntity *)childObject;
            }
        }
    }
  catch (...)
    {
    }
}

void
NclPresentationControlParser::addImportBaseToRuleBase (
    void *parentObject, void *childObject)
{
  string baseAlias, baseLocation;
  NclParser *compiler;
  NclDocument *importedDocument;
  RuleBase *createdBase;

  baseAlias = dom_element_get_attr((DOMElement *)childObject, "alias");

  baseLocation =
      dom_element_get_attr((DOMElement *)childObject, "documentURI");

  compiler = getNclParser ();
  importedDocument = compiler->importDocument (baseLocation);
  if (importedDocument == NULL)
    {
      return;
    }

  createdBase = importedDocument->getRuleBase ();
  if (createdBase == NULL)
    {
      return;
    }

  // insere a base compilada na base do documento
  try
    {
      ((RuleBase *)parentObject)
          ->addBase (createdBase, baseAlias, baseLocation);
    }
  catch (...)
    {
    }
}

void
NclPresentationControlParser::addBindRuleToDescriptorSwitch (
    void *parentObject, void *childObject)
{
  DescriptorSwitch *descriptorSwitch;
  DOMElement *bindRule;
  map<string, NodeEntity *> *descriptors;
  GenericDescriptor *descriptor;
  NclDocument *document;
  Rule *ncmRule;

  descriptorSwitch = (DescriptorSwitch *)parentObject;
  bindRule = (DOMElement *)childObject;

  if (switchConstituents->count (descriptorSwitch->getId ()) == 0)
    {
      return;
    }
  descriptors = (*switchConstituents)[descriptorSwitch->getId ()];
  string constituent = dom_element_get_attr(bindRule, "constituent");
  if (descriptors->count (constituent) == 0)
    {
      return;
    }

  descriptor = (GenericDescriptor *)(*descriptors)[constituent];

  if (descriptor == NULL)
    {
      return;
    }

  document = getNclParser ()->getNclDocument ();
  ncmRule = document->getRule (dom_element_get_attr(bindRule, "rule"));

  if (ncmRule == NULL)
    {
      return;
    }

  descriptorSwitch->addDescriptor (descriptor, ncmRule);
}

void
NclPresentationControlParser::addBindRuleToSwitch ( SwitchNode *switchNode,
                                                    DOMElement *bindRule)
{
  map<string, NodeEntity *> *nodes;
  Node *node;
  NclDocument *document;
  Rule *ncmRule;

  if (switchConstituents->count (switchNode->getId ()) == 0)
    {
      return;
    }

  nodes = (*switchConstituents)[switchNode->getId ()];
  if (nodes->count (dom_element_get_attr(bindRule, "constituent"))
      == 0)
    {
      return;
    }

  node = (NodeEntity *)(*nodes)[
      dom_element_get_attr(bindRule, "constituent") ];

  if (node == NULL)
    {
      return;
    }

  document = getNclParser ()->getNclDocument ();
  ncmRule = document->getRule (dom_element_get_attr(bindRule, "rule"));

  if (ncmRule == NULL)
    {
      return;
    }

  switchNode->addNode (node, ncmRule);
}

void
NclPresentationControlParser::addUnmappedNodesToSwitch (SwitchNode *switchNode)
{
  map<string, NodeEntity *> *nodes;
  map<string, NodeEntity *>::iterator i;

  if (switchConstituents->count (switchNode->getId ()) == 0)
    {
      return;
    }

  nodes = (*switchConstituents)[switchNode->getId ()];
  i = nodes->begin ();
  while (i != nodes->end ())
    {
      if (switchNode->getNode (i->second->getId ()) == NULL)
        {
          switchNode->addNode (i->second, new Rule ("fake"));
        }
      else
        {
          i->second->setParentComposition (switchNode);
        }
      ++i;
    }
}

void
NclPresentationControlParser::addDefaultComponentToSwitch (
    void *parentObject, void *childObject)
{
  SwitchNode *switchNode;
  DOMElement *defaultComponent;
  map<string, NodeEntity *> *nodes;
  NodeEntity *node;

  switchNode = (SwitchNode *)parentObject;
  defaultComponent = (DOMElement *)childObject;

  if (switchConstituents->count (switchNode->getId ()) == 0)
    {
      return;
    }

  nodes = (*switchConstituents)[switchNode->getId ()];
  string component =
      dom_element_get_attr(defaultComponent, "component");
  if (nodes->count (component) == 0)
    {
      return;
    }

  node = (NodeEntity *)(*nodes)[component];

  if (node == NULL)
    {
      return;
    }

  switchNode->setDefaultNode (node);
}

void
NclPresentationControlParser::addDefaultDescriptorToDescriptorSwitch (
    void *parentObject, void *childObject)
{
  DescriptorSwitch *descriptorSwitch;
  DOMElement *defaultDescriptor;
  map<string, NodeEntity *> *descriptors;
  GenericDescriptor *descriptor;

  descriptorSwitch = (DescriptorSwitch *)parentObject;
  defaultDescriptor = (DOMElement *)childObject;

  if (switchConstituents->count (descriptorSwitch->getId ()) == 0)
    {
      return;
    }

  descriptors = (*switchConstituents)[descriptorSwitch->getId ()];
  if (descriptors->count (
        dom_element_get_attr(defaultDescriptor, "descriptor"))
      == 0)
    {
      return;
    }

  descriptor = (GenericDescriptor *)(*descriptors)[
      dom_element_get_attr(defaultDescriptor, "descriptor") ];

  if (descriptor == NULL)
    {
      return;
    }

  descriptorSwitch->setDefaultDescriptor (descriptor);
}

void
NclPresentationControlParser::addContextToSwitch (void *parentObject,
                                                  void *childObject)
{
  addNodeToSwitch ((SwitchNode *)parentObject, (NodeEntity *)childObject);
}

void
NclPresentationControlParser::addMediaToSwitch (void *parentObject,
                                                void *childObject)
{
  addNodeToSwitch ((SwitchNode *)parentObject, (NodeEntity *)childObject);
}

void
NclPresentationControlParser::addSwitchToSwitch (void *parentObject,
                                                 void *childObject)
{
  addNodeToSwitch ((SwitchNode *)parentObject, (NodeEntity *)childObject);
}

void
NclPresentationControlParser::addNodeToSwitch (SwitchNode *switchNode,
                                               NodeEntity *node)
{
  map<string, NodeEntity *> *nodes;

  if (switchConstituents->count (switchNode->getId ()) == 0)
    {
      (*switchConstituents)[switchNode->getId ()]
          = new map<string, NodeEntity *>;
    }

  nodes = (*switchConstituents)[switchNode->getId ()];
  if (nodes->count (node->getId ()) == 0)
    {
      (*nodes)[node->getId ()] = node;
    }
}

void *
NclPresentationControlParser::posCompileSwitch (
    DOMElement *parentElement, void *parentObject)
{
  DOMNodeList *elementNodeList = parentElement->getChildNodes ();
  int size = (int) elementNodeList->getLength ();

  for (int i = 0; i < size; i++)
    {
      DOMNode *node = elementNodeList->item (i);
      if (node->getNodeType () == DOMNode::ELEMENT_NODE)
        {
          DOMElement *element = (DOMElement *)node;
          string tagname = dom_element_tagname(element);
          if (XMLString::compareIString (tagname.c_str (), "context") == 0)
            {
             Node *elementObject
                  = (Node *)(getNclParser ()->getNode (
                               dom_element_get_attr(element, "id")));

              if (elementObject->instanceOf ("ContextNode"))
                {
                  _nclParser->getComponentsParser ()
                          ->posCompileContext (element, elementObject);
                }
            }
          else if (XMLString::compareIString (tagname.c_str (), "switch")
                   == 0)
            {
              Node * elementObject = getNclParser ()
                  ->getNode ( dom_element_get_attr(element, "id") );

              if (unlikely (elementObject == NULL))
                {
                  syntax_error ("node '%s' should be a switch",
                         dom_element_get_attr(element, "id").c_str ());
                }
              else if (elementObject->instanceOf ("SwitchNode"))
                {
                  posCompileSwitch (element, elementObject);
                }
            }
        }
    }

  return NclPresentationControlParser::posCompileSwitch2 (parentElement,
                                                          parentObject);
}

GINGA_NCLCONV_END
