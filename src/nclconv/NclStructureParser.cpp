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

void *
NclStructureParser::parseBody (DOMElement *parentElement,
                               void *objGrandParent)
{
  void *parentObject;
  DOMNodeList *elementNodeList;
  int i, size;
  DOMNode *node;
  DOMElement *element;
  string elementTagName;
  void *elementObject;

  parentObject = createBody (parentElement, objGrandParent);
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
              elementObject = getComponentsParser()->parseMedia (element, parentObject);

              if (elementObject != NULL)
                {
                  // add media to body
                  getComponentsParser ()->addMediaToContext (parentObject, elementObject);
                }
            }
          else if (XMLString::compareIString (elementTagName.c_str (),
                                              "context")
                   == 0)
            {
              elementObject = getComponentsParser()->parseContext (element, parentObject);

              if (elementObject != NULL)
                {
                  // add context to body
                  getComponentsParser ()->addContextToContext (parentObject, elementObject);
                }
            }
          else if (XMLString::compareIString (elementTagName.c_str (),
                                              "switch")
                   == 0)
            {
              elementObject = getPresentationControlParser ()->parseSwitch (
                  element, parentObject);

              if (elementObject != NULL)
                {
                  // add switch to body
                  getComponentsParser ()->addSwitchToContext (parentObject, elementObject);
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
          elementObject = getInterfacesParser ()->parseProperty (
              (DOMElement *)node, parentObject);

          if (elementObject != NULL)
            {
              // add property to body
              getComponentsParser ()->addPropertyToContext (parentObject,
                                                            elementObject);
            }
        }
    }

  return parentObject;
}

