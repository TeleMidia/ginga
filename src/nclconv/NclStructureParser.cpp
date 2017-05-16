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
#include "NclStructureParser.h"

#include "NclComponentsParser.h"

#include "NclDocumentParser.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_NCLCONV_BEGIN

NclStructureParser::NclStructureParser (NclDocumentParser *documentParser)
    : ModuleParser (documentParser)
{
}

ContextNode *
NclStructureParser::parseBody (DOMElement *parentElement,
                               NclDocument *objGrandParent)
{
  DOMNodeList *elementNodeList;
  int i, size;
  DOMNode *node;
  DOMElement *element;
  string elementTagName;
  void *elementObject;

  ContextNode *parentObject = createBody (parentElement, objGrandParent);
  g_assert_nonnull (parentObject);

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
              elementObject = _documentParser->getComponentsParser()
                      ->parseMedia (element, parentObject);

              if (elementObject != NULL)
                {
                  // add media to body
                  _documentParser->getComponentsParser ()
                          ->addMediaToContext (parentObject, elementObject);
                }
            }
          else if (XMLString::compareIString (elementTagName.c_str (),
                                              "context")
                   == 0)
            {
              elementObject = _documentParser->getComponentsParser()
                      ->parseContext (element, parentObject);

              if (elementObject != NULL)
                {
                  // add context to body
                  _documentParser->getComponentsParser ()
                          ->addContextToContext (parentObject, elementObject);
                }
            }
          else if (XMLString::compareIString (elementTagName.c_str (),
                                              "switch")
                   == 0)
            {
              elementObject = _documentParser->getPresentationControlParser ()
                      ->parseSwitch (element, parentObject);

              if (elementObject != NULL)
                {
                  // add switch to body
                  _documentParser->getComponentsParser ()
                          ->addSwitchToContext (parentObject, elementObject);
                }
            }
        }
    }

  for (i = 0; i < size; i++)
    {
      node = elementNodeList->item (i);
      if (node->getNodeType () == DOMNode::ELEMENT_NODE
          && XMLString::compareIString (((DOMElement *)node)->getTagName (),
                                        XMLString::transcode ("property"))
                 == 0)
        {
          elementObject = _documentParser->getInterfacesParser ()
                  ->parseProperty ((DOMElement *)node, parentObject);

          if (elementObject != NULL)
            {
              // add property to body
              _documentParser->getComponentsParser ()
                      ->addPropertyToContext (parentObject, elementObject);
            }
        }
    }

  return parentObject;
}

