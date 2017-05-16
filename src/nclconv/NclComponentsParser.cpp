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
  Node *media;
  DOMNodeList *elementNodeList;
  int i, size;
  DOMNode *node;
  DOMElement *element;

  media = createMedia (parentElement);
  g_assert_nonnull (media);

  elementNodeList = parentElement->getChildNodes ();
  size = (int) elementNodeList->getLength ();

  for (i = 0; i < size; i++)
    {
      node = elementNodeList->item (i);
      if (node->getNodeType () == DOMNode::ELEMENT_NODE)
        {
          element = (DOMElement *)node;

          string tagname = _nclParser->getTagname(element);

          if (XMLString::compareIString (tagname.c_str(), "area") == 0)
            {
              Anchor *area = _nclParser->getInterfacesParser ()
                      ->parseArea (element, media);

              if (area)
                {
                  addAreaToMedia ((ContentNode*)media, area);
                }
            }
          else if (XMLString::compareIString (tagname.c_str(), "property")
                   == 0)
            {
              PropertyAnchor *prop = _nclParser->getInterfacesParser ()
                      ->parseProperty (element, media);

              if (prop)
                {
                  addPropertyToMedia ((ContentNode *)media, prop);
                }
            }
        }
    }

  return media;
}

Node *
NclComponentsParser::parseContext (DOMElement *parentElement)
{
  Node *context;
  DOMNodeList *elementNodeList;
  int i, size;
  DOMNode *node;
  DOMElement *element;

  context = createContext (parentElement);
  g_assert_nonnull (context);

  elementNodeList = parentElement->getChildNodes ();
  size = (int) elementNodeList->getLength ();

  for (i = 0; i < size; i++)
    {
      node = elementNodeList->item (i);
      if (node->getNodeType () == DOMNode::ELEMENT_NODE)
        {
          element = (DOMElement *) node;
          string tagname = _nclParser->getTagname(element);
          if (XMLString::compareIString (tagname.c_str(), "media") == 0)
            {
              Node *media = parseMedia (element);
              if (media)
                {
                  addMediaToContext (context, media);
                }
            }
          else if (XMLString::compareIString (tagname.c_str(), "context") == 0)
            {
              Node *child_context = parseContext (element);
              if (child_context)
                {
                  addContextToContext (context, child_context);
                }
            }
          else if (XMLString::compareIString (tagname.c_str(), "switch") == 0)
            {
              Node *switch_node =
                      _nclParser->getPresentationControlParser ()
                        ->parseSwitch (element);

              if (switch_node)
                {
                  addSwitchToContext (context, switch_node);
                }
            }
        }
    }

  for (i = 0; i < size; i++)
    {
      node = elementNodeList->item (i);
      if (node->getNodeType () == DOMNode::ELEMENT_NODE)
      {
        string tagname = _nclParser->getTagname((DOMElement *)node);
        if(XMLString::compareIString (tagname.c_str(), "property") == 0)
        {
          PropertyAnchor *prop = _nclParser->getInterfacesParser ()
                  ->parseProperty ((DOMElement *)node, context);

          if (prop)
            {
              addPropertyToContext (context, prop);
            }
        }
      }
    }

  return context;
}

void *
NclComponentsParser::posCompileContext2 (DOMElement *parentElement,
                                         void *parentObject)
{
  int i, size;
  DOMNode *node;
  void *elementObject;

  DOMNodeList *elementNodeList = parentElement->getChildNodes ();
  size = (int) elementNodeList->getLength ();

  for (i = 0; i < size; i++)
    {
      node = elementNodeList->item (i);
      if (node->getNodeType () == DOMNode::ELEMENT_NODE)
        {
          string tagname = _nclParser->getTagname((DOMElement *)node);
          if (XMLString::compareIString (tagname.c_str(), "link") == 0)
            {
              elementObject = _nclParser->getLinkingParser ()
                    ->parseLink ((DOMElement *)node, parentObject);

              if (elementObject != NULL)
                {
                  addLinkToContext (parentObject, elementObject);
                }
            }
        }
    }

  for (i = 0; i < size; i++)
    {
      node = elementNodeList->item (i);
      if (node->getNodeType () == DOMNode::ELEMENT_NODE)
        {
          string tagname = _nclParser->getTagname((DOMElement *)node);
          if (XMLString::compareIString (tagname.c_str(), "port") == 0)
            {
              elementObject = _nclParser->getInterfacesParser ()
                      ->parsePort ((DOMElement *)node, parentObject);

              if (elementObject != NULL)
                {
                  addPortToContext (parentObject, elementObject);
                }
            }
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

  if (unlikely (!_nclParser->hasAttribute(parentElement, "id")))
    syntax_error ("context: missing id");

  id = _nclParser->getAttribute(parentElement, "id");

  node = getNclParser ()->getNode (id);
  if (unlikely (node != NULL))
    syntax_error ("context '%s': duplicated id", id.c_str ());

  if (_nclParser->hasAttribute (parentElement, "refer"))
    {
      attValue = _nclParser->getAttribute(parentElement, "refer");
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

  if (_nclParser->hasAttribute (parentElement, "descriptor"))
    {
      // adicionar um descritor a um objeto de midia
      attValue = _nclParser->getAttribute(parentElement, "descriptor");

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
          string tagname = _nclParser->getTagname(element);
          if (XMLString::compareIString (tagname.c_str(), "context") == 0)
            {
              if (parentObject != NULL)
                {
                  string id = _nclParser->getAttribute(element, "id");
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
              string id = _nclParser->getAttribute(element, "id");
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

  if (unlikely (!_nclParser->hasAttribute(parentElement, "id")))
    syntax_error ("media: missing id");

  id = _nclParser->getAttribute(parentElement, "id");

  node = getNclParser ()->getNode (id);
  if (unlikely (node != NULL))
    syntax_error ("media '%s': duplicated id", id.c_str ());

  if (_nclParser->hasAttribute(parentElement, "refer"))
    {
      attValue = _nclParser->getAttribute(parentElement, "refer");

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
      if (_nclParser->hasAttribute(parentElement, "instance"))
        {
          attValue = _nclParser->getAttribute(parentElement, "instance");

          ((ReferNode *)node)->setInstanceType (attValue);
        }
      ((ReferNode *)node)->setReferredEntity (referNode);

      return node;
    }

  node = new ContentNode (id, NULL, "");

  if (_nclParser->hasAttribute(parentElement, "type"))
    {
      string type = _nclParser->getAttribute(parentElement, "type");
      ((ContentNode *)node)->setNodeType (type);
    }

  if (_nclParser->hasAttribute(parentElement, "src"))
    {
      string src = _nclParser->getAttribute(parentElement, "src");

      if (unlikely (src == ""))
        syntax_error ("media '%s': missing src", id.c_str ());

      if (!xpathisuri (src) && !xpathisabs (src))
        src = xpathbuildabs (getNclParser ()->getDirName (), src);

      ((ContentNode *)node)->setContent (new AbsoluteReferenceContent (src));
    }

  if (_nclParser->hasAttribute(parentElement, "descriptor"))
    {
      attValue = _nclParser->getAttribute(parentElement, "descriptor");

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
