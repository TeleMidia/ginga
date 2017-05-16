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
#include "NclParser.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_NCLCONV_BEGIN

NclImportParser::NclImportParser (NclParser *nclParser)
    : ModuleParser (nclParser)
{
}

void
NclImportParser::parseImportedDocumentBase (DOMElement *parentElement)
{
  // pre-compile attributes
  g_assert_nonnull (parentElement);

  DOMNodeList *elementNodeList = parentElement->getChildNodes ();
  for (int i = 0; i < (int)elementNodeList->getLength (); i++)
    {
      DOMNode *node = elementNodeList->item (i);
      if (node->getNodeType () == DOMNode::ELEMENT_NODE)
        {
          DOMElement *element = (DOMElement *)node;
          string tagname = _nclParser->getTagname(element);
          if (XMLString::compareIString (tagname.c_str (), "importNCL") == 0)
            {
              DOMElement *elementObject = parseImportNCL (element);
              if (elementObject)
                {
                  addImportNCLToImportedDocumentBase (elementObject);
                }
            }
        }
    }
}

DOMElement *
NclImportParser::parseImportNCL (DOMElement *parentElement)
{
  return parentElement; // ???
}

DOMElement *
NclImportParser::parseImportBase (DOMElement *parentElement)
{
  return parentElement;
}

void
NclImportParser::addImportNCLToImportedDocumentBase (DOMElement *childObject)
{
  string docAlias, docLocation;
  NclParser *compiler;
  NclDocument *thisDocument, *importedDocument;

  docAlias = _nclParser->getAttribute(childObject, "alias");

  docLocation = _nclParser->getAttribute(childObject, "documentURI");

  compiler = getNclParser ();
  importedDocument = compiler->importDocument (docLocation);
  if (importedDocument != NULL)
    {
      thisDocument = getNclParser ()->getNclDocument ();
      thisDocument->addDocument (importedDocument, docAlias, docLocation);
    }
}

GINGA_NCLCONV_END
