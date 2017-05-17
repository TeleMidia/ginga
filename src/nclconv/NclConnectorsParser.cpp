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

#include "NclConnectorsParser.h"

#include "NclParser.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_NCLCONV_BEGIN

NclConnectorsParser::NclConnectorsParser (NclParser *nclParser)
    : ModuleParser (nclParser)
{
}

SimpleCondition *
NclConnectorsParser::parseSimpleCondition (DOMElement *simpleCond_element)
{
  SimpleCondition *conditionExpression;
  string attValue, roleLabel;

  roleLabel = dom_element_get_attr(simpleCond_element, "role");

  conditionExpression = new SimpleCondition (roleLabel);

  compileRoleInformation (conditionExpression, simpleCond_element);

  // transition
  if (dom_element_has_attr(simpleCond_element, "transition"))
    {
      attValue = dom_element_get_attr(simpleCond_element, "transition");

      conditionExpression->setTransition (
            EventUtil::getTransitionCode (attValue));
    }

  // param
  if (conditionExpression->getEventType () == EventUtil::EVT_SELECTION)
    {
      if (dom_element_has_attr(simpleCond_element, "key"))

        {
          attValue = dom_element_get_attr(simpleCond_element, "key");

          conditionExpression->setKey (attValue);
        }
    }

  // qualifier
  if (dom_element_has_attr(simpleCond_element, "qualifier"))
    {
      attValue = dom_element_get_attr(simpleCond_element, "qualifier");

      if (attValue == "or")
        {
          conditionExpression->setQualifier (CompoundCondition::OP_OR);
        }
      else
        {
          conditionExpression->setQualifier (CompoundCondition::OP_AND);
        }
    }

  // delay
  if (dom_element_has_attr(simpleCond_element, "delay"))
    {
      attValue = dom_element_get_attr(simpleCond_element, "delay");

      if (attValue[0] == '$')
        {
          conditionExpression->setDelay (attValue);
        }
      else
        {
          double delayValue;
          delayValue = xstrtod (
                           attValue.substr (0, (attValue.length () - 1)))
                       * 1000;

          conditionExpression->setDelay (xstrbuild ("%d", (int) delayValue));
        }
    }

  return conditionExpression;
}

CompoundCondition *
NclConnectorsParser::parseCompoundCondition (DOMElement *compoundCond_element)
{
  CompoundCondition *compoundCond =
      createCompoundCondition (compoundCond_element);
  g_assert_nonnull (compoundCond);

  for ( DOMElement *child: dom_element_children(compoundCond_element) )
    {
      string tagname = dom_element_tagname(child);

      if ( tagname == "simpleCondition")
        {
          SimpleCondition *simpleCond = parseSimpleCondition (child);
          if (simpleCond)
            {
              addSimpleConditionToCompoundCondition (compoundCond, simpleCond);
            }
        }
      else if ( tagname == "assessmentStatement" )
        {
          AssessmentStatement *elementObj = parseAssessmentStatement (child);

          if (elementObj)
            {
              addAssessmentStatementToCompoundCondition (compoundCond,
                                                         elementObj);
            }
        }
      else if ( tagname == "compoundCondition")
        {
          CompoundCondition *compoundCond_child =
              parseCompoundCondition (child);

          if (compoundCond_child)
            {
              addCompoundConditionToCompoundCondition (compoundCond,
                                                       compoundCond_child);
            }
        }
      else if ( tagname ==  "compoundStatement")
        {
          CompoundStatement *compoundStatement = parseCompoundStatement (child);

          if (compoundStatement)
            {
              addCompoundStatementToCompoundCondition (compoundCond,
                                                       compoundStatement);
            }
        }
      else
        {
          syntax_warning( "'%s' is not known as child of 'compoundCondition'."
                          " It will be ignored.",
                          tagname.c_str() );
        }
    }

  return compoundCond;
}