void
NclStructureParser::parseHead (DOMElement *parentElement)
{
  void *parentObject = NULL;
  void *elementObject = NULL;
  DOMNodeList *elementNodeList;
  int i, size;
  DOMNode *node;
  NclDocument *nclDoc = getDocumentParser()->getNclDocument();
  g_assert_nonnull (nclDoc);

  parentObject = parentElement;
  g_assert_nonnull (parentObject);

  elementNodeList = parentElement->getChildNodes ();

  size = (int) elementNodeList->getLength ();

  for (i = 0; i < size; i++)
    {
      node = elementNodeList->item (i);
      if (node->getNodeType () == DOMNode::ELEMENT_NODE
          && XMLString::compareIString (
                 ((DOMElement *)node)->getTagName (),
                 XMLString::transcode ("importedDocumentBase"))
                 == 0)
        {
          elementObject = _documentParser->getImportParser ()
                 ->parseImportedDocumentBase ((DOMElement *)node, parentObject);

          if (elementObject != NULL)
            {
              // addImportedDocumentBaseToHead (parentObject, elementObject);

              break;
            }
        }
    }

  for (i = 0; i < size; i++)
    {
      node = elementNodeList->item (i);
      if (node->getNodeType () == DOMNode::ELEMENT_NODE
          && XMLString::compareIString (((DOMElement *)node)->getTagName (),
                                        XMLString::transcode ("regionBase"))
                 == 0)
        {
          RegionBase *regionBase = _documentParser->getLayoutParser ()
                  ->parseRegionBase ((DOMElement *)node, parentObject);

          if (regionBase != NULL)
            {
              nclDoc->addRegionBase(regionBase);
            }
        }
    }

  for (i = 0; i < size; i++)
    {
      node = elementNodeList->item (i);
      if (node->getNodeType () == DOMNode::ELEMENT_NODE
          && XMLString::compareIString (((DOMElement *)node)->getTagName (),
                                        XMLString::transcode ("ruleBase"))
                 == 0)
        {
          RuleBase *ruleBase = _documentParser->getPresentationControlParser ()
                  ->parseRuleBase ((DOMElement *)node, parentObject);

          if (ruleBase != NULL)
            {
              nclDoc->setRuleBase (ruleBase);
              break;
            }
        }
    }

  for (i = 0; i < size; i++)
    {
      node = elementNodeList->item (i);
      if (node->getNodeType () == DOMNode::ELEMENT_NODE
          && XMLString::compareIString (
                 ((DOMElement *)node)->getTagName (),
                 XMLString::transcode ("transitionBase"))
                 == 0)
        {
          TransitionBase *transBase = _documentParser->getTransitionParser ()
                  ->parseTransitionBase ((DOMElement *)node, parentObject);

          if (transBase != NULL)
            {
              nclDoc->setTransitionBase (transBase);
              break;
            }
        }
    }

  for (i = 0; i < size; i++)
    {
      node = elementNodeList->item (i);
      if (node->getNodeType () == DOMNode::ELEMENT_NODE
          && XMLString::compareIString (
                 ((DOMElement *)node)->getTagName (),
                 XMLString::transcode ("descriptorBase"))
                 == 0)
        {
          DescriptorBase *descBase
              = _documentParser->getPresentationSpecificationParser ()
                  ->parseDescriptorBase ((DOMElement *)node, parentObject);

          if (descBase != NULL)
            {
              nclDoc->setDescriptorBase (descBase);
              break;
            }
        }
    }

  for (i = 0; i < size; i++)
    {
      node = elementNodeList->item (i);
      if (node->getNodeType () == DOMNode::ELEMENT_NODE
          && XMLString::compareIString (
                 ((DOMElement *)node)->getTagName (),
                 XMLString::transcode ("connectorBase"))
                 == 0)
        {
          ConnectorBase *connBase = _documentParser->getConnectorsParser ()
                  ->parseConnectorBase ((DOMElement *)node, parentObject);

          if (connBase != NULL)
            {
              nclDoc->setConnectorBase(connBase);
              break;
            }
        }
    }

  for (i = 0; i < size; i++)
    {
      node = elementNodeList->item (i);
      if (node->getNodeType () == DOMNode::ELEMENT_NODE
          && XMLString::compareIString (((DOMElement *)node)->getTagName (),
                                        XMLString::transcode ("meta"))
                 == 0)
        {
          Meta *meta = _documentParser->getMetainformationParser ()
                  ->parseMeta ((DOMElement *)node, parentObject);

          if (meta != NULL)
            {
              nclDoc->addMetainformation (meta);
              break;
            }
        }
    }

  for (i = 0; i < size; i++)
    {
      node = elementNodeList->item (i);
      if (node->getNodeType () == DOMNode::ELEMENT_NODE
          && XMLString::compareIString (((DOMElement *)node)->getTagName (),
                                        XMLString::transcode ("metadata"))
                 == 0)
        {
          Metadata *metadata = _documentParser->getMetainformationParser ()
                  ->parseMetadata ((DOMElement *)node, parentObject);

          if (elementObject != NULL)
            {
              nclDoc->addMetadata (metadata);
              break;
            }
        }
    }
}

NclDocument*
NclStructureParser::parseNcl (DOMElement *parentElement, void *objGrandParent)
{
  DOMNodeList *elementNodeList;
  int i, size;
  DOMNode *node;

  NclDocument* parentObject = createNcl (parentElement, objGrandParent);
  g_assert_nonnull (parentObject);

  elementNodeList = parentElement->getChildNodes ();
  size = (int) elementNodeList->getLength ();

  for (i = 0; i < size; i++)
    {
      node = elementNodeList->item (i);
      if (node->getNodeType () == DOMNode::ELEMENT_NODE
          && XMLString::compareIString (((DOMElement *)node)->getTagName (),
                                        XMLString::transcode ("head"))
                 == 0)
        {
          parseHead ((DOMElement *)node);
        }
    }

  for (i = 0; i < size; i++)
    {
      node = elementNodeList->item (i);
      if (node->getNodeType () == DOMNode::ELEMENT_NODE
          && XMLString::compareIString (((DOMElement *)node)->getTagName (),
                                        XMLString::transcode ("body"))
                 == 0)
        {
          ContextNode *body = parseBody ((DOMElement *)node, parentObject);
          if (body != NULL)
            {
              posCompileBody ((DOMElement *)node, body);
              addBodyToNcl (parentObject, body);
              break;
            }
        }
    }

  return parentObject;
}

