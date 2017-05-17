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
  this->switchConstituents = new map<string, map<string, Node *> *>;
}

DOMElement *
NclPresentationControlParser::parseBindRule (DOMElement *bindRule_element)
{
  return bindRule_element; // ?
}

RuleBase *
NclPresentationControlParser::parseRuleBase (DOMElement *ruleBase_element)
{
  RuleBase *ruleBase = createRuleBase (ruleBase_element);
  g_assert_nonnull (ruleBase);

  for (DOMElement *child: dom_element_children(ruleBase_element))
    {
      string tagname = dom_element_tagname(child);
      if ( tagname == "importBase")
        {
          DOMElement *elementObject = _nclParser->getImportParser ()
              ->parseImportBase (child);
          if (elementObject)
            {
              addImportBaseToRuleBase (ruleBase, elementObject);
            }
        }
      else if (tagname == "rule")
        {
          SimpleRule *rule = parseRule (child);
          if (rule)
            {
              ruleBase->addRule (rule);
            }
        }
      else if (tagname == "compositeRule")
        {
          CompositeRule *compositeRule = parseCompositeRule (child);
          if (compositeRule)
            {
              ruleBase->addRule (compositeRule);
            }
        }
      else
        {
          syntax_warning( "'%s' is not known as child of 'ruleBase'."
                          " It will be ignored.",
                          tagname.c_str() );
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
NclPresentationControlParser::parseSwitch (DOMElement *switch_element)
{
  Node *switch_node = createSwitch (switch_element);
  if (unlikely (switch_node == NULL))
    {
      syntax_error ( "switch: bad parent '%s'",
                     dom_element_tagname(switch_element).c_str() );
    }

  for (DOMElement *element: dom_element_children(switch_element))
    {
      string tagname = dom_element_tagname(element);
      if ( tagname == "media")
        {
          Node *media = _nclParser->getComponentsParser ()
              ->parseMedia (element);
          if (media)
            {
              addNodeToSwitch (switch_node, media);
            }
        }
      else if (tagname == "context")
        {
          Node *ctx = _nclParser->getComponentsParser ()->parseContext (element);
          if (ctx)
            {
              addNodeToSwitch (switch_node, ctx);
            }
        }
      else if (tagname == "switch")
        {
          Node *switch_child = parseSwitch (element);
          if (switch_child)
            {
              addNodeToSwitch (switch_node, switch_child);
            }
        }
      else
        {
          // syntax warning ?
        }
    }

    for (DOMElement *child: dom_element_children(switch_element))
      {
        string tagname = dom_element_tagname(child);

        if (tagname == "bindRule")
          {
            DOMElement *bindRule = parseBindRule (child);
            if (bindRule)
              {
                addBindRuleToSwitch ((SwitchNode *)switch_node, bindRule);
              }
          }
        else if (tagname == "defaultComponent")
          {
            DOMElement *defaultComponent = parseDefaultComponent (child);
            if (defaultComponent)
              {
                addDefaultComponentToSwitch ((SwitchNode*)switch_node,
                                              defaultComponent);
              }
          }
      }

  addUnmappedNodesToSwitch ((SwitchNode *)switch_node);

  return switch_node;
}

void *
NclPresentationControlParser::posCompileSwitch2 (DOMElement *switch_element,
                                                 SwitchNode *switchNode)
{
  for (DOMElement *child: dom_element_children(switch_element))
    {
      string tagname = dom_element_tagname(child);
      if (tagname == "switchPort")
        {
          SwitchPort *switchPort = _nclParser->getInterfacesParser ()
              ->parseSwitchPort (child, switchNode);
          if (switchPort)
            {
              switchNode->addPort (switchPort);
            }
        }
      else
        {
          // syntax warning ?
        }
    }

  return switchNode;
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
NclPresentationControlParser::parseCompositeRule (
    DOMElement *compositeRule_element)
{
  CompositeRule *compositeRule = createCompositeRule (compositeRule_element);
  g_assert_nonnull (compositeRule);

  for (DOMElement *child: dom_element_children(compositeRule_element))
    {
      string tagname = dom_element_tagname(child);
      if (tagname == "rule")
        {
          SimpleRule *simpleRule = parseRule (child);
          if (simpleRule)
            {
              compositeRule->addRule (simpleRule);
            }
        }
      else if (tagname == "compositeRule")
        {
          CompositeRule *child_compositeRule = parseCompositeRule (child);
          if (child_compositeRule)
            {
              compositeRule->addRule (child_compositeRule);
            }
        }
      else
        {
          syntax_warning( "'%s' is not known as child of 'compositeRule'."
                          " It will be ignored.",
                          tagname.c_str() );
        }
    }

  return compositeRule;
}

DescriptorSwitch *
NclPresentationControlParser::parseDescriptorSwitch (
    DOMElement *descriptorSwitch_element)
{
  DescriptorSwitch *descriptorSwitch =
      createDescriptorSwitch (descriptorSwitch_element);
  g_assert_nonnull (descriptorSwitch);

  for (DOMElement *child: dom_element_children(descriptorSwitch_element))
    {
      string tagname = dom_element_tagname(child);
      if ( tagname == "descriptor")
        {
          Descriptor* desc = _nclParser->getPresentationSpecificationParser ()
                                ->parseDescriptor (child);
          if (desc)
            {
              addDescriptorToDescriptorSwitch (descriptorSwitch, desc);
            }
        }
      else
        {
          // syntax warning ?
        }
    }

  for (DOMElement *child: dom_element_children(descriptorSwitch_element))
    {
      string tagname = dom_element_tagname(child);
      if (tagname == "bindRule")
        {
          DOMElement *bindRule = parseBindRule (child);
          if (bindRule)
            {
              addBindRuleToDescriptorSwitch (descriptorSwitch, bindRule);
            }
        }
      else if (tagname == "defaultDescriptor")
        {
          DOMElement *defaultDescriptor = parseDefaultDescriptor (child);
          if (defaultDescriptor)
            {
              addDefaultDescriptorToDescriptorSwitch (descriptorSwitch,
                                                      defaultDescriptor);
            }
        }
    }

  return descriptorSwitch;
}

NclPresentationControlParser::~NclPresentationControlParser ()
{
  map<string, map<string, Node *> *>::iterator i;

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
  map<string, map<string, Node *> *>::iterator i;

  map<string, Node *> *hTable;
  map<string, Node *>::iterator j;

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
      = new map<string, Node *>;

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
      = new map<string, Node *>;

  return descriptorSwitch;
}

void
NclPresentationControlParser::addDescriptorToDescriptorSwitch (
    void *parentObject, void *childObject)
{
  map<string, Node *> *descriptors;
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
    RuleBase *ruleBase, DOMElement *childObject)
{
  string baseAlias, baseLocation;
  NclParser *compiler;
  NclDocument *importedDocument;
  RuleBase *importedRuleBase;

  baseAlias = dom_element_get_attr(childObject, "alias");

  baseLocation =
      dom_element_get_attr(childObject, "documentURI");

  compiler = getNclParser ();
  importedDocument = compiler->importDocument (baseLocation);
  g_assert_nonnull(importedDocument);

  importedRuleBase = importedDocument->getRuleBase ();
  g_assert_nonnull(importedRuleBase);

  // insere a base compilada na base do documento
  try
    {
      ruleBase->addBase (importedRuleBase, baseAlias, baseLocation);
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
  map<string, Node *> *descriptors;
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
  map<string, Node *> *nodes;
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

  node = (NodeEntity *)(*nodes)[dom_element_get_attr(bindRule, "constituent") ];

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
  map<string, Node *> *nodes;
  map<string, Node *>::iterator i;

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
    SwitchNode *switchNode, DOMElement *defaultComponent)
{
  map<string, Node *> *nodes;
  Node *node;

  if (switchConstituents->count (switchNode->getId ()) == 0)
    {
      return;
    }

  nodes = (*switchConstituents)[switchNode->getId ()];
  string component = dom_element_get_attr(defaultComponent, "component");
  if (nodes->count (component) == 0)
    {
      return;
    }

  node = (*nodes)[component];

  if (node == NULL)
    {
      return;
    }

  switchNode->setDefaultNode (node);
}

void
NclPresentationControlParser::addDefaultDescriptorToDescriptorSwitch (
    DescriptorSwitch *descriptorSwitch, DOMElement *defaultDescriptor)
{
  map<string, Node *> *descriptors;
  GenericDescriptor *descriptor;

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
NclPresentationControlParser::addNodeToSwitch ( Node *switchNode, Node *node)
{
  map<string, Node *> *nodes;

  if (switchConstituents->count (switchNode->getId ()) == 0)
    {
      (*switchConstituents)[switchNode->getId ()] = new map<string, Node *>();
    }

  nodes = (*switchConstituents)[switchNode->getId ()];
  if (nodes->count (node->getId ()) == 0)
    {
      (*nodes)[node->getId ()] = node;
    }
}

void *
NclPresentationControlParser::posCompileSwitch (
    DOMElement *switchElement, SwitchNode *switchNode)
{
  for(DOMElement *child: dom_element_children(switchElement))
    {
      string tagname = dom_element_tagname(child);
      if (tagname == "context")
        {
          string id = dom_element_get_attr(child, "id");
          Node *node = getNclParser ()->getNode (id);

          if (node->instanceOf ("ContextNode"))
            {
              getNclParser ()->
                  getComponentsParser ()->posCompileContext (
                    child, (ContextNode*)node);
            }
        }
      else if (tagname ==  "switch")
        {
          string id = dom_element_get_attr(child, "id");
          Node * node = getNclParser ()->getNode (id);
          if (unlikely (node == NULL))
            {
              syntax_error ("node '%s' should be a switch",
                            dom_element_get_attr(child, "id").c_str ());
            }
          else if (node->instanceOf ("SwitchNode"))
            {
              posCompileSwitch (child, (SwitchNode*)node);
            }
        }
    }

  return posCompileSwitch2 (switchElement, switchNode);
}

GINGA_NCLCONV_END
