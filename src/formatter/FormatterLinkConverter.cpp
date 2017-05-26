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
#include "FormatterLinkConverter.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_FORMATTER_BEGIN

FormatterLinkConverter::FormatterLinkConverter (
    FormatterConverter *compiler)
{
  this->compiler = compiler;
}

FormatterLinkConverter::~FormatterLinkConverter () { compiler = NULL; }

NclFormatterCausalLink *
FormatterLinkConverter::createCausalLink (
    CausalLink *ncmLink, NclCompositeExecutionObject *parentObject)
{
  CausalConnector *connector;
  ConditionExpression *conditionExpression;
  Action *actionExpression;
  NclLinkCondition *formatterCondition;
  NclLinkAction *formatterAction;
  NclFormatterCausalLink *formatterLink;
  vector<NclLinkAction *> *acts;
  vector<NclLinkAction *>::iterator i;
  NclLinkAssignmentAction *action;
  string value;
  NclFormatterEvent *event;
  Animation *anim;

  clog << "FormatterLinkConverter::createCausalLink inside '";
  clog << parentObject->getId () << "' from ncmlinkId '";
  clog << ncmLink->getId () << "'" << endl;

  if (ncmLink == NULL)
    {
      clog << "FormatterLinkConverter::createCausalLink Warning!";
      clog << " cannot create formatter link inside '";
      clog << parentObject->getId () << "' NCM link is NULL" << endl;
      return NULL;
    }

  if (!ncmLink->isConsistent ())
    {
      clog << "FormatterLinkConverter::createCausalLink Warning!";
      clog << " cannot create formatter link inside '";
      clog << parentObject->getId () << "' from ncmlinkId '";
      clog << ncmLink->getId () << "': inconsistent link (check min and ";
      clog << "max attributes!)" << endl;
      return NULL;
    }

  // compile link condition and verify if it is a trigger condition
  connector = (CausalConnector *)(ncmLink->getConnector ());
  conditionExpression = connector->getConditionExpression ();
  formatterCondition
      = createCondition ((TriggerExpression *)conditionExpression, ncmLink,
                         parentObject);

  if (formatterCondition == NULL
      || !(formatterCondition->instanceOf ("NclLinkTriggerCondition")))
    {
      clog << "FormatterLinkConverter::createCausalLink Warning!";
      clog << " cannot create formatter link inside '";
      clog << parentObject->getId () << "' from ncmlinkId '";
      clog << ncmLink->getId () << "' with a unknown condition (";
      clog << formatterCondition << ")" << endl;
      if (formatterCondition != NULL)
        {
          delete formatterCondition;
        }
      return NULL;
    }

  // compile link action
  actionExpression = connector->getAction ();
  formatterAction
      = createAction (actionExpression, ncmLink, parentObject);

  if (formatterAction == NULL)
    {
      clog << "FormatterLinkConverter::createCausalLink Warning!";
      clog << " cannot create formatter link inside '";
      clog << parentObject->getId () << "' from ncmlinkId '";
      clog << ncmLink->getId () << "' with a NULL action (";
      clog << endl;
      return NULL;
    }

  // create formatter causal link
  formatterLink = new NclFormatterCausalLink (
      (NclLinkTriggerCondition *)formatterCondition, formatterAction,
      ncmLink, (NclCompositeExecutionObject *)parentObject);

  //&got
  if (formatterCondition->instanceOf ("NclLinkCompoundTriggerCondition"))
    {
      acts = formatterAction->getImplicitRefRoleActions ();
      if (acts != NULL)
        {
          i = acts->begin ();
          while (i != acts->end ())
            {
              action = (NclLinkAssignmentAction *)(*i);
              value = action->getValue ();
              if (value != "" && value.substr (0, 1) == "$")
                {
                  event = ((NclLinkRepeatAction *)(*i))->getEvent ();
                  setImplicitRefAssessment (
                      value.substr (1, value.length ()), ncmLink, event);
                }

              anim = action->getAnimation ();
              if (anim != NULL)
                {
                  value = anim->getDuration ();
                  if (value != "" && value.substr (0, 1) == "$")
                    {
                      event = ((NclLinkRepeatAction *)(*i))->getEvent ();
                      setImplicitRefAssessment (
                          value.substr (1, value.length ()), ncmLink,
                          event);
                    }

                  value = anim->getBy ();
                  if (value != "" && value.substr (0, 1) == "$")
                    {
                      event = ((NclLinkRepeatAction *)(*i))->getEvent ();
                      setImplicitRefAssessment (
                          value.substr (1, value.length ()), ncmLink,
                          event);
                    }
                }
              ++i;
            }

          delete acts;
        }
    }

  return formatterLink;
}

