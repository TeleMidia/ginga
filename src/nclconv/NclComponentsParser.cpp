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

#include "NclDocumentConverter.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_NCLCONV_BEGIN

NclComponentsParser::NclComponentsParser (NclDocumentParser *parser)
    : ModuleParser (parser)
{
}

void *
NclComponentsParser::parseMedia (DOMElement *parentElement,
                                 void *objGrandParent)
{
  void *parentObject;
  DOMNodeList *elementNodeList;
  int i, size;
  DOMNode *node;
  DOMElement *element;
  string elementTagName;
  void *elementObject = NULL;

  parentObject = createMedia (parentElement, objGrandParent);
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

          if (XMLString::compareIString (elementTagName.c_str (), "area")
              == 0)
            {
              elementObject = getInterfacesParser ()->parseArea (
                  element, parentObject);

              if (elementObject != NULL)
                {
                  addAreaToMedia (parentObject, elementObject);
                }
            }
          else if (XMLString::compareIString (elementTagName.c_str (),
                                              "property")
                   == 0)
            {
              elementObject = getInterfacesParser ()->parseProperty (
                  element, parentObject);

              if (elementObject != NULL)
                {
                  addPropertyToMedia (parentObject, elementObject);
                }
            }
        }
    }

  return parentObject;
}

void *
NclComponentsParser::parseContext (DOMElement *parentElement,
                                   void *objGrandParent)
{
  void *parentObject;
  DOMNodeList *elementNodeList;
  int i, size;
  DOMNode *node;
  DOMElement *element;
  string elementTagName;
  void *elementObject = NULL;

  parentObject = createContext (parentElement, objGrandParent);
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
              elementObject = parseMedia (element, parentObject);
              if (elementObject != NULL)
                {
                  addMediaToContext (parentObject, elementObject);
                }
            }
          else if (XMLString::compareIString (elementTagName.c_str (),
                                              "context")
                   == 0)
            {
              elementObject = parseContext (element, parentObject);
              if (elementObject != NULL)
                {
                  addContextToContext (parentObject, elementObject);
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
                  addSwitchToContext (parentObject, elementObject);
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
              addPropertyToContext (parentObject, elementObject);
            }
        }
    }

  return parentObject;
}

void *
NclComponentsParser::posCompileContext2 (DOMElement *parentElement,
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
                                        XMLString::transcode ("link"))
                 == 0)
        {
          elementObject = getLinkingParser ()->parseLink (
              (DOMElement *)node, parentObject);

          if (elementObject != NULL)
            {
              addLinkToContext (parentObject, elementObject);
            }
        }
    }

  for (i = 0; i < size; i++)
    {
      node = elementNodeList->item (i);
      if (node->getNodeType () == DOMNode::ELEMENT_NODE
          && XMLString::compareIString (((DOMElement *)node)->getTagName (),
                                        XMLString::transcode ("port"))
                 == 0)
        {
          elementObject = getInterfacesParser ()->parsePort (
              (DOMElement *)node, parentObject);

          if (elementObject != NULL)
            {
              addPortToContext (parentObject, elementObject);
            }
        }
    }

  return parentObject;
}

NclPresentationSpecificationParser *
NclComponentsParser::getPresentationSpecificationParser ()
{
  return presentationSpecificationParser;
}

void
NclComponentsParser::setPresentationSpecificationParser (
    NclPresentationSpecificationParser *presentationSpecificationParser)
{
  this->presentationSpecificationParser = presentationSpecificationParser;
}

NclLinkingParser *
NclComponentsParser::getLinkingParser ()
{
  return linkingParser;
}

void
NclComponentsParser::setLinkingParser (NclLinkingParser *linkingParser)
{
  this->linkingParser = linkingParser;
}

NclInterfacesParser *
NclComponentsParser::getInterfacesParser ()
{
  return interfacesParser;
}

void
NclComponentsParser::setInterfacesParser (
    NclInterfacesParser *interfacesParser)
{
  this->interfacesParser = interfacesParser;
}

NclPresentationControlParser *
NclComponentsParser::getPresentationControlParser ()
{
  return presentationControlParser;
}

