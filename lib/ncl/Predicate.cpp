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
#include "Predicate.h"



GINGA_NCL_BEGIN

Predicate::Predicate (PredicateType type)
{
  _type = type;
}

Predicate::~Predicate ()
{
}

PredicateType
Predicate::getType ()
{
  return _type;
}

void
Predicate::initTest (Assessment *op1, PredicateTestType test,
                     Assessment *op2)
{
  g_assert_nonnull (op1);
  g_assert_nonnull (op2);
  g_assert (_type == PredicateType::ATOM);
  g_assert_null (_atom.left);
  g_assert_null (_atom.right);
  _atom.test = test;
  _atom.left = op1;
  _atom.right = op2;
}

void
Predicate::getTest (Assessment **op1, PredicateTestType *test,
                    Assessment **op2)
{
  g_assert (_type == PredicateType::ATOM);
  tryset (op1, _atom.left);
  tryset (test, _atom.test);
  tryset (op2, _atom.right);
}

void
Predicate::addChild (Predicate *child)
{
  g_assert_nonnull (child);
  switch (_type)
    {
    case PredicateType::NEGATION:
      g_assert (_children.size () == 0);
    case PredicateType::CONJUNCTION:
    case PredicateType::DISJUNCTION:
      break;
    case PredicateType::ATOM:
    default:
      g_assert_not_reached  ();
    }
  _children.push_back (child);
}

const vector<Predicate *> *
Predicate::getChildren ()
{
  return &_children;
}

GINGA_NCL_END