void
FormatterLinkConverter::setImplicitRefAssessment (const string &roleId,
                                                  CausalLink *ncmLink,
                                                  NclFormatterEvent *event)
{
  NclFormatterEvent *refEvent;
  vector<Node *> *ncmPerspective;
  vector<Bind *> *binds;
  vector<Bind *>::iterator i;
  NclNodeNesting *refPerspective;
  NclExecutionObject *refObject;
  InterfacePoint *refInterface;
  string value;

  if (event->instanceOf ("NclAttributionEvent"))
    {
      binds = ncmLink->getBinds ();
      i = binds->begin ();
      while (i != binds->end ())
        {
          value = (*i)->getRole ()->getLabel ();
          if (roleId == value)
            {
              refInterface = (*i)->getInterfacePoint ();
              if (refInterface != NULL
                  && refInterface->instanceOf ("PropertyAnchor"))
                {
                  ncmPerspective = (*i)->getNode ()->getPerspective ();
                  refPerspective = new NclNodeNesting (ncmPerspective);

                  delete ncmPerspective;

                  /*clog << "FormatterLinkConverter::";
                  clog << "createCausalLink '";
                  clog << " perspective = '";
                  clog << refPerspective->getId() << "'";
                  if ((*j)->getDescriptor() != NULL) {
                          clog << " descriptor = '";
                          clog << (*j)->getDescriptor()->getId();
                          clog << "'";
                  }
                  clog << endl;*/

                  refObject = compiler->getExecutionObjectFromPerspective (
                      refPerspective, (*i)->getDescriptor ());

                  delete refPerspective;

                  refEvent
                      = compiler->getEvent (refObject, refInterface,
                                            EventUtil::EVT_ATTRIBUTION, "");

                  ((NclAttributionEvent *)event)
                      ->setImplicitRefAssessmentEvent (roleId, refEvent);

                  break;
                }
            }
          ++i;
        }
    }
}

NclLinkAction *
FormatterLinkConverter::createAction (
    Action *actionExpression, CausalLink *ncmLink,
    NclCompositeExecutionObject *parentObject)
{
  double delay;
  SimpleAction *sae;
  CompoundAction *cae;
  vector<Bind *> *binds;
  int i, size;
  string delayObject;
  NclLinkSimpleAction *simpleAction;
  NclLinkCompoundAction *compoundAction;

  if (actionExpression == NULL)
    {
      clog << "FormatterLinkConverter::createAction ";
      clog << "Warning! ActionExpression is NULL" << endl;
      return NULL;
    }

  if (actionExpression->instanceOf ("SimpleAction"))
    {
      sae = (SimpleAction *)actionExpression;
      binds = ncmLink->getRoleBinds (sae);
      if (binds != NULL)
        {
          size = (int) binds->size ();
          if (size == 1)
            {
              return createSimpleAction (sae, (*binds)[0], ncmLink,
                                         parentObject);
            }
          else if (size > 1)
            {
              compoundAction
                  = new NclLinkCompoundAction (sae->getQualifier ());

              for (i = 0; i < size; i++)
                {
                  simpleAction = createSimpleAction (
                      sae, (*binds)[i], ncmLink, parentObject);

                  if (simpleAction == NULL)
                    {
                      clog << "FormatterLinkConverter::createAction ";
                      clog << "Warning! cannot create compound action: ";
                      clog << "found invalid action(s)" << endl;
                      delete compoundAction;
                      return NULL;
                    }
                  compoundAction->addAction (simpleAction);
                }

              return compoundAction;
            }
          else
            {
              clog << "FormatterLinkConverter::createAction ";
              clog << "Warning! cannot create action of link '";
              clog << ncmLink->getId () << "' because ";
              clog << "number of binds is = " << size << endl;
              return NULL;
            }
        }
    }
  else
    { // CompoundAction)
      delayObject = actionExpression->getDelay ();
      delay = compileDelay (ncmLink, delayObject, NULL);
      cae = (CompoundAction *)actionExpression;
      return createCompoundAction (cae->getOperator (), delay,
                                   cae->getActions (), ncmLink,
                                   parentObject);
    }

  clog << "FormatterLinkConverter::createAction ";
  clog << "Warning! cannot create action of link '";
  clog << ncmLink->getId () << "' returning NULL" << endl;
  return NULL;
}

