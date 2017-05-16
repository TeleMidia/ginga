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

#include "NclDocumentParser.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_NCLCONV_BEGIN

NclPresentationControlParser::NclPresentationControlParser (
    NclDocumentParser *documentParser)
    : ModuleParser (documentParser)
{
  this->switchConstituents = new map<string, map<string, NodeEntity *> *>;
}

void *
NclPresentationControlParser::parseBindRule (DOMElement *parentElement,
                                             arg_unused(void *objGrandParent))
{
  return parentElement; // ?
}

void *
NclPresentationControlParser::parseRuleBase (DOMElement *parentElement,
                                             void *objGrandParent)
{
  void *parentObject;
  DOMNodeList *elementNodeList;
  DOMElement *element;
  DOMNode *node;
  string elementTagName;
  void *elementObject;

  parentObject = createRuleBase (parentElement, objGrandParent);
  g_assert_nonnull (parentObject);

  elementNodeList = parentElement->getChildNodes ();
  for (int i = 0; i < (int)elementNodeList->getLength (); i++)
    {
      node = elementNodeList->item (i);
      if (node->getNodeType () == DOMNode::ELEMENT_NODE)
        {
          element = (DOMElement *)node;
          elementTagName = XMLString::transcode (element->getTagName ());

          if (XMLString::compareIString (elementTagName.c_str (),
                                         "importBase")
              == 0)
            {
              elementObject = _documentParser->getImportParser ()
                      ->parseImportBase (element, parentObject);

              if (elementObject != NULL)
                {
                  addImportBaseToRuleBase (parentObject, elementObject);
                }
            }
          else if (XMLString::compareIString (elementTagName.c_str (),
                                              "rule")
                   == 0)
            {
              elementObject = parseRule (element, parentObject);
              if (elementObject != NULL)
                {
                  addRuleToRuleBase (parentObject, elementObject);
                }
            }
          else if (XMLString::compareIString (elementTagName.c_str (),
                                              "compositeRule")
                   == 0)
            {
              elementObject = parseCompositeRule (element, parentObject);
              if (elementObject != NULL)
                {
                  addCompositeRuleToRuleBase (parentObject, elementObject);
                }
            }
        }
    }

  return parentObject;
}

void *
NclPresentationControlParser::parseRule (DOMElement *parentElement,
                                         arg_unused(void *objGrandParent))
{
  SimpleRule *simplePresentationRule;
  short ruleOp;

  ruleOp = convertComparator (XMLString::transcode (
      parentElement->getAttribute (XMLString::transcode ("comparator"))));

  char *var = XMLString::transcode (
      parentElement->getAttribute (XMLString::transcode ("var")));

  char *value = XMLString::transcode (
      parentElement->getAttribute (XMLString::transcode ("value")));

  XMLString::trim (var);
  XMLString::trim (value);
  simplePresentationRule
      = new SimpleRule (XMLString::transcode (parentElement->getAttribute (
                            XMLString::transcode ("id"))),
                        var, ruleOp, value);

  return simplePresentationRule;
}

void *
NclPresentationControlParser::parseSwitch (DOMElement *parentElement,
                                           void *objGrandParent)
{
  void *parentObject;
  DOMNodeList *elementNodeList;
  int i, size;
  DOMNode *node;
  DOMElement *element;
  string elementTagName;
  void *elementObject;

  parentObject = createSwitch (parentElement, objGrandParent);
  if (unlikely (parentObject == NULL))
    {
      syntax_error ("switch: bad parent '%s'",
                    string (XMLString::transcode
                            (parentElement->getNodeName ())).c_str ());
    }

  elementNodeList = parentElement->getChildNodes ();
  size = (int) elementNodeList->getLength ();

  for (i = 0; i < size; i++)
    {
      node = elementNodeList->item (i);
      if (node->getNodeType () == DOMNode::ELEMENT_NODE)
        {
          element = (DOMElement *)node;
          elementTagName = XMLString::transcode (element->getTagName ());
          if (XMLString::compareIString (elementTagName.c_str (), "media")
              == 0)
            {
              elementObject
                  = _documentParser->getComponentsParser ()
                        ->parseMedia (element, parentObject);

              if (elementObject != NULL)
                {
                  addMediaToSwitch (parentObject, elementObject);
                }
            }
          else if (XMLString::compareIString (elementTagName.c_str (),
                                              "context")
                   == 0)
            {
              elementObject
                  = _documentParser->getComponentsParser ()
                        ->parseContext (element, parentObject);

              if (elementObject != NULL)
                {
                  addContextToSwitch (parentObject, elementObject);
                }
            }
          else if (XMLString::compareIString (elementTagName.c_str (),
                                              "switch")
                   == 0)
            {
              elementObject = parseSwitch (element, parentObject);
              if (elementObject != NULL)
                {
                  addSwitchToSwitch (parentObject, elementObject);
                }
            }
        }
    }

  for (i = 0; i < size; i++)
    {
      node = elementNodeList->item (i);
      if (node->getNodeType () == DOMNode::ELEMENT_NODE)
        {
          element = (DOMElement *)node;
          elementTagName = XMLString::transcode (element->getTagName ());
          if (XMLString::compareIString (elementTagName.c_str (),
                                         "bindRule")
              == 0)
            {
              elementObject
                  = parseBindRule ((DOMElement *)node, parentObject);

              if (elementObject != NULL)
                {
                  addBindRuleToSwitch (parentObject, elementObject);
                }
            }
          else if (XMLString::compareIString (elementTagName.c_str (),
                                              "defaultComponent")
                   == 0)
            {
              elementObject = parseDefaultComponent ((DOMElement *)node,
                                                     parentObject);

              if (elementObject != NULL)
                {
                  addDefaultComponentToSwitch (parentObject, elementObject);
                }
            }
        }
    }

  addUnmappedNodesToSwitch (parentObject);

  return parentObject;
}

