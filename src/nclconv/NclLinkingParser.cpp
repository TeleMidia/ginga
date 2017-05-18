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

NclLinkingParser::NclLinkingParser (NclParser *nclParser)
    : ModuleParser (nclParser)
{
}

Bind *
NclLinkingParser::parseBind (DOMElement *bind_element, Link *link)
{
  Bind *bind = createBind (bind_element, link);
  g_assert_nonnull (bind);

  for (DOMElement *child: dom_element_children (bind_element))
    {
      string tagname = dom_element_tagname(child);
      if (tagname == "bindParam")
        {
          Parameter *param = parseLinkOrBindParam (child);
          if (param)
            {
              bind->addParameter (param);
            }
        }
      else
        {
          syntax_warning( "'%s' is not known as child of 'bind'."
                          " It will be ignored.",
                          tagname.c_str() );
        }
    }

  return bind;
}

Parameter *
NclLinkingParser::parseLinkOrBindParam (DOMElement *parentElement)
{
  Parameter *param;
  param = new Parameter (
      dom_element_get_attr(parentElement, "name"),
      dom_element_get_attr(parentElement, "value") );

  return param;
}

Link *
NclLinkingParser::parseLink (DOMElement *link_element,
                             CompositeNode *compositeNode)
{
  Link *link = createLink (link_element, compositeNode);
  g_assert_nonnull (link);

  for (DOMElement *child: dom_element_children(link_element))
    {
      string tagname = dom_element_tagname(child);
      if (tagname == "linkParam")
        {
          Parameter *param = parseLinkOrBindParam(child);
          if (param)
            {
              link->addParameter (param);
            }
        }
      else if (tagname == "bind")
        {
          Bind *bind = parseBind (child, link);
          if (bind)
            {
              // nothing to do, since to be created the bind needs to be
              // associated with its link
            }
        }
      else
        {
          syntax_warning( "'%s' is not known as child of 'link'."
                          " It will be ignored.",
                          tagname.c_str() );
        }
    }

  return link;
}

Bind *
NclLinkingParser::createBind (DOMElement *bind_element, Link *link)
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

  role = _connector->getRole (dom_element_get_attr(bind_element, "role"));
  component = dom_element_get_attr(bind_element, "component");

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

  if (dom_element_try_get_attr(interfaceId, bind_element, "interface"))
    {
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
  if (dom_element_has_attr(bind_element, "descriptor"))
    {
      document = getNclParser ()->getNclDocument ();
      descriptor = document->getDescriptor (
            dom_element_get_attr(bind_element, "descriptor") );
    }
  else
    {
      descriptor = NULL;
    }

  if (role == NULL)
    {
      // &got
      if (dom_element_try_get_attr(roleId, bind_element, "role"))
        {
          ConditionExpression *condition;
          CompoundCondition *compoundCondition;
          AssessmentStatement *statement;
          AttributeAssessment *assessment;
          ValueAssessment *otherAssessment;

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

  return link->bind (anchorNode, interfacePoint, descriptor, role->getLabel ());
}

Link *
NclLinkingParser::createLink (DOMElement *link_element,
                              CompositeNode *compositeNode)
{
  NclDocument *document = getNclParser ()->getNclDocument ();
  string connectorId =
      dom_element_get_attr(link_element, "xconnector");

  _connector = document->getConnector (connectorId);
  if (unlikely (_connector == NULL))
    {
      syntax_error ("link: bad xconnector '%s'", connectorId.c_str ());
    }

  g_assert (_connector->instanceOf ("CausalConnector"));

  Link *link = new CausalLink (dom_element_get_attr (link_element, "id"),
                               _connector);
  _composite = compositeNode;

  return link;
}

GINGA_NCLCONV_END
