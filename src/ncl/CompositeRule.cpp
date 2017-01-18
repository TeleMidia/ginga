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

#include "config.h"
#include "ncl/CompositeRule.h"

GINGA_NCL_BEGIN

CompositeRule::CompositeRule (string id, short someOperator) : Rule (id)
{
  rules = new vector<Rule *>;
  setOperator (someOperator);
  typeSet.insert ("CompositeRule");
}

CompositeRule::CompositeRule (string id, Rule *firstRule, Rule *secondRule,
                              short someOperator)
    : Rule (id)
{

  rules = new vector<Rule *>;
  setOperator (someOperator);
  rules->push_back (firstRule);
  rules->push_back (secondRule);
  typeSet.insert ("CompositeRule");
}

CompositeRule::~CompositeRule ()
{
  vector<Rule *>::iterator i;

  if (rules != NULL)
    {
      i = rules->begin ();
      while (i != rules->begin ())
        {
          delete *i;
          ++i;
        }

      delete rules;
      rules = NULL;
    }
}

bool
CompositeRule::addRule (Rule *rule)
{
  rules->push_back (rule);
  return true;
}

vector<Rule *> *
CompositeRule::getRules ()
{
  if (rules->begin () == rules->end ())
    return NULL;

  return rules;
}

unsigned int
CompositeRule::getNumRules ()
{
  return rules->size ();
}

short
CompositeRule::getOperator ()
{
  return ruleOperator;
}

bool
CompositeRule::removeRule (Rule *rule)
{
  vector<Rule *>::iterator iterRule;

  iterRule = rules->begin ();
  while (iterRule != rules->end ())
    {
      if ((*(*iterRule)).getId () == rule->getId ())
        {
          rules->erase (iterRule);
          return true;
        }
      ++iterRule;
    }
  return false;
}

string
CompositeRule::toString ()
{
  string result;
  vector<Rule *>::iterator ruleIterator;
  Rule *rule;

  result = "(";

  for (ruleIterator = rules->begin (); ruleIterator != rules->end ();
       ++ruleIterator)
    {

      rule = (Rule *)(*ruleIterator);
      result = result + (rule->getId ()) + " " + opStr + " ";
    }

  if ((result.length () - opStr.length () - 2) > 0)
    result = result.substr (0, result.length () - opStr.length () - 2);

  return result + ")";
}

void
CompositeRule::setOperator (short op)
{
  switch (op)
    {
    case OP_OR:
      ruleOperator = OP_OR;
      opStr = "OR";
      break;

    case OP_AND:
      ruleOperator = OP_AND;
      opStr = "AND";
      break;

    default:
      ruleOperator = OP_AND;
      opStr = "AND";
      break;
    }
}

GINGA_NCL_END
