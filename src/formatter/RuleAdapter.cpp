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
#include "RuleAdapter.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_FORMATTER_BEGIN

RuleAdapter::RuleAdapter (Settings *settings)
{
  this->settings = settings;
  ruleListenMap = new map<string, vector<Rule *> *>;
  entityListenMap = new map<Rule *, vector<NclExecutionObjectSwitch *> *>;
  descListenMap = new map<Rule *, vector<DescriptorSwitch *> *>;
}

RuleAdapter::~RuleAdapter ()
{
  if (settings != NULL)
    settings = NULL;

  reset ();

  if (ruleListenMap != NULL)
    {
      delete ruleListenMap;
      ruleListenMap = NULL;
    }

  if (entityListenMap != NULL)
    {
      delete entityListenMap;
      entityListenMap = NULL;
    }

  if (descListenMap != NULL)
    {
      delete descListenMap;
      descListenMap = NULL;
    }
}

void
RuleAdapter::reset ()
{
  if (ruleListenMap != NULL)
    {
      vector<Rule *> *rules;
      map<string, vector<Rule *> *>::iterator i;

      i = ruleListenMap->begin ();
      while (i != ruleListenMap->end ())
        {
          rules = i->second;
          if (rules != NULL)
            {
              delete rules;
              rules = NULL;
            }
          ++i;
        }

      ruleListenMap->clear ();
    }

  if (entityListenMap != NULL)
    {
      map<Rule *, vector<NclExecutionObjectSwitch *> *>::iterator j;
      vector<NclExecutionObjectSwitch *> *objects;

      j = entityListenMap->begin ();
      while (j != entityListenMap->end ())
        {
          objects = j->second;
          if (objects != NULL)
            {
              delete objects;
              objects = NULL;
            }
          ++j;
        }

      entityListenMap->clear ();
    }

  if (descListenMap != NULL)
    {
      map<Rule *, vector<DescriptorSwitch *> *>::iterator k;
      vector<DescriptorSwitch *> *descs;

      k = descListenMap->begin ();
      while (k != descListenMap->end ())
        {
          descs = k->second;
          if (descs != NULL)
            {
              delete descs;
              descs = NULL;
            }
          ++k;
        }

      descListenMap->clear ();
    }
}

Settings *
RuleAdapter::getSettings ()
{
  return this->settings;
}

void
RuleAdapter::adapt (NclCompositeExecutionObject *compositeObject,
                    bool force)
{
  NclExecutionObject *object;
  map<string, NclExecutionObject *> *objs;
  map<string, NclExecutionObject *>::iterator i;

  objs = compositeObject->getExecutionObjects ();
  if (objs != NULL)
    {
      i = objs->begin ();
      while (i != objs->end ())
        {
          object = i->second;
          if (object->instanceOf ("NclExecutionObjectSwitch"))
            {
              initializeRuleObjectRelation (
                  (NclExecutionObjectSwitch *)object);

              adapt ((NclExecutionObjectSwitch *)object, force);
              object = ((NclExecutionObjectSwitch *)object)
                           ->getSelectedObject ();
            }

          adaptDescriptor (object);
          if (object->instanceOf ("NclCompositeExecutionObject"))
            {
              adapt ((NclCompositeExecutionObject *)object, force);
            }
          ++i;
        }
      delete objs;
      objs = NULL;
    }
}

void
RuleAdapter::initializeAttributeRuleRelation (Rule *topRule, Rule *rule)
{
  vector<Rule *> *ruleVector = NULL;
  vector<Rule *>::iterator rules;

  if (rule->instanceOf ("SimpleRule"))
    {
      map<string, vector<Rule *> *>::iterator i;
      for (i = ruleListenMap->begin (); i != ruleListenMap->end (); ++i)
        {
          if (((SimpleRule *)rule)->getAttribute () == i->first)
            {
              ruleVector = i->second;
              break;
            }
        }

      if (ruleVector == NULL)
        {
          ruleVector = new vector<Rule *>;
          (*ruleListenMap)[(((SimpleRule *)rule)->getAttribute ())]
              = ruleVector;
        }
      ruleVector->push_back (topRule);
    }
  else
    {
      ruleVector = ((CompositeRule *)rule)->getRules ();
      if (ruleVector != NULL)
        {
          rules = ruleVector->begin ();
          while (rules != ruleVector->end ())
            {
              initializeAttributeRuleRelation (topRule, (Rule *)(*rules));
              ++rules;
            }
        }
    }
}

void
RuleAdapter::initializeRuleObjectRelation (
    arg_unused (NclExecutionObjectSwitch *objectAlternatives))
{
}

void
RuleAdapter::adapt (arg_unused (NclExecutionObjectSwitch *objectAlternatives),
                    arg_unused (bool force))
{
}

