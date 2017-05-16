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
#include "NclLinkingParser.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_NCLCONV_BEGIN

NclLinkingParser::NclLinkingParser (NclParser *nclParser,
                                  arg_unused(NclConnectorsParser *connCompiler))
    : ModuleParser (nclParser)
{
}

Bind *
NclLinkingParser::parseBind (DOMElement *parentElement,
                             Link *objGrandParent)
{
  Bind *bind = createBind (parentElement, objGrandParent);
  g_assert_nonnull (bind);

  DOMNodeList *elementNodeList = parentElement->getChildNodes ();
  for (int i = 0; i < (int)elementNodeList->getLength (); i++)
    {
      DOMNode *node = elementNodeList->item (i);
      if (node->getNodeType () == DOMNode::ELEMENT_NODE)
        {
          DOMElement *element = (DOMElement *)node;
          string tagname = dom_element_tagname(element);

          if (XMLString::compareIString (tagname.c_str (), "bindParam") == 0)
            {
              Parameter *param = parseBindParam (element);
              if (param)
                {
                  addBindParamToBind (bind, param);
                }
            }
        }
    }

  return bind;
}

Parameter *
NclLinkingParser::parseLinkParam (DOMElement *parentElement)
{
  Parameter *param;
  param = new Parameter (
      dom_element_get_attr(parentElement, "name"),
      dom_element_get_attr(parentElement, "value") );

  return param;
}

Parameter *
NclLinkingParser::parseBindParam (DOMElement *parentElement)
{
  Parameter *param;
  param = new Parameter (
      dom_element_get_attr(parentElement, "name"),
      dom_element_get_attr(parentElement, "value") );

  return param;
}

Link *
NclLinkingParser::parseLink (DOMElement *parentElement,
                             void *objGrandParent)
{
  Link *link = createLink (parentElement, objGrandParent);
  g_assert_nonnull (link);

  DOMNodeList *elementNodeList = parentElement->getChildNodes ();
  for (int i = 0; i < (int)elementNodeList->getLength (); i++)
    {
      DOMNode *node = elementNodeList->item (i);
      if (node->getNodeType () == DOMNode::ELEMENT_NODE)
        {
          DOMElement *element = (DOMElement *)node;
          string tagname = dom_element_tagname(element);

          if (XMLString::compareIString (tagname.c_str (), "linkParam") == 0)
            {
              Parameter *param = parseLinkParam (element);
              if (param)
                {
                  addLinkParamToLink (link, param);
                }
            }
          else if (XMLString::compareIString (tagname.c_str (), "bind") == 0)
            {
              Bind *bind = parseBind (element, link);
              if (bind)
                {
                  addBindToLink (link, bind);
                }
            }
        }
    }

  return link;
}


void
NclLinkingParser::addBindToLink (arg_unused (Link *parentObject), arg_unused (Bind *childObject))
{
  // nothing to do, since to be created the bind needs to be associated
  // with
  // its link
}

void
NclLinkingParser::addBindParamToBind (Bind *parentObject,
                                      Parameter *childObject)
{
  parentObject->addParameter (childObject);
}

void
NclLinkingParser::addLinkParamToLink (Link *parentObject,
                                      Parameter *childObject)
{
  parentObject->addParameter (childObject);
}

Bind *
NclLinkingParser::createBind (DOMElement *parentElement, Link *objGrandParent)
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

  role = _connector->getRole (
        dom_element_get_attr(parentElement, "role"));

  component = dom_element_get_attr(parentElement, "component");

  if (_composite->getId () == component)
    {
      anchorNode = (Node *)_composite;
    }
  else
    {
      anchorNode = (Node *)(_composite->getNode (component));
    }

  if (unlikely (anchorNode == NULL))
    {
      syntax_error ("bind: bad interface for component '%s'",
                    component.c_str ());
    }

  anchorNodeEntity = (NodeEntity *)(anchorNode->getDataEntity ());

  if (dom_element_has_attr(parentElement, "interface"))
    {
      interfaceId = dom_element_get_attr(parentElement, "interface");

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
  if (dom_element_has_attr(parentElement, "descriptor"))
    {
      document = getNclParser ()->getNclDocument ();
      descriptor = document->getDescriptor (
            dom_element_get_attr(parentElement, "descriptor") );
    }
  else
    {
      descriptor = NULL;
    }

  if (role == NULL)
    {
      // &got
      if (dom_element_has_attr(parentElement, "role"))
        {
          ConditionExpression *condition;
          CompoundCondition *compoundCondition;
          AssessmentStatement *statement;
          AttributeAssessment *assessment;
          ValueAssessment *otherAssessment;

          roleId = dom_element_get_attr(parentElement, "role");

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
              = ((CausalConnector *)_connector)->getConditionExpression ();

          if (condition->instanceOf ("CompoundCondition"))
            {
              ((CompoundCondition *)condition)
                  ->addConditionExpression (statement);
            }
          else
            {
              compoundCondition = new CompoundCondition (
                  condition, statement, CompoundCondition::OP_OR);

              ((CausalConnector *)_connector)
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

  return objGrandParent->bind (
        anchorNode, interfacePoint, descriptor, role->getLabel () );
}

Link *
NclLinkingParser::createLink (DOMElement *parentElement,
                              void *objGrandParent)
{
  NclDocument *document = getNclParser ()->getNclDocument ();
  string connectorId =
      dom_element_get_attr(parentElement, "xconnector");

  _connector = document->getConnector (connectorId);
  if (unlikely (_connector == NULL))
    {
      syntax_error ("link: bad xconnector '%s'", connectorId.c_str ());
    }

  g_assert (_connector->instanceOf ("CausalConnector"));

  Link *link = new CausalLink (getId (parentElement), _connector);
  _composite = (CompositeNode *) objGrandParent;

  return link;
}

string
NclLinkingParser::getId (DOMElement *element)
{
  string strRet = "";
  if (dom_element_has_attr (element, "id"))
    {
      strRet = dom_element_get_attr (element, "id");
    }
  else
    {
      strRet = "";
    }
  return strRet;
}


GINGA_NCLCONV_END
