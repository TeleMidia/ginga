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

#include "ncl/NclDocument.h"
using namespace ::ginga::ncl;

GINGA_NCLCONV_BEGIN

class DocumentParser : public ErrorHandler
{
protected:
  NclDocument *ncl;             // NCL document
  string path;                  // document's absolute path
  string dirname;               // directory part of document's path

  virtual void initialize () = 0;
  virtual void *parseRootElement (DOMElement *rootElement) = 0;

public:
  DocumentParser ();
  virtual ~DocumentParser ();

  string getPath ();
  string getDirName ();
  void setNclDocument (NclDocument *);
  NclDocument *getNclDocument ();

  void warning (const SAXParseException &);
  void error (const SAXParseException &);
  void fatalError (const SAXParseException &);
  void resetErrors () {};

  NclDocument *parse (const string &);
};

GINGA_NCLCONV_END

#endif /* DOCUMENT_PARSER_H */
