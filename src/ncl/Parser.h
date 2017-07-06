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

#ifndef PARSER_H
#define PARSER_H

GINGA_PRAGMA_DIAG_PUSH ()
GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)
GINGA_PRAGMA_DIAG_IGNORE (-Wundef)
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/sax/SAXParseException.hpp>
XERCES_CPP_NAMESPACE_USE
GINGA_PRAGMA_DIAG_POP ()

#include "Area.h"
#include "AreaLabeled.h"
#include "Comparator.h"
#include "CompositeRule.h"
#include "CompoundAction.h"
#include "CompoundCondition.h"
#include "Descriptor.h"
#include "NclDocument.h"
#include "Property.h"
#include "Rule.h"
#include "SimpleRule.h"
#include "Switch.h"
#include "TransitionUtil.h"
#include "ValueAssessment.h"

GINGA_NCL_BEGIN

class Parser : public ErrorHandler
{
public:
  static NclDocument *parse (const string &, int, int);

private:
  NclDocument *_doc;            // NCL document
  string _path;                 // document's absolute path
  string _dirname;              // directory part of document's path
  int _width;                   // screen width (in pixels)
  int _height;                  // screen height (in pixels)

  Parser (int, int);
  ~Parser ();
  NclDocument *parse0 (const string &);

  void parseNcl (DOMElement *);
  void parseHead (DOMElement *);

  NclDocument *parse1 (const string &);
  NclDocument *parseImportNCL (DOMElement *, string *, string *);
  Base *parseImportBase (DOMElement *, NclDocument **, string *, string *);
  void parseImportedDocumentBase (DOMElement *);

  RuleBase *parseRuleBase (DOMElement *);
  CompositeRule *parseCompositeRule (DOMElement *);
  SimpleRule *parseRule (DOMElement *);

  TransitionBase *parseTransitionBase (DOMElement *);
  Transition *parseTransition (DOMElement *);

  RegionBase *parseRegionBase (DOMElement *);
  Region *parseRegion (DOMElement *, RegionBase *, Region *);

  DescriptorBase *parseDescriptorBase (DOMElement *);
  Descriptor *parseDescriptor (DOMElement *);

  ConnectorBase *parseConnectorBase (DOMElement *);
  Connector *parseCausalConnector (DOMElement *);
  CompoundCondition *parseCompoundCondition (DOMElement *);
  SimpleCondition *parseSimpleCondition (DOMElement *);
  CompoundStatement *parseCompoundStatement (DOMElement *);
  AssessmentStatement *parseAssessmentStatement (DOMElement *);
  AttributeAssessment *parseAttributeAssessment (DOMElement *);
  ValueAssessment *parseValueAssessment (DOMElement *);
  CompoundAction *parseCompoundAction (DOMElement *);
  SimpleAction *parseSimpleAction (DOMElement *);

  Context *parseBody (DOMElement *);
  void solveNodeReferences (Composition *);
  void posCompileContext (DOMElement *, Context *);
  void posCompileSwitch (DOMElement *, Switch *);

  Node *parseContext (DOMElement *);
  Port *parsePort (DOMElement *, Composition *);

  map<string, map<string, Node *> *> _switchMap; // FIXME
  Node *parseSwitch (DOMElement *);
  Node *parseBindRule (DOMElement *, Composition *, Rule **);
  SwitchPort *parseSwitchPort (DOMElement *, Switch *);
  Port *parseMapping (DOMElement *, Switch *, SwitchPort *);

  Node *parseMedia (DOMElement *);
  Property *parseProperty (DOMElement *);
  Anchor *parseArea (DOMElement *);

  Link *parseLink (DOMElement *, Context *);
  Parameter *parseLinkParam (DOMElement *);
  Bind *parseBind (DOMElement *, Link *, Context *);
  Parameter *parseBindParam (DOMElement *);

  // From ErrorHandler.
  void warning (const SAXParseException &);
  void error (const SAXParseException &);
  void fatalError (const SAXParseException &);
  void resetErrors () {};
};

GINGA_NCL_END

#endif // PARSER_H
