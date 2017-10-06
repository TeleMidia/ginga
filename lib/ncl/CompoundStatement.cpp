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
#include "CompoundStatement.h"

GINGA_NCL_BEGIN

/**
 * @brief Creates a new compound statement.
 * @param conj True if statement is a conjunction.
 * @param neg True if statement is negated.
 */
CompoundStatement::CompoundStatement (bool conj, bool neg) : Statement ()
{
  _conjunction = conj;
  _negated = neg;
}

/**
 * @brief Destroys compound statement.
 */
CompoundStatement::~CompoundStatement ()
{
  _statements.clear ();
}

/**
 * @brief Tests whether statement is a conjunction.
 */
bool
CompoundStatement::isConjunction ()
{
  return _conjunction;
}

/**
 * @brief Tests whether statement is negated.
 */
bool
CompoundStatement::isNegated ()
{
  return _negated;
}

/**
 * @brief Gets all child statements.
 */
const vector<Statement *> *
CompoundStatement::getStatements ()
{
  return &_statements;
}

/**
 * @brief Adds child statement.
 * @param statement Child statement.
 */
void
CompoundStatement::addStatement (Statement *statement)
{
  g_assert_nonnull (statement);
  _statements.push_back (statement);
}

GINGA_NCL_END
