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
#include "CompoundCondition.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_NCL_BEGIN

CompoundCondition::CompoundCondition () : TriggerExpression ()
{
  _expressions = new vector<Condition *>;
}

CompoundCondition::CompoundCondition (Condition *c1,
                                      Condition *c2)
    : TriggerExpression ()
{
  _expressions = new vector<Condition *>;
  _expressions->push_back (c1);
  _expressions->push_back (c2);
}

CompoundCondition::~CompoundCondition ()
{
  vector<Condition *>::iterator i;

  if (_expressions != NULL)
    {
      i = _expressions->begin ();
      while (i != _expressions->end ())
        {
          delete *i;
          ++i;
        }

      delete _expressions;
      _expressions = NULL;
    }
}

const vector<Condition *> *
CompoundCondition::getConditions ()
{
  return _expressions;
}

void
CompoundCondition::addCondition (Condition *condition)
{
  g_assert_nonnull (condition);
  _expressions->push_back (condition);
}

void
CompoundCondition::removeCondition (
    Condition *condition)
{
  vector<Condition *>::iterator iterator;
  vector<Condition *>::iterator i;

  iterator = _expressions->begin ();
  while (iterator != _expressions->end ())
    {
      if ((*iterator) == condition)
        {
          i = _expressions->erase (iterator);
          if (i == _expressions->end ())
            return;
        }
      ++iterator;
    }
}

GINGA_NCL_END