NclLinkCondition *
FormatterLinkConverter::createCondition (
    ConditionExpression *ncmExpression, CausalLink *ncmLink,
    NclCompositeExecutionObject *parentObject)
{
  if (ncmExpression->instanceOf ("TriggerExpression"))
    {
      return createCondition ((TriggerExpression *)ncmExpression, ncmLink,
                              parentObject);
    }
  else
    { // IStatement
      return createStatement ((Statement *)ncmExpression, ncmLink,
                              parentObject);
    }
}

NclLinkCompoundTriggerCondition *
FormatterLinkConverter::createCompoundTriggerCondition (
    short op, double delay,
    vector<ConditionExpression *> *ncmChildConditions, CausalLink *ncmLink,
    NclCompositeExecutionObject *parentObject)
{
  NclLinkCompoundTriggerCondition *condition;
  ConditionExpression *ncmChildCondition;
  NclLinkCondition *childCondition;

  if (op == CompoundCondition::OP_AND)
    {
      condition = new NclLinkAndCompoundTriggerCondition ();
    }
  else
    {
      condition = new NclLinkCompoundTriggerCondition ();
    }

  if (delay > 0)
    {
      condition->setDelay (delay);
    }

  if (ncmChildConditions != NULL)
    {
      vector<ConditionExpression *>::iterator i;
      i = ncmChildConditions->begin ();
      while (i != ncmChildConditions->end ())
        {
          ncmChildCondition = (*i);
          childCondition = createCondition (ncmChildCondition, ncmLink,
                                            parentObject);

          condition->addCondition (childCondition);
          ++i;
        }
    }

  return condition;
}

NclLinkCondition *
FormatterLinkConverter::createCondition (
    TriggerExpression *condition, CausalLink *ncmLink,
    NclCompositeExecutionObject *parentObject)
{
  double delay;
  SimpleCondition *ste;
  CompoundCondition *cte;
  vector<Bind *> *binds;
  int i, size;
  string delayObject;
  NclLinkCompoundTriggerCondition *compoundCondition;
  NclLinkTriggerCondition *simpleCondition;

  if (condition->instanceOf ("SimpleCondition"))
    {
      ste = (SimpleCondition *)condition;
      binds = ncmLink->getRoleBinds (ste);
      if (binds != NULL)
        {
          size = (int) binds->size ();
          if (size == 1)
            {
              return createSimpleCondition (ste, (*binds)[0], ncmLink,
                                            parentObject);
            }
          else if (size > 1)
            {
              if (ste->getQualifier () == CompoundCondition::OP_AND)
                {
                  compoundCondition
                      = new NclLinkAndCompoundTriggerCondition ();
                }
              else
                {
                  compoundCondition
                      = new NclLinkCompoundTriggerCondition ();
                }

              for (i = 0; i < size; i++)
                {
                  simpleCondition = createSimpleCondition (
                      ste, (*binds)[i], ncmLink, parentObject);

                  compoundCondition->addCondition (simpleCondition);
                }
              return compoundCondition;
            }
          else
            {
              clog << "FormatterLinkConverter::createCondition ";
              clog << "Warning! cannot create condition of link '";
              clog << ncmLink->getId () << "' because ";
              clog << "number of binds is = " << size << endl;
              return NULL;
            }
        }
    }
  else
    { // CompoundCondition
      delayObject = condition->getDelay ();
      delay = compileDelay (ncmLink, delayObject, NULL);
      cte = (CompoundCondition *)condition;
      return createCompoundTriggerCondition (cte->getOperator (), delay,
                                             cte->getConditions (), ncmLink,
                                             parentObject);
    }

  clog << "FormatterLinkConverter::createCondition ";
  clog << "Warning! cannot create condition of link '";
  clog << ncmLink->getId () << "' returning NULL" << endl;
  return NULL;
}

