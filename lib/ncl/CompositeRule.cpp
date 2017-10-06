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

#include "aux-ginga.h"
#include "CompositeRule.h"

GINGA_NCL_BEGIN

/**
 * @brief Creates a new composite rule.
 * @param ncl Parent document.
 * @param id Rule id.
 * @param conj True if composite rule is a conjunction.
 */
CompositeRule::CompositeRule (NclDocument *ncl, const string &id, bool conj)
  : Rule (ncl, id)
{
  _conjunction = conj;
}

/**
 * @brief Destroys composite rule.
 */
CompositeRule::~CompositeRule ()
{
  _rules.clear ();
}

/**
 * @brief Tests whether composite rule is a conjunction.
 */
bool
CompositeRule::isConjunction ()
{
  return _conjunction;
}

/**
 * @brief Adds rule to composite rule.
 */
void
CompositeRule::addRule (Rule *rule)
{
  g_assert_nonnull (rule);
  _rules.push_back (rule);
}

/**
 * @brief Gets all child rules.
 */
const vector<Rule *> *
CompositeRule::getRules ()
{
  return &_rules;
}

GINGA_NCL_END
