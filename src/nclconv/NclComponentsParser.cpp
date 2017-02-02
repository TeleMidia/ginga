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

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_NCLCONV_BEGIN

NclComponentsParser::NclComponentsParser (DocumentParser *documentParser)
    : ModuleParser (documentParser)
{
}

void *
NclComponentsParser::parseMedia (DOMElement *parentElement,
                                 void *objGrandParent)
{
  clog << "parseMedia" << endl;
  void *parentObject;
  DOMNodeList *elementNodeList;
  int i, size;
  DOMNode *node;
  DOMElement *element;
  string elementTagName;
  void *elementObject = NULL;

  parentObject = createMedia (parentElement, objGrandParent);
  if (parentObject == NULL)
    {
      return NULL;
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
  if (parentObject == NULL)
    {
      return NULL;
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
NclComponentsParser::posCompileContext (DOMElement *parentElement,
                                        void *parentObject)
{
  clog << "posCompileContext" << endl;
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

GINGA_NCLCONV_END