void
NclStructureParser::addBodyToNcl (arg_unused (void *parentObject),
                                  arg_unused (void *childObject))
{
}

void
NclStructureParser::addRegionBaseToHead (RegionBase *regionBase)
{
  getDocumentParser ()->getNclDocument ()->addRegionBase (regionBase);
}


ContextNode *
NclStructureParser::createBody (DOMElement *parentElement,
                                void *objGrandParent)
{
  // criar composicao a partir do elemento body do documento ncl
  // fazer uso do nome da composicao body que foi atribuido pelo
  // compilador
  NclDocument *document;
  ContextNode *context;

  document = getDocumentParser ()->getNclDocument ();
  if (!parentElement->hasAttribute (XMLString::transcode ("id")))
    {
      parentElement->setAttribute (
          XMLString::transcode ("id"),
          XMLString::transcode (document->getId ().c_str ()));

      context = (ContextNode *) _documentParser->getComponentsParser ()
                    ->createContext (parentElement, objGrandParent);

      parentElement->removeAttribute (XMLString::transcode ("id"));
    }
  else
    {
      context = (ContextNode *)_documentParser->getComponentsParser ()
                    ->createContext (parentElement, objGrandParent);
    }
  document->setBody (context);
  return context;
}

void
NclStructureParser::solveNodeReferences (CompositeNode *composition)
{
  Node *node;
  NodeEntity *nodeEntity;
  Entity *referredNode;
  vector<Node *> *nodes;
  vector<Node *>::iterator it;
  bool deleteNodes = false;

  if (composition->instanceOf ("SwitchNode"))
    {
      deleteNodes = true;
      nodes = _documentParser->getPresentationControlParser ()
                ->getSwitchConstituents ((SwitchNode *)composition);
    }
  else
    {
      nodes = composition->getNodes ();
    }

  if (nodes == NULL)
    {
      return;
    }

  for (it = nodes->begin (); it != nodes->end (); ++it)
    {
      node = *it;
      if (node != NULL)
        {
          if (node->instanceOf ("ReferNode"))
            {
              referredNode = ((ReferNode *)node)->getReferredEntity ();
              if (referredNode != NULL)
                {
                  if (referredNode->instanceOf ("ReferredNode"))
                    {
                      nodeEntity
                          = (NodeEntity *)(getDocumentParser ()->getNode (
                                                   referredNode->getId ()));

                      if (nodeEntity != NULL)
                        {
                          ((ReferNode *)node)
                              ->setReferredEntity (
                                  nodeEntity->getDataEntity ());
                        }
                      else
                        {
                          syntax_error ("media: bad refer '%s'",
                                        referredNode->getId ().c_str ());
                        }
                    }
                }
            }
          else if (node->instanceOf ("CompositeNode"))
            {
              solveNodeReferences ((CompositeNode *)node);
            }
        }
    }

  if (deleteNodes)
    {
      delete nodes;
    }
}

void *
NclStructureParser::posCompileBody (DOMElement *parentElement,
                                    ContextNode *body)
{
  solveNodeReferences (body);

  return _documentParser->getComponentsParser ()
          ->posCompileContext (parentElement, body);
}

NclDocument*
NclStructureParser::createNcl (DOMElement *parentElement,
                               arg_unused (void *objGrandParent))
{
  string docName;
  NclDocument *document;

  if (parentElement->hasAttribute (XMLString::transcode ("id")))
    {
      docName = XMLString::transcode (
          parentElement->getAttribute (XMLString::transcode ("id")));
    }

  if (docName == "")
    docName = "ncl";

  document = new NclDocument (docName, _documentParser->getPath ());
  _documentParser->setNclDocument (document);

  return document;
}

GINGA_NCLCONV_END