NclLinkAssessmentStatement *
FormatterLinkConverter::createAssessmentStatement (
    AssessmentStatement *assessmentStatement, Bind *bind, Link *ncmLink,
    NclCompositeExecutionObject *parentObject)
{
  NclLinkAttributeAssessment *mainAssessment;
  NclLinkAssessment *otherAssessment;
  AttributeAssessment *aa;
  NclLinkAssessmentStatement *statement;
  ValueAssessment *valueAssessment;
  string paramValue;
  Parameter *connParam, *param;
  vector<Bind *> *otherBinds;

  mainAssessment = createAttributeAssessment (
      assessmentStatement->getMainAssessment (), bind, ncmLink,
      parentObject);

  if (assessmentStatement->getOtherAssessment ()->instanceOf (
          "ValueAssessment"))
    {
      valueAssessment
          = (ValueAssessment *)(assessmentStatement->getOtherAssessment ());

      paramValue = valueAssessment->getValue ();
      if (paramValue[0] == '$')
        { // instanceOf("Parameter")
          connParam = new Parameter (
              paramValue.substr (1, paramValue.length () - 1), "");

          param = bind->getParameter (connParam->getName ());
          if (param == NULL)
            {
              param = ncmLink->getParameter (connParam->getName ());
            }

          if (param != NULL)
            {
              paramValue = param->getValue ();
            }
        }

      otherAssessment = new NclLinkValueAssessment (paramValue);
    }
  else
    {
      aa = (AttributeAssessment *)(assessmentStatement
                                       ->getOtherAssessment ());

      otherBinds = ncmLink->getRoleBinds (aa);
      if (otherBinds != NULL && !otherBinds->empty ())
        {
          otherAssessment = createAttributeAssessment (
              aa, (*otherBinds)[0], ncmLink, parentObject);
        }
      else
        {
          otherAssessment = createAttributeAssessment (
              aa, NULL, ncmLink, parentObject);
        }
    }
  statement = new NclLinkAssessmentStatement (
      assessmentStatement->getComparator (), mainAssessment,
      otherAssessment);

  return statement;
}

NclLinkStatement *
FormatterLinkConverter::createStatement (
    Statement *statementExpression, Link *ncmLink,
    NclCompositeExecutionObject *parentObject)
{
  AssessmentStatement *as;
  CompoundStatement *cs;
  vector<Bind *> *binds;
  int size;
  NclLinkStatement *statement;
  NclLinkStatement *childStatement;
  vector<Statement *> *statements;
  vector<Statement *>::iterator i;
  Statement *ncmChildStatement;

  if (statementExpression->instanceOf ("AssessmentStatement"))
    {
      as = (AssessmentStatement *)statementExpression;
      binds = ncmLink->getRoleBinds (as->getMainAssessment ());
      if (binds != NULL)
        {
          size = (int) binds->size ();
          if (size == 1)
            {
              statement = createAssessmentStatement (
                  as, (*binds)[0], ncmLink, parentObject);
            }
          else
            {
              clog << "FormatterLinkConverter::createStatement ";
              clog << "Warning! cannot create statement of link '";
              clog << ncmLink->getId () << "' because ";
              clog << "binds size = '" << size << "'" << endl;
              return NULL;
            }
        }
      else
        {
          clog << "FormatterLinkConverter::createStatement ";
          clog << "Warning! cannot create statement of link '";
          clog << ncmLink->getId () << "' because ";
          clog << "binds == NULL" << endl;
          return NULL;
        }
    }
  else
    { // CompoundStatement
      cs = (CompoundStatement *)statementExpression;
      statement = new NclLinkCompoundStatement (cs->getOperator ());
      ((NclLinkCompoundStatement *)statement)
          ->setNegated (cs->isNegated ());
      statements = cs->getStatements ();
      if (statements != NULL)
        {
          i = statements->begin ();
          while (i != statements->end ())
            {
              ncmChildStatement = (*i);
              childStatement = createStatement (ncmChildStatement, ncmLink,
                                                parentObject);

              ((NclLinkCompoundStatement *)statement)
                  ->addStatement (childStatement);

              ++i;
            }
        }
    }

  return statement;
}

