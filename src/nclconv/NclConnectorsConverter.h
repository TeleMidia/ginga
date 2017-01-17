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

#ifndef NCL_CONNECTORS_CONVERTER_H
#define NCL_CONNECTORS_CONVERTER_H

#include "ginga.h"

#include "ncl/Parameter.h"
using namespace ::br::pucrio::telemidia::ncl;

#include "ncl/animation/Animation.h"
using namespace ::br::pucrio::telemidia::ncl::animation;

#include "ncl/connectors/EventUtil.h"
#include "ncl/connectors/ValueAssessment.h"
#include "ncl/connectors/CompoundAction.h"
#include "ncl/connectors/CompoundCondition.h"
#include "ncl/connectors/CompoundStatement.h"
#include "ncl/connectors/CausalConnector.h"
#include "ncl/connectors/AttributeAssessment.h"
#include "ncl/connectors/AssessmentStatement.h"
#include "ncl/connectors/SimpleAction.h"
#include "ncl/connectors/SimpleCondition.h"
#include "ncl/connectors/ConnectorBase.h"
using namespace ::br::pucrio::telemidia::ncl::connectors;

#include "ncl/util/Comparator.h"
using namespace ::br::pucrio::telemidia::ncl::util;

#include "util/functions.h"
using namespace ::ginga::util;

#include "ncl/NclDocument.h"
using namespace ::br::pucrio::telemidia::ncl;

#include "DocumentParser.h"
using namespace ::br::pucrio::telemidia::converter::framework;

#include "NclConnectorsParser.h"
using namespace ::br::pucrio::telemidia::converter::framework::ncl;

#include <xercesc/dom/DOM.hpp>
XERCES_CPP_NAMESPACE_USE;

BR_PUCRIO_TELEMIDIA_CONVERTER_NCL_BEGIN

class NclConnectorsConverter : public NclConnectorsParser
{
private:
  Connector *connector;

public:
  NclConnectorsConverter(DocumentParser *documentParser);
  void addCausalConnectorToConnectorBase(void *parent, void *child);
  void addConnectorParamToCausalConnector(void *parent, void *child);
  void addImportBaseToConnectorBase(void *parent, void *child);
  void *createCausalConnector(DOMElement *parentElement,
                              void *objGrandParent);
  void *createConnectorBase(DOMElement *parentElement,
                            void *objGrandParent);
  void *createConnectorParam(DOMElement *parentElement,
                             void *objGrandParent);

private:
  void compileRoleInformation(Role *role, DOMElement *parentElement);

public:
  void *createSimpleCondition(DOMElement *parentElement,
                              void *objGrandParent);
  void *createCompoundCondition(DOMElement *parentElement,
                                void *objGrandParent);
  void *createAttributeAssessment(DOMElement *parentElement,
                                  void *objGrandParent);
  void *createValueAssessment(DOMElement *parentElement,
                              void *objGrandParent);
  void *createAssessmentStatement(DOMElement *parentElement,
                                  void *objGrandParent);
  void *createCompoundStatement(DOMElement *parentElement,
                                void *objGrandParent);
  void *createSimpleAction(DOMElement *parentElement, void *objGrandParent);
  void *createCompoundAction(DOMElement *parentElement,
                             void *objGrandParent);

private:
  Parameter *getParameter(string paramName);
  short convertActionType(string actionType);

public:
  static short convertEventState(string eventState);
  void addSimpleConditionToCompoundCondition(void *parent,void *child);
  void addCompoundConditionToCompoundCondition(void *parent,void *child);
  void addAssessmentStatementToCompoundCondition(void *parent,void *child);
  void addCompoundStatementToCompoundCondition(void *parent,void *child);
  void addAttributeAssessmentToAssessmentStatement(void *parent,
                                                   void *child);
  void addValueAssessmentToAssessmentStatement(void *parent, void *child);
  void addAssessmentStatementToCompoundStatement(void *parent, void *child);
  void addCompoundStatementToCompoundStatement(void *parent, void *child);
  void addSimpleActionToCompoundAction(void *parent, void *child);
  void addCompoundActionToCompoundAction(void *parent, void *child);
  void addSimpleConditionToCausalConnector(void *parent, void *child);
  void addCompoundConditionToCausalConnector(void *parent, void *child);
  void addSimpleActionToCausalConnector(void *parent, void *child);
  void addCompoundActionToCausalConnector(void *parent, void *child);
  void addAssessmentStatementToConstraintConnector(void *parent,
                                                   void *child);
  void addCompoundStatementToConstraintConnector(void *parent, void *child);
  void addConstraintConnectorToConnectorBase(void *parent, void *child);
  void *createConstraintConnector(DOMElement *parentElement,
                                  void *objGrandParent);
};

BR_PUCRIO_TELEMIDIA_CONVERTER_NCL_END

#endif
