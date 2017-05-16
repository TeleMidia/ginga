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

#include "NclParser.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_NCLCONV_BEGIN

NclStructureParser::NclStructureParser (NclParser *nclParser)
    : ModuleParser (nclParser)
{
}

ContextNode *
NclStructureParser::parseBody (DOMElement *body_element)
{
  ContextNode *body = createBody (body_element);
  g_assert_nonnull (body);

  DOMElement *child;
  FOR_EACH_DOM_ELEM_CHILD(child, body_element)
    {
      if (dom_element_tagname(child) == "media")
        {
          Node *media = _nclParser->getComponentsParser()->parseMedia (child);
          if (media)
            {
              // add media to body
              _nclParser->getComponentsParser ()
                  ->addMediaToContext (body, media);
            }
        }
      else if (dom_element_tagname(child) == "context")
        {
          Node *child_context = _nclParser->getComponentsParser()
              ->parseContext (child);
          if (child_context)
            {
              // add context to body
              _nclParser->getComponentsParser ()
                  ->addContextToContext (body, child_context);
            }
        }
      else if (dom_element_tagname(child) == "switch")
        {
          Node *switch_node = _nclParser->getPresentationControlParser ()
              ->parseSwitch (child);

          if (switch_node)
            {
              // add switch to body
              _nclParser->getComponentsParser ()
                  ->addSwitchToContext (body, switch_node);
            }
        }
    }

  FOR_EACH_DOM_ELEM_CHILD(child, body_element)
    {
      if(dom_element_tagname(child) == "property")
        {
          PropertyAnchor *prop = _nclParser->getInterfacesParser ()
              ->parseProperty (child, body);

          if (prop)
            {
              // add property to body
              _nclParser->getComponentsParser ()
                  ->addPropertyToContext (body, prop);
            }
        }
    }

  return body;
}

void
NclStructureParser::parseHead (DOMElement *head_element)
{
  NclDocument *nclDoc = getNclParser()->getNclDocument();

  cout << this << " " << nclDoc << endl;
  g_assert_nonnull (nclDoc);

  DOMElement *child;
  FOR_EACH_DOM_ELEM_CHILD(child, head_element)
    {
      if (dom_element_tagname(child) ==  "importedDocumentBase")
        {
          _nclParser->getImportParser ()->parseImportedDocumentBase (child);
        }
    }

  FOR_EACH_DOM_ELEM_CHILD(child, head_element)
    {
      if(dom_element_tagname(child) == "regionBase")
        {
          RegionBase *regionBase = _nclParser->getLayoutParser ()
              ->parseRegionBase (child);

          if (regionBase)
            {
              nclDoc->addRegionBase(regionBase);
            }
        }
    }

  FOR_EACH_DOM_ELEM_CHILD(child, head_element)
    {
      if (dom_element_tagname(child) == "ruleBase")
        {
          RuleBase *ruleBase = _nclParser->getPresentationControlParser ()
                    ->parseRuleBase (child);
          if (ruleBase)
            {
              nclDoc->setRuleBase (ruleBase);
              break;
            }
        }
    }

  FOR_EACH_DOM_ELEM_CHILD(child, head_element)
    {
      if (dom_element_tagname(child) == "transitionBase")
        {
          TransitionBase *transBase = _nclParser->getTransitionParser ()
                ->parseTransitionBase (child);
          if (transBase)
            {
              nclDoc->setTransitionBase (transBase);
              break;
            }
        }
    }

  FOR_EACH_DOM_ELEM_CHILD(child, head_element)
    {
      if (dom_element_tagname(child) == "descriptorBase")
        {
          DescriptorBase *descBase =
              _nclParser->getPresentationSpecificationParser ()
              ->parseDescriptorBase (child);

          if (descBase)
            {
              nclDoc->setDescriptorBase (descBase);
              break;
            }
        }
    }

  FOR_EACH_DOM_ELEM_CHILD(child, head_element)
    {
      if (dom_element_tagname(child) == "connectorBase")
        {
          ConnectorBase *connBase = _nclParser->getConnectorsParser ()
              ->parseConnectorBase (child, nclDoc);
          if (connBase)
            {
              nclDoc->setConnectorBase(connBase);
              break;
            }
        }
    }

  FOR_EACH_DOM_ELEM_CHILD(child, head_element)
    {
      if (dom_element_tagname(child) == "meta")
        {
          Meta *meta = _nclParser->getMetainformationParser ()
              ->parseMeta (child);
          if (meta)
            {
              nclDoc->addMetainformation (meta);
              break;
            }
        }
    }

  FOR_EACH_DOM_ELEM_CHILD(child, head_element)
    {
      if (dom_element_tagname(child)  == "metadata")
        {
          Metadata *metadata = _nclParser->getMetainformationParser ()
              ->parseMetadata (child);
          if (metadata)
            {
              nclDoc->addMetadata (metadata);
              break;
            }
        }
    }
}

