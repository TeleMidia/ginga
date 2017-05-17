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
#include "NclComponentsParser.h"

#include "NclParser.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_NCLCONV_BEGIN

NclComponentsParser::NclComponentsParser (NclParser *nclParser)
    : ModuleParser (nclParser)
{
}

Node *
NclComponentsParser::parseMedia (DOMElement *parentElement)
{
  Node *media = createMedia (parentElement);
  g_assert_nonnull (media);

  for (DOMElement *element:
       dom_element_children(parentElement))
    {
      if (dom_element_tagname(element) == "area")
        {
          Anchor *area = _nclParser->getInterfacesParser ()
              ->parseArea (element, media);
          if (area)
            {
              addAreaToMedia ((ContentNode*)media, area);
            }
        }
      else if (dom_element_tagname(element) == "property")
        {
          PropertyAnchor *prop = _nclParser->getInterfacesParser ()
              ->parseProperty (element, media);
          if (prop)
            {
              addPropertyToMedia ((ContentNode *)media, prop);
            }
        }
    }

  return media;
}

Node *
NclComponentsParser::parseContext (DOMElement *parentElement)
{
  Node *context = createContext (parentElement);
  g_assert_nonnull (context);

  for (DOMElement *element: dom_element_children (parentElement))
    {
      string tagname = dom_element_tagname(element);
      if (tagname == "media")
        {
          Node *media = parseMedia (element);
          if (media)
            {
              addMediaToContext (context, media);
            }
        }
      else if (tagname == "context")
        {
          Node *child_context = parseContext (element);
          if (child_context)
            {
              addContextToContext (context, child_context);
            }
        }
      else if (tagname == "switch")
        {
          Node *switch_node = _nclParser->getPresentationControlParser ()
                        ->parseSwitch (element);
          if (switch_node)
            {
              addSwitchToContext (context, switch_node);
            }
        }
      else
        {
          // syntax warning ?
        }
    }


  for (DOMElement *element:
       dom_element_children_by_tagname (parentElement, "property"))
    {
      PropertyAnchor *prop = _nclParser->getInterfacesParser ()
          ->parseProperty (element, context);

      if (prop)
        {
          addPropertyToContext (context, prop);
        }
    }

  return context;
}

void *
NclComponentsParser::posCompileContext2 (DOMElement *context_element,
                                         ContextNode *parentObject)
{
  for(DOMElement *child:
      dom_element_children_by_tagname (context_element, "link"))
    {
      Link *link = _nclParser->getLinkingParser ()->parseLink (child,
                                                               parentObject);
      if (link)
        {
          addLinkToContext (parentObject, link);
        }
    }

  for(DOMElement *child:
      dom_element_children_by_tagname (context_element, "port"))
    {
      Port *port = _nclParser->getInterfacesParser () ->parsePort (child,
                                                                  parentObject);
      if (port)
        {
          addPortToContext (parentObject, port);
        }
    }
  return parentObject;
}

void
NclComponentsParser::addPortToContext (Entity *context, Port *port)
{
  if (context->instanceOf ("ContextNode"))
    {
      ((ContextNode*)context)->addPort (port);
    }
}

void
NclComponentsParser::addPropertyToContext (Entity *context,
                                           Anchor *property)
{
  if (context->instanceOf ("ContextNode"))
    {
      ((ContextNode *)context)->addAnchor (property);
    }
  else if (context->instanceOf ("ReferNode"))
    {
      ((ReferNode *)context)->addAnchor (property);
    }
}

void
NclComponentsParser::addContextToContext (Entity *context, Node *child_context)
{
  if (context->instanceOf ("ContextNode"))
    {
      // adicionar composicao aa composicao
      addNodeToContext ((ContextNode *)context, child_context);
    }
}

void
NclComponentsParser::addSwitchToContext (Entity *context, Node *switchNode)
{
  if (context->instanceOf ("ContextNode"))
    {
      // adicionar switch aa composicao
      addNodeToContext ((ContextNode *)context, switchNode);
    }
}