void *
NclStructureParser::parseHead (DOMElement *parentElement,
                               arg_unused(void *objGrandParent))
{
  void *parentObject = NULL;
  DOMNodeList *elementNodeList;
  int i, size;
  DOMNode *node;
  void *elementObject = NULL;

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
          elementObject = getImportParser ()->parseImportedDocumentBase (
              (DOMElement *)node, parentObject);

          if (elementObject != NULL)
            {
              addImportedDocumentBaseToHead (parentObject, elementObject);

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
          elementObject = getLayoutParser ()->parseRegionBase (
              (DOMElement *)node, parentObject);

          if (elementObject != NULL)
            {
              addRegionBaseToHead (parentObject, elementObject);
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
          elementObject = getPresentationControlParser ()->parseRuleBase (
              (DOMElement *)node, parentObject);

          if (elementObject != NULL)
            {
              addRuleBaseToHead (parentObject, elementObject);
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
          elementObject = getTransitionParser ()->parseTransitionBase (
              (DOMElement *)node, parentObject);

          if (elementObject != NULL)
            {
              addTransitionBaseToHead (parentObject, elementObject);
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
          elementObject
              = getPresentationSpecificationParser ()->parseDescriptorBase (
                  (DOMElement *)node, parentObject);

          if (elementObject != NULL)
            {
              addDescriptorBaseToHead (parentObject, elementObject);
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
          elementObject = getConnectorsParser ()->parseConnectorBase (
              (DOMElement *)node, parentObject);

          if (elementObject != NULL)
            {
              addConnectorBaseToHead (parentObject, elementObject);
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
          elementObject = getMetainformationParser ()->parseMeta (
              (DOMElement *)node, parentObject);

          if (elementObject != NULL)
            {
              addMetaToHead (parentObject, elementObject);
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
          elementObject = getMetainformationParser ()->parseMetadata (
              (DOMElement *)node, parentObject);

          if (elementObject != NULL)
            {
              addMetadataToHead (parentObject, elementObject);
              break;
            }
        }
    }

  return parentObject;
}

void *
NclStructureParser::parseNcl (DOMElement *parentElement,
                              void *objGrandParent)
{
  void *parentObject = NULL;
  DOMNodeList *elementNodeList;
  int i, size;
  DOMNode *node;
  void *elementObject = NULL;

  parentObject = createNcl (parentElement, objGrandParent);
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
          elementObject = parseHead ((DOMElement *)node, parentObject);
          ;
          if (elementObject != NULL)
            {
              //addHeadToNcl (parentObject, elementObject);
              break;
            }
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
          elementObject = parseBody ((DOMElement *)node, parentObject);
          if (elementObject != NULL)
            {
              posCompileBody ((DOMElement *)node, elementObject);
              addBodyToNcl (parentObject, elementObject);
              break;
            }
        }
    }

  return parentObject;
}

NclTransitionParser *
NclStructureParser::getTransitionParser ()
{
  return transitionParser;
}

void
NclStructureParser::setTransitionParser (
    NclTransitionParser *transitionParser)
{
  this->transitionParser = transitionParser;
}

NclPresentationSpecificationParser *
NclStructureParser::getPresentationSpecificationParser ()
{
  return presentationSpecificationParser;
}

void
NclStructureParser::setPresentationSpecificationParser (
    NclPresentationSpecificationParser *presentationSpecificationParser)
{
  this->presentationSpecificationParser = presentationSpecificationParser;
}

NclComponentsParser *
NclStructureParser::getComponentsParser ()
{
  return componentsParser;
}

void
NclStructureParser::setComponentsParser (
    NclComponentsParser *componentsParser)
{
  this->componentsParser = componentsParser;
}

NclLinkingParser *
NclStructureParser::getLinkingParser ()
{
  return linkingParser;
}

void
NclStructureParser::setLinkingParser (NclLinkingParser *linkingParser)
{
  this->linkingParser = linkingParser;
}

NclLayoutParser *
NclStructureParser::getLayoutParser ()
{
  return layoutParser;
}

void
NclStructureParser::setLayoutParser (NclLayoutParser *layoutParser)
{
  this->layoutParser = layoutParser;
}

NclMetainformationParser *
NclStructureParser::getMetainformationParser ()
{
  return metainformationParser;
}

NclInterfacesParser *
NclStructureParser::getInterfacesParser ()
{
  return interfacesParser;
}

void
NclStructureParser::setInterfacesParser (
    NclInterfacesParser *interfacesParser)
{
  this->interfacesParser = interfacesParser;
}

NclPresentationControlParser *
NclStructureParser::getPresentationControlParser ()
{
  return presentationControlParser;
}

void
NclStructureParser::setPresentationControlParser (
    NclPresentationControlParser *presentationControlParser)
{
  this->presentationControlParser = presentationControlParser;
}

NclConnectorsParser *
NclStructureParser::getConnectorsParser ()
{
  return connectorsParser;
}

void
NclStructureParser::setConnectorsParser (
    NclConnectorsParser *connectorsParser)
{
  this->connectorsParser = connectorsParser;
}

NclImportParser *
NclStructureParser::getImportParser ()
{
  return importParser;
}

void
NclStructureParser::setImportParser (NclImportParser *importParser)
{
  this->importParser = importParser;
}

void
NclStructureParser::setMetainformationParser (
    NclMetainformationParser *metainformationParser)
{
  this->metainformationParser = metainformationParser;
}

void
NclStructureParser::addBodyToNcl (arg_unused (void *parentObject),arg_unused (void *childObject))
{
}


void
NclStructureParser::addDescriptorBaseToHead (arg_unused (void *parentObject),
                                                void *childObject)
{
  NclDocument *document;
  document = getDocumentParser ()->getNclDocument ();
  document->setDescriptorBase ((DescriptorBase *)childObject);
}

void
NclStructureParser::addRegionBaseToHead (arg_unused (void *parentObject),
                                            void *childObject)
{
  NclDocument *document;
  document = getDocumentParser ()->getNclDocument ();
  document->addRegionBase ((RegionBase *)childObject);
}

void
NclStructureParser::addTransitionBaseToHead (arg_unused (void *parentObject),
                                                void *childObject)
{
  NclDocument *document;

  document = getDocumentParser ()->getNclDocument ();
  document->setTransitionBase ((TransitionBase *)childObject);
}
void
NclStructureParser::addRuleBaseToHead (arg_unused (void *parentObject),
                                       void *childObject)
{
  NclDocument *document = getDocumentParser ()->getNclDocument ();
  document->setRuleBase ((RuleBase *)childObject);
}

void
NclStructureParser::addConnectorBaseToHead (arg_unused (void *parentObject),
                                            void *childObject)
{
  NclDocument *document = getDocumentParser ()->getNclDocument ();
  document->setConnectorBase ((ConnectorBase *)childObject);
}

void *
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

      context = (ContextNode *) getComponentsParser ()
                    ->createContext (parentElement, objGrandParent);

      parentElement->removeAttribute (XMLString::transcode ("id"));
    }
  else
    {
      context = (ContextNode *)getComponentsParser ()
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
      nodes = getPresentationControlParser ()
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
                                       void *parentObject)
{
  solveNodeReferences ((CompositeNode *)parentObject);

  return getComponentsParser ()->posCompileContext (parentElement,
                                                    parentObject);
}

void *
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

  document = new NclDocument (docName, documentParser->getPath ());
  documentParser->setNclDocument (document);
  return document;
}

void
NclStructureParser::addImportedDocumentBaseToHead (arg_unused (void *parentObject),
                                                   arg_unused (void *childObject))
{
}

void
NclStructureParser::addMetaToHead (arg_unused (void *parentObject),
                                   void *childObject)
{
  NclDocument *document;

  document = getDocumentParser ()->getNclDocument ();
  document->addMetainformation ((Meta *)childObject);
}

void
NclStructureParser::addMetadataToHead (arg_unused (void *parentObject),
                                       void *childObject)
{
  NclDocument *document = getDocumentParser ()->getNclDocument ();
  document->addMetadata ((Metadata *)childObject);
}

GINGA_NCLCONV_END