NclLinkAttributeAssessment *
FormatterLinkConverter::createAttributeAssessment (
    AttributeAssessment *attributeAssessment, Bind *bind, Link *ncmLink,
    NclCompositeExecutionObject *parentObject)
{
  NclFormatterEvent *event;

  event = createEvent (bind, ncmLink, parentObject);
  return new NclLinkAttributeAssessment (
      event, attributeAssessment->getAttributeType ());
}

NclLinkSimpleAction *
FormatterLinkConverter::createSimpleAction (
    SimpleAction *sae, Bind *bind, Link *ncmLink,
    NclCompositeExecutionObject *parentObject)
{
  NclFormatterEvent *event;
  SimpleActionType actionType;
  short eventType = -1;
  NclLinkSimpleAction *action;
  Parameter *connParam;
  Parameter *param;
  string paramValue;
  Animation *animation;
  int repeat;
  double delay;
  Animation *newAnimation;
  bool isUsing;

  newAnimation = new Animation ();
  isUsing = false;
  action = NULL;
  event = createEvent (bind, ncmLink, parentObject);

  actionType = sae->getActionType ();
  if (event != NULL)
    {
      eventType = bind->getRole ()->getEventType ();
      event->setEventType (eventType);
    }
  else
    {
      clog << "FormatterLinkConverter::createSimpleAction Warning! ";
      clog << "Trying to create a simple action with a NULL event";
      clog << endl;
    }

  switch (actionType)
    {
    case ACT_START:
    case ACT_SET:
      if (eventType == EventUtil::EVT_PRESENTATION)
        {
          action = new NclLinkRepeatAction (event, actionType);

          // repeat
          paramValue = sae->getRepeat ();
          if (paramValue == "")
            {
              repeat = 0;
            }
          else if (paramValue[0] == '$')
            {
              connParam = new Parameter (
                  paramValue.substr (1, paramValue.length () - 1), "");

              param = bind->getParameter (connParam->getName ());
              if (param == NULL)
                {
                  param = ncmLink->getParameter (connParam->getName ());
                }

              if (param == NULL)
                {
                  repeat = 0;
                }
              else
                {
                  repeat = xstrtoint (param->getValue (), 10);
                }
            }
          else
            {
              repeat = xstrtoint (paramValue, 10);
            }

          ((NclLinkRepeatAction *)action)->setRepetitions (repeat);

          // repeatDelay
          paramValue = sae->getRepeatDelay ();
          delay = compileDelay (ncmLink, paramValue, bind);
          ((NclLinkRepeatAction *)action)->setRepetitionInterval (delay);
        }
      else if (eventType == EventUtil::EVT_ATTRIBUTION)
        {
          paramValue = sae->getValue ();
          if (paramValue != "" && paramValue[0] == '$')
            {
              connParam = new Parameter (
                  paramValue.substr (1, paramValue.length () - 1), "");

              param = bind->getParameter (connParam->getName ());
              if (param == NULL)
                {
                  param = ncmLink->getParameter (connParam->getName ());
                }

              delete connParam;
              connParam = NULL;

              if (param != NULL)
                {
                  paramValue = param->getValue ();
                }
              else
                {
                  paramValue = "";
                }
            }

          action
              = new NclLinkAssignmentAction (event, actionType, paramValue);

          // animation
          animation = sae->getAnimation ();

          if (animation != NULL)
            {
              string durVal = "0";
              string byVal = "0";

              paramValue = animation->getDuration ();
              if (paramValue[0] == '$')
                {
                  connParam = new Parameter (
                      paramValue.substr (1, paramValue.length () - 1), "");

                  param = bind->getParameter (connParam->getName ());
                  if (param == NULL)
                    {
                      param = ncmLink->getParameter (connParam->getName ());
                    }

                  delete connParam;
                  connParam = NULL;

                  if (param != NULL)
                    {
                      durVal = param->getValue ();
                    }

                  newAnimation->setDuration (durVal);
                }
              else
                {
                  durVal = paramValue;
                  newAnimation->setDuration (durVal);
                }

              paramValue = animation->getBy ();
              if (paramValue[0] == '$')
                {
                  connParam = new Parameter (
                      paramValue.substr (1, paramValue.length () - 1), "");

                  param = bind->getParameter (connParam->getName ());
                  if (param == NULL)
                    {
                      param = ncmLink->getParameter (connParam->getName ());
                    }

                  delete connParam;
                  connParam = NULL;

                  if (param != NULL)
                    {
                      byVal = param->getValue ();
                    }

                  newAnimation->setBy (byVal);
                }
              else
                {
                  byVal = paramValue;
                  newAnimation->setBy (byVal);
                }

              if (durVal != "0")
                {
                  isUsing = true;
                  ((NclLinkAssignmentAction *)action)
                      ->setAnimation (newAnimation);
                }
            }
        }
      else
        {
          clog << "FormatterLinkConverter::createSimpleAction ";
          clog << "Warning! Unknown event type '" << eventType;
          clog << "' for action type '";
          clog << actionType << "'" << endl;
        }
      break;

    case ACT_STOP:
    case ACT_PAUSE:
    case ACT_RESUME:
    case ACT_ABORT:
      action = new NclLinkSimpleAction (event, actionType);
      break;

    default:
      action = NULL;
      clog << "FormatterLinkConverter::createSimpleAction ";
      clog << "Warning! Unknown action type '";
      clog << actionType << "'" << endl;
      break;
    }

  if (action != NULL)
    {
      paramValue = sae->getDelay ();
      delay = compileDelay (ncmLink, paramValue, bind);
      action->setWaitDelay (delay);
    }

  if (!isUsing)
    {
      delete newAnimation;
    }

  return action;
}