void
NclComponentsParser::addMediaToContext (Entity *context, Node *media)
{
  if (context->instanceOf ("ContextNode"))
    {
      // adicionar media aa composicao
      addNodeToContext ((ContextNode *)context, media);
    }
}

void
NclComponentsParser::addLinkToContext (ContextNode *context, Link *link)
{
  if (context->instanceOf ("ContextNode")) //There is no other possibility!!
    {
      vector<Role *> *roles = link->getConnector ()->getRoles ();
      g_assert_nonnull(roles);

      for (Role *role: *roles)
        {
          unsigned int min = role->getMinCon ();
          unsigned int max = role->getMaxCon ();

          if (link->getNumRoleBinds (role) < min)
            {
              syntax_error ("link: too few binds for role '%s': %d",
                            role->getLabel ().c_str (), min);
            }
          else if (max > 0 && (link->getNumRoleBinds (role) > max))
            {
              syntax_error ("link: too many binds for role '%s': %d",
                            role->getLabel ().c_str (), max);
              return;
            }
        }
      delete roles;

      context->addLink (link);
    }
}

void
NclComponentsParser::addNodeToContext (ContextNode *contextNode,
                                       Node *node)
{
  // adicionar um noh aa composicao
  contextNode->addNode (node);
}

void
NclComponentsParser::addAnchorToMedia (ContentNode *contentNode, Anchor *anchor)
{
  if (unlikely (contentNode->getAnchor (anchor->getId ()) != NULL))
    {
      syntax_error ("media '%s': duplicated area '%s'",
                    contentNode->getId ().c_str (),
                    anchor->getId ().c_str ());
    }
  contentNode->addAnchor (anchor);
}

void
NclComponentsParser::addAreaToMedia (ContentNode *media, Anchor *area)
{
  addAnchorToMedia (media, area);
}

void
NclComponentsParser::addPropertyToMedia (ContentNode *media, Anchor *property)
{
  addAnchorToMedia (media, property);
}

Node *
NclComponentsParser::createContext (DOMElement *parentElement)
{
  NclDocument *document;
  string id, attValue;
  Node *node;
  Entity *referNode;
  ContextNode *context;
  GenericDescriptor *descriptor;

  if (unlikely (!dom_element_has_attr(parentElement, "id")))
    syntax_error ("context: missing id");

  id = dom_element_get_attr(parentElement, "id");

  node = getNclParser ()->getNode (id);
  if (unlikely (node != NULL))
    syntax_error ("context '%s': duplicated id", id.c_str ());

  if (dom_element_has_attr (parentElement, "refer"))
    {
      attValue = dom_element_get_attr(parentElement, "refer");
      try
        {
          referNode = (ContextNode *)getNclParser ()->getNode (attValue);

          if (referNode == NULL)
            {
              document = getNclParser ()->getNclDocument ();
              referNode = (ContextNode *)(document->getNode (attValue));
              if (referNode == NULL)
                {
                  referNode = (Entity *)(new ReferredNode (
                      attValue, (void *)parentElement));
                }
            }
        }
      catch (...)
        {
          syntax_error ("context '%s': bad refer '%s'", id.c_str (),
                        attValue.c_str ());
        }

      node = new ReferNode (id);
      ((ReferNode *)node)->setReferredEntity (referNode);

      return node;
    }

  context = new ContextNode (id);

  if (dom_element_has_attr (parentElement, "descriptor"))
    {
      // adicionar um descritor a um objeto de midia
      attValue = dom_element_get_attr(parentElement, "descriptor");

      document = getNclParser ()->getNclDocument ();
      descriptor = document->getDescriptor (attValue);
      if (descriptor != NULL)
        {
          context->setDescriptor (descriptor);
        }
      else
        {
          syntax_error ("context '%s': bad descriptor '%s'", id.c_str (),
                        attValue.c_str ());
        }
    }

  return context;
}