void *
NclPresentationControlParser::posCompileSwitch2 (DOMElement *parentElement,
                                                 void *parentObject)
{
  DOMNodeList *elementNodeList;
  int i, size;
  DOMNode *node;
  void *elementObject;

  elementNodeList = parentElement->getChildNodes ();
  size = (int) elementNodeList->getLength ();

  for (i = 0; i < size; i++)
    {
      node = elementNodeList->item (i);
      if (node->getNodeType () == DOMNode::ELEMENT_NODE
          && XMLString::compareIString (((DOMElement *)node)->getTagName (),
                                        XMLString::transcode ("switchPort"))
                 == 0)
        {
          elementObject = _documentParser->getInterfacesParser ()
                  ->parseSwitchPort ((DOMElement *)node, parentObject);

          if (elementObject != NULL)
            {
              addSwitchPortToSwitch (parentObject, elementObject);
            }
        }
    }

  return parentObject;
}

void *
NclPresentationControlParser::parseDefaultComponent (DOMElement *parentElement,
                                               arg_unused(void *objGrandParent))
{
  return parentElement; // ?
}

void *
NclPresentationControlParser::parseDefaultDescriptor (DOMElement *parentElement,
                                               arg_unused(void *objGrandParent))
{
  return parentElement; // ?
}

void *
NclPresentationControlParser::parseCompositeRule (DOMElement *parentElement,
                                                  void *objGrandParent)
{
  void *parentObject;
  DOMNodeList *elementNodeList;
  DOMElement *element;
  DOMNode *node;
  string elementTagName;
  void *elementObject;

  parentObject = createCompositeRule (parentElement, objGrandParent);
  g_assert_nonnull (parentObject);

  elementNodeList = parentElement->getChildNodes ();
  for (int i = 0; i < (int)elementNodeList->getLength (); i++)
    {
      node = elementNodeList->item (i);
      if (node->getNodeType () == DOMNode::ELEMENT_NODE)
        {
          element = (DOMElement *)node;
          elementTagName = XMLString::transcode (element->getTagName ());

          if (XMLString::compareIString (elementTagName.c_str (), "rule")
              == 0)
            {
              elementObject = parseRule (element, parentObject);
              if (elementObject != NULL)
                {
                  addRuleToCompositeRule (parentObject, elementObject);
                }
            }
          else if (XMLString::compareIString (elementTagName.c_str (),
                                              "compositeRule")
                   == 0)
            {
              elementObject = parseCompositeRule (element, parentObject);
              if (elementObject != NULL)
                {
                  addCompositeRuleToCompositeRule (parentObject,
                                                   elementObject);
                }
            }
        }
    }

  return parentObject;
}

