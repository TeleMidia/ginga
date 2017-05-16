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
#include "NclImportParser.h"
#include "NclDocumentParser.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_NCLCONV_BEGIN

NclImportParser::NclImportParser (NclDocumentParser *documentParser)
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
  parentObject = parentElement;
  g_assert_nonnull (parentObject);

  elementNodeList = parentElement->getChildNodes ();
  for (int i = 0; i < (int)elementNodeList->getLength (); i++)
    {
      node = elementNodeList->item (i);
      if (node->getNodeType () == DOMNode::ELEMENT_NODE)
        {
          element = (DOMElement *)node;
          elementTagName = XMLString::transcode (element->getTagName ());
          if (XMLString::compareIString (elementTagName.c_str (),
                                         "importNCL")
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
                                 arg_unused (void *objGrandParent))
{
  return parentElement; // ???
}

void *
NclImportParser::parseImportBase (DOMElement *parentElement,
                                  arg_unused (void *objGrandParent))
{
  return parentElement;
}

void
NclImportParser::addImportNCLToImportedDocumentBase (arg_unused (void *parentObject),
                                                        void *childObject)
{
  string docAlias, docLocation;
  NclDocumentParser *compiler;
  NclDocument *thisDocument, *importedDocument;

  docAlias = XMLString::transcode (
      ((DOMElement *)childObject)
          ->getAttribute (XMLString::transcode ("alias")));

  docLocation = XMLString::transcode (
      ((DOMElement *)childObject)
          ->getAttribute (XMLString::transcode ("documentURI")));

  compiler = getDocumentParser ();
  importedDocument = compiler->importDocument (docLocation);
  if (importedDocument != NULL)
    {
      thisDocument = getDocumentParser ()->getNclDocument ();
      thisDocument->addDocument (importedDocument, docAlias, docLocation);
    }
}

GINGA_NCLCONV_END