AssessmentStatement *
NclConnectorsParser::parseAssessmentStatement (
    DOMElement *assessmentStatement_element)
{
  AssessmentStatement *assessmentStatement =
      createAssessmentStatement (assessmentStatement_element);
  g_assert_nonnull (assessmentStatement);

  for ( DOMElement *child: dom_element_children(assessmentStatement_element) )
    {
      string tagname = dom_element_tagname(child);
      if (tagname == "attributeAssessment")
        {
          AttributeAssessment *attributeAssessment =
              parseAttributeAssessment (child);
          if (attributeAssessment)
            {
              addAttributeAssessmentToAssessmentStatement (assessmentStatement,
                                                           attributeAssessment);
            }
        }
      else if (tagname == "valueAssessment")
        {
          ValueAssessment *elementObj = parseValueAssessment (child);
          if (elementObj)
            {
              addValueAssessmentToAssessmentStatement (assessmentStatement,
                                                       elementObj);
            }
        }
      else
        {
          syntax_warning( "'%s' is not known as child of 'assessmentStatement'."
                          " It will be ignored.",
                          tagname.c_str() );
        }
    }

  return assessmentStatement;
}

AttributeAssessment *
NclConnectorsParser::parseAttributeAssessment (DOMElement *attributeAssessment_element)
{
  AttributeAssessment *attributeAssessment;
  string attValue;

  string roleLabel = dom_element_get_attr(attributeAssessment_element, "role");

  attributeAssessment = new AttributeAssessment (roleLabel);

  // event type
  if (dom_element_has_attr(attributeAssessment_element, "eventType"))
    {
      attValue = dom_element_get_attr(attributeAssessment_element, "eventType");

      attributeAssessment->setEventType (EventUtil::getTypeCode (attValue));
    }

  // event type
  if (dom_element_has_attr(attributeAssessment_element, "attributeType"))
    {
      attValue = dom_element_get_attr(attributeAssessment_element, "attributeType");

      attributeAssessment->setAttributeType (
          EventUtil::getAttributeTypeCode (attValue));
    }

  // parameter
  if (attributeAssessment->getEventType () == EventUtil::EVT_SELECTION)
    {
      if (dom_element_has_attr(attributeAssessment_element, "key"))
        {
          attValue = dom_element_get_attr(attributeAssessment_element, "key");

          attributeAssessment->setKey (attValue);
        }
    }

  // testing offset
  if (dom_element_has_attr(attributeAssessment_element, "offset"))
    {
      attValue = dom_element_get_attr(attributeAssessment_element, "offset");

      attributeAssessment->setOffset (attValue);
    }

  return attributeAssessment;
}

ValueAssessment *
NclConnectorsParser::parseValueAssessment (DOMElement *valueAssessment_element)
{
  string attValue = dom_element_get_attr(valueAssessment_element, "value");

  return new ValueAssessment (attValue);
}

CompoundStatement *
NclConnectorsParser::parseCompoundStatement (
    DOMElement *compoundStatement_element)
{
  CompoundStatement *compoundStatement =
      createCompoundStatement (compoundStatement_element);
  g_assert_nonnull (compoundStatement);

  for ( DOMElement *child: dom_element_children(compoundStatement_element) )
    {
      string tagname = dom_element_tagname(child);

      if (tagname == "assessmentStatement")
        {
          AssessmentStatement *assessmentStatement =
              parseAssessmentStatement (child);
          if (assessmentStatement)
            {
              addAssessmentStatementToCompoundStatement (compoundStatement,
                                                         assessmentStatement);
            }
        }
      else if (tagname == "compoundStatement")
        {
          CompoundStatement *compoundStatement_child =
              parseCompoundStatement (child);
          if (compoundStatement_child)
            {
              addCompoundStatementToCompoundStatement (compoundStatement,
                                                       compoundStatement_child);
            }
        }
      else
        {
          syntax_warning( "'%s' is not known as child of 'compoundStatement'."
                          " It will be ignored.",
                          tagname.c_str() );
        }
    }

  return compoundStatement;
}

