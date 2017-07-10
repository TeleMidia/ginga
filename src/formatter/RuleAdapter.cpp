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

GINGA_FORMATTER_BEGIN

RuleAdapter::RuleAdapter (Settings *settings)
{
  this->settings = settings;
  ruleListenMap = new map<string, vector<Rule *> *>;
  entityListenMap = new map<Rule *, vector<ExecutionObjectSwitch *> *>;
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
      map<Rule *, vector<ExecutionObjectSwitch *> *>::iterator j;
      vector<ExecutionObjectSwitch *> *objects;

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
}

Settings *
RuleAdapter::getSettings ()
{
  return this->settings;
}

void
RuleAdapter::adapt (ExecutionObjectContext *compositeObject,
                    bool force)
{
  ExecutionObject *object;
  map<string, ExecutionObject *> *objs;
  map<string, ExecutionObject *>::iterator i;

  objs = compositeObject->getExecutionObjects ();
  if (objs != NULL)
    {
      i = objs->begin ();
      while (i != objs->end ())
        {
          object = i->second;
          if (instanceof (ExecutionObjectSwitch *, object))
            {
              object = ((ExecutionObjectSwitch *)object)
                           ->getSelectedObject ();
            }

          if (instanceof (ExecutionObjectContext *, object))
            {
              adapt ((ExecutionObjectContext *)object, force);
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

  if (instanceof (SimpleRule *, rule))
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
      const vector<Rule *> *vec = ((CompositeRule *)rule)->getRules ();
      for (auto rule: *vec)
        {
          initializeAttributeRuleRelation (topRule, rule);
          ++rules;
        }
    }
}

Node *
RuleAdapter::adaptSwitch (Switch *swtch)
{
  const vector<Node *> *nodes;
  const vector<Rule *> *rules;

  nodes = swtch->getNodes ();
  rules = swtch->getRules ();
  for (size_t i = 0; i < rules->size (); i++)
    if (evaluateRule (rules->at (i)))
      return nodes->at (i);
  return swtch->getDefaultNode ();
}

bool
RuleAdapter::evaluateRule (Rule *rule)
{
  if (instanceof (SimpleRule *, rule))
    {
      return evaluateSimpleRule ((SimpleRule *)rule);
    }
  else if (instanceof (CompositeRule *, rule))
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
  if (rule->isConjunction ())
    {
      for (auto child: *rule->getRules ())
        if (!evaluateRule (child))
          return false;
      return true;
    }
  else
    {
      for (auto child: *rule->getRules ())
        if (evaluateRule (child))
          return true;
      return false;
    }
  g_assert_not_reached ();
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
