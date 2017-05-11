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
#include "NclLinkingConverter.h"

GINGA_NCLCONV_BEGIN

NclLinkingConverter::NclLinkingConverter (
    DocumentParser *documentParser, arg_unused (NclConnectorsConverter *connCompiler))
    : NclLinkingParser (documentParser)
{
}

void
NclLinkingConverter::addBindToLink (arg_unused (void *parentObject), arg_unused (void *childObject))
{
  // nothing to do, since to be created the bind needs to be associated
  // with
  // its link
}

void
NclLinkingConverter::addBindParamToBind (void *parentObject,
                                         void *childObject)
{
  ((Bind *)parentObject)->addParameter ((Parameter *)childObject);
}

void
NclLinkingConverter::addLinkParamToLink (void *parentObject,
                                         void *childObject)
{
  ((Link *)parentObject)->addParameter ((Parameter *)childObject);
}

void *
NclLinkingConverter::createBind (DOMElement *parentElement,
                                 void *objGrandParent)
{
  string component, roleId, interfaceId;
  Role *role;
  Node *anchorNode;
  NodeEntity *anchorNodeEntity;
  InterfacePoint *interfacePoint = NULL;
  NclDocument *document;
  GenericDescriptor *descriptor;
  set<ReferNode *> *sInsts;
  set<ReferNode *>::iterator i;

  role = connector->getRole (XMLString::transcode (
      parentElement->getAttribute (XMLString::transcode ("role"))));

  component = XMLString::transcode (
      parentElement->getAttribute (XMLString::transcode ("component")));

  if (composite->getId () == component)
    {
      anchorNode = (Node *)composite;
    }
  else
    {
      anchorNode = (Node *)(composite->getNode (component));
    }

  if (unlikely (anchorNode == NULL))
    {
      syntax_error ("bind: bad interface for component '%s'",
                    component.c_str ());
    }

  anchorNodeEntity = (NodeEntity *)(anchorNode->getDataEntity ());

  if (parentElement->hasAttribute (XMLString::transcode ("interface")))
    {
      interfaceId = XMLString::transcode (
          parentElement->getAttribute (XMLString::transcode ("interface")));

      if (anchorNodeEntity == NULL)
        {
          interfacePoint = NULL;
        }
      else
        {
          if (anchorNode->instanceOf ("ReferNode")
              && ((ReferNode *)anchorNode)->getInstanceType () == "new")
            {
              interfacePoint = anchorNode->getAnchor (interfaceId);
            }
          else
            {
              interfacePoint = anchorNodeEntity->getAnchor (interfaceId);
            }
        }

      if (interfacePoint == NULL)
        {
          if (anchorNodeEntity != NULL
              && anchorNodeEntity->instanceOf ("CompositeNode"))
            {
              interfacePoint = ((CompositeNode *)anchorNodeEntity)
                                   ->getPort (interfaceId);
            }
          else
            {
              interfacePoint = anchorNode->getAnchor (interfaceId);

              if (interfacePoint == NULL)
                {
                  sInsts = anchorNodeEntity->getInstSameInstances ();
                  if (sInsts != NULL)
                    {
                      i = sInsts->begin ();
                      while (i != sInsts->end ())
                        {
                          interfacePoint = (*i)->getAnchor (interfaceId);
                          if (interfacePoint != NULL)
                            {
                              break;
                            }
                          ++i;
                        }
                    }
                }
            }
        }
    }
  else if (anchorNodeEntity != NULL)
    {
      if (anchorNode->instanceOf ("ReferNode")
          && ((ReferNode *)anchorNode)->getInstanceType () == "new")
        {
          interfacePoint = anchorNode->getAnchor (0);
          if (interfacePoint == NULL)
            {
              interfacePoint = new LambdaAnchor (anchorNode->getId ());
              anchorNode->addAnchor (0, (Anchor *)interfacePoint);
            }
        }
      else if (anchorNodeEntity->instanceOf ("Node"))
        {
          interfacePoint = anchorNodeEntity->getAnchor (0);
        }
      else
        {
          syntax_error ("bind: bad interface for entity '%s'",
                        anchorNodeEntity->getId ().c_str ());
        }
    }
  else
    {
      interfacePoint = anchorNode->getAnchor (0);
    }

  // atribui o bind ao elo (link)
  if (parentElement->hasAttribute (XMLString::transcode ("descriptor")))
    {
      document = getDocumentParser ()->getNclDocument ();
      descriptor = document->getDescriptor (
          XMLString::transcode (parentElement->getAttribute (
              XMLString::transcode ("descriptor"))));
    }
  else
    {
      descriptor = NULL;
    }

  if (role == NULL)
    {
      // &got
      if (parentElement->hasAttribute (XMLString::transcode ("role")))
        {
          ConditionExpression *condition;
          CompoundCondition *compoundCondition;
          AssessmentStatement *statement;
          AttributeAssessment *assessment;
          ValueAssessment *otherAssessment;

          roleId = XMLString::transcode (
              parentElement->getAttribute (XMLString::transcode ("role")));

          assessment = new AttributeAssessment (roleId);
          assessment->setEventType (EventUtil::EVT_ATTRIBUTION);
          assessment->setAttributeType (EventUtil::ATT_NODE_PROPERTY);
          assessment->setMinCon (0);
          assessment->setMaxCon (Role::UNBOUNDED);

          otherAssessment = new ValueAssessment (roleId);

          statement = new AssessmentStatement (Comparator::CMP_NE);
          statement->setMainAssessment (assessment);
          statement->setOtherAssessment (otherAssessment);

          condition
              = ((CausalConnector *)connector)->getConditionExpression ();

          if (condition->instanceOf ("CompoundCondition"))
            {
              ((CompoundCondition *)condition)
                  ->addConditionExpression (statement);
            }
          else
            {
              compoundCondition = new CompoundCondition (
                  condition, statement, CompoundCondition::OP_OR);

              ((CausalConnector *)connector)
                  ->setConditionExpression (
                      (ConditionExpression *)compoundCondition);
            }
          role = (Role *)assessment;
        }
      else
        {
          syntax_error ("bind: missing role");
        }
    }

  return ((Link *)objGrandParent)
      ->bind (anchorNode, interfacePoint, descriptor, role->getLabel ());
}

