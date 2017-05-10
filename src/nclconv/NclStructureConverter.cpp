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
#include "NclStructureConverter.h"

GINGA_NCLCONV_BEGIN

NclStructureConverter::NclStructureConverter (
    DocumentParser *documentParser)
    : NclStructureParser (documentParser)
{
}

void
NclStructureConverter::addBodyToNcl (arg_unused (void *parentObject), arg_unused (void *childObject))
{
}

void
NclStructureConverter::addPortToBody (void *parentObject, void *childObject)
{
  getComponentsParser ()->addPortToContext (parentObject, childObject);
}

void
NclStructureConverter::addPropertyToBody (void *parentObject,
                                          void *childObject)
{
  getComponentsParser ()->addPropertyToContext (parentObject, childObject);
}

void
NclStructureConverter::addContextToBody (void *parentObject,
                                         void *childObject)
{
  getComponentsParser ()->addContextToContext (parentObject, childObject);
}

void
NclStructureConverter::addSwitchToBody (void *parentObject,
                                        void *childObject)
{
  getComponentsParser ()->addSwitchToContext (parentObject, childObject);
}

void
NclStructureConverter::addDescriptorBaseToHead (arg_unused (void *parentObject),
                                                void *childObject)
{
  NclDocument *document;
  document = (NclDocument *)getDocumentParser ()->getObject ("return",
                                                             "document");

  document->setDescriptorBase ((DescriptorBase *)childObject);
}

void
NclStructureConverter::addHeadToNcl (arg_unused (void *parentObject), arg_unused (void *childObject))
{
  // nothing to do
}

void
NclStructureConverter::addRegionBaseToHead (arg_unused (void *parentObject),
                                            void *childObject)
{
  NclDocument *document;
  document = (NclDocument *)getDocumentParser ()->getObject ("return",
                                                             "document");

  document->addRegionBase ((RegionBase *)childObject);
}

void
NclStructureConverter::addTransitionBaseToHead (arg_unused (void *parentObject),
                                                void *childObject)
{
  NclDocument *document;

  document = (NclDocument *)(getDocumentParser ()->getObject ("return",
                                                              "document"));
  document->setTransitionBase ((TransitionBase *)childObject);
}

void
NclStructureConverter::addLinkToBody (void *parentObject, void *childObject)
{
  getComponentsParser ()->addLinkToContext (parentObject, childObject);
}

void
NclStructureConverter::addMediaToBody (void *parentObject,
                                       void *childObject)
{
  getComponentsParser ()->addMediaToContext (parentObject, childObject);
}

void
NclStructureConverter::addRuleBaseToHead (arg_unused (void *parentObject),
                                          void *childObject)
{
  NclDocument *document;
  document = (NclDocument *)getDocumentParser ()->getObject ("return",
                                                             "document");

  document->setRuleBase ((RuleBase *)childObject);
}

void
NclStructureConverter::addConnectorBaseToHead (arg_unused (void *parentObject),
                                               void *childObject)
{
  NclDocument *document;
  document = (NclDocument *)getDocumentParser ()->getObject ("return",
                                                             "document");

  document->setConnectorBase ((ConnectorBase *)childObject);
}

void *
NclStructureConverter::createBody (DOMElement *parentElement,
                                   void *objGrandParent)
{
  // criar composicao a partir do elemento body do documento ncl
  // fazer uso do nome da composicao body que foi atribuido pelo
  // compilador
  NclDocument *document;
  ContextNode *context;

  document = (NclDocument *)getDocumentParser ()->getObject ("return",
                                                             "document");

  if (!parentElement->hasAttribute (XMLString::transcode ("id")))
    {
      parentElement->setAttribute (
          XMLString::transcode ("id"),
          XMLString::transcode (document->getId ().c_str ()));

      context = (ContextNode *)((NclComponentsConverter *)
                                    getComponentsParser ())
                    ->createContext (parentElement, objGrandParent);

      parentElement->removeAttribute (XMLString::transcode ("id"));
    }
  else
    {
      context = (ContextNode *)((NclComponentsConverter *)
                                    getComponentsParser ())
                    ->createContext (parentElement, objGrandParent);
    }
  document->setBody (context);
  return context;
}

void
NclStructureConverter::solveNodeReferences (CompositeNode *composition)
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
      nodes = ((NclPresentationControlConverter *)
                   getPresentationControlParser ())
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
                          = (NodeEntity *)(((NclDocumentConverter *)
                                                getDocumentParser ())
                                               ->getNode (
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
NclStructureConverter::posCompileBody (DOMElement *parentElement,
                                       void *parentObject)
{
  solveNodeReferences ((CompositeNode *)parentObject);
  return NclStructureParser::posCompileBody (parentElement, parentObject);
}

void *
NclStructureConverter::createHead (DOMElement *parentElement,
                                   arg_unused (void *objGrandParent))
{
  return parentElement;
}

void *
NclStructureConverter::createNcl (DOMElement *parentElement,
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
    {
      docName = "nclDocument";
    }

  document = new NclDocument (docName, documentParser->getDocumentUri ());
  getDocumentParser ()->addObject ("return", "document", document);
  return document;
}

void
NclStructureConverter::addImportedDocumentBaseToHead (arg_unused (void *parentObject),
                                                      arg_unused (void *childObject))
{
}

void
NclStructureConverter::addMetaToHead (arg_unused (void *parentObject), void *childObject)
{
  NclDocument *document;

  document = (NclDocument *)getDocumentParser ()->getObject ("return",
                                                             "document");

  document->addMetainformation ((Meta *)childObject);
}

void
NclStructureConverter::addMetadataToHead (arg_unused (void *parentObject),
                                          void *childObject)
{
  NclDocument *document;

  document = (NclDocument *)getDocumentParser ()->getObject ("return",
                                                             "document");

  document->addMetadata ((Metadata *)childObject);
}

GINGA_NCLCONV_END