void *
NclPresentationControlParser::parseDescriptorSwitch (
    DOMElement *parentElement, void *objGrandParent)
{
  void *parentObject;
  DOMNodeList *elementNodeList;
  int i, size;
  DOMElement *element;
  DOMNode *node;
  string elementTagName;
  void *elementObject;

  parentObject = createDescriptorSwitch (parentElement, objGrandParent);
  g_assert_nonnull (parentObject);

  elementNodeList = parentElement->getChildNodes ();
  size = (int) elementNodeList->getLength ();

  for (i = 0; i < size; i++)
    {
      node = elementNodeList->item (i);
      if (node->getNodeType () == DOMNode::ELEMENT_NODE
          && XMLString::compareIString (((DOMElement *)node)->getTagName (),
                                        XMLString::transcode ("descriptor"))
                 == 0)
        {
          elementObject
              = _documentParser->getPresentationSpecificationParser ()
                  ->parseDescriptor ((DOMElement *)node, parentObject);

          if (elementObject != NULL)
            {
              addDescriptorToDescriptorSwitch (parentObject, elementObject);
            }
        }
    }

  for (i = 0; i < size; i++)
    {
      node = elementNodeList->item (i);
      if (node->getNodeType () == DOMNode::ELEMENT_NODE)
        {
          element = (DOMElement *)node;
          elementTagName = XMLString::transcode (element->getTagName ());
          if (XMLString::compareIString (elementTagName.c_str (),
                                         "bindRule")
              == 0)
            {
              elementObject
                  = parseBindRule ((DOMElement *)node, parentObject);

              if (elementObject != NULL)
                {
                  addBindRuleToDescriptorSwitch (parentObject,
                                                 elementObject);
                }
            }
          else if (XMLString::compareIString (elementTagName.c_str (),
                                              "defaultDescriptor")
                   == 0)
            {
              elementObject = parseDefaultDescriptor ((DOMElement *)node,
                                                      parentObject);

              if (elementObject != NULL)
                {
                  addDefaultDescriptorToDescriptorSwitch (parentObject,
                                                          elementObject);
                }
            }
        }
    }

  return parentObject;
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

void *
NclPresentationControlParser::createCompositeRule (
    DOMElement *parentElement, arg_unused (void *objGrandParent))
{
  CompositeRule *compositePresentationRule;
  short ruleOp = CompositeRule::OP_AND;

  if (XMLString::compareIString (
          XMLString::transcode (parentElement->getAttribute (
              XMLString::transcode ("operator"))),
          "and")
      == 0)
    {
      ruleOp = CompositeRule::OP_AND;
    }
  else if (XMLString::compareIString (
               XMLString::transcode (parentElement->getAttribute (
                   XMLString::transcode ("operator"))),
               "or")
           == 0)
    {
      ruleOp = CompositeRule::OP_OR;
    }

  // cria regra composta
  compositePresentationRule = new CompositeRule (
      XMLString::transcode (
          parentElement->getAttribute (XMLString::transcode ("id"))),
      ruleOp);

  return compositePresentationRule;
}

void *
NclPresentationControlParser::createSwitch (DOMElement *parentElement,
                                            arg_unused (void *objGrandParent))
{
  string id;
  Node *node;
  string attValue;
  Entity *referNode;
  NclDocument *document;
  SwitchNode *switchNode;

  if (unlikely (!parentElement->hasAttribute (XMLString::transcode ("id"))))
    syntax_error ("switch: missing id");

  id = XMLString::transcode (
      parentElement->getAttribute (XMLString::transcode ("id")));

  node = getDocumentParser ()->getNode (id);
  if (unlikely (node != NULL))
    syntax_error ("switch '%s': duplicated id", id.c_str ());

  if (parentElement->hasAttribute (XMLString::transcode ("refer")))
    {
      attValue = XMLString::transcode (
          parentElement->getAttribute (XMLString::transcode ("refer")));

      try
        {
          referNode
              = (SwitchNode *) getDocumentParser ()->getNode (attValue);

          if (referNode == NULL)
            {
              document = getDocumentParser ()->getNclDocument ();
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

void *
NclPresentationControlParser::createRuleBase (DOMElement *parentElement,
                                              arg_unused (void *objGrandParent))
{
  RuleBase *ruleBase;
  ruleBase = new RuleBase (XMLString::transcode (
      parentElement->getAttribute (XMLString::transcode ("id"))));

  return ruleBase;
}

void *
NclPresentationControlParser::createDescriptorSwitch (
    DOMElement *parentElement, arg_unused (void *objGrandParent))
{
  DescriptorSwitch *descriptorSwitch
      = new DescriptorSwitch (XMLString::transcode (
          parentElement->getAttribute (XMLString::transcode ("id"))));

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
  NclDocumentParser *compiler;
  NclDocument *importedDocument;
  RuleBase *createdBase;

  // apanha o alias a localizacao da base
  baseAlias = XMLString::transcode (
      ((DOMElement *)childObject)
          ->getAttribute (XMLString::transcode ("alias")));

  baseLocation = XMLString::transcode (
      ((DOMElement *)childObject)
          ->getAttribute (XMLString::transcode ("documentURI")));

  compiler = getDocumentParser ();
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
  if (descriptors->count (XMLString::transcode (
          bindRule->getAttribute (XMLString::transcode ("constituent"))))
      == 0)
    {
      return;
    }

  descriptor = (GenericDescriptor *)(*descriptors)[XMLString::transcode (
      bindRule->getAttribute (XMLString::transcode ("constituent")))];

  if (descriptor == NULL)
    {
      return;
    }

  document = getDocumentParser ()->getNclDocument ();
  ncmRule = document->getRule (XMLString::transcode (
      bindRule->getAttribute (XMLString::transcode ("rule"))));

  if (ncmRule == NULL)
    {
      return;
    }

  descriptorSwitch->addDescriptor (descriptor, ncmRule);
}

void
NclPresentationControlParser::addBindRuleToSwitch (void *parentObject,
                                                      void *childObject)
{
  SwitchNode *switchNode;
  DOMElement *bindRule;
  map<string, NodeEntity *> *nodes;
  Node *node;
  NclDocument *document;
  Rule *ncmRule;

  switchNode = (SwitchNode *)parentObject;
  bindRule = (DOMElement *)childObject;

  if (switchConstituents->count (switchNode->getId ()) == 0)
    {
      return;
    }

  nodes = (*switchConstituents)[switchNode->getId ()];
  if (nodes->count (XMLString::transcode (
          bindRule->getAttribute (XMLString::transcode ("constituent"))))
      == 0)
    {
      return;
    }

  node = (NodeEntity *)(*nodes)[XMLString::transcode (
      bindRule->getAttribute (XMLString::transcode ("constituent")))];

  if (node == NULL)
    {
      return;
    }

  document = getDocumentParser ()->getNclDocument ();
  ncmRule = document->getRule (XMLString::transcode (
      bindRule->getAttribute (XMLString::transcode ("rule"))));

  if (ncmRule == NULL)
    {
      return;
    }

  switchNode->addNode (node, ncmRule);
}

void
NclPresentationControlParser::addUnmappedNodesToSwitch (
    void *parentObject)
{
  SwitchNode *switchNode;

  map<string, NodeEntity *> *nodes;
  map<string, NodeEntity *>::iterator i;

  switchNode = (SwitchNode *)parentObject;

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
  if (nodes->count (XMLString::transcode (defaultComponent->getAttribute (
          XMLString::transcode ("component"))))
      == 0)
    {
      return;
    }

  node = (NodeEntity *)(*nodes)[XMLString::transcode (
      defaultComponent->getAttribute (XMLString::transcode ("component")))];

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
          XMLString::transcode (defaultDescriptor->getAttribute (
              XMLString::transcode ("descriptor"))))
      == 0)
    {
      return;
    }

  descriptor = (GenericDescriptor *)(*descriptors)[XMLString::transcode (
      defaultDescriptor->getAttribute (
          XMLString::transcode ("descriptor")))];

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
  DOMNodeList *elementNodeList;
  int i, size;
  DOMNode *node;
  DOMElement *element;
  string elementTagName;
  Node *elementObject;

  elementNodeList = parentElement->getChildNodes ();
  size = (int) elementNodeList->getLength ();

  for (i = 0; i < size; i++)
    {
      node = elementNodeList->item (i);
      if (node->getNodeType () == DOMNode::ELEMENT_NODE)
        {
          element = (DOMElement *)node;
          elementTagName = XMLString::transcode (element->getTagName ());
          if (XMLString::compareIString (elementTagName.c_str (), "context")
              == 0)
            {
              elementObject
                  = (Node *)(getDocumentParser ()->getNode (XMLString::transcode (
                                     element->getAttribute (
                                         XMLString::transcode ("id")))));

              if (elementObject->instanceOf ("ContextNode"))
                {
                  _documentParser->getComponentsParser ()
                          ->posCompileContext (element, elementObject);
                }
            }
          else if (XMLString::compareIString (elementTagName.c_str (),
                                              "switch")
                   == 0)
            {
              elementObject = getDocumentParser ()
                                  ->getNode (XMLString::transcode (
                                      element->getAttribute (
                                          XMLString::transcode ("id"))));

              if (unlikely (elementObject == NULL))
                {
                  syntax_error ("node '%s' should be a switch",
                                string (XMLString::transcode
                                        (element->getAttribute
                                         (XMLString::transcode
                                          ("id")))).c_str ());
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

short
NclPresentationControlParser::convertComparator (const string &comparator)
{
  if (comparator == "eq")
    return Comparator::CMP_EQ;

  else if (comparator == "ne")
    return Comparator::CMP_NE;

  else if (comparator == "gt")
    return Comparator::CMP_GT;

  else if (comparator == "lt")
    return Comparator::CMP_LT;

  else if (comparator == "ge")
    return Comparator::CMP_GTE;

  else if (comparator == "le")
    return Comparator::CMP_LTE;

  return -1;
}

GINGA_NCLCONV_END
