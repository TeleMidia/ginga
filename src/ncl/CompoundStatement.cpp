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
#include "CompoundStatement.h"

GINGA_NCL_BEGIN

CompoundStatement::CompoundStatement () : Statement ()
{
  statements = new vector<Statement *>;
  myOperator = OP_OR;
  negated = false;

  typeSet.insert ("CompoundStatement");
}

CompoundStatement::CompoundStatement (Statement *p1, Statement *p2,
                                      short op)
    : Statement ()
{

  statements = new vector<Statement *>;
  negated = false;
  myOperator = op;

  statements->push_back (p1);
  statements->push_back (p2);

  typeSet.insert ("CompoundStatement");
}

CompoundStatement::~CompoundStatement ()
{
  vector<Statement *>::iterator i;

  if (statements != NULL)
    {
      i = statements->begin ();
      while (i != statements->end ())
        {
          delete *i;
          ++i;
        }

      delete statements;
      statements = NULL;
    }
}

void
CompoundStatement::setOperator (short op)
{
  switch (op)
    {
    case OP_AND:
      myOperator = op;
      break;

    case OP_OR:
    default:
      myOperator = OP_OR;
      break;
    }
}

short
CompoundStatement::getOperator ()
{
  return myOperator;
}

vector<Statement *> *
CompoundStatement::getStatements ()
{
  if (statements->empty ())
    return NULL;
  return statements;
}

void
CompoundStatement::addStatement (Statement *statement)
{
  statements->push_back (statement);
}

void
CompoundStatement::removeStatement (Statement *statement)
{
  vector<Statement *>::iterator iterator;
  vector<Statement *>::iterator i;

  iterator = statements->begin ();
  while (iterator != statements->end ())
    {
      if ((*iterator) == statement)
        {
          i = statements->erase (iterator);
          if (i == statements->end ())
            return;
        }
      ++iterator;
    }
}

void
CompoundStatement::setNegated (bool newNegated)
{
  negated = newNegated;
}

bool
CompoundStatement::isNegated ()
{
  return negated;
}

vector<Role *> *
CompoundStatement::getRoles ()
{
  vector<Role *> *roles;
  int i, size;
  Statement *statement;
  vector<Role *> *childRoles;

  roles = new vector<Role *>;
  size = (int) statements->size ();
  for (i = 0; i < size; i++)
    {
      statement = (Statement *)((*statements)[i]);
      if (statement->instanceOf ("AssessmentStatement"))
        {
          childRoles = ((AssessmentStatement *)statement)->getRoles ();
        }
      else
        { // ICompoundStatement
          childRoles = ((CompoundStatement *)statement)->getRoles ();
        }

      vector<Role *>::iterator it;
      for (it = childRoles->begin (); it != childRoles->end (); ++it)
        {
          roles->push_back (*it);
        }

      delete childRoles;
    }
  return roles;
}

GINGA_NCL_END