NclLinkCompoundAction *
FormatterLinkConverter::createCompoundAction (
    short op, double delay, vector<Action *> *ncmChildActions,
    CausalLink *ncmLink, NclCompositeExecutionObject *parentObject)
{
  NclLinkCompoundAction *action;
  Action *ncmChildAction;
  NclLinkAction *childAction;

  action = new NclLinkCompoundAction (op);
  if (delay > 0)
    {
      action->setWaitDelay (delay);
    }

  if (ncmChildActions != NULL)
    {
      vector<Action *>::iterator i;
      i = ncmChildActions->begin ();
      while (i != ncmChildActions->end ())
        {
          ncmChildAction = (*i);
          childAction = createAction (ncmChildAction, ncmLink, parentObject);

          if (childAction != NULL)
            {
              action->addAction (childAction);
            }
          else
            {
              clog << "FormatterLinkConverter::createCompoundAction ";
              clog << "creating link '" << ncmLink->getId () << "' ";
              clog << "Warning! Can't create ";
              if (ncmChildAction->instanceOf ("SimpleAction"))
                {
                  clog << "simple action type '";
                  clog
                      << ((SimpleAction *)ncmChildAction)->getActionType ();
                  clog << "'" << endl;
                }
              else if (ncmChildAction->instanceOf ("CompoundAction"))
                {
                  clog << "inner compound action " << endl;
                }
            }
          ++i;
        }
    }

  return action;
}

NclLinkTriggerCondition *
FormatterLinkConverter::createSimpleCondition (
    SimpleCondition *simpleCondition, Bind *bind, Link *ncmLink,
    NclCompositeExecutionObject *parentObject)
{
  NclFormatterEvent *event;
  double delay;
  string delayObject;
  NclLinkTriggerCondition *condition;

  event = createEvent (bind, ncmLink, parentObject);
  condition = new NclLinkTransitionTriggerCondition (
      event, simpleCondition->getTransition (), bind);

  delayObject = simpleCondition->getDelay ();
  delay = compileDelay (ncmLink, delayObject, bind);
  if (delay > 0)
    {
      condition->setDelay (delay);
    }
  return condition;
}

