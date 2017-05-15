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
  NclImportParser *importParser;
  Connector *connector;

public:
  NclConnectorsParser (NclDocumentParser *documentParser);

  void *parseSimpleCondition (DOMElement *parentElement,
                              void *objGrandParent);

  void *createSimpleCondition (DOMElement *parentElement, void *objGrandParent);

  void *parseCompoundCondition (DOMElement *parentElement,
                                void *objGrandParent);

  void *createCompoundCondition (DOMElement *parentElement,
                                 void *objGrandParent);

  void addSimpleConditionToCompoundCondition (void *parentObject,
                                              void *childObject);

  void addCompoundConditionToCompoundCondition (void *parentObject,
                                                void *childObject);

  void addAssessmentStatementToCompoundCondition (void *parentObject,
                                                  void *childObject);

  void addCompoundStatementToCompoundCondition (void *parentObject,
                                                void *childObject);

  void *parseAssessmentStatement (DOMElement *parentElement,
                                  void *objGrandParent);

  void *createAssessmentStatement (DOMElement *parentElement,
                                   void *objGrandParent);

  void addAttributeAssessmentToAssessmentStatement (void *parentObject,
                                                    void *childObject);

  void addValueAssessmentToAssessmentStatement (void *parentObject,
                                                void *childObject);

  void *parseAttributeAssessment (DOMElement *parentElement,
                                  void *objGrandParent);

  void *createAttributeAssessment (DOMElement *parentElement,
                                   void *objGrandParent);

  void *parseValueAssessment (DOMElement *parentElement,
                              void *objGrandParent);

  void *createValueAssessment (DOMElement *parentElement,
                               void *objGrandParent);

  void *parseCompoundStatement (DOMElement *parentElement,
                                void *objGrandParent);

  void *createCompoundStatement (DOMElement *parentElement,
                                 void *objGrandParent);

  void addAssessmentStatementToCompoundStatement (void *parentObject,
                                                  void *childObject);

  void addCompoundStatementToCompoundStatement (void *parentObject,
                                                void *childObject);

  void *parseSimpleAction (DOMElement *parentElement, void *objGrandParent);
  void *createSimpleAction (DOMElement *parentElement, void *objGrandParent);

  void *parseCompoundAction (DOMElement *parentElement, void *objGrandParent);
  void *createCompoundAction (DOMElement *parentElement, void *objGrandParent);


  void addSimpleActionToCompoundAction (void *parentObject, void *childObject);

  void addCompoundActionToCompoundAction (void *parentObject,
                                          void *childObject);

  void *parseConnectorParam (DOMElement *parentElement,
                             void *objGrandParent);

  void *createConnectorParam (DOMElement *parentElement, void *objGrandParent);

  void *parseCausalConnector (DOMElement *parentElement,
                              void *objGrandParent);

  void *createCausalConnector (DOMElement *parentElement, void *objGrandParent);

  void addSimpleConditionToCausalConnector (void *parentObject,
                                            void *childObject);

  void addCompoundConditionToCausalConnector (void *parentObject,
                                              void *childObject);

  void addSimpleActionToCausalConnector (void *parentObject, void *childObject);

  void addCompoundActionToCausalConnector (void *parentObject,
                                           void *childObject);

  void addConnectorParamToCausalConnector (void *parentObject,
                                           void *childObject);

  void *parseConnectorBase (DOMElement *parentElement, void *objGrandParent);

  void *createConnectorBase (DOMElement *parentElement, void *objGrandParent);

  void addImportBaseToConnectorBase (void *parentObject, void *childObject);

  void addCausalConnectorToConnectorBase (void *parentObject,
                                          void *childObject);

  NclImportParser *getImportParser ();
  void setImportParser (NclImportParser *importParser);

  static short convertEventState (const string &eventState);

private:
  void compileRoleInformation (Role *role, DOMElement *parentElement);
  Parameter *getParameter (const string &paramName);
  SimpleActionType convertActionType (const string &actionType);
};

GINGA_NCLCONV_END

#endif /*NCLCONNECTORSPARSER_H_*/
