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
#include "RuleBase.h"

GINGA_NCL_BEGIN

RuleBase::RuleBase (const string &id) : Base (id)
{
  _ruleSet = new vector<Rule *>;
}

RuleBase::~RuleBase ()
{
  vector<Rule *>::iterator i;

  if (_ruleSet != NULL)
    {
      i = _ruleSet->begin ();
      while (i != _ruleSet->end ())
        {
          delete *i;
          ++i;
        }

      delete _ruleSet;
      _ruleSet = NULL;
    }
}

bool
RuleBase::addRule (Rule *rule)
{
  if (rule == NULL)
    return false;

  vector<Rule *>::iterator i;
  for (i = _ruleSet->begin (); i != _ruleSet->end (); ++i)
    {
      if (*i == rule)
        return false;
    }

  _ruleSet->push_back (rule);
  return true;
}

bool
RuleBase::addBase (Base *base, const string &alias, const string &location)
{
  if (instanceof (RuleBase *, base))
    {
      return Base::addBase (base, alias, location);
    }
  return false;
}

void
RuleBase::clear ()
{
  _ruleSet->clear ();
  Base::clear ();
}

Rule *
RuleBase::getRuleLocally (const string &ruleId)
{
  vector<Rule *>::iterator rules;

  rules = _ruleSet->begin ();
  while (rules != _ruleSet->end ())
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
RuleBase::getRule (const string &ruleId)
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
  if (_baseAliases.find (prefix) != _baseAliases.end ())
    {
      base = (RuleBase *)(_baseAliases[prefix]);
      return base->getRule (suffix);
    }
  else if (_baseLocations.find (prefix) != _baseLocations.end ())
    {
      base = (RuleBase *)(_baseLocations[prefix]);
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
  return _ruleSet;
}

bool
RuleBase::removeRule (Rule *rule)
{
  vector<Rule *>::iterator i;
  for (i = _ruleSet->begin (); i != _ruleSet->end (); ++i)
    {
      if (*i == rule)
        {
          _ruleSet->erase (i);
          return true;
        }
    }
  return false;
}

GINGA_NCL_END
