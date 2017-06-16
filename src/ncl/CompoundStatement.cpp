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

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_NCL_BEGIN

CompoundStatement::CompoundStatement () : Statement ()
{
  _statements = new vector<Statement *>;
  _myOperator = OP_OR;
  _negated = false;

  typeSet.insert ("CompoundStatement");
}

CompoundStatement::CompoundStatement (Statement *p1, Statement *p2,
                                      short op)
    : Statement ()
{
  _statements = new vector<Statement *>;
  _negated = false;
  _myOperator = op;

  _statements->push_back (p1);
  _statements->push_back (p2);

  typeSet.insert ("CompoundStatement");
}

CompoundStatement::~CompoundStatement ()
{
  vector<Statement *>::iterator i;

  if (_statements != NULL)
    {
      i = _statements->begin ();
      while (i != _statements->end ())
        {
          delete *i;
          ++i;
        }

      delete _statements;
      _statements = NULL;
    }
}

void
CompoundStatement::setOperator (short op)
{
  switch (op)
    {
    case OP_AND:
      _myOperator = op;
      break;

    case OP_OR:
    default:
      _myOperator = OP_OR;
      break;
    }
}

short
CompoundStatement::getOperator ()
{
  return _myOperator;
}

vector<Statement *> *
CompoundStatement::getStatements ()
{
  if (_statements->empty ())
    return NULL;
  return _statements;
}

void
CompoundStatement::addStatement (Statement *statement)
{
  _statements->push_back (statement);
}

void
CompoundStatement::removeStatement (Statement *statement)
{
  vector<Statement *>::iterator iterator;
  vector<Statement *>::iterator i;

  iterator = _statements->begin ();
  while (iterator != _statements->end ())
    {
      if ((*iterator) == statement)
        {
          i = _statements->erase (iterator);
          if (i == _statements->end ())
            return;
        }
      ++iterator;
    }
}

void
CompoundStatement::setNegated (bool newNegated)
{
  _negated = newNegated;
}

bool
CompoundStatement::isNegated ()
{
  return _negated;
}

vector<Role *> *
CompoundStatement::getRoles ()
{
  vector<Role *> *roles;
  int i, size;
  Statement *statement;
  vector<Role *> *childRoles;

  roles = new vector<Role *>;
  size = (int) _statements->size ();
  for (i = 0; i < size; i++)
    {
      statement = (Statement *)((*_statements)[i]);
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
