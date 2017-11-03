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
#include "FormatterPredicate.h"

GINGA_NAMESPACE_BEGIN

FormatterPredicate::FormatterPredicate (PredicateType type)
{
  _type = type;
  _parent = nullptr;
}

FormatterPredicate::~FormatterPredicate ()
{
  for (auto child: _children)
    delete child;
}

PredicateType
FormatterPredicate::getType ()
{
  return _type;
}

FormatterPredicate *
FormatterPredicate::clone ()
{
  FormatterPredicate *clone = new FormatterPredicate (_type);
  if (_type == PredicateType::ATOM)
    {
      clone->setTest (_atom.left, _atom.test, _atom.right);
    }
  else
    {
      for (auto child: _children)
        clone->addChild (child->clone ());
    }
  return clone;
}

void
FormatterPredicate::setTest (const string &left, PredicateTestType test,
                             const string &right)
{
  g_assert (_type == PredicateType::ATOM);
  _atom.test = test;
  _atom.left = left;
  _atom.right = right;
}

void
FormatterPredicate::getTest (string *left, PredicateTestType *test,
                             string *right)
{
  g_assert (_type == PredicateType::ATOM);
  tryset (left, _atom.left);
  tryset (test, _atom.test);
  tryset (right, _atom.right);
}

void
FormatterPredicate::addChild (FormatterPredicate *child)
{
  g_assert_nonnull (child);
  switch (_type)
    {
    case PredicateType::NEGATION:
      g_assert (_children.size () == 0);
    case PredicateType::CONJUNCTION:
    case PredicateType::DISJUNCTION:
      break;
    case PredicateType::FALSUM:
    case PredicateType::VERUM:
    case PredicateType::ATOM:
    default:
      g_assert_not_reached  ();
    }
  child->initParent (this);
  _children.push_back (child);
}

const vector<FormatterPredicate *> *
FormatterPredicate::getChildren ()
{
  return &_children;
}

void
FormatterPredicate::initParent (FormatterPredicate *parent)
{
  g_assert_null (_parent);
  g_assert_nonnull (parent);
  _parent = parent;
}

FormatterPredicate *
FormatterPredicate::getParent ()
{
  return _parent;
}

GINGA_NAMESPACE_END
