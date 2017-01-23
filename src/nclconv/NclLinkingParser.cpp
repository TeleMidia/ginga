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
#include "NclLinkingParser.h"

GINGA_NCLCONV_BEGIN

NclLinkingParser::NclLinkingParser (DocumentParser *documentParser)
    : ModuleParser (documentParser)
{
}

void *
NclLinkingParser::parseBind (DOMElement *parentElement,
                             void *objGrandParent)
{
  clog << "parseBind" << endl;
  void *parentObject;
  DOMNodeList *elementNodeList;
  DOMElement *element;
  DOMNode *node;
  string elementTagName;
  void *elementObject;

  parentObject = createBind (parentElement, objGrandParent);
  if (parentObject == NULL)
    {
      return NULL;
    }

  elementNodeList = parentElement->getChildNodes ();
  for (int i = 0; i < (int)elementNodeList->getLength (); i++)
    {
      node = elementNodeList->item (i);
      if (node->getNodeType () == DOMNode::ELEMENT_NODE)
        {
          element = (DOMElement *)node;
          elementTagName = XMLString::transcode (element->getTagName ());
          clog << ">>" << elementTagName.c_str () << ": ";
          clog << XMLString::transcode (
                      element->getAttribute (XMLString::transcode ("id")))
               << endl;

          if (XMLString::compareIString (elementTagName.c_str (),
                                         "bindParam")
              == 0)
            {
              elementObject = parseBindParam (element, parentObject);
              if (elementObject != NULL)
                {
                  addBindParamToBind (parentObject, elementObject);
                }
            }
        }
    }

  return parentObject;
}

void *
NclLinkingParser::parseLinkParam (DOMElement *parentElement,
                                  void *objGrandParent)
{
  clog << "parseLinkParam" << endl;
  return createLinkParam (parentElement, objGrandParent);
}

void *
NclLinkingParser::parseBindParam (DOMElement *parentElement,
                                  void *objGrandParent)
{
  clog << "parseBindParam" << endl;
  return createBindParam (parentElement, objGrandParent);
}

void *
NclLinkingParser::parseLink (DOMElement *parentElement,
                             void *objGrandParent)
{
  clog << "parseLink" << endl;
  void *parentObject;
  DOMNodeList *elementNodeList;
  DOMElement *element;
  DOMNode *node;
  string elementTagName;
  void *elementObject;

  parentObject = createLink (parentElement, objGrandParent);
  if (parentObject == NULL)
    {
      return NULL;
    }

  elementNodeList = parentElement->getChildNodes ();
  for (int i = 0; i < (int)elementNodeList->getLength (); i++)
    {
      node = elementNodeList->item (i);
      if (node->getNodeType () == DOMNode::ELEMENT_NODE)
        {
          element = (DOMElement *)node;
          elementTagName = XMLString::transcode (element->getTagName ());
          clog << ">>" << elementTagName.c_str () << ": ";
          clog << XMLString::transcode (
                      element->getAttribute (XMLString::transcode ("id")))
               << endl;

          if (XMLString::compareIString (elementTagName.c_str (),
                                         "linkParam")
              == 0)
            {
              elementObject = parseLinkParam (element, parentObject);
              if (elementObject != NULL)
                {
                  addLinkParamToLink (parentObject, elementObject);
                }
            }
          else if (XMLString::compareIString (elementTagName.c_str (),
                                              "bind")
                   == 0)
            {
              elementObject = parseBind (element, parentObject);
              if (elementObject != NULL)
                {
                  addBindToLink (parentObject, elementObject);
                }
            }
        }
    }

  return parentObject;
}

GINGA_NCLCONV_END
