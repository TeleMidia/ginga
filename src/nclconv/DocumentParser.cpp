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
  initializeUserCurrentPath ();
  documentTree = NULL;
  iUriD = "";
  fUriD = "";
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
DocumentParser::parse (const string &_uri, const string &iUriD, const string &fUriD)
{
  DOMElement *rootElement;
  string uri = _uri;

  this->iUriD = iUriD;
  this->fUriD = fUriD;

  if (!isXmlStr (uri))
    {
      string::size_type pos;
      while (true)
        {
          pos = uri.find_first_of (fUriD);
          if (pos == string::npos)
            break;
          uri.replace (pos, 1, iUriD);
        }

      uri = absoluteFile (getDocumentPath (), uri);
    }

  documentTree = (DOMDocument *) XMLParsing::parse (uri);
  g_assert_nonnull (documentTree);

  rootElement = (DOMElement *) documentTree->getDocumentElement ();
  return parse (rootElement, uri);
}

void *
DocumentParser::parse (DOMElement *rootElement, const string &_uri)
{
  void *root;
  string uri = _uri;

  if (!isXmlStr (uri))
    {
      string::size_type pos;
      while (true)
        {
          pos = uri.find_first_of (fUriD);
          if (pos == string::npos)
            break;
          uri.replace (pos, 1, iUriD);
        }
    }

  documentUri = uri;

  if (!isXmlStr (uri))
    {
      setDocumentPath (getPath (uri) + iUriD);
      setDocumentPath (documentPath);
    }

  root = parseRootElement (rootElement);
  if (unlikely (root == NULL))
    {
      g_error ("%s: bad NCL document tree", uri.c_str ());
      exit (EXIT_FAILURE);
    }

  return root;
}

string
DocumentParser::absoluteFile (const string &basePath, const string &fileName)
{
  if (isXmlStr (fileName))
    {
      return fileName;
    }

  if (isAbsolutePath (fileName))
    {
      return fileName;
    }

  return basePath + iUriD + fileName;
}

void
DocumentParser::initializeUserCurrentPath ()
{
  userCurrentPath.assign (g_get_current_dir ());
  if (userCurrentPath.find_last_of (iUriD) != userCurrentPath.length () - 1)
    {
      userCurrentPath = userCurrentPath + iUriD;
    }
}

string
DocumentParser::getPath (const string &filename)
{
  string path;
  string::size_type i;

  i = filename.find_last_of (iUriD);
  if (i != string::npos)
    {
      path = filename.substr (0, i);
    }
  else
    {
      path = "";
    }

  return path;
}

string
DocumentParser::getIUriD ()
{
  return iUriD;
}

string
DocumentParser::getFUriD ()
{
  return fUriD;
}

string
DocumentParser::getUserCurrentPath ()
{
  return userCurrentPath;
}

bool
DocumentParser::checkUriPrefix (const string &uri)
{
  string::size_type len;

  len = uri.length ();
  if ((len >= 10 && uri.substr (0, 10) == "x-sbtvdts:")
      || (len >= 9 && uri.substr (0, 9) == "sbtvd-ts:")
      || (len >= 7 && uri.substr (0, 7) == "http://")
      || (len >= 8 && uri.substr (0, 8) == "https://")
      || (len >= 6 && uri.substr (0, 6) == "ftp://")
      || (len >= 7 && uri.substr (0, 7) == "file://")
      || (len >= 6 && uri.substr (0, 6) == "tcp://")
      || (len >= 6 && uri.substr (0, 6) == "udp://")
      || (len >= 6 && uri.substr (0, 6) == "rtp://")
      || (len >= 13 && uri.substr (0, 13) == "ncl-mirror://")
      || (len >= 7 && uri.substr (0, 7) == "rtsp://"))
    {
      return true;
    }

  return false;
}

bool
DocumentParser::isAbsolutePath (const string &p)
{
  string path = p;
  string::size_type i, len;

  if (isXmlStr (path))
    {
      return true;
    }

  len = path.length ();
  if (checkUriPrefix (path))
    {
      return true;
    }

  i = path.find_first_of (fUriD);
  while (i != string::npos)
    {
      path.replace (i, 1, iUriD);
      i = path.find_first_of (fUriD);
    }

  if ((len >= 1 && path.substr (0, 1) == iUriD)
      || (len >= 2 && path.substr (1, 2) == ":" + iUriD))
    {
      return true;
    }

  return false;
}

bool
DocumentParser::isXmlStr (const string &location)
{
  if (location.find ("<") != std::string::npos
      || location.find ("?xml") != std::string::npos
      || location.find ("|") != std::string::npos)
    {
      return true;
    }

  return false;
}

string
DocumentParser::getAbsolutePath (const string &path)
{
  string newPath = path;

  string::size_type pos;
  while (true)
    {
      pos = newPath.find_first_of (fUriD);
      if (pos == string::npos)
        {
          break;
        }
      newPath.replace (pos, 1, iUriD);
    }

  if (!isAbsolutePath (path))
    {
      newPath = documentPath + newPath;
      if (!isAbsolutePath (newPath))
        {
          newPath = getUserCurrentPath () + newPath;
        }
    }

  return newPath.substr (0, newPath.find_last_of (iUriD));
}

string
DocumentParser::getDocumentPath ()
{
  return documentPath;
}

void
DocumentParser::setDocumentPath (const string &p)
{
  string::size_type pos;
  string path = p;
  while (true)
    {
      pos = path.find_first_of (fUriD);
      if (pos == string::npos)
        {
          break;
        }

      path.replace (pos, 1, iUriD);
    }

  documentPath = path;
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
DocumentParser::importDocument (DocumentParser *parser, const string &docLocation)
{
  string uri;

  if (!isAbsolutePath (docLocation))
    {
      uri = absoluteFile (getDocumentPath (), docLocation);
    }
  else
    {
      uri = docLocation;
    }

  parser->parse (uri, iUriD, fUriD);

  return true;
}

GINGA_NCLCONV_END
