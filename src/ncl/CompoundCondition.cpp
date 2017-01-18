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
#include "CompoundCondition.h"

GINGA_NCL_BEGIN

CompoundCondition::CompoundCondition () : TriggerExpression ()
{

  expressions = new vector<ConditionExpression *>;
  typeSet.insert ("CompoundCondition");
}

CompoundCondition::CompoundCondition (ConditionExpression *c1,
                                      ConditionExpression *c2, short op)
    : TriggerExpression ()
{

  expressions = new vector<ConditionExpression *>;
  expressions->push_back (c1);
  expressions->push_back (c2);
  myOperator = op;
  typeSet.insert ("CompoundCondition");
}

CompoundCondition::~CompoundCondition ()
{
  vector<ConditionExpression *>::iterator i;

  if (expressions != NULL)
    {
      i = expressions->begin ();
      while (i != expressions->end ())
        {
          delete *i;
          ++i;
        }

      delete expressions;
      expressions = NULL;
    }
}

void
CompoundCondition::setOperator (short op)
{
  myOperator = op;
}

short
CompoundCondition::getOperator ()
{
  return myOperator;
}

vector<ConditionExpression *> *
CompoundCondition::getConditions ()
{
  if (expressions->empty ())
    return NULL;

  return expressions;
}

void
CompoundCondition::addConditionExpression (ConditionExpression *condition)
{

  if (condition == NULL)
    {
      clog << "CompoundCondition::addConditionExpression ";
      clog << "Warning! Trying to add a NULL condition." << endl;
    }
  else
    {
      expressions->push_back (condition);
    }
}

void
CompoundCondition::removeConditionExpression (
    ConditionExpression *condition)
{

  vector<ConditionExpression *>::iterator iterator;
  vector<ConditionExpression *>::iterator i;

  iterator = expressions->begin ();
  while (iterator != expressions->end ())
    {
      if ((*iterator) == condition)
        {
          i = expressions->erase (iterator);
          if (i == expressions->end ())
            return;
        }
      ++iterator;
    }
}

vector<Role *> *
CompoundCondition::getRoles ()
{
  vector<Role *> *roles;
  int i, size;
  ConditionExpression *condition;
  vector<Role *> *childRoles;

  roles = new vector<Role *>;
  size = expressions->size ();
  for (i = 0; i < size; i++)
    {
      condition = (*expressions)[i];
      if (condition == NULL)
        {
          clog << "CompoundCondition::getRoles ";
          clog << "Warning! condition = NULL." << endl;
        }
      else if (condition->instanceOf ("SimpleCondition"))
        {
          roles->push_back ((SimpleCondition *)condition);
        }
      else
        {
          if (condition->instanceOf ("CompoundCondition"))
            {
              childRoles = ((CompoundCondition *)condition)->getRoles ();
            }
          else if (condition->instanceOf ("AssessmentStatement"))
            {
              childRoles = ((AssessmentStatement *)condition)->getRoles ();
            }
          else
            { // ICompoundStatement
              childRoles = ((CompoundStatement *)condition)->getRoles ();
            }

          vector<Role *>::iterator it;
          for (it = childRoles->begin (); it != childRoles->end (); ++it)
            {
              roles->push_back (*it);
            }

          delete childRoles;
        }
    }
  return roles;
}

GINGA_NCL_END
