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
#include "CompositeRule.h"

GINGA_NCL_BEGIN

CompositeRule::CompositeRule (const string &id, short someOperator) : Rule (id)
{
  _rules = new vector<Rule *>;
  setOperator (someOperator);
}

CompositeRule::CompositeRule (const string &id, Rule *firstRule, Rule *secondRule,
                              short someOperator)
    : Rule (id)
{
  _rules = new vector<Rule *>;
  setOperator (someOperator);
  _rules->push_back (firstRule);
  _rules->push_back (secondRule);
}

CompositeRule::~CompositeRule ()
{
  vector<Rule *>::iterator i;

  if (_rules != NULL)
    {
      i = _rules->begin ();
      while (i != _rules->begin ())
        {
          delete *i;
          ++i;
        }

      delete _rules;
      _rules = NULL;
    }
}

bool
CompositeRule::addRule (Rule *rule)
{
  _rules->push_back (rule);
  return true;
}

vector<Rule *> *
CompositeRule::getRules ()
{
  return _rules;
}

short
CompositeRule::getOperator ()
{
  return _ruleOperator;
}

bool
CompositeRule::removeRule (Rule *rule)
{
  vector<Rule *>::iterator iterRule;

  iterRule = _rules->begin ();
  while (iterRule != _rules->end ())
    {
      if ((*(*iterRule)).getId () == rule->getId ())
        {
          _rules->erase (iterRule);
          return true;
        }
      ++iterRule;
    }
  return false;
}

void
CompositeRule::setOperator (short op)
{
  switch (op)
    {
    case OP_OR:
      _ruleOperator = OP_OR;
      _opStr = "OR";
      break;

    case OP_AND:
      _ruleOperator = OP_AND;
      _opStr = "AND";
      break;

    default:
      _ruleOperator = OP_AND;
      _opStr = "AND";
      break;
    }
}

GINGA_NCL_END
