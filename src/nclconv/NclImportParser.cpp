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

#include "config.h"
#include "NclImportParser.h"

GINGA_NCLCONV_BEGIN

NclImportParser::NclImportParser (DocumentParser *documentParser)
    : ModuleParser (documentParser)
{
}

void *
NclImportParser::parseImportedDocumentBase (DOMElement *parentElement,
                                            void *objGrandParent)
{

  void *parentObject;
  DOMNodeList *elementNodeList;
  DOMElement *element;
  DOMNode *node;
  string elementTagName;
  void *elementObject;

  // pre-compile attributes
  parentObject = createImportedDocumentBase (parentElement, objGrandParent);

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
          if (XMLString::compareIString (elementTagName.c_str (), "importNCL")
              == 0)
            {

              elementObject = parseImportNCL (element, parentObject);
              if (elementObject != NULL)
                {
                  addImportNCLToImportedDocumentBase (parentObject,
                                                      elementObject);
                }
            }
        }
    }

  return parentObject;
}

void *
NclImportParser::parseImportNCL (DOMElement *parentElement,
                                 void *objGrandParent)
{

  return createImportNCL (parentElement, objGrandParent);
}

void *
NclImportParser::parseImportBase (DOMElement *parentElement,
                                  void *objGrandParent)
{

  return createImportBase (parentElement, objGrandParent);
}

GINGA_NCLCONV_END
