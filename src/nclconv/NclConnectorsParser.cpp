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

#include "NclDocumentParser.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_NCLCONV_BEGIN

NclConnectorsParser::NclConnectorsParser (NclDocumentParser *documentParser)
    : ModuleParser (documentParser)
{
}

void *
NclConnectorsParser::parseSimpleCondition (DOMElement *parentElement,
                                           void *objGrandParent)
{
  SimpleCondition *conditionExpression;
  string attValue, roleLabel;

  roleLabel = _documentParser->getAttribute(parentElement, "role");

  conditionExpression = new SimpleCondition (roleLabel);

  compileRoleInformation (conditionExpression, parentElement);

  // transition
  if (_documentParser->hasAttribute(parentElement, "transition"))
    {
      attValue = _documentParser->getAttribute(parentElement, "transition");

      conditionExpression->setTransition (
          EventUtil::getTransitionCode (attValue));
    }

  // parametro
  if (conditionExpression->getEventType () == EventUtil::EVT_SELECTION)
    {
      if (_documentParser->hasAttribute(parentElement, "key"))

        {
          attValue = _documentParser->getAttribute(parentElement, "key");

          conditionExpression->setKey (attValue);
        }
    }

  // qualifier
  if (_documentParser->hasAttribute(parentElement, "qualifier"))
    {
      attValue = _documentParser->getAttribute(parentElement, "qualifier");

      if (attValue == "or")
        {
          conditionExpression->setQualifier (CompoundCondition::OP_OR);
        }
      else
        {
          conditionExpression->setQualifier (CompoundCondition::OP_AND);
        }
    }

  // testar delay
  if (_documentParser->hasAttribute(parentElement, "delay"))
    {
      attValue = _documentParser->getAttribute(parentElement, "delay");

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

void *
NclConnectorsParser::parseCompoundCondition (DOMElement *parentElement,
                                             arg_unused(void *objGrandParent))
{
  void *parentObject = NULL;
  DOMNodeList *elementNodeList;
  DOMElement *element;
  DOMNode *node;
  string elementTagName = "";
  void *elementObject = NULL;

  parentObject = createCompoundCondition (parentElement, objGrandParent);
  g_assert_nonnull (parentObject);

  elementNodeList = parentElement->getChildNodes ();
  for (int i = 0; i < (int)elementNodeList->getLength (); i++)
    {
      node = elementNodeList->item (i);
      if (node->getNodeType () == DOMNode::ELEMENT_NODE)
        {
          element = (DOMElement *)node;
          string tagname = _documentParser->getTagname(element);

          if (XMLString::compareIString (tagname.c_str (), "simpleCondition")
              == 0)
            {
              elementObject = parseSimpleCondition (element, parentObject);
              if (elementObject != NULL)
                {
                  addSimpleConditionToCompoundCondition (parentObject,
                                                         elementObject);
                }
            }
          else if (XMLString::compareIString (tagname.c_str (),
                                              "assessmentStatement")
                   == 0)
            {
              elementObject
                  = parseAssessmentStatement (element, parentObject);

              if (elementObject != NULL)
                {
                  addAssessmentStatementToCompoundCondition (parentObject,
                                                             elementObject);
                }
            }
          else if (XMLString::compareIString (elementTagName.c_str (),
                                              "compoundCondition")
                   == 0)
            {
              elementObject
                  = parseCompoundCondition (element, parentObject);

              if (elementObject != NULL)
                {
                  addCompoundConditionToCompoundCondition (parentObject,
                                                           elementObject);
                }
            }
          else if (XMLString::compareIString (elementTagName.c_str (),
                                              "compoundStatement")
                   == 0)
            {
              elementObject
                  = parseCompoundStatement (element, parentObject);

              if (elementObject != NULL)
                {
                  addCompoundStatementToCompoundCondition (parentObject,
                                                           elementObject);
                }
            }
        }
    }

  return parentObject;
}

void *
NclConnectorsParser::parseAssessmentStatement (DOMElement *parentElement,
                                               void *objGrandParent)
{
  void *parentObject = NULL;
  DOMNodeList *elementNodeList;
  DOMElement *element;
  DOMNode *node;
  string elementTagName = "";
  void *elementObject = NULL;

  parentObject = createAssessmentStatement (parentElement, objGrandParent);
  g_assert_nonnull (parentObject);

  elementNodeList = parentElement->getChildNodes ();
  for (int i = 0; i < (int)elementNodeList->getLength (); i++)
    {
      node = elementNodeList->item (i);
      if (node->getNodeType () == DOMNode::ELEMENT_NODE)
        {
          element = (DOMElement *)node;
          string tagname = _documentParser->getTagname(element);
          if (XMLString::compareIString (tagname.c_str (),
                                         "attributeAssessment")
              == 0)
            {
              elementObject
                  = parseAttributeAssessment (element, parentObject);

              if (elementObject != NULL)
                {
                  addAttributeAssessmentToAssessmentStatement (
                      parentObject, elementObject);
                }
            }
          else if (XMLString::compareIString (tagname.c_str (),
                                              "valueAssessment")
                   == 0)
            {
              elementObject = parseValueAssessment (element, parentObject);
              if (elementObject != NULL)
                {
                  addValueAssessmentToAssessmentStatement (parentObject,
                                                           elementObject);
                }
            }
        }
    }

  return parentObject;
}

void *
NclConnectorsParser::parseAttributeAssessment (DOMElement *parentElement,
                                              arg_unused (void *objGrandParent))
{
  AttributeAssessment *attributeAssessment;
  string attValue;

  string roleLabel = _documentParser->getAttribute(parentElement, "role");

  attributeAssessment = new AttributeAssessment (roleLabel);

  // event type
  if (_documentParser->hasAttribute(parentElement, "eventType"))
    {
      attValue = _documentParser->getAttribute(parentElement, "eventType");

      attributeAssessment->setEventType (EventUtil::getTypeCode (attValue));
    }

  // event type
  if (_documentParser->hasAttribute(parentElement, "attributeType"))
    {
      attValue = _documentParser->getAttribute(parentElement, "attributeType");

      attributeAssessment->setAttributeType (
          EventUtil::getAttributeTypeCode (attValue));
    }

  // parameter
  if (attributeAssessment->getEventType () == EventUtil::EVT_SELECTION)
    {
      if (_documentParser->hasAttribute(parentElement, "key"))
        {
          attValue = _documentParser->getAttribute(parentElement, "key");

          attributeAssessment->setKey (attValue);
        }
    }

  // testing offset
  if (_documentParser->hasAttribute(parentElement, "offset"))
    {
      attValue = _documentParser->getAttribute(parentElement, "offset");

      attributeAssessment->setOffset (attValue);
    }

  return attributeAssessment;
}

void *
NclConnectorsParser::parseValueAssessment (DOMElement *parentElement,
                                           arg_unused(void *objGrandParent))
{
  string attValue = _documentParser->getAttribute(parentElement, "value");

  return new ValueAssessment (attValue);
}

void *
NclConnectorsParser::parseCompoundStatement (DOMElement *parentElement,
                                             void *objGrandParent)
{
  void *parentObject = NULL;
  DOMNodeList *elementNodeList;
  DOMElement *element;
  DOMNode *node;
  string elementTagName = "";
  void *elementObject = NULL;

  parentObject = createCompoundStatement (parentElement, objGrandParent);
  g_assert_nonnull (parentObject);

  elementNodeList = parentElement->getChildNodes ();
  for (int i = 0; i < (int)elementNodeList->getLength (); i++)
    {
      node = elementNodeList->item (i);
      if (node->getNodeType () == DOMNode::ELEMENT_NODE)
        {
          element = (DOMElement *)node;
          string tagname = _documentParser->getTagname(element);
          if (XMLString::compareIString (tagname.c_str (),
                                         "assessmentStatement")
              == 0)
            {
              elementObject
                  = parseAssessmentStatement (element, parentObject);

              if (elementObject != NULL)
                {
                  addAssessmentStatementToCompoundStatement (parentObject,
                                                             elementObject);
                }
            }
          else if (XMLString::compareIString (elementTagName.c_str (),
                                              "compoundStatement")
                   == 0)
            {
              elementObject
                  = parseCompoundStatement (element, parentObject);

              if (elementObject != NULL)
                {
                  addCompoundStatementToCompoundStatement (parentObject,
                                                           elementObject);
                }
            }
        }
    }

  return parentObject;
}

void *
NclConnectorsParser::parseSimpleAction (DOMElement *parentElement,
                                        arg_unused (void *objGrandParent))
{
  SimpleAction *actionExpression;
  string attValue;

  attValue = _documentParser->getAttribute(parentElement, "role");

  actionExpression = new SimpleAction (attValue);

  // transition
  if (_documentParser->hasAttribute(parentElement, "actionType"))
    {
      attValue = _documentParser->getAttribute(parentElement, "actionType");

      actionExpression->setActionType (convertActionType (attValue));
    }

  if (_documentParser->hasAttribute(parentElement, "eventType"))
    {
      attValue = _documentParser->getAttribute(parentElement, "eventType");

      actionExpression->setEventType (EventUtil::getTypeCode (attValue));
    }

  // animation
  if (actionExpression->getEventType () == EventUtil::EVT_ATTRIBUTION
      && actionExpression->getActionType () == ACT_START)
    {
      Animation *animation = NULL;
      string durVal = "";
      string byVal = "";

      if (_documentParser->hasAttribute(parentElement, "duration"))
        {
          durVal = _documentParser->getAttribute(parentElement, "duration");
        }

      if (_documentParser->hasAttribute(parentElement, "by"))
        {
          byVal = _documentParser->getAttribute(parentElement, "by");
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

  compileRoleInformation (actionExpression, parentElement);

  if (_documentParser->hasAttribute(parentElement, "qualifier"))
    {
      string qualifier = _documentParser->getAttribute(parentElement,
                                                       "qualifier");

      if (XMLString::compareIString (qualifier.c_str(), "seq") == 0)
        {
          actionExpression->setQualifier (CompoundAction::OP_SEQ);
        }
      else
        { // any
          actionExpression->setQualifier (CompoundAction::OP_PAR);
        }
    }

  // testing delay
  if (_documentParser->hasAttribute(parentElement, "delay"))
    {
      attValue = _documentParser->getAttribute(parentElement, "delay");

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
  if (_documentParser->hasAttribute(parentElement, "repeatDelay"))
    {
      attValue = _documentParser->getAttribute(parentElement, "repeatDelay");

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
  if (_documentParser->hasAttribute(parentElement, "repeat"))
    {
      attValue = _documentParser->getAttribute(parentElement, "repeat");

      if (XMLString::compareIString (attValue.c_str (), "indefinite") == 0)
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
  if (_documentParser->hasAttribute(parentElement, "value"))
    {
      attValue = _documentParser->getAttribute(parentElement, "value");

      actionExpression->setValue (attValue);
    }

  // returning action expression
  return actionExpression;
}

void *
NclConnectorsParser::parseCompoundAction (DOMElement *parentElement,
                                          void *objGrandParent)
{
  void *parentObject = NULL;
  DOMNodeList *elementNodeList;
  DOMElement *element;
  DOMNode *node;
  void *elementObject = NULL;

  parentObject = createCompoundAction (parentElement, objGrandParent);
  g_assert_nonnull (parentObject);

  elementNodeList = parentElement->getChildNodes ();
  for (int i = 0; i < (int)elementNodeList->getLength (); i++)
    {
      node = elementNodeList->item (i);
      if (node->getNodeType () == DOMNode::ELEMENT_NODE)
        {
          element = (DOMElement *)node;
          string tagname = _documentParser->getTagname(element);
          if (XMLString::compareIString (tagname.c_str (), "simpleAction")
              == 0)
            {
              elementObject = parseSimpleAction (element, parentObject);
              if (elementObject != NULL)
                {
                  addSimpleActionToCompoundAction (parentObject,
                                                   elementObject);
                }
            }
          else if (XMLString::compareIString (tagname.c_str (),
                                              "compoundAction")
                   == 0)
            {
              elementObject = parseCompoundAction (element, parentObject);
              if (elementObject != NULL)
                {
                  addCompoundActionToCompoundAction (parentObject,
                                                     elementObject);
                }
            }
        }
    }

  return parentObject;
}

void *
NclConnectorsParser::parseConnectorParam (DOMElement *parentElement,
                                          void *objGrandParent)
{
  Parameter *parameter;
  parameter = new Parameter (
        _documentParser->getAttribute(parentElement, "name"),
        _documentParser->getAttribute(parentElement, "type") );

  return parameter;
}

void *
NclConnectorsParser::parseCausalConnector (DOMElement *parentElement,
                                           void *objGrandParent)
{
  void *parentObject = NULL;
  DOMNodeList *elementNodeList;
  DOMElement *element;
  DOMNode *node;
  void *elementObject = NULL;

  // pre-compile attributes

  parentObject = createCausalConnector (parentElement, objGrandParent);
  g_assert_nonnull (parentObject);

  elementNodeList = parentElement->getChildNodes ();
  for (int i = 0; i < (int)elementNodeList->getLength (); i++)
    {
      node = elementNodeList->item (i);
      if (node->getNodeType () == DOMNode::ELEMENT_NODE)
        {
          element = (DOMElement *)node;
          string tagname = _documentParser->getTagname(element);

          if (XMLString::compareIString (tagname.c_str (), "simpleCondition")
              == 0)
            {
              elementObject = parseSimpleCondition (element, parentObject);

              if (elementObject != NULL)
                {
                  addSimpleConditionToCausalConnector (parentObject,
                                                       elementObject);
                }
            }
          else if (XMLString::compareIString (tagname.c_str (), "simpleAction")
                   == 0)
            {
              elementObject = parseSimpleAction (element, parentObject);

              if (elementObject != NULL)
                {
                  addSimpleActionToCausalConnector (parentObject,
                                                    elementObject);
                }
            }
          else if (XMLString::compareIString (tagname.c_str (),
                                              "compoundAction")
                   == 0)
            {
              elementObject = parseCompoundAction (element, parentObject);

              if (elementObject != NULL)
                {
                  addCompoundActionToCausalConnector (parentObject,
                                                      elementObject);
                }
            }
          else if (XMLString::compareIString (tagname.c_str (),
                                              "connectorParam")
                   == 0)
            {
              elementObject = parseConnectorParam (element, parentObject);

              if (elementObject != NULL)
                {
                  addConnectorParamToCausalConnector (parentObject,
                                                      elementObject);
                }
            }
          else if (XMLString::compareIString (tagname.c_str (),
                                              "compoundCondition")
                   == 0)
            {
              elementObject
                  = parseCompoundCondition (element, parentObject);

              if (elementObject != NULL)
                {
                  addCompoundConditionToCausalConnector (parentObject,
                                                         elementObject);
                }
            }
        }
    }

  return parentObject;
}

ConnectorBase *
NclConnectorsParser::parseConnectorBase (DOMElement *parentElement,
                                         void *objGrandParent)
{
  ConnectorBase *parentObject = NULL;
  DOMNodeList *elementNodeList;
  DOMElement *element;
  DOMNode *node;
  void *elementObject = NULL;

  parentObject = createConnectorBase (parentElement, objGrandParent);
  g_assert_nonnull (parentObject);

  elementNodeList = parentElement->getChildNodes ();
  for (int i = 0; i < (int)elementNodeList->getLength (); i++)
    {
      node = elementNodeList->item (i);
      if (node->getNodeType () == DOMNode::ELEMENT_NODE)
        {
          element = (DOMElement *)node;
          string elementTagName = _documentParser->getTagname(element);

          if (XMLString::compareIString (elementTagName.c_str (), "importBase")
              == 0)
            {
              elementObject = _documentParser->getImportParser ()
                      ->parseImportBase (element);

              if (elementObject != NULL)
                {
                  addImportBaseToConnectorBase (parentObject, elementObject);
                }
            }
          else if (XMLString::compareIString (elementTagName.c_str (),
                                              "causalConnector")
                   == 0)
            {
              elementObject = parseCausalConnector (element, parentObject);
              if (elementObject != NULL)
                {
                  addCausalConnectorToConnectorBase (parentObject,
                                                     elementObject);
                }
            }
        }
    }

  return parentObject;
}

void
NclConnectorsParser::addCausalConnectorToConnectorBase (
    void *parentObject, void *childObject)
{
  ((ConnectorBase *)parentObject)->addConnector ((Connector *)childObject);
}

void
NclConnectorsParser::addConnectorParamToCausalConnector (
    void *parentObject, void *childObject)
{
  ((Connector *)parentObject)->addParameter ((Parameter *)childObject);
}

void
NclConnectorsParser::addImportBaseToConnectorBase (void *parentObject,
                                                   void *childObject)
{
  string baseAlias, baseLocation;
  NclDocumentParser *compiler;
  NclDocument *importedDocument;
  ConnectorBase *connectorBase;

  // get the external base alias and location
  baseAlias = _documentParser->getAttribute((DOMElement *)childObject, "alias");
  baseLocation = _documentParser->getAttribute((DOMElement *)childObject,
                                               "documentURI");

  compiler = getDocumentParser ();

  importedDocument = compiler->importDocument (baseLocation);
  if (unlikely (importedDocument == NULL))
    {
      syntax_error ("importBase '%s': bad documentURI '%s'",
                    baseAlias.c_str (),
                    baseLocation.c_str ());
    }

  connectorBase = importedDocument->getConnectorBase ();
  if (unlikely (connectorBase == NULL))
    {
      syntax_error ("importBase '%s': no connector base in '%s'",
                    baseAlias.c_str (),
                    baseLocation.c_str ());
    }

  ((ConnectorBase *)parentObject)
      ->addBase (connectorBase, baseAlias, baseLocation);
}

void *
NclConnectorsParser::createCausalConnector (DOMElement *parentElement,
                                            arg_unused (void *objGrandParent))
{
  string connectorId = "";
  connectorId = _documentParser->getAttribute(parentElement, "id");
  connector = new CausalConnector (connectorId);
  return connector;
}

ConnectorBase *
NclConnectorsParser::createConnectorBase (DOMElement *parentElement,
                                          arg_unused (void *objGrandParent))
{
  ConnectorBase *connBase;
  string connBaseId= _documentParser->getAttribute(parentElement, "id");
  connBase = new ConnectorBase (connBaseId);
  return connBase;
}

void
NclConnectorsParser::compileRoleInformation (Role *role,
                                             DOMElement *parentElement)
{
  string attValue;
  // event type
  if (_documentParser->hasAttribute(parentElement, "eventType"))
    {
      attValue = _documentParser->getAttribute(parentElement, "eventType");
      role->setEventType (EventUtil::getTypeCode (attValue));
    }

  //  cardinality
  if (_documentParser->hasAttribute(parentElement, "min"))
    {
      attValue = _documentParser->getAttribute(parentElement, "min");
      ((Role *)role)->setMinCon ((xstrto_int (attValue)));
    }

  if (_documentParser->hasAttribute(parentElement, "max"))
    {
      attValue = _documentParser->getAttribute(parentElement,"max");

      if (XMLString::compareIString (attValue.c_str (), "unbounded") == 0)
        {
          ((Role *)role)->setMaxCon (Role::UNBOUNDED);
        }
      else
        {
          ((Role *)role)->setMaxCon (xstrto_int (attValue));
        }
    }
}

void *
NclConnectorsParser::createCompoundCondition (DOMElement *parentElement,
                                              arg_unused (void *objGrandParent))
{
  CompoundCondition *conditionExpression;
  string attValue;

  conditionExpression = new CompoundCondition ();

  string op = _documentParser->getAttribute(parentElement, "operator");

  if (XMLString::compareIString (op.c_str(), "and") == 0)
    {
      conditionExpression->setOperator (CompoundCondition::OP_AND);
    }
  else
    {
      conditionExpression->setOperator (CompoundCondition::OP_OR);
    }

  //  testar delay
  if (_documentParser->hasAttribute(parentElement, "delay"))
    {
      attValue = _documentParser->getAttribute(parentElement, "delay");

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


void *
NclConnectorsParser::createAssessmentStatement (
    DOMElement *parentElement, arg_unused (void *objGrandParent))
{
  AssessmentStatement *assessmentStatement;
  string attValue;

  if (_documentParser->hasAttribute(parentElement, "comparator"))
    {
      attValue = _documentParser->getAttribute(parentElement, "comparator");

      assessmentStatement
          = new AssessmentStatement (Comparator::fromString (attValue));
    }
  else
    {
      assessmentStatement = new AssessmentStatement (Comparator::CMP_EQ);
    }

  return assessmentStatement;
}

void *
NclConnectorsParser::createCompoundStatement (DOMElement *parentElement,
                                              arg_unused (void *objGrandParent))
{
  CompoundStatement *compoundStatement = new CompoundStatement ();

  string op = _documentParser->getAttribute(parentElement, "operator");
  if (XMLString::compareIString (op.c_str(), "and") == 0)
    {
      compoundStatement->setOperator (CompoundStatement::OP_AND);
    }
  else
    {
      compoundStatement->setOperator (CompoundStatement::OP_OR);
    }

  // testing isNegated
  if (_documentParser->hasAttribute(parentElement, "isNegated"))
    {
      string attValue =
          _documentParser->getAttribute(parentElement, "isNegated");

      compoundStatement->setNegated (
          XMLString::compareIString (attValue.c_str (), "true") == 0);
    }

  return compoundStatement;
}

void *
NclConnectorsParser::createCompoundAction (DOMElement *parentElement,
                                           arg_unused (void *objGrandParent))
{
  CompoundAction *actionExpression = new CompoundAction ();
  string op = _documentParser->getAttribute(parentElement, "operator");;

  if (XMLString::compareIString (op.c_str(), "seq") == 0)
    {
      actionExpression->setOperator (CompoundAction::OP_SEQ);
    }
  else
    {
      actionExpression->setOperator (CompoundAction::OP_PAR);
    }

  //  testar delay
  if (_documentParser->hasAttribute(parentElement, "delay"))
    {
      string attValue = _documentParser->getAttribute(parentElement, "delay");

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
    void *parentObject, void *childObject)
{
  ((CompoundCondition *)parentObject)
      ->addConditionExpression ((ConditionExpression *)childObject);
}

void
NclConnectorsParser::addCompoundConditionToCompoundCondition (
    void *parentObject, void *childObject)
{
  ((CompoundCondition *)parentObject)
      ->addConditionExpression ((ConditionExpression *)childObject);
}

void
NclConnectorsParser::addAssessmentStatementToCompoundCondition (
    void *parentObject, void *childObject)
{
  ((CompoundCondition *)parentObject)
      ->addConditionExpression ((ConditionExpression *)childObject);
}

void
NclConnectorsParser::addCompoundStatementToCompoundCondition (
    void *parentObject, void *childObject)
{
  ((CompoundCondition *)parentObject)
      ->addConditionExpression ((ConditionExpression *)childObject);
}

void
NclConnectorsParser::addAttributeAssessmentToAssessmentStatement (
    void *parentObject, void *childObject)
{
  AssessmentStatement *statement;

  statement = (AssessmentStatement *)parentObject;
  if (statement->getMainAssessment () == NULL)
    {
      statement->setMainAssessment ((AttributeAssessment *)childObject);
    }
  else
    {
      statement->setOtherAssessment ((AttributeAssessment *)childObject);
    }
}

void
NclConnectorsParser::addValueAssessmentToAssessmentStatement (
    void *parentObject, void *childObject)
{
  ((AssessmentStatement *)parentObject)
      ->setOtherAssessment ((ValueAssessment *)childObject);
}

void
NclConnectorsParser::addAssessmentStatementToCompoundStatement (
    void *parentObject, void *childObject)
{
  ((CompoundStatement *)parentObject)
      ->addStatement ((Statement *)childObject);
}

void
NclConnectorsParser::addCompoundStatementToCompoundStatement (
    void *parentObject, void *childObject)
{
  ((CompoundStatement *)parentObject)
      ->addStatement ((Statement *)childObject);
}

void
NclConnectorsParser::addSimpleActionToCompoundAction (void *parentObject,
                                                      void *childObject)
{
  ((CompoundAction *)parentObject)->addAction ((Action *)childObject);
}

void
NclConnectorsParser::addCompoundActionToCompoundAction (
    void *parentObject, void *childObject)
{
  ((CompoundAction *)parentObject)->addAction ((Action *)childObject);
}

void
NclConnectorsParser::addSimpleConditionToCausalConnector (
    void *parentObject, void *childObject)
{
  ((CausalConnector *)parentObject)
      ->setConditionExpression ((ConditionExpression *)childObject);
}

void
NclConnectorsParser::addCompoundConditionToCausalConnector (
    void *parentObject, void *childObject)
{
  ((CausalConnector *)parentObject)
      ->setConditionExpression ((ConditionExpression *)childObject);
}

void
NclConnectorsParser::addSimpleActionToCausalConnector (
    void *parentObject, void *childObject)
{
  ((CausalConnector *)parentObject)->setAction ((Action *)childObject);
}

void
NclConnectorsParser::addCompoundActionToCausalConnector (
    void *parentObject, void *childObject)
{
  ((CausalConnector *)parentObject)->setAction ((Action *)childObject);
}

GINGA_NCLCONV_END
