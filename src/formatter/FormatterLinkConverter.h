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

#ifndef FORMATTERLINKCONVERTER_H_
#define FORMATTERLINKCONVERTER_H_

#include "ncl/Action.h"
#include "ncl/AssessmentStatement.h"
#include "ncl/CausalConnector.h"
#include "ncl/ValueAssessment.h"
#include "ncl/CompoundAction.h"
#include "ncl/CompoundStatement.h"
#include "ncl/CompoundCondition.h"
#include "ncl/ConditionExpression.h"
#include "ncl/AttributeAssessment.h"
#include "ncl/Role.h"
#include "ncl/SimpleAction.h"
#include "ncl/SimpleCondition.h"
#include "ncl/Statement.h"
#include "ncl/TriggerExpression.h"
#include "ncl/EventUtil.h"
using namespace ::ginga::ncl;

#include "ncl/Bind.h"
#include "ncl/CausalLink.h"
#include "ncl/Link.h"
using namespace ::ginga::ncl;

#include "ncl/Node.h"
using namespace ::ginga::ncl;

#include "ncl/InterfacePoint.h"
using namespace ::ginga::ncl;

#include "ncl/Parameter.h"
using namespace ::ginga::ncl;

#include "NclFormatterCausalLink.h"
#include "NclLinkAction.h"
#include "NclLinkAssessment.h"
#include "NclLinkAndCompoundTriggerCondition.h"
#include "NclLinkAssessmentStatement.h"
#include "NclLinkValueAssessment.h"
#include "NclLinkAssignmentAction.h"
#include "NclLinkCompoundAction.h"
#include "NclLinkAttributeAssessment.h"
#include "NclLinkCompoundStatement.h"
#include "NclLinkCompoundTriggerCondition.h"
#include "NclLinkRepeatAction.h"
#include "NclLinkSimpleAction.h"
#include "NclLinkCondition.h"
#include "NclLinkStatement.h"
#include "NclLinkTriggerCondition.h"
#include "NclLinkTransitionTriggerCondition.h"

#include "NclFormatterEvent.h"

#include "NclCompositeExecutionObject.h"
#include "NclExecutionObject.h"
#include "NclNodeNesting.h"

#include "FormatterConverter.h"

GINGA_FORMATTER_BEGIN

class FormatterLinkConverter
{
private:
  FormatterConverter *compiler;

public:
  FormatterLinkConverter (FormatterConverter *compiler);
  ~FormatterLinkConverter ();

  NclFormatterCausalLink *
  createCausalLink (CausalLink *ncmLink,
                    NclCompositeExecutionObject *parentObject);

private:
  void setImplicitRefAssessment (const string &roleId, CausalLink *ncmLink,
                                 NclFormatterEvent *event);

  NclLinkAction *createAction (Action *actionExpression,
                               CausalLink *ncmLink,
                               NclCompositeExecutionObject *parentObject);

  NclLinkCondition *
  createCondition (ConditionExpression *ncmExpression, CausalLink *ncmLink,
                   NclCompositeExecutionObject *parentObject);

  NclLinkCompoundTriggerCondition *createCompoundTriggerCondition (
      short op, GingaTime delay,
      vector<ConditionExpression *> *ncmChildConditions,
      CausalLink *ncmLink, NclCompositeExecutionObject *parentObject);

  NclLinkCondition *createCondition (
      TriggerExpression *triggerExpression, CausalLink *ncmLink,
      NclCompositeExecutionObject *parentObject);

  NclLinkAssessmentStatement *createAssessmentStatement (
      AssessmentStatement *assessmentStatement, Bind *bind, Link *ncmLink,
      NclCompositeExecutionObject *parentObject);

  NclLinkStatement *
  createStatement (Statement *statementExpression, Link *ncmLink,
                   NclCompositeExecutionObject *parentObject);

  NclLinkAttributeAssessment *createAttributeAssessment (
      AttributeAssessment *attributeAssessment, Bind *bind, Link *ncmLink,
      NclCompositeExecutionObject *parentObject);

  NclLinkSimpleAction *
  createSimpleAction (SimpleAction *sae, Bind *bind, Link *ncmLink,
                      NclCompositeExecutionObject *parentObject);

  NclLinkCompoundAction *createCompoundAction (
      short op, GingaTime delay, vector<Action *> *ncmChildActions,
      CausalLink *ncmLink, NclCompositeExecutionObject *parentObject);

  NclLinkTriggerCondition *createSimpleCondition (
      SimpleCondition *condition, Bind *bind, Link *ncmLink,
      NclCompositeExecutionObject *parentObject);

  NclFormatterEvent *createEvent (Bind *bind, Link *ncmLink,
                                  NclCompositeExecutionObject *parentObject);

  GingaTime getDelayParameter (Link *ncmLink, Parameter *connParam,
                               Bind *ncmBind);

  string getBindKey (Link *ncmLink, Bind *ncmBind);
  GingaTime compileDelay (Link *ncmLink, const string &delayObject, Bind *bind);
};

GINGA_FORMATTER_END

#endif /*FORMATTERLINKCONVERTER_H_*/
