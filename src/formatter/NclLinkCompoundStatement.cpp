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
#include "NclLinkCompoundStatement.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_FORMATTER_BEGIN

NclLinkCompoundStatement::NclLinkCompoundStatement (short op)
    : NclLinkStatement ()
{
  statements = new vector<NclLinkStatement *>;
  this->op = op;
  typeSet.insert ("NclLinkCompoundStatement");
}

NclLinkCompoundStatement::~NclLinkCompoundStatement ()
{
  vector<NclLinkStatement *>::iterator i;
  NclLinkStatement *statement;

  if (statements != NULL)
    {
      for (i = statements->begin (); i != statements->end (); ++i)
        {
          statement = (NclLinkStatement *)(*i);
          delete statement;
          statement = NULL;
        }

      statements->clear ();
      delete statements;
      statements = NULL;
    }
}

short
NclLinkCompoundStatement::getOperator ()
{
  return op;
}

void
NclLinkCompoundStatement::addStatement (NclLinkStatement *statement)
{
  statements->push_back (statement);
}

vector<NclLinkStatement *> *
NclLinkCompoundStatement::getStatements ()
{
  return statements;
}

bool
NclLinkCompoundStatement::isNegated ()
{
  return negated;
}

void
NclLinkCompoundStatement::setNegated (bool neg)
{
  negated = neg;
}

bool
NclLinkCompoundStatement::returnEvaluationResult (bool result)
{
  return (negated ^ result);
}

vector<NclEvent *> *
NclLinkCompoundStatement::getEvents ()
{
  if (statements->empty ())
    return NULL;

  vector<NclEvent *> *events = new vector<NclEvent *>;
  vector<NclEvent *> *statementEvents;
  vector<NclEvent *>::iterator j;
  vector<NclLinkStatement *>::iterator i;
  NclLinkStatement *statement;

  for (i = statements->begin (); i != statements->end (); i++)
    {
      statement = (NclLinkStatement *)(*i);
      statementEvents = statement->getEvents ();
      if (statementEvents != NULL)
        {
          for (j = statementEvents->begin (); j != statementEvents->end ();
               j++)
            {
              events->push_back (*j);
            }
          delete statementEvents;
          statementEvents = NULL;
        }
    }

  if (events->begin () == events->end ())
    {
      delete events;
      return NULL;
    }

  return events;
}

bool
NclLinkCompoundStatement::evaluate ()
{
  int i, size;
  NclLinkStatement *childStatement;

  size = (int) statements->size ();
  if (op == CompoundStatement::OP_OR)
    {
      for (i = 0; i < size; i++)
        {
          childStatement = (NclLinkStatement *)(*statements)[i];
          if (childStatement->evaluate ())
            return returnEvaluationResult (true);
        }
      return returnEvaluationResult (false);
    }
  else
    {
      for (i = 0; i < size; i++)
        {
          childStatement = (NclLinkStatement *)(*statements)[i];
          if (!childStatement->evaluate ())
            return returnEvaluationResult (false);
        }
      return returnEvaluationResult (true);
    }
}

GINGA_FORMATTER_END
