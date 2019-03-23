/* Copyright (C) 2006-2018 PUC-Rio/Laboratorio TeleMidia

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
along with Ginga.  If not, see <https://www.gnu.org/licenses/>.  */

#include "aux-ginga.h"
#include "Predicate.h"

GINGA_NAMESPACE_BEGIN

Predicate::Predicate (Predicate::Type type)
{
  _type = type;
  _parent = nullptr;
  _atom.test = Predicate::EQ;
  _atom.left = "";
  _atom.right = "";
}

Predicate::~Predicate ()
{
  for (auto child : _children)
    delete child;
}

Predicate::Type
Predicate::getType ()
{
  return _type;
}

string
Predicate::toString ()
{
  switch (_type)
    {
    case Predicate::FALSUM:
      return "false";
    case Predicate::VERUM:
      return "true";
    case Predicate::ATOM:
      {
        string test;
        switch (_atom.test)
          {
          case Predicate::EQ:
            test = "==";
            break;
          case Predicate::NE:
            test = "!=";
            break;
          case Predicate::LT:
            test = "<";
            break;
          case Predicate::LE:
            test = "<=";
            break;
          case Predicate::GT:
            test = ">";
            break;
          case Predicate::GE:
            test = ">=";
            break;
          default:
            g_assert_not_reached ();
          }
        test = (_atom.left[0] == '$') ? _atom.left + test
                                      : "'" + _atom.left + "'" + test;
        test = (_atom.right[0] == '$') ? test + _atom.right
                                       : test + "'" + _atom.right + "'";
        return test;
      }
    default:
      {
        string conn;
        switch (_type)
          {
          case Predicate::NEGATION:
            conn = "not";
            break;
          case Predicate::CONJUNCTION:
            conn = "and";
            break;
          case Predicate::DISJUNCTION:
            conn = "or";
            break;
          default:
            g_assert_not_reached ();
          }
        conn += "(";
        if (_children.size () > 0)
          {
            auto it = _children.begin ();
            conn += (*it)->toString ();
            while (++it != _children.end ())
              conn += ", " + (*it)->toString ();
          }
        conn += ")";
        return conn;
      }
    }
  g_assert_not_reached ();
}

Predicate *
Predicate::clone ()
{
  Predicate *clone = new Predicate (_type);
  if (_type == Predicate::ATOM)
    {
      clone->setTest (_atom.left, _atom.test, _atom.right);
    }
  else
    {
      for (auto child : _children)
        clone->addChild (child->clone ());
    }
  return clone;
}

void
Predicate::setTest (const string &left, Predicate::Test test,
                    const string &right)
{
  g_assert (_type == Predicate::ATOM);
  _atom.test = test;
  _atom.left = left;
  _atom.right = right;
}

void
Predicate::getTest (string *left, Predicate::Test *test, string *right)
{
  g_assert (_type == Predicate::ATOM);
  tryset (left, _atom.left);
  tryset (test, _atom.test);
  tryset (right, _atom.right);
}

const list<Predicate *> *
Predicate::getChildren ()
{
  return &_children;
}

void
Predicate::addChild (Predicate *child)
{
  g_assert_nonnull (child);
  switch (_type)
    {
    case Predicate::NEGATION:
    case Predicate::CONJUNCTION:
    case Predicate::DISJUNCTION:
      break;
    case Predicate::FALSUM:
    case Predicate::VERUM:
    case Predicate::ATOM:
    default:
      g_assert_not_reached ();
    }
  child->initParent (this);
  _children.push_back (child);
}

void
Predicate::initParent (Predicate *parent)
{
  g_assert_null (_parent);
  g_assert_nonnull (parent);
  _parent = parent;
}

Predicate *
Predicate::getParent ()
{
  return _parent;
}

GINGA_NAMESPACE_END
