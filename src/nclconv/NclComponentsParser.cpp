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

  for (DOMElement *element:
       dom_element_children(parentElement))
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
    }


  for (DOMElement *element:
       dom_element_children_by_tagname(parentElement, "property"))
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
NclComponentsParser::posCompileContext2 (DOMElement *parentElement,
                                         void *parentObject)
{
  for(DOMElement *element:
      dom_element_children_by_tagname(parentElement, "link"))
    {
      Link *link = _nclParser->getLinkingParser ()->parseLink (element,
                                                               parentObject);
      if (link)
        {
          addLinkToContext (parentObject, link);
        }
    }

  for(DOMElement *element:
      dom_element_children_by_tagname(parentElement, "port"))
    {
      Port *port = _nclParser->getInterfacesParser () ->parsePort (element,
                                                                  parentObject);

      if (port)
        {
          addPortToContext (parentObject, port);
        }
    }
  return parentObject;
}

void
NclComponentsParser::addPortToContext (void *parentObject, void *childObject)
{
  if (((Entity *)parentObject)->instanceOf ("ContextNode"))
    {
      ((ContextNode *)parentObject)->addPort ((Port *)childObject);
    }
}

void
NclComponentsParser::addPropertyToContext (void *parentObject,
                                           void *childObject)
{
  if (((Entity *)parentObject)->instanceOf ("ContextNode"))
    {
      ((ContextNode *)parentObject)->addAnchor ((Anchor *)childObject);
    }
  else if (((Entity *)parentObject)->instanceOf ("ReferNode"))
    {
      ((ReferNode *)parentObject)->addAnchor ((Anchor *)childObject);
    }
}

void
NclComponentsParser::addContextToContext (void *parentObject, void *childObject)
{
  if (((Entity *)parentObject)->instanceOf ("ContextNode"))
    {
      // adicionar composicao aa composicao
      addNodeToContext ((ContextNode *)parentObject,
                        (NodeEntity *)childObject);
    }
}

void
NclComponentsParser::addSwitchToContext (void *parentObject, void *childObject)
{
  if (((Entity *)parentObject)->instanceOf ("ContextNode"))
    {
      // adicionar switch aa composicao
      addNodeToContext ((ContextNode *)parentObject,
                        (NodeEntity *)childObject);
    }
}

void
NclComponentsParser::addMediaToContext (void *parentObject, void *childObject)
{
  if (((Entity *)parentObject)->instanceOf ("ContextNode"))
    {
      // adicionar media aa composicao
      addNodeToContext ((ContextNode *)parentObject,
                        (NodeEntity *)childObject);
    }
}

void
NclComponentsParser::addLinkToContext (void *parentObject, void *childObject)
{
  int min;
  int max;
  Role *role;
  vector<Role *>::iterator i;

  if (((Entity *)parentObject)->instanceOf ("ContextNode"))
    {
      vector<Role *> *roles;

      roles = ((Link *)childObject)->getConnector ()->getRoles ();
      if (roles != NULL)
        {
          i = roles->begin ();
          while (i != roles->end ())
            {
              role = *i;
              // if (role->instanteOf("CardinalityRole") {
              min = role->getMinCon ();
              max = role->getMaxCon ();

              /*} else {
                      min = 1;
                      max = 1;
              }*/

              if (((Link *)childObject)->getNumRoleBinds (role)
                  < (unsigned int)min)
                {
                  syntax_error ("link: too few binds for role '%s': %d",
                                role->getLabel ().c_str (), min);
                }
              else if (max > 0
                       && ((Link *)childObject)->getNumRoleBinds (role)
                              > (unsigned int)max)
                {
                  syntax_error ("link: too many binds for role '%s': %d",
                                role->getLabel ().c_str (), max);
                  return;
                }
              ++i;
            }
          delete roles;
        }

      ((ContextNode *)parentObject)->addLink ((Link *)childObject);
    }
}

void
NclComponentsParser::addNodeToContext (ContextNode *contextNode,
                                       NodeEntity *node)
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
                                        void *parentObject)
{
  DOMNodeList *elementNodeList;
  int i, size;
  DOMNode *node;
  DOMElement *element;
  void *elementObject;

  elementNodeList = parentElement->getChildNodes ();
  size = (int) elementNodeList->getLength ();

  for (i = 0; i < size; i++)
    {
      node = elementNodeList->item (i);
      if (node->getNodeType () == DOMNode::ELEMENT_NODE)
        {
          element = (DOMElement *)node;
          string tagname = dom_element_tagname(element);
          if (XMLString::compareIString (tagname.c_str(), "context") == 0)
            {
              if (parentObject != NULL)
                {
                  string id = dom_element_get_attr(element, "id");
                  elementObject
                      = ((ContextNode *)parentObject)->getNode (id);

                  try
                    {
                      if (((NodeEntity *)elementObject)->instanceOf ("ContextNode"))
                        {
                          posCompileContext (element, elementObject);
                        }
                    }
                  catch (...)
                    {
                      // treating error with the <NodeEntity*> casting
                    }
                }
            }
          else if (XMLString::compareIString (tagname.c_str(), "switch") == 0)
            {
              string id = dom_element_get_attr(element, "id");
              elementObject = getNclParser ()->getNode (id);

              if (unlikely (elementObject == NULL))
                {
                  syntax_error ("bad switch '%s'", id.c_str ());

                }
              else if (((NodeEntity *)elementObject)
                           ->instanceOf ("SwitchNode"))
                {
                  _nclParser->getPresentationControlParser ()
                          ->posCompileSwitch (element, elementObject);
                }
            }
        }
    }

  return posCompileContext2 (parentElement, parentObject);
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