SimpleAction *
NclConnectorsParser::parseSimpleAction (DOMElement *simpleAction_element)
{
  SimpleAction *actionExpression;
  string attValue;

  attValue = dom_element_get_attr(simpleAction_element, "role");

  actionExpression = new SimpleAction (attValue);

  // transition
  if (dom_element_has_attr(simpleAction_element, "actionType"))
    {
      attValue = dom_element_get_attr(simpleAction_element, "actionType");

      actionExpression->setActionType (convertActionType (attValue));
    }

  if (dom_element_has_attr(simpleAction_element, "eventType"))
    {
      attValue = dom_element_get_attr(simpleAction_element, "eventType");

      actionExpression->setEventType (EventUtil::getTypeCode (attValue));
    }

  // animation
  if (actionExpression->getEventType () == EventUtil::EVT_ATTRIBUTION
      && actionExpression->getActionType () == ACT_START)
    {
      Animation *animation = NULL;
      string durVal = "";
      string byVal = "";

      if (dom_element_has_attr(simpleAction_element, "duration"))
        {
          durVal = dom_element_get_attr(simpleAction_element, "duration");
        }

      if (dom_element_has_attr(simpleAction_element, "by"))
        {
          byVal = dom_element_get_attr(simpleAction_element, "by");
        }

      if (durVal != "" || byVal != "")
        {
          animation = new Animation ();

          if (durVal[0] == '$')
            {
              animation->setDuration (durVal);
            }
          else
            {
              if (durVal.find ("s") != std::string::npos)
                {
                  animation->setDuration (xstrbuild ("%d", xstrto_int (durVal.substr (0, durVal.length () - 1))));
                }
              else
                {
                  animation->setDuration (xstrbuild ("%d", (xstrto_int (durVal))));
                }
            }

          if (byVal.find ("s") != std::string::npos)
            {
              animation->setBy (xstrbuild ("%d", (xstrto_int (byVal.substr (0, byVal.length () - 1)))));
            }
          else
            {
              guint64 i;
              if (_xstrtoull (byVal, &i))
                animation->setBy (xstrbuild ("%u", (guint) i));
              else
                animation->setBy ("indefinite"); // default
            }
        }

      actionExpression->setAnimation (animation);
    }

  compileRoleInformation (actionExpression, simpleAction_element);

  if (dom_element_has_attr(simpleAction_element, "qualifier"))
    {
      string qualifier = dom_element_get_attr(simpleAction_element,
                                              "qualifier");

      if (qualifier == "seq")
        {
          actionExpression->setQualifier (CompoundAction::OP_SEQ);
        }
      else
        { // any
          actionExpression->setQualifier (CompoundAction::OP_PAR);
        }
    }

  // testing delay
  if (dom_element_has_attr(simpleAction_element, "delay"))
    {
      attValue = dom_element_get_attr(simpleAction_element, "delay");

      if (attValue[0] == '$')
        {
          actionExpression->setDelay (attValue);
        }
      else
        {
          actionExpression->setDelay (
              xstrbuild ("%d", (xstrto_int (
                        attValue.substr (0, attValue.length () - 1))
                    * 1000)));
        }
    }

  //  testing repeatDelay
  if (dom_element_has_attr(simpleAction_element, "repeatDelay"))
    {
      attValue = dom_element_get_attr(simpleAction_element, "repeatDelay");

      actionExpression->setDelay (attValue);
      if (attValue[0] == '$')
        {
          actionExpression->setDelay (attValue);
        }
      else
        {
          actionExpression->setDelay (
              xstrbuild ("%d", (xstrto_int (
                        attValue.substr (0, attValue.length () - 1))
                    * 1000)));
        }
    }

  // repeat
  if (dom_element_has_attr(simpleAction_element, "repeat"))
    {
      attValue = dom_element_get_attr(simpleAction_element, "repeat");

      if (attValue == "indefinite")
        {
          // This is insane :@
          actionExpression->setRepeat (xstrbuild ("%d", 2 ^ 30));
        }
      else
        {
          actionExpression->setRepeat (attValue);
        }
    }

  // testing value
  if (dom_element_has_attr(simpleAction_element, "value"))
    {
      attValue = dom_element_get_attr(simpleAction_element, "value");

      actionExpression->setValue (attValue);
    }

  // returning action expression
  return actionExpression;
}

CompoundAction *
NclConnectorsParser::parseCompoundAction (DOMElement *compoundAction_element)
{
  CompoundAction *compoundAction =
      createCompoundAction (compoundAction_element);
  g_assert_nonnull (compoundAction);

  for (DOMElement *child: dom_element_children(compoundAction_element))
    {
      string tagname = dom_element_tagname(child);
      if (tagname == "simpleAction")
        {
          SimpleAction *simpleAction = parseSimpleAction (child);
          if (simpleAction)
            {
              addSimpleActionToCompoundAction (compoundAction, simpleAction);
            }
        }
      else if (tagname == "compoundAction")
        {
          CompoundAction *elementObject = parseCompoundAction (child);
          if (elementObject)
            {
              addCompoundActionToCompoundAction (compoundAction, elementObject);
            }
        }
      else
        {
          syntax_warning( "'%s' is not known as child of 'compoundAction'."
                          " It will be ignored.",
                          tagname.c_str() );
        }
    }

  return compoundAction;
}