bool
RuleAdapter::adaptDescriptor (NclExecutionObject *executionObject)
{
  NclCascadingDescriptor *cascadingDescriptor;
  GenericDescriptor *selectedDescriptor;
  GenericDescriptor *unsolvedDescriptor;
  DescriptorSwitch *descAlternatives;
  int i, j, size;
  Rule *rule;
  bool selected, result;
  vector<DescriptorSwitch *> *objectVector;
  map<Rule *, vector<DescriptorSwitch *> *>::iterator k;
  bool adapted = false;

  clog << "RuleAdapter::adaptDescriptor for '";
  clog << executionObject->getId () << "'";
  clog << endl;

  cascadingDescriptor = executionObject->getDescriptor ();
  if (cascadingDescriptor == NULL)
    {
      return adapted;
    }

  j = 0;
  unsolvedDescriptor = cascadingDescriptor->getUnsolvedDescriptor (j);
  clog << "RuleAdapter::adaptDescriptor first unsolved descriptor ";
  clog << "address '" << unsolvedDescriptor << "'" << endl;
  while (unsolvedDescriptor != NULL)
    {
      j++;
      if (unsolvedDescriptor->instanceOf ("DescriptorSwitch"))
        {
          descAlternatives = (DescriptorSwitch *)unsolvedDescriptor;

          clog << "RuleAdapter::adaptDescriptor solving ";
          clog << "descriptor switch '";
          clog << descAlternatives->getId ();
          clog << "'";
          clog << endl;

          selectedDescriptor = descAlternatives->getSelectedDescriptor ();
          selected = false;
          size = descAlternatives->getNumRules ();
          for (i = 0; i < size; i++)
            {
              rule = descAlternatives->getRule (i);
              result = evaluateRule (rule);
              if (result && !selected)
                {
                  selected = true;
                  descAlternatives->select (
                      descAlternatives->getDescriptor (i));
                }

              if (descListenMap->count (rule) == 0)
                {
                  objectVector = new vector<DescriptorSwitch *>;
                  (*descListenMap)[rule] = objectVector;
                }
              else
                {
                  objectVector = ((*descListenMap)[rule]);
                }

              objectVector->push_back (descAlternatives);
            }
          if (!selected)
            {
              descAlternatives->selectDefault ();
            }

          if (selectedDescriptor
              != descAlternatives->getSelectedDescriptor ())
            {
              adapted = true;
            }
        }
         
      cascadingDescriptor->cascadeUnsolvedDescriptor ();
      unsolvedDescriptor = cascadingDescriptor->getUnsolvedDescriptor (j);
    }  
  return adapted;
}

Node *
RuleAdapter::adaptSwitch (SwitchNode *switchNode)
{
  int i, size;
  Rule *rule;
  Node *selectedNode;

  selectedNode = NULL;
  size = switchNode->getNumRules ();
  for (i = 0; i < size; i++)
    {
      rule = switchNode->getRule (i);
      if (evaluateRule (rule))
        {
          selectedNode = switchNode->getNode (i);
        }
    }

  if (selectedNode == NULL)
    {
      selectedNode = switchNode->getDefaultNode ();
    }

  return selectedNode;
}

bool
RuleAdapter::evaluateRule (Rule *rule)
{
  if (rule->instanceOf ("SimpleRule"))
    {
      return evaluateSimpleRule ((SimpleRule *)rule);
    }
  else if (rule->instanceOf ("CompositeRule"))
    {
      return evaluateCompositeRule ((CompositeRule *)rule);
    }
  else
    {
      return false;
    }
}

bool
RuleAdapter::evaluateCompositeRule (CompositeRule *rule)
{
  Rule *childRule;

  vector<Rule *> *rules;
  vector<Rule *>::iterator iterator;

  rules = (rule->getRules ()); // sf
  if (rules != NULL)
    {
      iterator = rules->begin ();
      switch (rule->getOperator ())
        {
        case CompositeRule::OP_OR:
          while (iterator != rules->end ())
            {
              childRule = (*iterator);
              if (evaluateRule (childRule))
                return true;
              ++iterator;
            }
          return false;

        case CompositeRule::OP_AND:

        default:
          while (iterator != rules->end ())
            {
              childRule = (*iterator);
              if (!evaluateRule (childRule))
                return false;
              ++iterator;
            }
          return true;
        }
    }
  return false;
}

bool
RuleAdapter::evaluateSimpleRule (SimpleRule *rule)
{
  string attribute;
  Comparator::Op op;
  string ruleValue;
  string attributeValue;

  attribute = rule->getAttribute ();
  attributeValue = settings->get (attribute);

  ruleValue = rule->getValue ();

  if (attributeValue == "")
    {
      return false;
    }

  op = rule->getOperator ();
  switch (op)
    {
    case Comparator::CMP_EQ:
      if (attributeValue == "" && ruleValue == "")
        {
          return true;
        }
      else if (attributeValue == "")
        {
          return false;
        }
      else
        {
          return Comparator::evaluate (attributeValue, ruleValue, op);
        }

    case Comparator::CMP_NE:
      if (attributeValue == "" && ruleValue == "")
        {
          return false;
        }
      else if (attributeValue == "")
        {
          return true;
        }
      else
        {
          return Comparator::evaluate (attributeValue, ruleValue, op);
        }

    default:
      return Comparator::evaluate (attributeValue, ruleValue, op);
    }
}

GINGA_FORMATTER_END
