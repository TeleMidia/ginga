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

#ifndef DOCUMENT_PARSER_H
#define DOCUMENT_PARSER_H

#include "ginga.h"
#include "XMLParsing.h"

GINGA_NCLCONV_BEGIN

class DocumentParser
{
protected:
  string documentPath;
  string userCurrentPath;
  string documentUri;
  string iUriD;
  string fUriD;
  DOMDocument *documentTree;
  map<string, void *> *genericTable;

public:
  DocumentParser ();
  virtual ~DocumentParser ();

protected:
  virtual void initialize () = 0;

public:
  void *parse (const string &uri, const string &iUriD, const string &fUriD);
  void *parse (DOMElement *rootElement, const string &uri);

protected:
  virtual void setDependencies ();
  virtual void *parseRootElement (DOMElement *rootElement) = 0;

private:
  void initializeUserCurrentPath ();
  string absoluteFile (const string &basePath, const string &filename);
  string getPath (const string &filename);

public:
  string getIUriD ();
  string getFUriD ();
  string getUserCurrentPath ();
  bool checkUriPrefix (const string &uri);
  bool isAbsolutePath (const string &path);
  bool isXmlStr (const string &location);

  string getAbsolutePath (const string &path);
  string getDocumentUri ();
  string getDocumentPath ();
  void setDocumentPath (const string &path);
  DOMDocument *getDocumentTree ();
  void addObject (const string &tableName, const string &key, void *value);
  void *getObject (const string &tableName, const string &key);
  void removeObject (const string &tableName, const string &key);
  void addObjectGrouped (const string &tableName, const string &key, void *value);
  bool importDocument (DocumentParser *parser, const string &docLocation);
};

GINGA_NCLCONV_END

#endif /* DOCUMENT_PARSER_H */