Parameter *
NclConnectorsParser::parseConnectorParam (DOMElement *connectorParam_element)
{
  Parameter *parameter;
  parameter = new Parameter (
        dom_element_get_attr(connectorParam_element, "name"),
        dom_element_get_attr(connectorParam_element, "type") );

  return parameter;
}

CausalConnector *
NclConnectorsParser::parseCausalConnector (DOMElement *causalConnector_element)
{
  // pre-compile attributes
  CausalConnector *causalConnector =
      createCausalConnector (causalConnector_element);
  g_assert_nonnull (causalConnector);

  for (DOMElement *child: dom_element_children(causalConnector_element) )
    {
      string tagname = dom_element_tagname(child);

      if (tagname == "simpleCondition")
        {
          SimpleCondition *simpleCondition = parseSimpleCondition (child);

          if (simpleCondition)
            {
              addSimpleConditionToCausalConnector (causalConnector, simpleCondition);
            }
        }
      else if (tagname == "simpleAction")
        {
          SimpleAction *simpleAction = parseSimpleAction (child);

          if (simpleAction)
            {
              addSimpleActionToCausalConnector (causalConnector, simpleAction);
            }
        }
      else if (tagname == "compoundAction")
        {
          CompoundAction *compoundAction = parseCompoundAction (child);

          if (compoundAction)
            {
              addCompoundActionToCausalConnector (causalConnector, compoundAction);
            }
        }
      else if (tagname == "connectorParam")
        {
          Parameter *param = parseConnectorParam (child);

          if (param)
            {
              addConnectorParamToCausalConnector (causalConnector, param);
            }
        }
      else if (tagname == "compoundCondition")
        {
          CompoundCondition *compoundCond = parseCompoundCondition (child);

          if (compoundCond)
            {
              addCompoundConditionToCausalConnector (causalConnector,
                                                     compoundCond);
            }
        }
      else
        {
          syntax_warning( "'%s' is not known as child of 'causalConnector'."
                          " It will be ignored.",
                          tagname.c_str() );
        }
    }

  return causalConnector;
}

ConnectorBase *
NclConnectorsParser::parseConnectorBase (DOMElement *connectorBase_element)
{
  ConnectorBase *connectorBase =
      createConnectorBase (connectorBase_element);
  g_assert_nonnull (connectorBase);

  for (DOMElement *child: dom_element_children(connectorBase_element))
    {
      string tagname = dom_element_tagname(child);
      if (tagname == "importBase")
        {
          DOMElement *importBase_element =
              _nclParser->getImportParser ()->parseImportBase (child);
          if (importBase_element)
            {
              addImportBaseToConnectorBase (connectorBase, importBase_element);
            }
        }
      else if (tagname ==  "causalConnector")
        {
          CausalConnector *causalConnector = parseCausalConnector (child);
          if (causalConnector)
            {
              addCausalConnectorToConnectorBase (connectorBase,
                                                 causalConnector);
            }
        }
      else
        {
          syntax_warning( "'%s' is not known as child of 'connectorBase'."
                          " It will be ignored.",
                          tagname.c_str() );
        }
    }

  return connectorBase;
}

void
NclConnectorsParser::addCausalConnectorToConnectorBase (
    ConnectorBase *connectorBase, Connector *causalConnector)
{
  connectorBase->addConnector (causalConnector);
}

void
NclConnectorsParser::addConnectorParamToCausalConnector (
    Connector *connector, Parameter *param)
{
  connector->addParameter (param);
}

void
NclConnectorsParser::addImportBaseToConnectorBase (ConnectorBase *connectorBase,
                                                   DOMElement *childObject)
{
  string baseAlias, baseLocation;
  NclParser *compiler;
  NclDocument *importedDocument;
  ConnectorBase *importedConnectorBase;

  // get the external base alias and location
  baseAlias = dom_element_get_attr(childObject, "alias");
  baseLocation = dom_element_get_attr(childObject, "documentURI");

  compiler = getNclParser ();

  importedDocument = compiler->importDocument (baseLocation);
  if (unlikely (importedDocument == NULL))
    {
      syntax_error ("importBase '%s': bad documentURI '%s'",
                    baseAlias.c_str (),
                    baseLocation.c_str ());
    }

  importedConnectorBase = importedDocument->getConnectorBase ();
  if (unlikely (importedConnectorBase == NULL))
    {
      syntax_error ("importBase '%s': no connector base in '%s'",
                    baseAlias.c_str (),
                    baseLocation.c_str ());
    }

  connectorBase->addBase (importedConnectorBase, baseAlias, baseLocation);
}