NclFormatterEvent *
FormatterLinkConverter::createEvent (
    Bind *bind, Link *ncmLink, NclCompositeExecutionObject *parentObject)
{
  NclNodeNesting *endPointNodeSequence;
  NclNodeNesting *endPointPerspective;
  Node *parentNode;
  NclExecutionObject *executionObject;
  InterfacePoint *interfacePoint;
  string key;
  NclFormatterEvent *event = NULL;
  vector<Node *> *seq;

  endPointPerspective = parentObject->getNodePerspective ();

  // parent object may be a refer
  parentNode = endPointPerspective->getAnchorNode ();

  // teste para verificar se ponto terminal eh o proprio contexto ou
  // refer para o proprio contexto
  seq = bind->getNodeNesting ();
  endPointNodeSequence = new NclNodeNesting (seq);
  if (endPointNodeSequence->getAnchorNode ()
          != endPointPerspective->getAnchorNode ()
      && endPointNodeSequence->getAnchorNode ()
             != parentNode->getDataEntity ())
    {
      endPointPerspective->append (endPointNodeSequence);
    }

  delete seq;
  delete endPointNodeSequence;

  try
    {
      executionObject = compiler->getExecutionObjectFromPerspective (
          endPointPerspective, bind->getDescriptor ());

      if (executionObject == NULL)
        {
          clog << "FormatterLinkConverter::createEvent Warning! ";
          clog << "can't find execution object for perspective '";
          clog << endPointPerspective->getId () << "'" << endl;

          delete endPointPerspective;
          return NULL;
        }
    }
  catch (exception *exc)
    {
      clog << "FormatterLinkConverter::createEvent Warning! ";
      clog << "can't execution object exception for perspective '";
      clog << endPointPerspective->getId () << "'" << endl;

      delete endPointPerspective;
      return NULL;
    }

  interfacePoint = bind->getEndPointInterface ();
  if (interfacePoint == NULL)
    {
      // TODO: This is an error, the formatter then return the main event
      clog << "FormatterLinkConverter::createEvent Warning! ";
      clog << "can't find an interface point for '";
      clog << endPointPerspective->getId () << "' bind '";
      clog << bind->getRole ()->getLabel () << "'" << endl;
      delete endPointPerspective;
      return executionObject->getWholeContentPresentationEvent ();
    }

  key = getBindKey (ncmLink, bind);
  event = compiler->getEvent (executionObject, interfacePoint,
                              bind->getRole ()->getEventType (), key);

  delete endPointPerspective;
  return event;
}

double
FormatterLinkConverter::getDelayParameter (Link *ncmLink,
                                           Parameter *connParam,
                                           Bind *ncmBind)
{
  Parameter *parameter;
  string param;

  parameter = NULL;
  if (ncmBind != NULL)
    {
      parameter = ncmBind->getParameter (connParam->getName ());
    }

  if (parameter == NULL)
    {
      parameter = ncmLink->getParameter (connParam->getName ());
    }

  if (parameter == NULL)
    {
      return 0.0;
    }
  else
    {
      try
        {
          param = parameter->getValue ();
          if (param == "")
            {
              return 0.0;
            }
          else
            {
              return xstrtod (param) * 1000;
            }
        }
      catch (exception *exc)
        {
          return 0.0;
        }
    }
}

string
FormatterLinkConverter::getBindKey (Link *ncmLink, Bind *ncmBind)
{
  Role *role;
  string keyValue;
  Parameter *param, *auxParam;
  string key;

  role = ncmBind->getRole ();
  if (role == NULL)
    {
      return "";
    }

  if (role->instanceOf ("SimpleCondition"))
    {
      keyValue = ((SimpleCondition *)role)->getKey ();
    }
  else if (role->instanceOf ("AttributeAssessment"))
    {
      keyValue = ((AttributeAssessment *)role)->getKey ();
    }
  else
    {
      return "";
    }

  if (keyValue == "")
    {
      key = "";
    }
  else if (keyValue[0] == '$')
    { // instanceof Parameter
      param
          = new Parameter (keyValue.substr (1, keyValue.length () - 1), "");

      auxParam = ncmBind->getParameter (param->getName ());
      if (auxParam == NULL)
        {
          auxParam = ncmLink->getParameter (param->getName ());
        }

      if (auxParam != NULL)
        {
          key = auxParam->getValue ();
        }
      else
        {
          key = "";
        }

      delete param;
    }
  else
    {
      key = keyValue;
    }

  return key;
}

double
FormatterLinkConverter::compileDelay (Link *ncmLink, const string &delayObject,
                                      Bind *bind)
{
  double delay;
  string::size_type pos;
  Parameter *param;
  string delayValue;

  if (delayObject == "")
    {
      delay = 0;
    }
  else
    {
      pos = delayObject.find ("$");
      if (pos != std::string::npos && pos == 0)
        { // instanceof Parameter
          delayValue = delayObject.substr (1, delayObject.length () - 1);
          param = new Parameter (delayValue, "");
          delay = getDelayParameter (ncmLink, param, bind);
          delete param;
        }
      else
        {
          delay = xstrtod (delayObject);
        }
    }

  return delay;
}

GINGA_FORMATTER_END