NclDocument *
NclStructureParser::parseNcl (DOMElement *ncl_element)
{
  NclDocument* parentObject = createNcl (ncl_element);
  g_assert_nonnull (parentObject);

  DOMElement *child;
  FOR_EACH_DOM_ELEM_CHILD(child, ncl_element)
    {
      if (dom_element_tagname(child) == "head")
        {
          parseHead (child);
        }
    }

  FOR_EACH_DOM_ELEM_CHILD(child, ncl_element)
    {
      if (dom_element_tagname(child) == "body")
        {
          ContextNode *body = parseBody (child);
          if (body)
            {
              posCompileBody (child, body);
              // addBodyToNcl (parentObject, body);
              break;
            }
        }
    }

  return parentObject;
}

ContextNode *
NclStructureParser::createBody (DOMElement *body_element)
{
  // criar composicao a partir do elemento body do documento ncl
  // fazer uso do nome da composicao body que foi atribuido pelo
  // compilador
  NclDocument *document;
  ContextNode *context;

  document = getNclParser ()->getNclDocument ();
  if (!dom_element_has_attr(body_element, "id"))
    {
      body_element->setAttribute (
          XMLString::transcode ("id"),
          XMLString::transcode (document->getId ().c_str ()));

      context = (ContextNode *)
          _nclParser->getComponentsParser ()->createContext (body_element);

      body_element->removeAttribute (XMLString::transcode ("id"));
    }
  else
    {
      context = (ContextNode *)_nclParser->getComponentsParser ()
                    ->createContext (body_element);
    }

  document->setBody (context);

  return context;
}

void
NclStructureParser::solveNodeReferences (CompositeNode *composition)
{
  NodeEntity *nodeEntity;
  Entity *referredNode;
  vector<Node *> *nodes;
  bool deleteNodes = false;

  if (composition->instanceOf ("SwitchNode"))
    {
      deleteNodes = true;
      nodes = _nclParser->getPresentationControlParser ()
                ->getSwitchConstituents ((SwitchNode *)composition);
    }
  else
    {
      nodes = composition->getNodes ();
    }

  if (nodes)
    {
      return;
    }

  for (Node *node : *nodes)
  {
    if (node != NULL)
      {
        if (node->instanceOf ("ReferNode"))
          {
            referredNode = ((ReferNode *)node)->getReferredEntity ();
            if (referredNode != NULL)
              {
                if (referredNode->instanceOf ("ReferredNode"))
                  {
                    nodeEntity = (NodeEntity *)(getNclParser ()->getNode (
                                                    referredNode->getId ()));

                    if (nodeEntity)
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

  return _nclParser->getComponentsParser ()
          ->posCompileContext (parentElement, body);
}

NclDocument*
NclStructureParser::createNcl (DOMElement *parentElement)
{
  string docName;
  NclDocument *document;

  if (dom_element_has_attr(parentElement, "id"))
    {
      docName = dom_element_get_attr(parentElement, "id");
    }

  if (docName == "")
    docName = "ncl";

  document = new NclDocument (docName, getNclParser()->getPath ());
  g_assert_nonnull (document);
  getNclParser()->setNclDocument (document);
  g_assert_nonnull (getNclParser()->getNclDocument());
  return document;
}

GINGA_NCLCONV_END