CausalConnector *
NclConnectorsParser::createCausalConnector (DOMElement *causalConnector_element)
{
  string connectorId = dom_element_get_attr(causalConnector_element, "id");
  CausalConnector *causalConn = new CausalConnector (connectorId);

  this->connector = causalConn;

  return causalConn;
}

ConnectorBase *
NclConnectorsParser::createConnectorBase (DOMElement *parentElement)
{
  ConnectorBase *connBase;
  string connBaseId= dom_element_get_attr(parentElement, "id");
  connBase = new ConnectorBase (connBaseId);
  return connBase;
}

void
NclConnectorsParser::compileRoleInformation (Role *role,
                                             DOMElement *role_element)
{
  string attValue;
  // event type
  if (dom_element_has_attr(role_element, "eventType"))
    {
      attValue = dom_element_get_attr(role_element, "eventType");
      role->setEventType (EventUtil::getTypeCode (attValue));
    }

  //  cardinality
  if (dom_element_has_attr(role_element, "min"))
    {
      attValue = dom_element_get_attr(role_element, "min");
      role->setMinCon ((xstrto_int (attValue)));
    }

  if (dom_element_has_attr(role_element, "max"))
    {
      attValue = dom_element_get_attr(role_element,"max");

      if (attValue == "unbounded")
        {
          role->setMaxCon (Role::UNBOUNDED);
        }
      else
        {
          role->setMaxCon (xstrto_int (attValue));
        }
    }
}

CompoundCondition *
NclConnectorsParser::createCompoundCondition (DOMElement *compoundCond_element)
{
  CompoundCondition *conditionExpression;
  string attValue;

  conditionExpression = new CompoundCondition ();

  string op = dom_element_get_attr(compoundCond_element, "operator");

  if (op == "and")
    {
      conditionExpression->setOperator (CompoundCondition::OP_AND);
    }
  else
    {
      conditionExpression->setOperator (CompoundCondition::OP_OR);
    }

  // delay
  if (dom_element_has_attr(compoundCond_element, "delay"))
    {
      attValue = dom_element_get_attr(compoundCond_element, "delay");

      if (attValue[0] == '$')
        {
          conditionExpression->setDelay (attValue);
        }
      else
        {
          double delayValue = xstrtod (attValue.substr (
                                  0, (attValue.length () - 1)))
                              * 1000;

          conditionExpression->setDelay (xstrbuild ("%d", (int) delayValue));
        }
    }

  return conditionExpression;
}


AssessmentStatement *
NclConnectorsParser::createAssessmentStatement (DOMElement *assessmentStatement_element)
{
  AssessmentStatement *assessmentStatement;
  string attValue;

  if (dom_element_has_attr(assessmentStatement_element, "comparator"))
    {
      attValue = dom_element_get_attr(assessmentStatement_element, "comparator");

      assessmentStatement
          = new AssessmentStatement (Comparator::fromString (attValue));
    }
  else
    {
      assessmentStatement = new AssessmentStatement (Comparator::CMP_EQ);
    }

  return assessmentStatement;
}

CompoundStatement *
NclConnectorsParser::createCompoundStatement (DOMElement *parentElement)
{
  CompoundStatement *compoundStatement = new CompoundStatement ();

  string op = dom_element_get_attr(parentElement, "operator");
  if (op == "and")
    {
      compoundStatement->setOperator (CompoundStatement::OP_AND);
    }
  else
    {
      compoundStatement->setOperator (CompoundStatement::OP_OR);
    }

  // testing isNegated
  if (dom_element_has_attr(parentElement, "isNegated"))
    {
      string attValue =
          dom_element_get_attr(parentElement, "isNegated");

      compoundStatement->setNegated (attValue == "true");
    }

  return compoundStatement;
}

