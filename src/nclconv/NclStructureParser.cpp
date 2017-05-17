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

  for (DOMElement *child : dom_element_children(body_element) )
    {
      string tagname = dom_element_tagname(child);
      if ( tagname == "media")
        {
          Node *media = _nclParser->getComponentsParser()->parseMedia (child);
          if (media)
            {
              // add media to body
              _nclParser->getComponentsParser ()
                  ->addMediaToContext (body, media);
            }
        }
      else if (tagname == "context")
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
      else if (tagname == "switch")
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
      else
        {
           // syntax_warning ?
        }
    }

  for (DOMElement *child :
       dom_element_children_by_tagname(body_element, "property") )
    {
      PropertyAnchor *prop = _nclParser->getInterfacesParser ()
          ->parseProperty (child, body);
      if (prop)
        {
          // add property to body
          _nclParser->getComponentsParser ()->addPropertyToContext (body, prop);
        }
    }

  return body;
}

void
NclStructureParser::parseHead (DOMElement *head_element)
{
  NclDocument *nclDoc = getNclParser()->getNclDocument();
  g_assert_nonnull (nclDoc);

  for (DOMElement *child :
       dom_element_children_by_tagname(head_element, "importedDocumentBase") )
    {
      _nclParser->getImportParser ()->parseImportedDocumentBase (child);
    }

  for (DOMElement *child :
       dom_element_children_by_tagname(head_element, "regionBase") )
    {
      RegionBase *regionBase = _nclParser->getLayoutParser ()
          ->parseRegionBase (child);
      if (regionBase)
        {
          nclDoc->addRegionBase(regionBase);
        }
    }

  for (DOMElement *child :
       dom_element_children_by_tagname(head_element, "ruleBase") )
    {
      RuleBase *ruleBase = _nclParser->getPresentationControlParser ()
          ->parseRuleBase (child);
      if (ruleBase)
        {
          nclDoc->setRuleBase (ruleBase);
          break;
        }
    }

  for (DOMElement *child :
       dom_element_children_by_tagname(head_element, "transitionBase") )
    {
      TransitionBase *transBase = _nclParser->getTransitionParser ()
          ->parseTransitionBase (child);
      if (transBase)
        {
          nclDoc->setTransitionBase (transBase);
          break;
        }
    }

  for (DOMElement *child :
       dom_element_children_by_tagname(head_element, "descriptorBase") )
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

  for (DOMElement *child :
       dom_element_children_by_tagname(head_element, "connectorBase") )
    {
      ConnectorBase *connBase = _nclParser->getConnectorsParser ()
          ->parseConnectorBase (child);
      if (connBase)
        {
          nclDoc->setConnectorBase(connBase);
          break;
        }
    }

  for (DOMElement *child :
       dom_element_children_by_tagname(head_element, "meta") )
    {
      Meta *meta = _nclParser->getMetainformationParser ()->parseMeta (child);
      if (meta)
        {
          nclDoc->addMetainformation (meta);
          break;
        }
    }

  for (DOMElement *child :
       dom_element_children_by_tagname(head_element, "metadata") )
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

NclDocument *
NclStructureParser::parseNcl (DOMElement *ncl_element)
{
  NclDocument* parentObject = createNcl (ncl_element);
  g_assert_nonnull (parentObject);

  for (DOMElement *child :
       dom_element_children_by_tagname(ncl_element, "head") )
    {
      parseHead (child);
    }

  for (DOMElement *child :
       dom_element_children_by_tagname(ncl_element, "body") )
    {
      ContextNode *body = parseBody (child);
      if (body)
        {
          posCompileBody (child, body);
          // addBodyToNcl (parentObject, body);
          break;
        }
    }

  // syntax_warn/err:
  // what if there are other children (different from <head> and <body>)

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
      XMLCh *attr_name = XMLString::transcode ("id");
      XMLCh *attr_value = XMLString::transcode(document->getId ().c_str ());

      body_element->setAttribute (attr_name, attr_value);

      context = (ContextNode *)
          _nclParser->getComponentsParser ()->createContext (body_element);

      body_element->removeAttribute (attr_name);

      XMLString::release(&attr_name);
      XMLString::release(&attr_value);
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

  docName = dom_element_get_attr (parentElement, "id");

  if (docName == "")
    docName = "ncl";

  document = new NclDocument (docName, getNclParser()->getPath ());
  g_assert_nonnull (document);

  getNclParser()->setNclDocument (document);

  return document;
}

GINGA_NCLCONV_END
