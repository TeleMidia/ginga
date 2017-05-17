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

#ifndef NCLCONNECTORSPARSER_H_
#define NCLCONNECTORSPARSER_H_

#include "ginga.h"

#include "ModuleParser.h"
#include "NclImportParser.h"

#include "ncl/NclDocument.h"
#include "ncl/CausalConnector.h"
#include "ncl/ValueAssessment.h"
#include "ncl/Comparator.h"
using namespace ::ginga::ncl;

GINGA_NCLCONV_BEGIN

class NclConnectorsParser : public ModuleParser
{
  // Ref Classes
private:
  Connector *connector;

public:
  explicit NclConnectorsParser (NclParser *nclParser);

  SimpleCondition *parseSimpleCondition (DOMElement *simpleCond_element);

  SimpleAction *parseSimpleAction (DOMElement *simpleAction_element);

  CompoundCondition *parseCompoundCondition (DOMElement *compoundCond_element);
  CompoundCondition *createCompoundCondition (DOMElement *compoundCond_element);

  AssessmentStatement *parseAssessmentStatement (DOMElement *assessmentStatement_element);
  AssessmentStatement *createAssessmentStatement (DOMElement *assessmentStatement_element);

  AttributeAssessment *parseAttributeAssessment (DOMElement *attributeAssessment_element);

  ValueAssessment *parseValueAssessment (DOMElement *valueAssessment_element);

  CompoundStatement *parseCompoundStatement (DOMElement *compoundStatement_element);
  CompoundStatement *createCompoundStatement (DOMElement *compoundStatement_element);

  CompoundAction *parseCompoundAction (DOMElement *compoundAction_element);
  CompoundAction *createCompoundAction (DOMElement *compoundAction_element);

  ConnectorBase *parseConnectorBase (DOMElement *connBase_element);
  ConnectorBase *createConnectorBase (DOMElement *connBase_element);

  Parameter *parseConnectorParam (DOMElement *connectorParam_element);

  CausalConnector *parseCausalConnector (DOMElement *causalConnector_element);
  CausalConnector *createCausalConnector (DOMElement *causalConnector_element);

private:
  void compileRoleInformation (Role *role, DOMElement *parentElement);

  void addAttributeAssessmentToAssessmentStatement (
      AssessmentStatement *parentObject, AttributeAssessment *childObject);

  void addImportBaseToConnectorBase (ConnectorBase *connectorBase,
                                     DOMElement *childObject);
};

GINGA_NCLCONV_END

#endif /*NCLCONNECTORSPARSER_H_*/