void *
NclLinkingConverter::createLink (DOMElement *parentElement,
                                 void *objGrandParent)
{
  NclDocument *document;
  Link *link;
  string connectorId;

  document = getDocumentParser ()->getNclDocument ();
  connectorId = XMLString::transcode (
      parentElement->getAttribute (XMLString::transcode ("xconnector")));

  connector = document->getConnector (connectorId);
  if (unlikely (connector == NULL))
    {
      syntax_error ("link: bad xconnector '%s'", connectorId.c_str ());
    }

  g_assert (connector->instanceOf ("CausalConnector"));

  link = new CausalLink (getId (parentElement), connector);
  composite = (CompositeNode *)objGrandParent;
  return link;
}

void *
NclLinkingConverter::createBindParam (DOMElement *parentElement,
                                      arg_unused (void *objGrandParent))
{
  Parameter *parameter;
  parameter = new Parameter (
      XMLString::transcode (
          parentElement->getAttribute (XMLString::transcode ("name"))),

      XMLString::transcode (
          parentElement->getAttribute (XMLString::transcode ("value"))));

  return parameter;
}

void *
NclLinkingConverter::createLinkParam (DOMElement *parentElement,
                                      arg_unused (void *objGrandParent))
{
  Parameter *parameter;
  parameter = new Parameter (
      XMLString::transcode (
          parentElement->getAttribute (XMLString::transcode ("name"))),

      XMLString::transcode (
          parentElement->getAttribute (XMLString::transcode ("value"))));

  return parameter;
}

string
NclLinkingConverter::getId (DOMElement *element)
{
  string strRet = "";
  if (element->hasAttribute (XMLString::transcode ("id")))
    {
      strRet = XMLString::transcode (
          element->getAttribute (XMLString::transcode ("id")));
    }
  else
    {
      strRet = "";
    }
  return strRet;
}

GINGA_NCLCONV_END