void *
NclComponentsParser::posCompileContext (DOMElement *parentElement,
                                        ContextNode *context)
{
  for(DOMElement *element: dom_element_children(parentElement))
    {
      string tagname = dom_element_tagname(element);
      if (tagname == "context")
        {
          if (context != NULL)
            {
              string id = dom_element_get_attr(element, "id");
              Node *node = context->getNode (id);

              if (unlikely (node == NULL))
                {
                  syntax_error ("bad context '%s'", id.c_str ());
                }
              else if (node->instanceOf ("ContextNode"))
                {
                  posCompileContext (element, (ContextNode*)node);
                }
            }
        }
      else if (tagname == "switch")
        {
          string id = dom_element_get_attr(element, "id");
          Node *node = getNclParser ()->getNode (id);
          if (unlikely (node == NULL))
            {
              syntax_error ("bad switch '%s'", id.c_str ());
            }
          else if (node->instanceOf ("SwitchNode"))
            {
              _nclParser->getPresentationControlParser()->
                  posCompileSwitch (element, (SwitchNode*)node);
            }
        }
    }

  return posCompileContext2 (parentElement, context);
}

Node *
NclComponentsParser::createMedia (DOMElement *parentElement)
{
  string attValue, id;
  NclDocument *document;
  Node *node;
  Entity *referNode;
  GenericDescriptor *descriptor;

  if (unlikely (!dom_element_has_attr(parentElement, "id")))
    syntax_error ("media: missing id");

  id = dom_element_get_attr(parentElement, "id");

  node = getNclParser ()->getNode (id);
  if (unlikely (node != NULL))
    syntax_error ("media '%s': duplicated id", id.c_str ());

  if (dom_element_has_attr(parentElement, "refer"))
    {
      attValue = dom_element_get_attr(parentElement, "refer");

      try
        {
          referNode = (ContentNode *) getNclParser ()->getNode (attValue);

          if (referNode == NULL)
            {
              document = getNclParser ()->getNclDocument ();
              referNode = (ContentNode *)document->getNode (attValue);
              if (referNode == NULL)
                {
                  referNode
                      = new ReferredNode (attValue, (void *)parentElement);
                }
            }
        }
      catch (...)
        {
          syntax_error ("media '%s': bad refer '%s'",
                        id.c_str (), attValue.c_str ());
        }

      node = new ReferNode (id);
      if (dom_element_has_attr(parentElement, "instance"))
        {
          attValue = dom_element_get_attr(parentElement, "instance");

          ((ReferNode *)node)->setInstanceType (attValue);
        }
      ((ReferNode *)node)->setReferredEntity (referNode);

      return node;
    }

  node = new ContentNode (id, NULL, "");

  if (dom_element_has_attr(parentElement, "type"))
    {
      string type = dom_element_get_attr(parentElement, "type");
      ((ContentNode *)node)->setNodeType (type);
    }

  if (dom_element_has_attr(parentElement, "src"))
    {
      string src = dom_element_get_attr(parentElement, "src");

      if (unlikely (src == ""))
        syntax_error ("media '%s': missing src", id.c_str ());

      if (!xpathisuri (src) && !xpathisabs (src))
        src = xpathbuildabs (getNclParser ()->getDirName (), src);

      ((ContentNode *)node)->setContent (new AbsoluteReferenceContent (src));
    }

  if (dom_element_has_attr(parentElement, "descriptor"))
    {
      attValue = dom_element_get_attr(parentElement, "descriptor");

      document = getNclParser ()->getNclDocument ();
      descriptor = document->getDescriptor (attValue);
      if (descriptor != NULL)
        {
          ((ContentNode *)node)->setDescriptor (descriptor);
        }
      else
        {
          syntax_error ("media '%s': bad descriptor '%s'",
                        id.c_str (), attValue.c_str ());
        }
    }
  return node;
}


GINGA_NCLCONV_END
