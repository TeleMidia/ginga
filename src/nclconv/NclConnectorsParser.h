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
  NclConnectorsParser (NclParser *nclParser);

  SimpleCondition *parseSimpleCondition (DOMElement *simpleCond_element);

  SimpleAction *parseSimpleAction (DOMElement *simpleAction_element);

  CompoundCondition *parseCompoundCondition (DOMElement *compoundCond_element);
  CompoundCondition *createCompoundCondition (DOMElement *compoundCond_element);

  AssessmentStatement *parseAssessmentStatement (DOMElement *parentElement);
  AssessmentStatement *createAssessmentStatement (DOMElement *parentElement);

  AttributeAssessment *parseAttributeAssessment (DOMElement *parentElement);

  ValueAssessment *parseValueAssessment (DOMElement *parentElement);

  CompoundStatement *parseCompoundStatement (DOMElement *parentElement);
  CompoundStatement *createCompoundStatement (DOMElement *parentElement);

  CompoundAction *parseCompoundAction (DOMElement *parentElement);
  CompoundAction *createCompoundAction (DOMElement *compoundAction_element);

  ConnectorBase *parseConnectorBase (DOMElement *parentElement);
  ConnectorBase *createConnectorBase (DOMElement *parentElement);


  // util
  void addSimpleConditionToCompoundCondition (CompoundCondition *compoundCond,
                                              ConditionExpression *condExp);

  void addCompoundConditionToCompoundCondition (CompoundCondition *compoundCond,
                                                ConditionExpression *condExp);

  void addAssessmentStatementToCompoundCondition (CompoundCondition *compoundCond,
                                                  ConditionExpression *condExp);

  void addCompoundStatementToCompoundCondition (CompoundCondition *compoundCond,
                                                ConditionExpression *condExp);

  void addAttributeAssessmentToAssessmentStatement (
      AssessmentStatement *parentObject, AttributeAssessment *childObject);

  void addValueAssessmentToAssessmentStatement (AssessmentStatement *parentObject,
                                                ValueAssessment *childObject);
  void addAssessmentStatementToCompoundStatement (CompoundStatement *parentObject,
                                                  Statement *childObject);

  void addCompoundStatementToCompoundStatement (CompoundStatement *parentObject,
                                                Statement *childObject);

  void addSimpleActionToCompoundAction (CompoundAction *compoundAction,
                                        Action *action);

  void addCompoundActionToCompoundAction (void *parentObject,
                                          void *childObject);

  Parameter *parseConnectorParam (DOMElement *parentElement);

  CausalConnector *parseCausalConnector (DOMElement *parentElement);
  CausalConnector *createCausalConnector (DOMElement *causalConnector_element);

  void addSimpleConditionToCausalConnector (void *parentObject,
                                            void *childObject);

  void addCompoundConditionToCausalConnector (void *parentObject,
                                              void *childObject);

  void addSimpleActionToCausalConnector (void *parentObject, void *childObject);

  void addCompoundActionToCausalConnector (void *parentObject,
                                           void *childObject);

  void addConnectorParamToCausalConnector (Connector *connector,
                                           Parameter *param);

  void addImportBaseToConnectorBase (ConnectorBase *connectorBase,
                                     DOMElement *childObject);

  void addCausalConnectorToConnectorBase (ConnectorBase *connectorBase,
                                          Connector *connector);

  static short convertEventState (const string &eventState);

private:
  void compileRoleInformation (Role *role, DOMElement *parentElement);
  Parameter *getParameter (const string &paramName);
  SimpleActionType convertActionType (const string &actionType);
};

GINGA_NCLCONV_END

#endif /*NCLCONNECTORSPARSER_H_*/