void
NclComponentsParser::setPresentationControlParser (
    NclPresentationControlParser *presentationControlParser)
{
  this->presentationControlParser = presentationControlParser;
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
NclComponentsParser::addAreaToMedia (void *parentObject, void *childObject)
{
  addAnchorToMedia ((ContentNode *)parentObject, (Anchor *)childObject);
}

void
NclComponentsParser::addPropertyToMedia (void *parentObject, void *childObject)
{
  addAnchorToMedia ((ContentNode *)parentObject, (Anchor *)childObject);
}

void *
NclComponentsParser::createContext (DOMElement *parentElement,
                                    arg_unused (void *objGrandParent))
{
  NclDocument *document;
  string id, attValue;
  Node *node;
  Entity *referNode;
  ContextNode *context;
  GenericDescriptor *descriptor;

  if (unlikely (!parentElement->hasAttribute (XMLString::transcode ("id"))))
    syntax_error ("context: missing id");

  id = XMLString::transcode (
      parentElement->getAttribute (XMLString::transcode ("id")));

  node = ((NclDocumentConverter *)getDocumentParser ())->getNode (id);
  if (unlikely (node != NULL))
    syntax_error ("context '%s': duplicated id", id.c_str ());

  if (parentElement->hasAttribute (XMLString::transcode ("refer")))
    {
      attValue = XMLString::transcode (
          parentElement->getAttribute (XMLString::transcode ("refer")));

      try
        {
          referNode = (ContextNode *)((NclDocumentConverter *)
                                          getDocumentParser ())
                          ->getNode (attValue);

          if (referNode == NULL)
            {
              document = getDocumentParser ()->getNclDocument ();
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

  if (parentElement->hasAttribute (XMLString::transcode ("descriptor")))
    {
      // adicionar um descritor a um objeto de midia
      attValue = XMLString::transcode (parentElement->getAttribute (
          XMLString::transcode ("descriptor")));

      document = getDocumentParser ()->getNclDocument ();
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
  string elementTagName;
  void *elementObject;

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
              if (parentObject != NULL)
                {
                  elementObject
                      = ((ContextNode *)parentObject)
                            ->getNode (XMLString::transcode (
                                element->getAttribute (
                                    XMLString::transcode ("id"))));

                  try
                    {
                      if (((NodeEntity *)elementObject)
                              ->instanceOf ("ContextNode"))
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
          else if (XMLString::compareIString (elementTagName.c_str (),
                                              "switch")
                   == 0)
            {
              elementObject = ((NclDocumentConverter *)getDocumentParser ())
                                  ->getNode (XMLString::transcode (
                                      element->getAttribute (
                                          XMLString::transcode ("id"))));

              if (unlikely (elementObject == NULL))
                {
                  syntax_error ("bad switch '%s'",
                                string (XMLString::transcode
                                        (element->getAttribute
                                         (XMLString::transcode
                                          ("id")))).c_str ());

                }
              else if (((NodeEntity *)elementObject)
                           ->instanceOf ("SwitchNode"))
                {
                  getPresentationControlParser ()->posCompileSwitch (
                      element, elementObject);
                }
            }
        }
    }

  return posCompileContext2 (parentElement, parentObject);
}

void *
NclComponentsParser::createMedia (DOMElement *parentElement,
                                  arg_unused (void *objGrandParent))
{
  string attValue, id;
  NclDocument *document;
  Node *node;
  Entity *referNode;
  GenericDescriptor *descriptor;

  if (unlikely (!parentElement->hasAttribute (XMLString::transcode ("id"))))
    syntax_error ("media: missing id");

  id = XMLString::transcode (
      parentElement->getAttribute (XMLString::transcode ("id")));

  node = ((NclDocumentConverter *)getDocumentParser ())->getNode (id);
  if (unlikely (node != NULL))
    syntax_error ("media '%s': duplicated id", id.c_str ());

  if (parentElement->hasAttribute (XMLString::transcode ("refer")))
    {
      attValue = XMLString::transcode (
          parentElement->getAttribute (XMLString::transcode ("refer")));

      try
        {
          referNode = (ContentNode *)((NclDocumentConverter *)
                                          getDocumentParser ())
                          ->getNode (attValue);

          if (referNode == NULL)
            {
              document = getDocumentParser ()->getNclDocument ();
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
      if (parentElement->hasAttribute (XMLString::transcode ("instance")))
        {
          attValue = XMLString::transcode (parentElement->getAttribute (
              XMLString::transcode ("instance")));

          ((ReferNode *)node)->setInstanceType (attValue);
        }
      ((ReferNode *)node)->setReferredEntity (referNode);

      return node;
    }

  node = new ContentNode (id, NULL, "");

  if (parentElement->hasAttribute (XMLString::transcode ("type")))
    {
      ((ContentNode *)node)
          ->setNodeType (XMLString::transcode (
              parentElement->getAttribute (XMLString::transcode ("type"))));
    }

  if (parentElement->hasAttribute (XMLString::transcode ("src")))
    {
      string src = XMLString::transcode (
          parentElement->getAttribute (XMLString::transcode ("src")));

      if (unlikely (src == ""))
        syntax_error ("media '%s': missing src", id.c_str ());

      if (!xpathisuri (src) && !xpathisabs (src))
        src = xpathbuildabs (getDocumentParser ()->getDirName (), src);

      ((ContentNode *)node)->setContent
        (new AbsoluteReferenceContent (src));
    }

  if (parentElement->hasAttribute (XMLString::transcode ("descriptor")))
    {
      attValue = XMLString::transcode (parentElement->getAttribute (
          XMLString::transcode ("descriptor")));

      document = getDocumentParser ()->getNclDocument ();
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
