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
#include "DocumentParser.h"

GINGA_NCLCONV_BEGIN

DocumentParser::DocumentParser ()
{
  genericTable = new map<string, void *>;
  documentTree = NULL;
}

DocumentParser::~DocumentParser ()
{
  map<string, void *> *table;
  map<string, void *>::iterator i;

  if (genericTable != NULL)
    {
      i = genericTable->begin ();
      while (i != genericTable->end ())
        {
          table = (map<string, void *> *)(i->second);
          delete table;
          table = NULL;
          ++i;
        }

      delete genericTable;
      genericTable = NULL;
    }

  if (documentTree != NULL)
    {
      delete documentTree;
      documentTree = NULL;
    }
}

void *
DocumentParser::parse (const string &path)
{
  DOMElement *rootElement;

  documentTree = (DOMDocument *) XMLParsing::parse (path);
  g_assert_nonnull (documentTree);

  rootElement = (DOMElement *) documentTree->getDocumentElement ();
  return parse (rootElement, path);
}

void *
DocumentParser::parse (DOMElement *rootElement, const string &path)
{
  void *root;

  this->documentUri = path;
  this->documentPath = xpathdirname (path);
  root = parseRootElement (rootElement);
  g_assert_nonnull (root);

  return root;
}

string
DocumentParser::getDocumentPath ()
{
  return documentPath;
}

string
DocumentParser::getDocumentUri ()
{
  return documentUri;
}

DOMDocument *
DocumentParser::getDocumentTree ()
{
  return documentTree;
}

void
DocumentParser::addObject (const string &tableName,
                           const string &key, void *value)
{
  map<string, void *> *table;
  map<string, void *>::iterator i;

  i = genericTable->find (tableName);
  if (i != genericTable->end ())
    {
      table = (map<string, void *> *)i->second;
      (*table)[key] = value;
      return;
    }

  table = new map<string, void *>;
  (*table)[key] = value;
  (*genericTable)[tableName] = table;
}

void *
DocumentParser::getObject (const string &tableName, const string &key)
{
  map<string, void *> *table = NULL;
  map<string, void *>::iterator i;

  i = genericTable->find (tableName);
  if (i != genericTable->end ())
    {
      table = (map<string, void *> *)i->second;
    }

  if (table != NULL)
    {
      i = table->find (key);
      if (i != table->end ())
        {
          return i->second;
        }
    }

  return NULL;
}

bool
DocumentParser::importDocument (DocumentParser *parser, const string &path)
{
  string abspath;

  if (!xpathisabs (path))
    abspath = xpathbuildabs (getDocumentPath (), path);

  parser->parse (abspath);

  return true;
}

GINGA_NCLCONV_END
