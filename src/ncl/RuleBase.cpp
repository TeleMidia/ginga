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
#include "ncl/RuleBase.h"

GINGA_NCL_BEGIN

RuleBase::RuleBase (string id) : Base (id)
{
  ruleSet = new vector<Rule *>;
  typeSet.insert ("RuleBase");
}

RuleBase::~RuleBase ()
{
  vector<Rule *>::iterator i;

  if (ruleSet != NULL)
    {
      i = ruleSet->begin ();
      while (i != ruleSet->end ())
        {
          delete *i;
          ++i;
        }

      delete ruleSet;
      ruleSet = NULL;
    }
}

bool
RuleBase::addRule (Rule *rule)
{
  if (rule == NULL)
    return false;

  vector<Rule *>::iterator i;
  for (i = ruleSet->begin (); i != ruleSet->end (); ++i)
    {
      if (*i == rule)
        return false;
    }

  ruleSet->push_back (rule);
  return true;
}

bool
RuleBase::addBase (Base *base, string alias, string location)
{
  if (base->instanceOf ("RuleBase"))
    {
      return Base::addBase (base, alias, location);
    }
  return false;
}

void
RuleBase::clear ()
{
  ruleSet->clear ();
  Base::clear ();
}

Rule *
RuleBase::getRuleLocally (string ruleId)
{
  vector<Rule *>::iterator rules;

  rules = ruleSet->begin ();
  while (rules != ruleSet->end ())
    {
      if ((*rules)->getId () == ruleId)
        {
          return (*rules);
        }
      ++rules;
    }
  return NULL;
}

Rule *
RuleBase::getRule (string ruleId)
{
  string::size_type index;
  string prefix, suffix;
  RuleBase *base;

  index = ruleId.find_first_of ("#");
  if (index == string::npos)
    {
      return getRuleLocally (ruleId);
    }
  prefix = ruleId.substr (0, index);
  index++;
  suffix = ruleId.substr (index, ruleId.length () - index);
  if (baseAliases.find (prefix) != baseAliases.end ())
    {
      base = (RuleBase *)(baseAliases[prefix]);
      return base->getRule (suffix);
    }
  else if (baseLocations.find (prefix) != baseLocations.end ())
    {
      base = (RuleBase *)(baseLocations[prefix]);
      return base->getRule (suffix);
    }
  else
    {
      return NULL;
    }
}

vector<Rule *> *
RuleBase::getRules ()
{
  return ruleSet;
}

bool
RuleBase::removeRule (Rule *rule)
{
  vector<Rule *>::iterator i;
  for (i = ruleSet->begin (); i != ruleSet->end (); ++i)
    {
      if (*i == rule)
        {
          ruleSet->erase (i);
          return true;
        }
    }
  return false;
}

GINGA_NCL_END
