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

#ifndef PARSER_XERCES_C_H
#define PARSER_XERCES_C_H

GINGA_PRAGMA_DIAG_PUSH ()
GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)
GINGA_PRAGMA_DIAG_IGNORE (-Wundef)
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/sax/SAXParseException.hpp>
XERCES_CPP_NAMESPACE_USE
GINGA_PRAGMA_DIAG_POP ()

#include "NclAction.h"
#include "NclArea.h"
#include "NclAreaLabeled.h"
#include "NclDocument.h"
#include "NclProperty.h"
#include "NclSwitch.h"

GINGA_NCL_BEGIN

class ParserXercesC: public ErrorHandler
{
public:
  static NclDocument *parse (const string &, int, int, string *);

private:
  NclDocument *_doc;            // NCL document
  string _path;                 // document's absolute path
  string _dirname;              // directory part of document's path
  string _errmsg;               // last error
  int _width;                   // screen width (in pixels)
  int _height;                  // screen height (in pixels)

  map<string,map<string,string>> _descriptors; // cached descriptors
  map<string,map<string,string>> _regions;     // cached regions
  map<string,Predicate *> _rules;              // cached rules

  ParserXercesC (int, int);
  ~ParserXercesC ();

  string getErrMsg ();
  void setErrMsg (const string &);

  NclDocument *parse0 (const string &);
  bool parseNcl (DOMElement *);
  void parseHead (DOMElement *);

  NclDocument *parse1 (const string &);
  NclDocument *parseImportNCL (DOMElement *, string *, string *);
  NclConnectorBase *parseImportBase (DOMElement *, NclDocument **, string *, string *);
  void parseImportedDocumentBase (DOMElement *);

  void parseRuleBase (DOMElement *);
  void parseCompositeRule (DOMElement *, Predicate *);
  void parseRule (DOMElement *, Predicate *);

  void parseTransitionBase (DOMElement *);
  void parseTransition (DOMElement *);

  void parseRegionBase (DOMElement *);
  void parseRegion (DOMElement *, GingaRect);

  void parseDescriptorBase (DOMElement *);
  void parseDescriptor (DOMElement *);

  NclConnectorBase *parseConnectorBase (DOMElement *);
  NclConnector *parseCausalConnector (DOMElement *);

  Predicate *parseAssessmentStatement (DOMElement *);
  Predicate *parseCompoundStatement (DOMElement *);

  void parseCompoundCondition (DOMElement *, NclConnector *, Predicate *);
  void parseCondition (DOMElement *, NclConnector *, Predicate *);

  void parseCompoundAction (DOMElement *, NclConnector *);
  void parseSimpleAction (DOMElement *, NclConnector *);

  NclContext *parseBody (DOMElement *);
  void solveNodeReferences (NclComposition *);
  void posCompileContext (DOMElement *, NclContext *);
  void posCompileSwitch (DOMElement *, NclSwitch *);

  NclNode *parseContext (DOMElement *);
  NclPort *parsePort (DOMElement *, NclComposition *);

  NclNode *parseSwitch (DOMElement *);
  NclNode *parseMedia (DOMElement *);
  NclProperty *parseProperty (DOMElement *);
  NclAnchor *parseArea (DOMElement *);

  NclLink *parseLink (DOMElement *, NclContext *);
  NclBind *parseBind (DOMElement *, NclLink *, map<string, string> *, NclContext *);

  // From ErrorHandler.
  void warning (const SAXParseException &);
  void error (const SAXParseException &);
  void fatalError (const SAXParseException &);
  void resetErrors () {};
};

GINGA_NCL_END

#endif // PARSER_XERCESC_H