CompoundAction *
NclConnectorsParser::createCompoundAction (DOMElement *compoundAction_element)
{
  CompoundAction *actionExpression = new CompoundAction ();
  string op = dom_element_get_attr(compoundAction_element, "operator");;

  if (op == "seq")
    {
      actionExpression->setOperator (CompoundAction::OP_SEQ);
    }
  else
    {
      actionExpression->setOperator (CompoundAction::OP_PAR);
    }

  //  testar delay
  if (dom_element_has_attr(compoundAction_element, "delay"))
    {
      string attValue = dom_element_get_attr(compoundAction_element, "delay");

      if (attValue[0] == '$')
        {
          actionExpression->setDelay (attValue);
        }
      else
        {
          actionExpression->setDelay (
              xstrbuild ("%d", (xstrto_int (
                        attValue.substr (0, attValue.length () - 1))
                    * 1000)));
        }
    }

  return actionExpression;
}

Parameter *
NclConnectorsParser::getParameter (const string &paramName)
{
  return (Parameter *)(connector->getParameter (paramName));
}

SimpleActionType
NclConnectorsParser::convertActionType (const string &s)
{
  return SimpleAction::stringToActionType (s);
}

short
NclConnectorsParser::convertEventState (const string &eventState)
{
  if (eventState == "occurring")
    {
      return EventUtil::ST_OCCURRING;
    }
  else if (eventState == "paused")
    {
      return EventUtil::ST_PAUSED;
    }
  else if (eventState == "sleeping")
    {
      return EventUtil::ST_SLEEPING;
    }

  return -1;
}

void
NclConnectorsParser::addSimpleConditionToCompoundCondition (
    CompoundCondition *compoundCond, ConditionExpression *condExp)
{
  compoundCond->addConditionExpression (condExp);
}

void
NclConnectorsParser::addCompoundConditionToCompoundCondition (
    CompoundCondition *compoundCond, ConditionExpression *condExp)
{
  compoundCond->addConditionExpression (condExp);
}

void
NclConnectorsParser::addAssessmentStatementToCompoundCondition (
    CompoundCondition *compoundCond, ConditionExpression *condExp)
{
  compoundCond->addConditionExpression (condExp);
}

void
NclConnectorsParser::addCompoundStatementToCompoundCondition (
    CompoundCondition *compoundCond, ConditionExpression *condExp)
{
  compoundCond->addConditionExpression (condExp);
}

void
NclConnectorsParser::addAttributeAssessmentToAssessmentStatement (
    AssessmentStatement *asssessmentStatement,
    AttributeAssessment *attributeAssessment)
{
  if (asssessmentStatement->getMainAssessment () == NULL)
    {
      asssessmentStatement->setMainAssessment (attributeAssessment);
    }
  else
    {
      asssessmentStatement->setOtherAssessment (attributeAssessment);
    }
}

void
NclConnectorsParser::addValueAssessmentToAssessmentStatement (
    AssessmentStatement *parentObject, ValueAssessment *childObject)
{
  parentObject->setOtherAssessment (childObject);
}

void
NclConnectorsParser::addAssessmentStatementToCompoundStatement (
    CompoundStatement *compoundStatement, Statement *statement)
{
  compoundStatement->addStatement (statement);
}

void
NclConnectorsParser::addCompoundStatementToCompoundStatement (
    CompoundStatement *compoundStatement, Statement *statement)
{
  compoundStatement->addStatement (statement);
}

void
NclConnectorsParser::addSimpleActionToCompoundAction (CompoundAction *compoundAction,
                                                      Action *action)
{
  compoundAction->addAction (action);
}

void
NclConnectorsParser::addCompoundActionToCompoundAction (
    CompoundAction *compoundAction, Action *action)
{
  compoundAction->addAction (action);
}

void
NclConnectorsParser::addSimpleConditionToCausalConnector (
    CausalConnector *causalConnector, ConditionExpression *condExp)
{
  causalConnector->setConditionExpression (condExp);
}

void
NclConnectorsParser::addCompoundConditionToCausalConnector (
    CausalConnector *causalConnector, ConditionExpression *condExp)
{
  causalConnector->setConditionExpression (condExp);
}

void
NclConnectorsParser::addSimpleActionToCausalConnector (
    CausalConnector *causalConnector, Action *action)
{
  causalConnector->setAction (action);
}

void
NclConnectorsParser::addCompoundActionToCausalConnector (
    CausalConnector *causalConnector, Action *action)
{
  causalConnector->setAction (action);
}

GINGA_NCLCONV_END
